/*
 * gensequfile.c
 *
 * Copyright (c) Cortina-Systems Limited 2011.  All rights reserved.
 *
 * Author: Joe Hsu <joe.hsu@cortina-systems.com>
 *
 *  Goldengate sequence file generateion
 *
 * The program generates the sequence count and write it to file
 * with a random byte for tagging per 128 bytes.
 *
 * usage:
 *   gensequfile  <filename>  <size_in_mb>
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define RANDOM_TAG	0x80
#define	_1MB 		(1024*1024UL)
unsigned char absolute_buf[_1MB+1]={0};
main(int argc, char *argv[])
{
	unsigned long size, i;
	unsigned long absolute_addr=0UL;
	FILE *file;
	int fd;

	if (argc < 3)
	{
		printf("./gensequfile {filename} <size_in_mb>\r\n");
		exit(1);
	}

	size = atoi(argv[2]);
	printf("filename=%s, size=%dM\r\n", argv[1], size);
	fd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC, S_IWRITE|S_IREAD);
	//printf("fd=%d\r\n", fd);
	if (fd < 0)
	{
		printf("%s: open %s failed!!!\r\n", __func__, argv[1]);
		exit(1);
	}
	size *= 1024*1024UL;
	printf("Add a random number of one byte for tagging per %d bytes\n", RANDOM_TAG);
	printf("Writing...\n");

	srand(time(NULL));
	while (size)
	{
		for (i=0; i< _1MB; i+=4) {
			absolute_buf[i+0] = (unsigned char)(absolute_addr);
			absolute_buf[i+1] = (unsigned char)(absolute_addr >>  8);
			absolute_buf[i+2] = (unsigned char)(absolute_addr >> 16);
			if (i % RANDOM_TAG) {
				absolute_buf[i+3] = (unsigned char)(absolute_addr >> 24);
			} else {
				absolute_buf[i+3] = (unsigned char)rand();
			}
			absolute_addr += 4;
		}
		write(fd, absolute_buf, _1MB);
		size -= _1MB;
	};
	printf("\r\n");
	close(fd);
}
