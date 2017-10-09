/*
 * rgb2argb.c
 *
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *
 * Author: Joe Hsu <joe.hsu@cortina-systems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  Goldengate PrimeCell PL111 Color LCD Controller Test Program
 *
 * Description:
 *   This a a raw picture converter that converts the standrad.raw to argb.raw.
 *
 * usage:
 *   rgb2argb <srcrgb.raw> <desargb.raw>
 *   where the <srcrgb.raw> file can be generated with the following command (in linux)
 *   djpeg -outfile <output.raw> <input.jpg>
 */
#include <linux/stddef.h>	// NULL
#include <stdio.h>	// printf
#include <stdlib.h>	//
#include <asm/fcntl.h>	// O_RDWR
#include <asm/errno.h>	// EINVAL
#include <asm/mman.h>	// PROT_READ/PROT_WRITE
#include <sys/ioctl.h>	// ioctl
#include <string.h>
#include <dirent.h>	// DIR
#include <sys/stat.h>	// struct stat

#include "fbtest.h"

int rawHeaderDetect(unsigned char *file_buffer)
{
    unsigned char *ptr;
    int  offset=0;

    ptr = file_buffer;
    ptr = strchr(ptr, '\n');	// bypass 'P6'
    if (ptr) {
        ptr++;
        ptr = strchr(ptr, '\n');	// bypass '640 480'
        if (ptr) {
            ptr++;
            ptr = strchr(ptr, '\n');	// bypass '255'
            if (ptr) {
                ptr++;
                offset = ptr - file_buffer;
            }
        }
    }
    //DBGPRINT(1, "file raw header offset = %d\n", offset);
    return offset;
}

/*
int RGB2ARGB(int rgb)
{
    BYTE R= (BYTE)(rgb >> 16);
    BYTE G= (BYTE)(rgb >>  8);
    BYTE B= (BYTE)(rgb);

    printf("R=0x%02x\tG=0x%02x\tB=0x%02x\t\n", R, G, B);
    return (ARgB(0, R, G, B));
}
*/

int loadPicFile(FILE *srcfile, FILE *desfile)
{
    struct stat fileStat;
    int handle, ret=0;
    int offset_to_raw, rgb_cnt, argb_cnt, i, cnt;
    unsigned int srcfile_size, desfile_size, rgb_size, argb_size, rgb_size_div3;
    unsigned char *src_buffer, *des_buffer;

// get file info
    handle = fileno(srcfile);
    if (0 > fstat(handle, &fileStat)) {
        printf("Error read file stat!\n");
        return -EINVAL;
    }
    srcfile_size = fileStat.st_size;
    if (!srcfile_size) {
        printf("Zero file size error!\n");
        return -EINVAL;
    }
    printf("The source file size is %d\n", srcfile_size);

// Load source file to src ram
    // allocate a bulk of memory to load raw file raw data
    src_buffer = (unsigned char *)malloc(srcfile_size+2); // Add EOF to end of the string
    if (!src_buffer) {
        printf("Error allocate source buffer %d bytes\r\n", srcfile_size+2);
        return -EINVAL;
    }

    // read source file and store it to memory src_buffer
    if((fread(src_buffer, sizeof(unsigned char), srcfile_size, srcfile) != srcfile_size) || ferror(srcfile))
    {
        printf("Error reading source file!\r\n");
        free(src_buffer);
        return -EINVAL;
    }

// Calculate header info
    offset_to_raw = rawHeaderDetect(src_buffer);
    if (0 > offset_to_raw) {
        free(src_buffer);
        return -EINVAL;
    }
    rgb_size = srcfile_size - (unsigned int)offset_to_raw;
    printf("header size=%d, rgb_size=%d\n", offset_to_raw, rgb_size);

// add zero pad
    desfile_size = offset_to_raw + rgb_size/3*4;
    printf("The destination file size=%d\n", desfile_size);

    // allocate a bulk of ram for dest file
    des_buffer = (unsigned char *)malloc(desfile_size+2); // Add EOF to end of the string
    if (!des_buffer) {
        printf("Error allocate destination buffer %d bytes\r\n", desfile_size+2);
        return -EINVAL;
    }

    // add header info
    for (cnt=0; cnt<offset_to_raw; cnt++) {
        des_buffer[cnt] = src_buffer[cnt];
    }

    // add zero pad
    rgb_size_div3 = rgb_size/3;
    argb_cnt=0; rgb_cnt=0;
    for (cnt=0; cnt<rgb_size_div3; cnt++) {
        for (i=0; i<3; i++) {
            des_buffer[offset_to_raw + argb_cnt++] = src_buffer[offset_to_raw + rgb_cnt++];
        }
        des_buffer[offset_to_raw + argb_cnt++] = 0;
    }

// store dest ram to destination file
    fwrite(des_buffer, sizeof(unsigned char), desfile_size, desfile);

// release ram
    free(src_buffer);
    free(des_buffer);

    return ret;
}

int main(int argc, char *argv[])
{
    FILE *fileDescSrc, *fileDescDes;
    char *srcname, *desname;

    if (3 == argc) {
        srcname = argv[1];
        desname = argv[2];
        if (NULL == (fileDescSrc = fopen(srcname, "rb"))) {
            printf("Error open %s file!\n", srcname);
            return -ENOENT;
        }
        if (NULL == (fileDescDes = fopen(desname, "w+"))) {
            printf("Error open %s file!\n", desname);
            return -ENOENT;
        }
    } else {
        printf("usage:\n");
        printf("rgb2argb <srcrgb.raw> <desargb.raw>\n");
        return 0;
    }

    loadPicFile(fileDescSrc, fileDescDes);

    fclose(fileDescSrc);
    fclose(fileDescDes);
    return 0;
}

