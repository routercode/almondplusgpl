/*
 * fbtest.c
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
 * ping-pong buffer and RGB565 make rules:
 * gcc  -o fbtest fbtest.c \
 *         -DCONFIG_FB_G2_LCD_PINGPONG_BUFFER \
 *         -DCONFIG_FB_G2_LCD_POLLING_FB \
 *         -DCONFIG_FB_G2_LCD_RGB565
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
#undef CONFIG_FB_G2_LCD_PINGPONG_BUFFER
unsigned int tog_baseaddr;	// toggle mmap even/odd buffer and base address
int fbOpen(pFBDEV pfbdev)
{
	long int screen_size = 0;
    pfbdev->fb = open(pfbdev->dev, O_RDWR);
    if(pfbdev->fb < 0) {
        printf("Error opening %s: %m.\n", pfbdev->dev);
        return -EINVAL;
    }

    if (ioctl(pfbdev->fb,FBIOGET_FSCREENINFO,&(pfbdev->fb_fixscreeninfo))) {	// fix screen info
        printf("loading ioctl FBIOGET_FSCREENINFO failed\n");
        goto OPENFAIL;
    }

    if (ioctl(pfbdev->fb,FBIOGET_VSCREENINFO,&(pfbdev->fb_varscreeninfo))) {	// var screen info
        printf("loading ioctl FBIOGET_VSCREENINFO failed\n");
        goto OPENFAIL;
    }

    //rgb_default = pfbdev->fb_varscreeninfo.bits_per_pixel;
    //if (24 != rgb_default) {
    //    pfbdev->fb_varscreeninfo.bits_per_pixel = 24;
    //}
    //ioctl(pfbdev->fb,FBIOPUT_VSCREENINFO,&(pfbdev->fb_varscreeninfo));
    screen_size =  pfbdev->fb_varscreeninfo.xres *  pfbdev->fb_varscreeninfo.yres *  pfbdev->fb_varscreeninfo.bits_per_pixel / 8;
//    pfbdev->fb_mem_off = (unsigned long)(pfbdev->fb_fixscreeninfo.smem_start) & (~PAGE_MASK);
#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
    pfbdev->fb_mem_adr = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, pfbdev->fb, 0);
#else
    pfbdev->fb_mem_adr = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, pfbdev->fb, 0);
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER

    if (-1L == (long) pfbdev->fb_mem_adr) {
        printf("mmap error! mem:%p offset:%lu. Maybe the framebuffer size is mismatched.\n", pfbdev->fb_mem_adr, pfbdev->fb_mem_off);
        goto OPENFAIL;
    }

    //printf("virtual fb address: %p\n", pfbdev->fb_mem_adr);
    DBGPRINT(1, "virtual fb address: %p\n", pfbdev->fb_mem_adr);

// parameters initialization:
    //if (24 == pfbdev->fb_varscreeninfo.bits_per_pixel) {
    //    pfbdev->bpp  = BPP;	// change 3 bytes into 4 bytes rgb format
    //} else { 
        pfbdev->bpp  = pfbdev->fb_varscreeninfo.bits_per_pixel?
                       pfbdev->fb_varscreeninfo.bits_per_pixel/8:BPP;	// Bits/pixel
    //}
    //printf("<%s:%d> pfbdev->bpp=%d\n", __func__, __LINE__, pfbdev->bpp);
    if (1 == pfbdev->bpp)
        pfbdev->bpp = BPP;	// for debug only

    pfbdev->fb_x = pfbdev->fb_varscreeninfo.xres?
                   pfbdev->fb_varscreeninfo.xres:MAX_X;
    pfbdev->fb_y = pfbdev->fb_varscreeninfo.yres?
                   pfbdev->fb_varscreeninfo.yres:MAX_Y;
    pfbdev->bytes_per_hline = pfbdev->fb_x * pfbdev->bpp;
    pfbdev->rgb  = 0;

#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
    unsigned int status, check_mask;
    status=ioctl(pfbdev->fb, G2_FB_GET_MASK_TOGGLE,&io);
    check_mask = io.regs.val;	// Check if the toggle function of lcd driver has been masked or not.
    status=ioctl(pfbdev->fb, G2_FB_GET_BASE_ADDR,&io);
    //printf("<%s:%d> smem_start: 0x%08x, status=0x%08x\n", __func__, __LINE__, io.regs.val, status);
#if defined(CONFIG_FB_G2_LCD_POLLING_FB)
    if (pfbdev->fb_fixscreeninfo.smem_start != io.regs.val) {
        tog_baseaddr = check_mask ? fbdev.fb_fixscreeninfo.smem_len:0;	// start from even buffer
        //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
    } else {
        tog_baseaddr = check_mask ? 0:fbdev.fb_fixscreeninfo.smem_len;	// start from odd buffer
        //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
    }
#else
    if (pfbdev->fb_fixscreeninfo.smem_start != io.regs.val) {
        ioctl(pfbdev->fb, G2_FB_TOGGLE_BASE_ADDR,&io);
    }
    tog_baseaddr = fbdev.fb_fixscreeninfo.smem_len;	// start from odd buffer
#endif // CONFIG_FB_G2_LCD_POLLING_FB
#else
    tog_baseaddr = 0;
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER
    //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);

    return 0;

OPENFAIL:
    close(pfbdev->fb);
    return -EINVAL;
}

int fbClose(pFBDEV pfbdev)
{
    //pfbdev->fb_varscreeninfo.bits_per_pixel = rgb_default;
    //ioctl(pfbdev->fb,FBIOPUT_VSCREENINFO,&(pfbdev->fb_varscreeninfo));

#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
    unsigned int status;
    status = ioctl(pfbdev->fb, G2_FB_GET_BASE_ADDR,&io);
    //printf("smem_start: 0x%08x, status=0x%08x, tog_baseaddr=0x%08x\n", io.regs.val, status, tog_baseaddr);
    //if (pfbdev->fb_fixscreeninfo.smem_start != io.regs.val) {
    //    ioctl(pfbdev->fb, G2_FB_TOGGLE_BASE_ADDR,&io);
    //}
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER
    munmap(pfbdev->fb_mem_adr, pfbdev->fb_fixscreeninfo.smem_len);
    sleep(1);	// for completely showing picture
    close(pfbdev->fb);

    return 0;
}

int dumpFixScreenInfo(void)
{
    int status;

// Show fix screen info
    DBGPRINT(1, "ioctl=FBIOGET_FSCREENINFO\n");
    status = ioctl(fbdev.fb,FBIOGET_FSCREENINFO,&(fbdev.fb_fixscreeninfo));
    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }

    printf("\nfb_fixscreeninfo\n");
    printf("\tid: %s\n", fbdev.fb_fixscreeninfo.id);
    printf("\tsmem_start: 0x%lx\n", fbdev.fb_fixscreeninfo.smem_start);
    printf("\tsmem_len: 0x%x\n", fbdev.fb_fixscreeninfo.smem_len);
    printf("\ttype: %d\n", fbdev.fb_fixscreeninfo.type);
    printf("\ttype_aux: %d\n", fbdev.fb_fixscreeninfo.type_aux);
    printf("\tvisual: %d\n", fbdev.fb_fixscreeninfo.visual);
    printf("\txpanstep: %d\n", fbdev.fb_fixscreeninfo.xpanstep);
    printf("\typanstep: %d\n", fbdev.fb_fixscreeninfo.ypanstep);
    printf("\tline_length: %d\n", fbdev.fb_fixscreeninfo.line_length);
    printf("\tmmio_start: 0x%lx\n", fbdev.fb_fixscreeninfo.mmio_start);
    printf("\tmmio_len: 0x%x\n", fbdev.fb_fixscreeninfo.mmio_len);
    printf("\taccel: 0x%x\n", fbdev.fb_fixscreeninfo.accel);

    return 0;
}

int putVarScreenInfo(int argc, char *argv[])
{
    int status;
// load var screen info
    DBGPRINT(1, "ioctl=FBIOGET_VSCREENINFO\n");
    status = ioctl(fbdev.fb,FBIOGET_VSCREENINFO,&(fbdev.fb_varscreeninfo));
    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }
// modify field
    //fbdev.fb_varscreeninfo.xres_virtual = 1024;
    //fbdev.fb_varscreeninfo.yres_virtual = 15776;
    //fbdev.fb_varscreeninfo.bits_per_pixel = 8;

// update var screen info
    DBGPRINT(1, "ioctl=FBIOPUT_VSCREENINFO\n");
    status = ioctl(fbdev.fb,FBIOPUT_VSCREENINFO,&(fbdev.fb_varscreeninfo));
    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }
    dumpVarScreenInfo();
    return 0;
}

int dumpVarScreenInfo(void)
{
    int status;
// Show var screen info
    DBGPRINT(1, "ioctl=FBIOGET_VSCREENINFO\n");
    status = ioctl(fbdev.fb,FBIOGET_VSCREENINFO,&(fbdev.fb_varscreeninfo));

    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }

    printf("\nfb_varscreeninfo\n");
    printf("\txres: %d\n", fbdev.fb_varscreeninfo.xres);
    printf("\tyres: %d\n", fbdev.fb_varscreeninfo.yres);
    printf("\txres_virtual: %d\n", fbdev.fb_varscreeninfo.xres_virtual);
    printf("\tyres_virtual: %d\n", fbdev.fb_varscreeninfo.yres_virtual);
    printf("\txoffset: %d\n", fbdev.fb_varscreeninfo.xoffset);
    printf("\tyoffset: %d\n", fbdev.fb_varscreeninfo.yoffset);
    printf("\tbits_per_pixel: %d\n", fbdev.fb_varscreeninfo.bits_per_pixel);
    printf("\tgrayscale: %d\n", fbdev.fb_varscreeninfo.grayscale);
    printf("\tactivate: %d\n", fbdev.fb_varscreeninfo.activate);
    printf("\theight: %d\n", fbdev.fb_varscreeninfo.height);
    printf("\twidth: %d\n", fbdev.fb_varscreeninfo.width);
    printf("\tpixclock: %d\n", fbdev.fb_varscreeninfo.pixclock);
    printf("\tleft_margin: %d\n", fbdev.fb_varscreeninfo.left_margin);
    printf("\tright_margin: %d\n", fbdev.fb_varscreeninfo.right_margin);
    printf("\tupper_margin: %d\n", fbdev.fb_varscreeninfo.upper_margin);
    printf("\tlower_margin: %d\n", fbdev.fb_varscreeninfo.lower_margin);
    printf("\thsync_len: %d\n", fbdev.fb_varscreeninfo.hsync_len);
    printf("\tvsync_len: %d\n", fbdev.fb_varscreeninfo.vsync_len);
    printf("\tsync: %d\n", fbdev.fb_varscreeninfo.sync);
    printf("\tvmode: %d\n", fbdev.fb_varscreeninfo.vmode);
    printf("\tnonstd: %d\n", fbdev.fb_varscreeninfo.nonstd);
    printf("\tred.offset: %d\n", fbdev.fb_varscreeninfo.red.offset);
    printf("\tred.length: %d\n", fbdev.fb_varscreeninfo.red.length);
    printf("\tred.msb_right: %d\n", fbdev.fb_varscreeninfo.red.msb_right);
    printf("\tgreen.offset: %d\n", fbdev.fb_varscreeninfo.green.offset);
    printf("\tgreen.length: %d\n", fbdev.fb_varscreeninfo.green.length);
    printf("\tgreen.msb_right: %d\n", fbdev.fb_varscreeninfo.green.msb_right);
    printf("\tblue.offset: %d\n", fbdev.fb_varscreeninfo.blue.offset);
    printf("\tblue.length: %d\n", fbdev.fb_varscreeninfo.blue.length);
    printf("\tblue.msb_right: %d\n", fbdev.fb_varscreeninfo.blue.msb_right);

    return 0;
}

int g2Ioctl(int argc, char *argv[], int subcmd)
{
    int status = -1;

    DBGENTER(1);
    switch (argc) {
    case 5:
        io.regs.val = (unsigned int)atoi(argv[4]);
        DBGPRINT(1, "set val=0x%08x\n", io.regs.val);
    case 4:
        io.regs.off = (unsigned int)atoi(argv[3]);
        DBGPRINT(1, "offset=0x%08x\n", io.regs.off);
    case 3:
        status = ioctl(fbdev.fb, subcmd, &io);
        if (status) {
            printf("Error: 0x%08x\n", status);
        }
        break;
    default:
        break;
    }
    DBGLEAVE(1);
    return status;
}

int g2GetPL111(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (4 == argc) {
        //status = g2Ioctl(argc, argv, subcmd);
        io.regs.off = (unsigned int)atoi(argv[3]);
        DBGPRINT(1, "offset=0x%08x\n", io.regs.off);
        status = ioctl(fbdev.fb, G2_FB_GET_REG, &io);
        if (status) {
            printf("Error: 0x%08x\n", status);
        }
    }
    return status;
}

int g2SetAutoplay(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (3 == argc) {
        //status= g2Ioctl(argc, argv, subcmd);
        status = ioctl(fbdev.fb, G2_FB_SET_AUTOPLAY, &io);
        if (status) {
            printf("Error: 0x%08x\n", status);
        }
    }
    return status;
}

int g2ResetAutoplay(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (3 == argc) {
        //status = g2Ioctl(argc, argv, subcmd);
        status = ioctl(fbdev.fb, G2_FB_RESET_AUTOPLAY, &io);
        if (status) {
            printf("Error: 0x%08x\n", status);
        }
    }
    return status;
}

int g2ToggleFbBase(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (3 == argc) {
        //status = g2Ioctl(argc, argv, subcmd);
        if (G2_TOGGLE_BASE_ADDR == subcmd) {
            DBGPRINT(1, "ioctl=G2_TOGGLE_BASE_ADDR\n");
            status = ioctl(fbdev.fb, G2_FB_TOGGLE_BASE_ADDR, &io);
            tog_baseaddr ^= fbdev.fb_fixscreeninfo.smem_len;	// prepare and switch to next buffer
        // Get current framebuffer base address
            //sleep(1);
            //ioctl(fbdev.fb, G2_FB_TOGGLE_BASE_ADDR,&io);
            //printf("<%s:%d> change to tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
        }
    }
    return status;
}

int g2MaskToggle(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (3 == argc) {
        //status = g2Ioctl(argc, argv, subcmd);
        if (G2_MASK_TOGGLE == subcmd) {
            DBGPRINT(1, "ioctl=G2_MASK_TOGGLE\n");
            status = ioctl(fbdev.fb, G2_FB_MASK_TOGGLE, &io);
        } else if (G2_UNMASK_TOGGLE == subcmd) {
            DBGPRINT(1, "ioctl=G2_UNMASK_TOGGLE\n");
            status = ioctl(fbdev.fb, G2_FB_UNMASK_TOGGLE, &io);
        }
    }
    return status;
}

int g2SetPL111(int argc, char *argv[], int subcmd)
{
    int status = -1;

    if (5 == argc) {
        //status = g2Ioctl(argc, argv, subcmd);
        io.regs.off = (unsigned int)atoi(argv[3]);
        io.regs.val = (unsigned int)atoi(argv[4]);
        DBGPRINT(1, "offset=0x%08x, set val=0x%08x\n", io.regs.off, io.regs.val);
        status = ioctl(fbdev.fb, G2_FB_SET_REG, &io);
        if (status) {
            printf("Error: 0x%08x\n", status);
        }
    }
    return status;
}

int ioBlank(void)
{
    int status;

    DBGPRINT(1, "ioctl=FBIOBLANK\n");
    status = ioctl(fbdev.fb, FBIOBLANK,&(fbdev.fb_cmap));
    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }
    return 0;
}

int dumpGetCMAP(void)
{
    int status;

    DBGENTER(1);
    DBGPRINT(1, "ioctl=FBIOGETCMAP\n");
    status = ioctl(fbdev.fb, FBIOGETCMAP,&(fbdev.fb_cmap));
    if (status) {
        printf("Error: 0x%08x\n", status);
	return status;
    }

    printf("\tstart: %d\n", fbdev.fb_cmap.start);	// First entry
    printf("\tlen: %d\n", fbdev.fb_cmap.len);	// Number of entries, unit: 2 bytes.
    DBGLEAVE(1);

    return 0;
}

int dumpInfo(int argc, char *argv[])
{
    unsigned int cnt;

    DBGENTER(1);
    if (3 == argc) {
        for (cnt=0; cnt<FB_INFO_NUMBER; cnt++) {
            if (!strcmp(argv[2], fb_info[cnt])) {	// info match
                DBGPRINT(1, "You type %s matched!\n", argv[2]);
                switch (cnt) {
                case 0:
                    dumpFixScreenInfo();
                    break;
                case 1:
                    dumpVarScreenInfo();
                    break;
                case 2:
                    dumpGetCMAP();
                    break;
                //case 3:
                //    ioctl(fbdev.fb,FBIOBLANK,&(fbdev.fb_varscreeninfo));
                default:
                    ;
                }
            }
        }
    }
    DBGLEAVE(1);
    return 0;
}

int dumPl111Reg(int argc, char *argv[])
{
    unsigned int i,j;
    unsigned int seg[6]={0x0000, 0x0010, 0x0020, 0x0c00, 0x0fe0, 0x0ff0};  // Pl111 register segment offset

    DBGENTER(1);
    printf("\nPL111 addr\t      0x00\t      0x04\t      0x08\t      0x0c\n");
    for(j=0; j<6; j++) {
        printf("0x%08x\t", seg[j]);
        for(i=0; i<0x10; i+=4) {
            io.regs.off = i + seg[j];
            ioctl(fbdev.fb, G2_FB_GET_REG,&io);
            printf("0x%08x\t", io.regs.val);
        }
        printf("\n");
    }
    DBGLEAVE(1);
    return 0;
}

int ARGB2RGB(int ARGB)
{
    const int Mask = (1 << 24) - 1;	// 00000000, 11111111, 11111111, 11111111

    DBGPRINT(2,"Mask=0x%x\n", Mask);
    DBGPRINT(2,"ARGB: before mask=0x%x\n", ARGB);
    ARGB &= Mask;
    DBGPRINT(2,"ARGB: after mask=0x%x\n", ARGB);

    BYTE R= (BYTE)(ARGB >> 16);
    BYTE G= (BYTE)(ARGB >>  8);
    BYTE B= (BYTE)(ARGB);

    return (RGB(R, G, B));
}

int RGB2ARGB(int rgb)
{
    //const int Mask = (1 << 24) - 1;	// 00000000, 11111111, 11111111, 11111111

    //printf("Mask=0x%x\n", Mask);
    //printf("ARGB: before mask=0x%x\n", ARGB);
    //RGB &= Mask;
    //printf("ARGB: after mask=0x%x\n", ARGB);

    BYTE R= (BYTE)(rgb >> 16);
    BYTE G= (BYTE)(rgb >>  8);
    BYTE B= (BYTE)(rgb);

    return (ARgB(0, R, G, B));
}

void drawPoint(int x, int y, unsigned short color)
{
    unsigned char  *fb_addr;
    union pixelgrp *pixelgrp;
    unsigned int pixelcnt;
    unsigned char *__fb_buffer;

    __fb_buffer = fb_buffer + tog_baseaddr;	// toggle base address
//#if (NULL_FB == FALSE)
    fb_addr = __fb_buffer + (y * fbdev.bytes_per_hline) + (x * fbdev.bpp);
    pixelgrp = (union pixelgrp *)fb_addr;
    //printf("(x,y)=(%d, %d), bytespline=%d, fbdev.bpp=%d, __fb_buffer=%p, fb_addr=%p\n", x,y, fbdev.bytes_per_hline, fbdev.bpp, __fb_buffer, fb_addr);
    DBGPRINT(3,"(x,y)=(%d, %d), bytespline=%d, fbdev.bpp=%d, __fb_buffer=%p, fb_addr=%p\n", x,y, fbdev.bytes_per_hline, fbdev.bpp, __fb_buffer, fb_addr);

    if (2 == fbdev.bpp) {
    switch (color) {
    case RED:
        //pixelgrp->rgb.r = 0x1f;		// red
        pixelgrp->colori = 0x001f001f;		// red
        break;
    case GREEN:
        //pixelgrp->rgb.g = 0x3f;		// green
        pixelgrp->colori = 0x07e007e0;		// green
        break;
    case BLUE:
        //pixelgrp->rgb.b = 0x1f;		// blue
        pixelgrp->colori = 0xf800f800;		// blue
        break;
    case WHITE:
        //pixelgrp->colors = 0xffff;	// white
        pixelgrp->colori = 0xffffffff;	// white
        break;
    case BLACK:
        //pixelgrp->colors = 0x0000;	// black
        pixelgrp->colori = 0x00000000;	// black
        break;
    default:
        //pixelgrp->colors = color;
        pixelgrp->colori = color;
    }
    }

    if (4 == fbdev.bpp) {
    switch (color) {
    case RED:
        pixelgrp->colori = 0x000000ff;		// red
        break;
    case GREEN:
        pixelgrp->colori = 0x0000ff00;		// green
        break;
    case BLUE:
        pixelgrp->colori = 0x00ff0000;		// blue
        break;
    case WHITE:
        pixelgrp->colori = 0x00ffffff;		// white
        break;
    case BLACK:
        pixelgrp->colori = 0x00000000;		// black
        break;
    default:
        pixelgrp->colori = 0;
    }
    }
//#endif
}

void drawHline(int x, int y, int x1, unsigned short color)	// horizontal line
{
    for (; x < x1; x++) {
        drawPoint(x, y, color);
    }
}

void drawRect(int x, int y, int x1, int y1, unsigned short color)	// rectangle
{
    for (; y < y1; y++)
    {
        drawHline(x, y, x1, color);
    }
}

extern int colorTst(int argc, char *argv[]);
int drawTst(int argc, char *argv[])
{
    unsigned int x,y;
    unsigned short color = 0x3;	// default white color
    char *colorArg[] = {"", "color", "black"};

    DBGENTER(1);
    //bpp  = fbdev.fb_varscreeninfo.bits_per_pixel?
    //           fbdev.fb_varscreeninfo.bits_per_pixel:BPP;	// Bits/pixel
    //fb_x = fbdev.fb_varscreeninfo.xres?
    //           fbdev.fb_varscreeninfo.xres:MAX_X;
    //fb_y = fbdev.fb_varscreeninfo.yres?
    //           fbdev.fb_varscreeninfo.yres:MAX_Y;
    //DBGPRINT(1,"resolution: %d * %d; bpp=%d\n", fb_x, fb_y, bpp);

    if ((7 == argc) && (!strcmp(argv[6], "clear"))) {
        colorTst(3, colorArg);	// Reset background color to black
    }
    fb_buffer = fbdev.fb_mem_adr;
    //printf("address of fb_buffer=0x%x\r\n", *fb_buffer);
    //printf("address of fb_buffer=%p\r\n", fb_buffer);
    DBGPRINT(1,"address of fb_buffer=%p\r\n", fb_buffer);

    if (3 == argc) {	// load a figure.txt file to be drawn
        printf("Not implemented yet!!\r\n");
    } else if (3 < argc) {
        x = atoi(argv[2]);
        y = atoi(argv[3]);
        if (4 == argc) {
            drawPoint(x,y, color);
        } else if (5 == argc) {
            drawHline(x,y, atoi(argv[4]), color);
        } else {
            drawRect(x,y, atoi(argv[4]), atoi(argv[5]), color);
        }
/*        
        while(1) {
            printf("Any key...\r\n");
            sleep(3);
        }
*/
    } 
    DBGLEAVE(1);
    return 0;

MMAP_FAIL:	// Abnormal terminal
    return -EINVAL;
}

int colorTst(int argc, char *argv[])
{
    unsigned int fb_size = fbdev.fb_fixscreeninfo.smem_len;
    unsigned int cnt, pixelcnt;
    unsigned short *color_white;
    unsigned char *__fb_buffer;
    unsigned int pixelgrp_size, colormatch=FALSE;
    union pixelgrp pixelgrp;

    DBGENTER(1);
    if (3 == argc) {
        fb_buffer = fbdev.fb_mem_adr;
        pixelgrp_size = sizeof(pixelgrp);
        for (cnt=0; cnt<BCOLOR_NUMBER; cnt++) {
            if (!strcmp(argv[2], bcolor[cnt])) {	// color match
                DBGPRINT(1,"You type %s matched!\n", argv[2]);
                colormatch = TRUE;

    if (2 == fbdev.bpp) {
                //pixelgrp.colors = 0x0000;	// black
                pixelgrp.colori = 0x00000000;	// black
                if (RED == cnt) {		
                    //pixelgrp.rgb.r = 0x1f;	// red
                    pixelgrp.colori = 0x001f001f;		// red
                } else if (GREEN == cnt) {
                    //pixelgrp.rgb.g = 0x3f;	// green
                    pixelgrp.colori = 0x07e007e0;		// green
                } else if (BLUE == cnt) {
                    //pixelgrp.rgb.b = 0x1f;	// blue
                    pixelgrp.colori = 0xf800f800;		// blue
                } else if (WHITE == cnt) {
                    //pixelgrp.colors = 0xffff;	// white
                    pixelgrp.colori = 0xffffffff;	// white
                } else if (BLACK == cnt) {
                    //pixelgrp.colors = 0x0000;	// black
                    pixelgrp.colori = 0x00000000;	// black
                }
                //printf("pixelgrp.colors=0x%04x (BPP=2)\n", pixelgrp.colors);
                //DBGPRINT(1,"pixelgrp.colors=0x%04x\n", pixelgrp.colors);
                DBGPRINT(1,"pixelgrp.colori=0x%04x\n", pixelgrp.colori);
    }
    if (4 == fbdev.bpp) {
                if (RED == cnt) {		
                    pixelgrp.colori = 0x000000ff;	// red
                } else if (GREEN == cnt) {
                    pixelgrp.colori = 0x0000ff00;	// green
                } else if (BLUE == cnt) {
                    pixelgrp.colori = 0x00ff0000;	// blue
                } else if (WHITE == cnt) {
                    pixelgrp.colori = 0x00ffffff;	// white
                } else if (BLACK == cnt) {
                    pixelgrp.colori = 0x00000000;	// black
                }
                //printf("pixelgrp.colori=0x%04x (BPP=4)\n", pixelgrp.colori);
                DBGPRINT(1,"pixelgrp.colori=0x%04x\n", pixelgrp.colori);
    }
            }
        }
//#if (NULL_FB == FALSE)
        if (colormatch) {
            __fb_buffer = fb_buffer + tog_baseaddr;	// toggle base address
    if (2 == fbdev.bpp) {
            //DBGPRINT(2,"pixelgrp_size=%d\n", pixelgrp_size);
            //DBGPRINT(2,"pixelgrp.colors=0x%x\n", pixelgrp.colors);
            for (pixelcnt=0; pixelcnt < fb_size/pixelgrp_size; pixelcnt++) {
                memcpy(__fb_buffer, &pixelgrp, pixelgrp_size);
                __fb_buffer += pixelgrp_size;
            }
#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
#if defined(CONFIG_FB_G2_LCD_POLLING_FB)
            do {
                ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
                if (0 == io.regs.val) {
                    break;
                }
                //printf("<%s:%d> G2_FB_GET_TOGGLED, io.regs.val(a)=0x%08x\n", __func__, __LINE__, io.regs.val);
            } while(1);
#endif // CONFIG_FB_G2_LCD_POLLING_FB
            ioctl(fbdev.fb, G2_FB_TOGGLE_BASE_ADDR,&io);
            tog_baseaddr ^= fbdev.fb_fixscreeninfo.smem_len;	// prepare and switch to next buffer
            //printf("<%s:%d> G2_FB_TOGGLE_BASE_ADDR, tog_baseaddr(a)=0x%08x\n", __func__, __LINE__, tog_baseaddr);
            do {
                ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
                if (0 == io.regs.val) {
                    break;
                }
            } while(1);
            //sleep(1);
            //printf("<%s:%d> G2_FB_GET_TOGGLED, io.regs.val(a)=0x%08x\n", __func__, __LINE__, io.regs.val);
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER
    }
    if (4 == fbdev.bpp) {
            DBGPRINT(2,"pixelgrp_size=%d\n", pixelgrp_size);
            DBGPRINT(2,"pixelgrp.colori=0x%x\n", pixelgrp.colori);
            for (pixelcnt=0; pixelcnt < fb_size/pixelgrp_size; pixelcnt++) {
                memcpy(__fb_buffer, &pixelgrp, pixelgrp_size);
                __fb_buffer += pixelgrp_size;
            }
#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
#if defined(CONFIG_FB_G2_LCD_POLLING_FB)
            do {
                ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
                if (0 == io.regs.val) {
                    break;
                }
            } while(1);
#endif // CONFIG_FB_G2_LCD_POLLING_FB
            ioctl(fbdev.fb, G2_FB_TOGGLE_BASE_ADDR,&io);
            tog_baseaddr ^= fbdev.fb_fixscreeninfo.smem_len;	// prepare and switch to next buffer
            do {
                ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
                if (0 == io.regs.val) {
                    break;
                }
            } while(1);
            //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER
            //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
    }
        }
//#endif	// NULL_FB == FALSE
    }
    DBGLEAVE(1);
    return 0;
}

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
    DBGPRINT(1, "file raw header offset = %d\n", offset);
    return offset;
}

int showPicFile(char *filename, unsigned char *fb_buffer)
{
    FILE *fileDesc;
    int handle;
    struct stat fileStat;
    int offset_to_raw;
    unsigned int file_size, raw_size;
    unsigned char *file_buffer, *raw_buffer;
    struct rawHeader *rawHeader;
    unsigned char *__fb_buffer;

    __fb_buffer = fb_buffer + tog_baseaddr;

// Handle a picture file
    if (NULL == (fileDesc = fopen(filename, "rb"))) {
        printf("Error open %s file!\n", filename);
        return -ENOENT;
    }
    DBGPRINT(1, "\nOpen %s file successfully!\n", filename);
    handle = fileno(fileDesc);
    if (0 > fstat(handle, &fileStat)) {
        printf("Error read file stat!\n");
        fclose(fileDesc);
        return -EINVAL;
    }
    file_size = fileStat.st_size;
    if (!file_size) {
        printf("Zero file size error!\n");
        fclose(fileDesc);
        return -EINVAL;
    }
    DBGPRINT(1,"file size = %d\n", file_size);

    // allocate a bulk of memory to load raw file raw data
    file_buffer = (unsigned char *)malloc(file_size+2); // Add EOF to end of the string
    if (!file_buffer) {
        printf("Error allocate %s raw buffer %d bytes\r\n", filename, file_size+2);
        return -EINVAL;
    }

    // read file and store it to memory file_buffer
    if((fread(file_buffer, sizeof(unsigned char), file_size, fileDesc) != file_size) || ferror(fileDesc))
    {
        printf("Error reading %s file!\r\n", filename);
        free(file_buffer);
        fclose(fileDesc);
        return -EINVAL;
    }

    //printf("%s\n", filename);
    // handled file done
    fclose(fileDesc);

// Handle framebuffer
    // Raw Header Detect
    offset_to_raw = rawHeaderDetect(file_buffer);
    if (0 > offset_to_raw) {
        free(file_buffer);
        return -EINVAL;
    }
    raw_size = file_size - (unsigned int)offset_to_raw;
    DBGPRINT(1, "raw size=%d\n", raw_size);

    // Dump raw data to lcd panel
    raw_buffer = file_buffer + ((unsigned int)offset_to_raw/sizeof(unsigned char));	// skip raw header info
    //printf("<%s:%d> file_buffer=%p, raw_buffer=%p, __fb_buffer=%p\n", __func__, __LINE__, file_buffer, raw_buffer, __fb_buffer);
    DBGPRINT(1, "file_buffer=%p, raw_buffer=%p, __fb_buffer=%p\n", file_buffer, raw_buffer, __fb_buffer);
    memcpy(__fb_buffer, raw_buffer, raw_size);	// send to framebuffer
//#if defined(CONFIG_FB_G2_LCD_PINGPONG_BUFFER)
#ifdef CONFIG_FB_G2_LCD_PINGPONG_BUFFER
//#if defined(CONFIG_FB_G2_LCD_POLLING_FB)
#ifdef CONFIG_FB_G2_LCD_POLLING_FB
    do {
        ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
        if (0 == io.regs.val) {
            break;
        }
    } while(1);
#endif // CONFIG_FB_G2_LCD_POLLING_FB
    ioctl(fbdev.fb, G2_FB_TOGGLE_BASE_ADDR,&io);
    tog_baseaddr ^= fbdev.fb_fixscreeninfo.smem_len;	// switch to next buffer
    do {
        ioctl(fbdev.fb, G2_FB_GET_TOGGLED,&io);
        if (0 == io.regs.val) {
            break;
        }
    } while(1);
    //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
#endif // CONFIG_FB_G2_LCD_PINGPONG_BUFFER
    //printf("<%s:%d> tog_baseaddr=0x%08x\n", __func__, __LINE__, tog_baseaddr);
    DBGPRINT(1, "memory copied and done\n");

    // handled file memory done
    free(file_buffer);
    return 0;

CLOSE_FILE:
    fclose(fileDesc);
    return -EINVAL;
}

int chkFileExt(char *filename, char *fileext)
{
    unsigned int status;
    char *ptr;
    FILE *fileDesc;

    DBGPRINT(2, "filename=%s\n", filename);
// Step1: check file extension
    ptr = strrchr(filename, '.'); // check extension, match the last char '.'
    if((NULL == ptr) || (0 != strcasecmp(ptr, fileext))) {
        return 0;
    }

// Step2: Is a valid file?
    fileDesc = fopen(filename, "rb");
    if (NULL == fileDesc) {
        printf("'%s' is NOT a file.\n", filename);
        return 0;
    }
    fclose(fileDesc);

    return 1;
}

int pictureTst(int argc, char *argv[])
{
    //FILE *filedesc;
    DIR  *pic_dir;
    //struct stat fileStat;
    //int handle;
    //unsigned int file_size;
    unsigned int fb_size, cnt, repeat;
    static char *pic_dirname;
    struct dirent *dir_entry;
    char *ptr, *rel_filename;
    int  raw_files, status=0;
    FILELIST *pic_list;  // picture file list

    DBGENTER(1);
    if (argc < 2) {
        return 0;	// Do nothing, just return.
    }

// allocate framebuffer memory
    //bpp  = fbdev.fb_varscreeninfo.bits_per_pixel?
    //           fbdev.fb_varscreeninfo.bits_per_pixel:BPP;	// Bits/pixel
    //fb_x = fbdev.fb_varscreeninfo.xres?
    //           fbdev.fb_varscreeninfo.xres:MAX_X; //  非零
    //fb_y = fbdev.fb_varscreeninfo.yres?
    //           fbdev.fb_varscreeninfo.yres:MAX_Y; //  非零
    //printf("LCD resolution: %d * %d; bpp=%d bytes\n", fb_x, fb_y, bpp);
    fb_size  = fbdev.fb_fixscreeninfo.smem_len?
               fbdev.fb_fixscreeninfo.smem_len:(MAX_X * MAX_Y * BPP);

// Processing picture files
    // Check file...
    fb_buffer = fbdev.fb_mem_adr;
    if (0 < (pic_dir = opendir(argv[2]))) {	// open a directory
        pic_dirname = argv[2];
        DBGPRINT(1, "Opening %s directory successfully\n", pic_dirname);

        // get the number of strings we need to allocate.
        raw_files=0;
        while(NULL != (dir_entry = readdir(pic_dir))) {
            DBGPRINT(2, "pic_dirname=%s, dir_entry->d_name=%s\n", pic_dirname, dir_entry->d_name);
            rel_filename = (char *)malloc(strlen(pic_dirname) + strlen(dir_entry->d_name) +2);
            sprintf(rel_filename, "%s/%s", pic_dirname, dir_entry->d_name);
            DBGPRINT(2, "rel_filename=%s\n", rel_filename);
            if (chkFileExt(rel_filename, ".raw")) {
                raw_files++;  // How many raw files are in the given directory?
            }
            free(rel_filename);
        }
        if (0 == raw_files) {
            printf("There is no RAW file in %s directory\r\n", pic_dirname);
            closedir(pic_dir);
            return 0;
        }
        DBGPRINT(1, "There are %d RAW files in %s directory\r\n", raw_files, pic_dirname);

        // allocate the file list.
        rewinddir(pic_dir);		// reset the directory stream
        pic_list = (FILELIST *)malloc(raw_files * sizeof(FILELIST));
        raw_files=0;
        while(NULL != (dir_entry = readdir(pic_dir))) {
            DBGPRINT(2, "pic_dirname=%s, dir_entry->d_name=%s\n", pic_dirname, dir_entry->d_name);
            rel_filename = (char *)malloc(strlen(pic_dirname) + strlen(dir_entry->d_name) +2);
            sprintf(rel_filename, "%s/%s", pic_dirname, dir_entry->d_name);
            DBGPRINT(2, "rel_filename=%s\n", rel_filename);
            if (chkFileExt(rel_filename, ".raw")) {
                pic_list[raw_files].name = malloc(strlen(pic_dirname) + strlen(dir_entry->d_name) +1+1); // 1 for /; 1 for EOF.
                sprintf(pic_list[raw_files].name, "%s/%s", pic_dirname, dir_entry->d_name);	// copy dir_name + file_name to a string
                raw_files++;
            }
            free(rel_filename);
        }
        
        // Now, the directory has been arranged done. Let's close it.
        closedir(pic_dir);

        // process each picture file
        repeat=REPEATCNT;
        while(repeat--) {
            for(cnt=0; cnt<raw_files; cnt++) {
               status = showPicFile(pic_list[cnt].name, fb_buffer);
               if (status < 0) {	// Error or something wrong!
                   free(pic_list);
                   return status;
               }
               //sleep(1);	// joe20110421
            }
        }

        // free picture file strings
        free(pic_list);
    } else if (chkFileExt(argv[2], ".raw")) {
	// WARNING: don't use "name.raw" as directory name. It's failure if you do that.
        //printf("%s has a '.raw' file extension.\n", argv[2]);
        showPicFile(argv[2], fb_buffer);
    } else {
        printf("Unknown string '%s'\n", argv[2]);
    }

    DBGLEAVE(1);
    return 0;
}

int mmapTst(int argc, char *argv[])
{
    printf("<%s>\n", __func__);
    return 0;
}


int rawRGBTst(int argc, char *argv[])
{
    int cnt;

    printf("<%s>\n", __func__);
    if (argc) {
        for (cnt=0; cnt<argc; cnt++)
            printf("argv[%d]=%s\t", cnt, argv[cnt]);
        printf("\n");
    }
    return 0;
}

int ioctlFunc(int argc, char *argv[])
{
    int cnt, status = -1;

    DBGENTER(1);
    DBGPRINT(1, "<%s>\n", __func__);
    if (2 < argc) {
        for (cnt=0; cnt<FB_IOCTL_CMD; cnt++){
            if (!strcmp(argv[2], fb_ioctl_cmd[cnt])) {	// info match
                DBGPRINT(1, "You type %s matched!\n", argv[2]);
                switch (argc) {
                case 5:
                    switch (cnt) {
                    case IOPUT_VSCREENINFO:
                        status = putVarScreenInfo(argc, argv);
                        break;
                    case G2_SET_REG:
                        status = g2SetPL111(argc, argv, G2_SET_REG);
                        break;
                    }
                    break;
                case 4:
                    switch (cnt) {
                    case IOPUT_VSCREENINFO:
                        status = putVarScreenInfo(argc, argv);
                        break;
                    case G2_GET_REG:
                        status = g2GetPL111(argc, argv, G2_GET_REG);
                        break;
                    }
                    break;
                case 3:
                    switch (cnt) {
                    case IOGET_VSCREENINFO:
                        status = dumpVarScreenInfo();
                        break;
                    case IOPUT_VSCREENINFO:
                        status = putVarScreenInfo(argc, argv);
                        break;
                    case IOGET_FSCREENINFO:
                        status = dumpFixScreenInfo();
                        break;
                    case IOGETCMAP:
                        status = dumpGetCMAP();
                        break;
                    case IOBLANK:
                        status = ioBlank();
                        break;
                    case G2_TOGGLE_BASE_ADDR:
                        status = g2ToggleFbBase(argc, argv, G2_TOGGLE_BASE_ADDR);
                        break;
                    case G2_SET_AUTOPLAY:
                        status = g2SetAutoplay(argc, argv, G2_SET_AUTOPLAY);
                        break;
                    case G2_RESET_AUTOPLAY:
                        g2ResetAutoplay(argc, argv, G2_RESET_AUTOPLAY);
                        break;
                    case G2_MASK_TOGGLE:
                        status = g2MaskToggle(argc, argv, G2_MASK_TOGGLE);
                        break;
                    case G2_UNMASK_TOGGLE:
                        status = g2MaskToggle(argc, argv, G2_UNMASK_TOGGLE);
                        break;
                    }
                    break;
                }
            }
        }
    }
    DBGLEAVE(1);
    return status;
}

int dummyFunc(int argc, char *argv[])
{
    int cnt;

    DBGENTER(1);
    DBGPRINT(1, "<%s>\n", __func__);
    if (argc) {
        for (cnt=0; cnt<argc; cnt++)
            printf("argv[%d]=%s\t", cnt, argv[cnt]);
        printf("\n");
    }
    DBGLEAVE(1);
    return 0;
}

struct MenuRecord main_menu[] = 
{
    {"draw", "x (y) (x1) (y1)", "Drawing dots, lines, and rectangles test", "where parameters:\n\t    x\t\tis a 'figure.txt' file to be drawn if y is not given\n\t    (x, y)\tform the first pixel point of screen\n\t    (x1,y)\tform the second pixel point of screen. The x1 pairs with y if y1 is not given.\n\t    (x1,y1)\tform the second pixel point of screen. The y1 pairs with x1.\n", drawTst},
    //{"rgb", "555/565/888", "Raw RGB format test", "Select one type of RGB for displaying.\n", rawRGBTst},
    {"color", "red/green/blue/white/black", "Basic colors test", "Select one color for displaying.\n", colorTst},
    {"raw", "<directory/filename.raw>", "Grab raw picture file and show on lcd panel", "The string ended with '.raw' is considered as a raw filename otherwise\n\ta directory name. In the case of 'directory name', this application will\n\tgrab all raw files ended with '.raw' in the directory and display them on lcd.\n\tWARNING:\t Don't use 'name.raw' as a directory name. It's failure if you do that.\n", pictureTst},
    {"dumpinfo", "<parameters>", "Dump framebuffer related information", "where parameters:\n\t    fixscreen\tframebuffer fix screen information\n\t    varscreen\tframebuffer var screen information\n\t    getcmap\tget framebuffer color map\n", dumpInfo},
    {"reg", NULL, "Dump PL111 controller registers", NULL, dumPl111Reg},
    {"ioctl", NULL, "ioctl related function", "where parameters:\n\t    get_vscreeninfo\tget var screen info\n\t    put_vscreeninfo\tput var screen info\n\t    get_fscreeninfo\tget fix screen info\n\t    getcmap\t\tget color map\n\t    ioblank\t\tturn off framebuffer\n\t    g2_set_reg off val\tset G2 PL111 register\n\t    g2_get_reg off\tget G2 PL111 register\n\t    g2_set_autoplay\n\t    g2_reset_autoplay\n\t    g2_mask_toggle\n\t    g2_unmask_toggle\n\t    g2_toggle_base_addr\n", ioctlFunc},
    //{"dummy", NULL, "Dummy function", NULL, dummyFunc},
};

const unsigned int MAIN_MENU_OPTIONS=
    sizeof(main_menu)/sizeof(main_menu[0]);

void usage(void)
{
    int cnt;

    printf("usage:\n");

    DBGENTER(1);
    for(cnt=0; cnt<MAIN_MENU_OPTIONS; cnt++) {
        printf("    fbtest %s", main_menu[cnt].cmd);	// Display command
        if (main_menu[cnt].parms) {
            printf(" %s", main_menu[cnt].parms);	// Display command parameters
        }
        printf("\n");	// end command line
        if (main_menu[cnt].text) {
            if (strcmp(main_menu[cnt].cmd, "raw")) {
                printf("\t%s", main_menu[cnt].text);	// Display text
            } else {	// special command process
                printf("\tGrab %dx%d raw picture file and show on lcd panel",
                        fbdev.fb_varscreeninfo.xres?fbdev.fb_varscreeninfo.xres:MAX_X,
                        fbdev.fb_varscreeninfo.yres?fbdev.fb_varscreeninfo.yres:MAX_Y);	// Display raw text
            }
        }
        printf("\n");	// end description
        if (main_menu[cnt].usage) {
            printf("\t%s", main_menu[cnt].usage);	// Display usage
        }
        printf("\n");	// end description
    }
    DBGLEAVE(1);
}

int main(int argc, char *argv[])
{
    unsigned int cnt, status=OK;

    //printf("\n");

// open framebuffer device
//#if (NULL_FB == FALSE)
    strcpy(fbdev.dev, FB0);
    if (fbOpen(&fbdev)) {
        return -EINVAL;
    }
//#endif

    //ARGB2RGB(0x12345678);
    if (argc < 2) {
        usage();
    // Close framebuffer device
//#if (NULL_FB == FALSE)
        fbClose(&fbdev);
//#endif
        return 0;
    }

    for (cnt=0; cnt<MAIN_MENU_OPTIONS; cnt++) {
        if (!strcmp(main_menu[cnt].cmd, argv[1])) {
            DBGPRINT(2, "Your typing %s is CORRECT!\n", argv[1]);
            status = main_menu[cnt].function(argc, argv);
        }
    }

// Close framebuffer device
//#if (NULL_FB == FALSE)
    fbClose(&fbdev);
//#endif

    return status;
}
