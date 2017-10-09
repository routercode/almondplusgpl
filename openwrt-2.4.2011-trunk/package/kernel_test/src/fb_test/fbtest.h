/*
 * fbtest.h
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
 */
#ifndef _FBTOOLS_H_
#define _FBTOOLS_H_

#include <linux/fb.h>

typedef struct fbdev{
       int fb;  	// Framebuffer device descriptor
       unsigned long  fb_mem_off;	// screen base offset
       unsigned char *fb_mem_adr;	// screen base address
       struct fb_fix_screeninfo fb_fixscreeninfo;
       struct fb_var_screeninfo fb_varscreeninfo;
       struct fb_cmap           fb_cmap;
       int fb_x;
       int fb_y;
       unsigned int bpp;	// Bytes/pixel
       int bytes_per_hline;	// bytes per horizontal line
       int rgb;
       char dev[16];
} FBDEV, *pFBDEV;

typedef int (*pFunc)(int argc, char *argv[]);
struct MenuRecord
{
    const char *cmd;	// cmd for fbtest
    const char *parms;	// parameters for cmd
    const char *text;	// description for the cmd
    const char *usage;	// usage for the cmd
    pFunc       function;	// function call for the cmd
};

#define G2_FB_IOCTL_MAGIC   0xFF  // Select unused one. see Documentation/ioctl/ioctl-number.txt
                                  // group 32-63

#define G2_RGB_ORDER_555    0
#define G2_RGB_ORDER_565    1	
#define G2_RGB_ORDER_888    2	

#define G2_BUFFMT_NONINTERLACE 0
#define G2_BUFFMT_INTERLACE    1

#define G2_FB_BYPASS_SRC	_IOW(G2_FB_IOCTL_MAGIC,32 , unsigned int)	// Bypass Source
#define G2_FB_SET_FB_NUM	_IOW(G2_FB_IOCTL_MAGIC,33 , unsigned int) // SET framebuffer number
#define G2_FB_GET_FB_NUM	_IOR(G2_FB_IOCTL_MAGIC,34 , unsigned int) // GET framebuffer number
#define G2_FB_IOSBUFFMT		_IOW(G2_FB_IOCTL_MAGIC,35 , unsigned int) // SET buffer format
#define G2_FB_IOGBUFFMT		_IOR(G2_FB_IOCTL_MAGIC,36 , unsigned int) // GET buffer format
#define G2_FB_SET_RGB_ORDER	_IOW(G2_FB_IOCTL_MAGIC,37 , unsigned int) // SET RGB order
#define G2_FB_GET_RGB_ORDER	_IOR(G2_FB_IOCTL_MAGIC,38 , unsigned int) // GET RGB order
#define G2_FB_SET_REG		_IOW(G2_FB_IOCTL_MAGIC,39 , unsigned int) // SET CLCD controller registers
#define G2_FB_GET_REG		_IOWR(G2_FB_IOCTL_MAGIC,40, unsigned int) // GET CLCD controller registers
#define G2_FB_TOGGLE_BASE_ADDR  _IOW(G2_FB_IOCTL_MAGIC,41 , unsigned int) // Toggle base address
#define G2_FB_GET_BASE_ADDR     _IOWR(G2_FB_IOCTL_MAGIC,42, unsigned int) // Get current base address
#define G2_FB_GET_TOGGLED	_IOWR(G2_FB_IOCTL_MAGIC,43, unsigned int) // Get base address toggled
#define G2_FB_SET_AUTOPLAY	_IOW(G2_FB_IOCTL_MAGIC,44 , unsigned int) // Set CLCD driver auto play pictures
#define G2_FB_RESET_AUTOPLAY	_IOWR(G2_FB_IOCTL_MAGIC,45, unsigned int) // Reset CLCD driver auto play pictures
#define G2_FB_SET_BITS		_IOW(G2_FB_IOCTL_MAGIC,46 , unsigned int) // Set relative bits on given register
#define G2_FB_CLR_BITS		_IOWR(G2_FB_IOCTL_MAGIC,47, unsigned int) // Clear relative bits on given register
#define G2_FB_MASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,48, unsigned int) // Mask the toggle of ping-pong buffer operation
#define G2_FB_UNMASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,49, unsigned int) // UnMask the toggle of ping-pong buffer operation
#define G2_FB_GET_MASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,50, unsigned int) // Get the status of Masking toggle operation

union iodata {
    unsigned long dat;
    struct {
        unsigned int off;	// offset
        unsigned int val;	// value
    } regs;
};

union iodata io;

int fb_open(pFBDEV pFbdev);	// open a framebuffer device descriptor
int fb_close(pFBDEV pFbdev);	// close a framebuffer device descriptor

#define TRUE  1
#define FALSE 0
#define OK    0
#define FAIL  1
#define DONE  2
#define REPEATCNT 1

#define FB0 "/dev/fb0"	// Framebuffer device inode 0

#define MAX_X	1024
#define MAX_Y    768
#if defined(CONFIG_FB_G2_LCD_RGB565)
#define BPP        2    // 2 bytes (16 bits) RGB=565 per pixel
#else
#define BPP        4    // 3+1 bytes (24 bits) per pixel for PL111 controller, plus 1 dummy byte
#endif

#define RED_COLOR       0x1f	// only 5 bits
#define GREEN_COLOR     0x1f	// only 5 bits
#define BLUE_COLOR      0x1f	// only 5 bits

#define BGR	1	// default

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

unsigned int  rgb_default;
unsigned char *fb_buffer;
FBDEV fbdev;	// Define a framebuffer instance

//static int fb_x;
//static int fb_y;
//static unsigned int bpp;	// Bytes/pixel

enum IOCTL {
// Generic ioctl
    IOGET_VSCREENINFO=0,
    IOPUT_VSCREENINFO,
    IOGET_FSCREENINFO,
    //IOPUTCMAP,
    IOGETCMAP,
    //IOPAN_DISPLAY,
    //IO_CURSOR,
    //IOGET_CON2FBMAP,
    //IOPUT_CON2FBMAP,
    IOBLANK,
// G2-specific
    G2_SET_REG,
    G2_GET_REG,
    G2_TOGGLE_BASE_ADDR,
    G2_SET_AUTOPLAY,
    G2_RESET_AUTOPLAY,
    G2_MASK_TOGGLE,
    G2_UNMASK_TOGGLE
};

unsigned char *fb_ioctl_cmd[] = {
    "get_vscreeninfo",
    "put_vscreeninfo",
    "get_fscreeninfo",
    "getcmap",
    "ioblank",
    "g2_set_reg",
    "g2_get_reg",
    "g2_toggle_base_addr",
    "g2_set_autoplay",
    "g2_reset_autoplay",
    "g2_mask_toggle",
    "g2_unmask_toggle",
};
const unsigned int FB_IOCTL_CMD=	// the number of basic colors
    sizeof(fb_ioctl_cmd)/sizeof(fb_ioctl_cmd[0]);

enum COLOR {
    RED=0,
    GREEN,
    BLUE,
    WHITE,
    BLACK
};

unsigned char *bcolor[] = {	// basic colors
    "red",	// 0
    "green",	// 1
    "blue",	// 2
    "white",	// 3
    "black"	// 4
};

const unsigned int BCOLOR_NUMBER=	// the number of basic colors
    sizeof(bcolor)/sizeof(bcolor[0]);

unsigned char *fb_info[] = {	// framebuffer information
    "fixscreen",
    "varscreen",
    "getcmap"
};
const unsigned int FB_INFO_NUMBER=	// the number of basic colors
    sizeof(fb_info)/sizeof(fb_info[0]);

/*
struct fb_info {
    unsigned char *info[] = {
        "fixscreen",
        "varscreen",
        "getcmap"
    };
    int   (*)getinfo(int index);	// get function from this option
};
*/

typedef struct {
    char *name;  /* file name */
} FILELIST, *PFILELIST;

/*
union pixel {
    struct  rgb {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } rgb;
    struct argb {
        unsigned char a;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } argb;
    unsigned int  colori;	//  32-bit color format
    unsigned char colorc[4];	// 4*8-bit color format
} pixel;
*/

//#if (BPP == 2)
//union pixelgrp {	// group pixels
//    struct  rgb {
////#if defined(CONFIG_FB_G2_LCD_RGB565)
//        unsigned short r:5;	// [ 4: 0] red
//        unsigned short g:6;	// [10: 5] green
//        unsigned short b:5;	// [15:11] blue
////#else
////        unsigned short b:5;	// [ 4: 0] blue
////        unsigned short g:6;	// [10: 5] green
////        unsigned short r:5;	// [15:11] red
////#endif // CONFIG_FB_G2_LCD_RGB565
//    } rgb;
//    unsigned short colors;	//  16-bit color format
//};
//#endif

//#if (BPP == 3)
//union pixelgrp {	// group pixels
//    struct  rgb {
//        unsigned char b;
//        unsigned char g;
//        unsigned char r;
//    } rgb[4];
//    struct argb {
//        unsigned char b;
//        unsigned char g;
//        unsigned char r;
//        unsigned char a;
//    } argb[3];
//    unsigned int  colori[3];	//  32-bit color format
//    unsigned char colorc[12];	// 4*8-bit color format
//} pixelgrp;
//#endif

#if (BPP == 4)
union pixelgrp {	// group pixels
    struct  rgb {
        unsigned char dummy;	// bit[31:24] always zero
        unsigned char b;	// bit[23:16]
        unsigned char g;	// bit[15: 8]
        unsigned char r;	// bit[ 7: 0]
    } rgb;
    struct argb {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
    } argb;
    unsigned int  colori;	//  32-bit color format
    unsigned char colorc[4];	// 4*8-bit color format
} pixelgrp;
#endif

/*
struct rawHeader
{
    unsigned char *format;	// raw format type
    unsigned char *dimen;	// dimension
    unsigned char *depth;	// color depth
    unsigned int offset;	// headersize
};
*/

typedef unsigned char   BYTE;           //  8-bit
typedef unsigned short  WORD;           // 16-bit
typedef unsigned long   DWORD;          // 32-bit

#define RGB(r, g, b) (DWORD) (((BYTE) (r) | \
                              ((WORD) (g) << 8)) | \
                             ((DWORD) (b) << 16))

#define ARgB(a, r, g, b) (DWORD) (((BYTE) (r) | \
                                  ((WORD) (g) << 8)) | \
                                 ((DWORD) (b) << 16) | \
                                 ((DWORD) (a) << 24))


//#define FBDBG_LEVEL 10
#undef  FBDBG_LEVEL
#ifdef  FBDBG_LEVEL

#define MAX_DBG_INDENT_LEVEL    5
#define DBG_INDENT_SIZE         2
#define MAX_DBG_MESSAGES        0

static int dbg_indent;
static int dbg_cnt;
#define dbg_print(level, format, arg...)                        \
    if (level <= FBDBG_LEVEL) {                                 \
        if (!MAX_DBG_MESSAGES || dbg_cnt < MAX_DBG_MESSAGES) {  \
            int ind = dbg_indent;                               \
            unsigned long flags;                                \
            dbg_cnt++;                                          \
            if (ind > MAX_DBG_INDENT_LEVEL)                     \
                ind = MAX_DBG_INDENT_LEVEL;                     \
            printf("%*s", ind * DBG_INDENT_SIZE, "");           \
            printf(format, ## arg);                             \
        }                                                       \
    }

#define DBGPRINT        dbg_print

#define DBGENTER(level) do { \
                dbg_print(level, "%s: Entering\n", __FUNCTION__); \
                dbg_indent++; \
        } while (0)

#define DBGLEAVE(level) do { \
                dbg_indent--; \
                dbg_print(level, "%s: Leaving\n", __FUNCTION__); \
        } while (0)

#else	// FBDBG_LEVEL

#define DBGPRINT(level, format, ...)
#define DBGENTER(level)
#define DBGLEAVE(level)

#endif	// FBDBG_LEVEL

#endif
