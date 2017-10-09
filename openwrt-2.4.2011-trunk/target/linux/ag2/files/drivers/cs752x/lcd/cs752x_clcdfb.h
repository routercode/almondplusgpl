/*
 *  linux/drivers/video/cs752x_clcdfb.h
 *
 * Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
 *                Joe Hsu <joe.hsu@cortina-systems.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 *  Goldengate PrimeCell PL111 Color LCD Controller
 */
#ifndef __CS752X_CLCDFB_H__
#define __CS752X_CLCDFB_H__

#define G2_FB_IOCTL_MAGIC   0xFF  /* Select unused one. see Documentation/ioctl/ioctl-number.txt, group 32-63 */

#define G2_RGB_ORDER_555    0
#define G2_RGB_ORDER_565    1
#define G2_RGB_ORDER_888    2

#define G2_BUFFMT_NONINTERLACE 0
#define G2_BUFFMT_INTERLACE    1

#define G2_FB_BYPASS_SRC	_IOW(G2_FB_IOCTL_MAGIC,32 , unsigned int)	/* Bypass Source */
#define G2_FB_SET_FB_NUM	_IOW(G2_FB_IOCTL_MAGIC,33 , unsigned int)	/* SET framebuffer number */
#define G2_FB_GET_FB_NUM	_IOR(G2_FB_IOCTL_MAGIC,34 , unsigned int)	/* GET framebuffer number */
#define G2_FB_IOSBUFFMT		_IOW(G2_FB_IOCTL_MAGIC,35 , unsigned int)	/* SET buffer format */
#define G2_FB_IOGBUFFMT		_IOR(G2_FB_IOCTL_MAGIC,36 , unsigned int)	/* GET buffer format */
#define G2_FB_SET_RGB_ORDER	_IOW(G2_FB_IOCTL_MAGIC,37 , unsigned int)	/* SET RGB order */
#define G2_FB_GET_RGB_ORDER	_IOR(G2_FB_IOCTL_MAGIC,38 , unsigned int)	/* GET RGB order */
#define G2_FB_SET_REG		_IOW(G2_FB_IOCTL_MAGIC,39 , unsigned int)	/* SET CLCD controller registers */
#define G2_FB_GET_REG		_IOWR(G2_FB_IOCTL_MAGIC,40, unsigned int)	/* GET CLCD controller registers */
#define G2_FB_TOGGLE_BASE_ADDR	_IOW(G2_FB_IOCTL_MAGIC,41 , unsigned int)	/* Toggle base address */
#define G2_FB_GET_BASE_ADDR	_IOWR(G2_FB_IOCTL_MAGIC,42, unsigned int)	/* Get current base address */
#define G2_FB_GET_TOGGLED	_IOWR(G2_FB_IOCTL_MAGIC,43, unsigned int)	/* Get base address toggled */
#define G2_FB_SET_AUTOPLAY	_IOW(G2_FB_IOCTL_MAGIC,44 , unsigned int)	/* Set CLCD driver auto play pictures */
#define G2_FB_RESET_AUTOPLAY	_IOWR(G2_FB_IOCTL_MAGIC,45, unsigned int)	/* Reset CLCD driver auto play pictures */
#define G2_FB_SET_BITS		_IOW(G2_FB_IOCTL_MAGIC,46 , unsigned int)	/* Set relative bits on given register */
#define G2_FB_CLR_BITS		_IOWR(G2_FB_IOCTL_MAGIC,47, unsigned int)	/* Clear relative bits on given register */
#define G2_FB_MASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,48, unsigned int)	/* Mask the toggle of ping-pong buffer operation */
#define G2_FB_UNMASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,49, unsigned int)	/* UnMask the toggle of ping-pong buffer operation */
#define G2_FB_GET_MASK_TOGGLE	_IOWR(G2_FB_IOCTL_MAGIC,50, unsigned int)	/* Get the status of Masking toggle operation */
#define G2_FB_TURNON		_IOWR(G2_FB_IOCTL_MAGIC,51, unsigned int)	/* Turn on  the controller */
#define G2_FB_TURNOFF		_IOWR(G2_FB_IOCTL_MAGIC,52, unsigned int)	/* Turn off the controller */

#ifndef __IODATA__
#define __IODATA__
union iodata {
    unsigned long dat;
    struct {
        unsigned int off;
        unsigned int val;
    } regs;
};
#endif

static inline void cs752x_clcdfb_decode(struct clcd_fb *fb, struct clcd_regs *regs)
{
	u32 val, cpl;

	/*
	 * Program the CLCD controller registers and start the CLCD
	 */
	val = ((fb->fb.var.xres / 16) - 1) << 2;	/* PPL: Pixels-per-line */
	val |= (fb->fb.var.hsync_len - 1) << 8;		/* HSW: Horizontal synchronization pulse width */
	val |= (fb->fb.var.right_margin - 1) << 16;	/* HFP: Horizontal front porch */
	val |= (fb->fb.var.left_margin - 1) << 24;	/* HBP: Horizontal back porch */
	regs->tim0 = val;

	val = fb->fb.var.yres;  /* LPP: Lines per panel */
	if (fb->panel->cntl & CNTL_LCDDUAL)
		val /= 2;
	val -= 1; 				/* Program to the number of lines required, minus one */
	val |= (fb->fb.var.vsync_len - 1) << 10;/* VSW: Vertical synchronization pulse width */
	val |= fb->fb.var.lower_margin << 16;	/* VFP: Vertical front porch */
	val |= fb->fb.var.upper_margin << 24;	/* VBP: Vertical back porch */
	regs->tim1 = val;

	val = fb->panel->tim2;
	val |= fb->fb.var.sync & FB_SYNC_HOR_HIGH_ACT  ? 0 : TIM2_IHS; /* Invert hor sync */
	val |= fb->fb.var.sync & FB_SYNC_VERT_HIGH_ACT ? 0 : TIM2_IVS; /* Invert ver sync */

	cpl = fb->fb.var.xres_virtual;  	/* cpl: Clock per line. This field specifies the number of actual CLCP clocks to the LCD panel on each line. */
	if (fb->panel->cntl & CNTL_LCDTFT)	/* TFT */
		/* / 1 */;
	else if (!fb->fb.var.grayscale)		/* STN color */
		cpl = cpl * 8 / 3;
	else if (fb->panel->cntl & CNTL_LCDMONO8) /* STN monochrome, 8bit */
		cpl /= 8;
	else					/* STN monochrome, 4bit */
		cpl /= 4;

	regs->tim2 = val | ((cpl - 1) << 16);	/* Clock and Singal Polarity Control Register */

	regs->tim3 = fb->panel->tim3;		/* Line End Control Register */

	val = fb->panel->cntl;
	if (fb->fb.var.grayscale)
		val |= CNTL_LCDBW;

	switch (fb->fb.var.bits_per_pixel) {
	case 1:
		val |= CNTL_LCDBPP1;
		break;
	case 2:
		val |= CNTL_LCDBPP2;
		break;
	case 4:
		val |= CNTL_LCDBPP4;
		break;
	case 8:
		val |= CNTL_LCDBPP8;
		break;
	case 16:
		/*
		 * PL110 cannot choose between 5551 and 565 modes in
		 * its control register
		 */
#if defined(CONFIG_FB_CS752X_CLCD)
		if ((fb->dev->periphid & 0x000fffff) == 0x00041111)
#else
		if ((fb->dev->periphid & 0x000fffff) == 0x00041110)
#endif
			val |= CNTL_LCDBPP16;
		else if (fb->fb.var.green.length == 5)
			val |= CNTL_LCDBPP16;
		else
			val |= CNTL_LCDBPP16_565;
		break;
	case 32:
		val |= CNTL_LCDBPP24;
		break;
	}

	regs->cntl = val;
	regs->pixclock = fb->fb.var.pixclock;
}

/* #define CS752X_FBDBG_LEVEL 100 */
#undef  CS752X_FBDBG_LEVEL
#ifdef CS752X_FBDBG_LEVEL
#include <linux/spinlock.h>

#define MAX_DBG_INDENT_LEVEL	5
#define DBG_INDENT_SIZE		2
#define MAX_DBG_MESSAGES	0

#define dbg_print(level, format, arg...)                           \
       if (level <= CS752X_FBDBG_LEVEL) {                          \
           if (!MAX_DBG_MESSAGES || dbg_cnt < MAX_DBG_MESSAGES) {  \
               int ind = dbg_indent;                               \
               unsigned long flags;                                \
               spin_lock_irqsave(&dbg_spinlock, flags);            \
               dbg_cnt++;                                          \
               if (ind > MAX_DBG_INDENT_LEVEL)                     \
                  ind = MAX_DBG_INDENT_LEVEL;                      \
	       printk("%*s", ind * DBG_INDENT_SIZE, "");           \
               printk(format, ## arg);                             \
               spin_unlock_irqrestore(&dbg_spinlock, flags);       \
           }                                                       \
       }


#define DBGPRINT	dbg_print

#define DBGENTER(level)	do { \
		dbg_print(level, "%s: Enter\n", __FUNCTION__); \
		dbg_indent++; \
	} while (0)

#define DBGLEAVE(level)	do { \
		dbg_indent--; \
		dbg_print(level, "%s: Leave\n", __FUNCTION__); \
	} while (0)

#else		/* CS752X_FBDBG_LEVEL */

#define DBGPRINT(level, format, ...)
#define DBGENTER(level)
#define DBGLEAVE(level)

#endif		/* CS752X_FBDBG_LEVEL */

#endif		/* __CS752X_CLCDFB_H__ */
