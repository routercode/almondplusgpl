/*
 *  linux/drivers/video/cs752x_clcdfb.c
 *
 * Copyright (C) 2001 ARM Limited, by David A Rusling
 * Updated to 2.5, Deep Blue Solutions Ltd.
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/list.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/clk.h>
#include <linux/hardirq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

#include <asm/sizes.h>

#include <linux/dma-mapping.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/cs752x_clcd_regs.h>

static int    g2_lcd_enable;
static struct clcd_fb *fb;
#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
static int g2_lcd_toggled;
static int g2_lcd_mask_toggle;
#if !defined(CONFIG_FB_CS752X_CLCD_POLLING_FB)
wait_queue_head_t g2_lcd_wait_q;
#endif	/* CONFIG_FB_CS752X_CLCD_POLLING_FB */
#endif	/* CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER */
#if defined(CONFIG_FB_CS752X_CLCD_AUTOPLAY)
#define	TIMEOUT_VALUE (HZ/2)
static g2_lcd_autoplay;
static struct timer_list fb_tick;
#endif	/* CONFIG_FB_CS752X_CLCD_AUTOPLAY */

#if defined(CONFIG_FB_CS752X_CLCD_DEBUG)
#define TO_MAXCNT 32
static int    g2_pretimecnt, g2_abstimecnt, g2_timecnt;
static suseconds_t g2_current[TO_MAXCNT];
static struct timeval g2_now, g2_old;
#endif	/* CONFIG_FB_CS752X_CLCD_DEBUG */

#include "cs752x_clcdfb.h"

#define to_clcd(info)	container_of(info, struct clcd_fb, fb)

/* This is limited to 16 characters when displayed by X startup */
static char *cs752x_clcd_name = "cs752x_clcdfb";

#if defined(CONFIG_FB_PANEL_AT056TN53)
static
#endif
struct clcd_panel cs752x_panel = {
	.mode = {
		.name           = "640x480-VGA",/* Select InnoLux AT056TN53 */
		.refresh        = 60,		/* 1000000/(31.76 us *525 line) */
		.xres           = 640,
		.yres           = 480,
		.pixclock       = 39700,	/* PXLCLK clock time: 39.7 ns */
		.left_margin    = 134,		/* unit: pixel clk */
		.right_margin   = 16,		/* unit: pixel clk */
		.upper_margin   = 11,		/* unit: pixel line */
		.lower_margin   = 32,		/* unit: pixel line */
		.hsync_len      = 10,		/* unit: pixel clk */
		.vsync_len      = 2,		/* unit: pixel line */
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED,
	},
	.width  = 70,	/* in mm for hx8238a panel */
	.height = 52,	/* in mm for hx8238a panel */
#if defined(CONFIG_CORTINA_FPGA)
#if defined(CONFIG_FB_CS752X_CLCD_RGB888)
	.tim2   = 0x06,
	.cntl   = CNTL_LCDBPP24 | CNTL_LCDTFT | CNTL_LCDVCOMP(0) | CNTL_WATERMARK,
#endif  /* CONFIG_FB_CS752X_CLCD_RGB888 */
#if defined(CONFIG_FB_CS752X_CLCD_RGB565)
	.tim2   = 0x02,
	.cntl   = CNTL_LCDBPP16_565 | CNTL_LCDTFT | CNTL_LCDVCOMP(0) | CNTL_WATERMARK,
#endif  /* CONFIG_FB_CS752X_CLCD_RGB565 */
#endif  /* CONFIG_CORTINA_FPGA */
#if defined(CONFIG_CORTINA_ENGINEERING)
	.tim2   = 0x08000004, /* HX8238A pixel clock = 6.5MHz, LCD base clock = 250MHz */
		/* IPC: Invert panel clcok, 0=data is driven on LCD data lines on
		the RISing edge of CLCP, don't set TIM2_IPC. */
#if defined(CONFIG_FB_CS752X_CLCD_RGB888)
	.cntl   = CNTL_LCDBPP24 | CNTL_LCDTFT | CNTL_LCDVCOMP(0),
#endif  /* CONFIG_FB_CS752X_CLCD_RGB888 */
#if defined(CONFIG_FB_CS752X_CLCD_RGB565)
	.cntl   = CNTL_LCDBPP16_565 | CNTL_LCDTFT | CNTL_LCDVCOMP(0),
#endif  /* CONFIG_FB_CS752X_CLCD_RGB565 */
	  /* LCDBPP24: support 24 bits per pixel */
	  /* LCDTFT: TFT display. Don't use gray scaler */
	  /* BGR: red and blue DON'T swapped */
	  /* LCDVCOMP(0): generate interrupt at start of vertical synch */
#endif  /* CONFIG_CORTINA_ENGINEERING */
	.tim3   = 1,	/* Disable CLLE signal active */
#if defined(CONFIG_FB_CS752X_CLCD_RGB888)
	.bpp    = 32, /* bits per pixel, 24 bits for PL111 plus 8 dummy bits */
#endif
#if defined(CONFIG_FB_CS752X_CLCD_RGB565)
	.bpp    = 16, /* bits per pixel, 16 bits for PL111 */
#endif
};
#if defined(CONFIG_PANEL_HX8238A)
EXPORT_SYMBOL(cs752x_panel);
#endif	/* CONFIG_PANEL_HX8238A */

#if defined(CONFIG_PANEL_HX8238A)
extern void hx8238a_panel(struct fb_videomode *videomode);
#endif
#if defined(CONFIG_HDMI_VIDEO_ANX9805)
extern void anx9805_hdmi_panel(struct fb_videomode *videomode);
#endif

static struct clcd_panel *cs752x_clcd_panel(void)
{
	struct fb_videomode *videomode;
	struct clcd_panel *vga_panel = &cs752x_panel;

	videomode = &(vga_panel->mode);
#if defined(CONFIG_FB_PANEL_AT056TN53)
	vga_panel = &cs752x_panel;
#elif defined(CONFIG_PANEL_HX8238A)
	hx8238a_panel(videomode);
#elif defined(CONFIG_HDMI_VIDEO_ANX9805)
	anx9805_hdmi_panel(videomode);
#else
	vga_panel = NULL;
#endif
	return vga_panel;
}

static int cs752x_clcd_setup(struct clcd_fb *fb)
{
	size_t framesize, panel_size;
	dma_addr_t dma;
	u32 _framesize;

#if defined(CONFIG_FB_PANEL_AT056TN53)
	panel_size = VGA_640x480;
#elif defined(CONFIG_PANEL_HX8238A)
	panel_size = QVGA_320x240;
#else
	panel_size = VGA_640x480;
#endif

#if defined(CONFIG_FB_CS752X_CLCD_RGB565)
	framesize = panel_size * RBG565;
#else
	framesize = panel_size * RBG8888;
#endif
	fb->panel = cs752x_clcd_panel();

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
	_framesize = framesize << 1;
#else
	_framesize = framesize;
#endif
	fb->fb.screen_base = dma_alloc_writecombine(&fb->dev->dev, _framesize,
						&dma, GFP_KERNEL | GFP_DMA);

	if (!fb->fb.screen_base) {
		printk(KERN_ERR "CLCD: unable to map framebuffer\n");
		return -ENOMEM;
	}

	fb->fb.fix.smem_start = dma;
	fb->fb.fix.smem_len = framesize;

	return 0;
}

static int cs752x_clcd_mmap(struct clcd_fb *fb, struct vm_area_struct *vma)
{
	u32 _smem_len;	/* Length of frame buffer mem */
	int status;

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
	_smem_len = fb->fb.fix.smem_len << 1;
#else
	_smem_len = fb->fb.fix.smem_len;
#endif
	status = dma_mmap_writecombine(&fb->dev->dev, vma,
					fb->fb.screen_base,
					fb->fb.fix.smem_start,
					_smem_len);

	DBGPRINT(1, "<%s:%d>: status=0x%08x\n", __func__,__LINE__, status);
	return status;
}

static void cs752x_clcd_remove(struct clcd_fb *fb)
{
	u32 _smem_len;	/* Length of frame buffer mem */

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
	_smem_len = fb->fb.fix.smem_len << 1;
#else
	_smem_len = fb->fb.fix.smem_len;
#endif
	dma_free_writecombine( &fb->dev->dev,
				_smem_len,
				fb->fb.screen_base,
				fb->fb.fix.smem_start);
}

/*
 * Unfortunately, the enable/disable functions may be called either from
 * process or IRQ context, and we _need_ to delay.  This is _not_ good.
 */
static inline void clcdfb_msleep(unsigned int ms)
{
	if (in_atomic())
		mdelay(ms);
	else
		msleep(ms);
}

static irqreturn_t cs752x_clcd_irq(int irq, void *dev_id)
{
	unsigned int  lcd_int_pending;

	lcd_int_pending = readl(fb->regs + LCDMIS_REG);
	if (lcd_int_pending) {
#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
		if (lcd_int_pending & LNBUMIS) {/* LCD next base address update
						interrupt status bit */
			if (1 == g2_lcd_toggled) {
				g2_lcd_toggled = 0;
#if defined(CONFIG_FB_CS752X_CLCD_POLLING_FB)
				if (fb->fb.fix.smem_start == readl(fb->regs + LCDUPBASE_REG)) {
					writel(fb->fb.fix.smem_start + fb->fb.fix.smem_len,
						fb->regs + LCDUPBASE_REG);
			 	} else {
					writel(fb->fb.fix.smem_start,
						fb->regs + LCDUPBASE_REG);
				}

#else
				wake_up(&g2_lcd_wait_q);
#endif	/* CONFIG_FB_CS752X_CLCD_POLLING_FB */
			}
#if defined(CONFIG_FB_CS752X_CLCD_DEBUG)
			do_gettimeofday(&g2_now);
			g2_current[g2_timecnt] = g2_now.tv_usec - g2_old.tv_usec;
			g2_old.tv_usec = g2_now.tv_usec;
			g2_abstimecnt++;
			g2_timecnt = g2_abstimecnt % TO_MAXCNT;
			g2_current[g2_timecnt] = g2_timecnt;
#endif	/* CONFIG_FB_CS752X_CLCD_DEBUG */
		}
#endif	/* CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER */
		if (lcd_int_pending & FUFMIS) 	/* FIFO underflow, intr sts */
			printk("FIFO underflow\n");
	}
	writel(lcd_int_pending, fb->regs + LCDICR_REG); /* clear intr events */
	return IRQ_HANDLED;
}

static inline void cs752x_clcdfb_set_start(struct clcd_fb *fb)
{
	unsigned long ustart = fb->fb.fix.smem_start;
	unsigned long lstart;

	ustart += fb->fb.var.yoffset * fb->fb.fix.line_length;
	lstart = ustart + fb->fb.var.yres * fb->fb.fix.line_length / 2;

	writel(ustart, fb->regs + LCDUPBASE_REG);
	writel(lstart, fb->regs + LCDLPBASE_REG);
}

static void cs752x_clcdfb_enable(struct clcd_fb *fb, u32 cntl)
{
	g2_lcd_enable = 1;
	cntl |= CNTL_LCDEN;
	writel(cntl, fb->regs + LCDControl_REG);

	clcdfb_msleep(20);

	cntl |= CNTL_LCDPWR;
	writel(cntl, fb->regs + LCDControl_REG);
}

static void cs752x_clcdfb_disable(struct clcd_fb *fb)
{
	u32 val;

	g2_lcd_enable = 0;
	val = readl(fb->regs + LCDControl_REG);
	if (val & CNTL_LCDPWR) {
		val &= ~CNTL_LCDPWR;
		writel(val, fb->regs + LCDControl_REG);

		clcdfb_msleep(20);
	}
	if (val & CNTL_LCDEN) {
		val &= ~CNTL_LCDEN;
		writel(val, fb->regs + LCDControl_REG);
	}
}


static int cs752x_clcdfb_set_bitfields(struct clcd_fb *fb, struct fb_var_screeninfo *var)
{
	int ret = 0;

	memset(&var->transp, 0, sizeof(var->transp));

	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;

	switch (var->bits_per_pixel) {
	case 1:
	case 2:
	case 4:
	case 8:
		var->red.length	= var->bits_per_pixel;
		var->red.offset	= 0;
		var->green.length = var->bits_per_pixel;
		var->green.offset = 0;
		var->blue.length = var->bits_per_pixel;
		var->blue.offset = 0;
		break;
	case 16:
		var->red.length	= 5;
		var->blue.length = 5;
		/*
		 * Green length can be 5 or 6 depending whether
		 * we're operating in RGB555 or RGB565 mode.
		 */
		if (var->green.length != 5 && var->green.length != 6)
			var->green.length = 6;
		break;
	case 24:
	case 32:
		if (fb->panel->cntl & CNTL_LCDTFT) {
			var->red.length	= 8;
			var->green.length = 8;
			var->blue.length = 8;
			break;
		}
	default:
		printk("Invalid pixel color components setting\n");
		ret = -EINVAL;
		break;
	}

		DBGPRINT(1, "var->bits_per_pixel=0x%08x, ret=0x%08x\n", var->bits_per_pixel, ret);
	/*
	 * >= 16bpp displays have separate colour component bitfields
	 * encoded in the pixel data.  Calculate their position from
	 * the bitfield length defined above.
	 */
	if (ret == 0 && var->bits_per_pixel >= 16) {
		if (fb->panel->cntl & CNTL_BGR) {
			var->blue.offset = 0;
			var->green.offset = var->blue.offset + var->blue.length;
			var->red.offset = var->green.offset + var->green.length;
		} else {
			var->red.offset = 0;
			var->green.offset = var->red.offset + var->red.length;
			var->blue.offset = var->green.offset + var->green.length;
		}
	}
	return ret;
}

static int cs752x_clcdfb_open(struct fb_info *info, int user)
{
	struct clcd_fb *fb = to_clcd(info);
#if defined(CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF)
	u32 val;

	g2_lcd_enable = 1;
	val = readl(fb->regs + LCDControl_REG);
	val |= CNTL_LCDEN;
	writel(val, fb->regs + LCDControl_REG);	/* enable lcd */
#endif	/* CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF */

#if defined(CONFIG_FB_CS752X_CLCD_DEBUG)
	for(g2_timecnt=0; g2_timecnt<TO_MAXCNT; g2_timecnt++){
		g2_current[g2_timecnt] = g2_timecnt;
	}
	g2_abstimecnt = 0;
	g2_timecnt = 0;
#endif	/* CONFIG_FB_CS752X_CLCD_DEBUG */
	writel(LNBUIM, fb->regs + LCDIMSC_REG);	/* Enable FIFO underflow and next
						base address update interrupts */
	return 0;
}

static int cs752x_clcdfb_release(struct fb_info *info, int user)
{
	struct clcd_fb *fb = to_clcd(info);
#if defined(CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF)
	u32 val;

	g2_lcd_enable = 0;
	val = readl(fb->regs + LCDControl_REG);
	val &= ~CNTL_LCDEN;
	writel(val, fb->regs + LCDControl_REG);	/* disable lcd */
#endif	/* CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF */

	writel(0, fb->regs + LCDIMSC_REG);	/* Disable interrupts */
	return 0;
}

static int cs752x_clcdfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct clcd_fb *fb = to_clcd(info);
	int ret = -EINVAL;

	ret = clcdfb_check(fb, var);

	if (ret == 0 && var->xres_virtual * var->bits_per_pixel / 8 *
			var->yres_virtual > fb->fb.fix.smem_len) {
		DBGPRINT(1, "-EINVAL\n");
		DBGLEAVE(1);
		ret = -EINVAL;
	}

	if (ret == 0)
		ret = cs752x_clcdfb_set_bitfields(fb, var);

	if (ret)
		printk("<%s:%d> check framebuffer var ds fail\n", __func__, __LINE__);

	return ret;
}

static int cs752x_clcdfb_set_par(struct fb_info *info)
{
	struct clcd_fb *fb = to_clcd(info);
	struct clcd_regs regs;

	fb->fb.fix.line_length = fb->fb.var.xres_virtual *
				fb->fb.var.bits_per_pixel / 8;

	if (fb->fb.var.bits_per_pixel <= 8)
		fb->fb.fix.visual = FB_VISUAL_PSEUDOCOLOR;
	else
		fb->fb.fix.visual = FB_VISUAL_TRUECOLOR;

	cs752x_clcdfb_decode(fb, &regs);
	cs752x_clcdfb_disable(fb);

	writel(regs.tim0, fb->regs + LCDTiming0_REG);
	writel(regs.tim1, fb->regs + LCDTiming1_REG);
	writel(regs.tim2, fb->regs + LCDTiming2_REG);
	writel(regs.tim3, fb->regs + LCDTiming3_REG);

	cs752x_clcdfb_set_start(fb);
	fb->clcd_cntl = regs.cntl;
	cs752x_clcdfb_enable(fb, regs.cntl);

	DBGPRINT(1,
		"CLCD: Registers set to\n"
		"  %08x %08x %08x %08x\n"
		"  %08x %08x %08x %08x\n",
		readl(fb->regs + LCDTiming0_REG), readl(fb->regs+LCDTiming1_REG),
		readl(fb->regs + LCDTiming2_REG), readl(fb->regs+LCDTiming3_REG),
		readl(fb->regs + LCDUPBASE_REG),  readl(fb->regs+LCDLPBASE_REG),
		readl(fb->regs + LCDControl_REG), readl(fb->regs+LCDIMSC_REG));
#ifdef DEBUG
	printk(KERN_INFO
		"CLCD: Registers set to\n"
		"  %08x %08x %08x %08x\n"
		"  %08x %08x %08x %08x\n",
		readl(fb->regs + LCDTiming0_REG), readl(fb->regs+LCDTiming1_REG),
		readl(fb->regs + LCDTiming2_REG), readl(fb->regs+LCDTiming3_REG),
		readl(fb->regs + LCDUPBASE_REG),  readl(fb->regs+LCDLPBASE_REG),
		readl(fb->regs + LCDControl_REG), readl(fb->regs+LCDIMSC_REG));
#endif

	return 0;
}

static inline u32 convert_bitfield(int val, struct fb_bitfield *bf)
{
	unsigned int mask = (1 << bf->length) - 1;

	return (val >> (16 - bf->length) & mask) << bf->offset;
}

/*
 *  Set a single color register. The values supplied have a 16 bit
 *  magnitude.  Return != 0 for invalid regno.
 */
static int cs752x_clcdfb_setcolreg(unsigned int regno,
			unsigned int red, unsigned int green,
			unsigned int blue, unsigned int transp,
			struct fb_info *info)
{
	struct clcd_fb *fb = to_clcd(info);

	if (regno < 16)
		fb->cmap[regno] = convert_bitfield(transp, &fb->fb.var.transp)	|
				  convert_bitfield(blue, &fb->fb.var.blue)	|
				  convert_bitfield(green, &fb->fb.var.green)	|
				  convert_bitfield(red, &fb->fb.var.red);

	if (fb->fb.fix.visual == FB_VISUAL_PSEUDOCOLOR && regno < 256) {
		int hw_reg = CLCD_PALETTE + ((regno * 2) & ~3);
		u32 val, mask, newval;

		newval  = (red >> 11)  & 0x001f;
		newval |= (green >> 6) & 0x03e0;
		newval |= (blue >> 1)  & 0x7c00;

		/*
		 * 3.2.11: if we're configured for big endian
		 * byte order, the palette entries are swapped.
		 */
		if (fb->clcd_cntl & CNTL_BEBO)
			regno ^= 1;

		if (regno & 1) {
			newval <<= 16;
			mask = 0x0000ffff;
		} else {
			mask = 0xffff0000;
		}
		val = readl(fb->regs + hw_reg) & mask;
		writel(val | newval, fb->regs + hw_reg);
	}
	return regno > 255;
}

/*
 *  Blank the screen if blank_mode != 0, else unblank. If blank == NULL
 *  then the caller blanks by setting the CLUT (Color Look Up Table) to all
 *  black. Return 0 if blanking succeeded, != 0 if un-/blanking failed due
 *  to e.g. a video mode which doesn't support it. Implements VESA suspend
 *  and powerdown modes on hardware that supports disabling hsync/vsync:
 *    blank_mode == 2: suspend vsync
 *    blank_mode == 3: suspend hsync
 *    blank_mode == 4: powerdown
 */
static int cs752x_clcdfb_blank(int blank_mode, struct fb_info *info)
{
	struct clcd_fb *fb = to_clcd(info);

	DBGPRINT(1, "<%s:%d> start\n",__func__,__LINE__);
	if (blank_mode != 0)
		cs752x_clcdfb_disable(fb);
	else
		cs752x_clcdfb_enable(fb, fb->clcd_cntl);

	DBGPRINT(1, "<%s:%d> stop\n",__func__,__LINE__);
	return 0;
}

static int cs752x_clcdfb_mmap(struct fb_info *info,
		   struct vm_area_struct *vma)
{
	struct clcd_fb *fb = to_clcd(info);
	unsigned long len, off = vma->vm_pgoff << PAGE_SHIFT;
	int ret = -EINVAL;

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
	len = info->fix.smem_len << 1;	/* dual buffer */
#else
	len = info->fix.smem_len;	/* single buffer */
#endif
	if (off <= len && vma->vm_end - vma->vm_start <= len - off)
		ret = cs752x_clcd_mmap(fb, vma);

	return ret;
}

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
void cs752x_toggle_base_addr(struct fb_info *info,
			unsigned int flag, union iodata *io)
{
	struct clcd_fb *fb = to_clcd(info);

	if (G2_FB_GET_BASE_ADDR == flag) {
		io->regs.val = readl(fb->regs + LCDUPBASE_REG);
	} else if (G2_FB_TOGGLE_BASE_ADDR == flag) {
#if !defined(CONFIG_FB_CS752X_CLCD_POLLING_FB)
		if (fb->fb.fix.smem_start == readl(fb->regs + LCDUPBASE_REG)) {
			writel(fb->fb.fix.smem_start + fb->fb.fix.smem_len, fb->regs + LCDUPBASE_REG);	/*  Odd base address */
		} else {
			writel(fb->fb.fix.smem_start, fb->regs + LCDUPBASE_REG);				/* Even base address */
		}
#endif	/* CONFIG_FB_CS752X_CLCD_POLLING_FB */
	} else if (G2_FB_GET_TOGGLED == flag) {
		io->regs.val = g2_lcd_toggled;
	}
}

void cs752x_get_mask_toggle(struct fb_info *info, unsigned int flag,
		       union iodata *io)
{
	if (G2_FB_GET_MASK_TOGGLE == flag)
		io->regs.val = g2_lcd_mask_toggle;	/* Provide the status of this flag to application */
}
#endif

void cs752x_pl111_bits(struct fb_info *info, unsigned int flag, union iodata *io)
{
	struct clcd_fb *fb = to_clcd(info);
	unsigned int regvalue;

	regvalue = readl(fb->regs + io->regs.off);/* read value from register */
	DBGPRINT(1, "fb->regs=%p, io->regs.off=%x\n", fb->regs, io->regs.off);
	if (G2_FB_CLR_BITS == flag) 	/* Reset relative bits on given register */
		io->regs.val &= ~regvalue;

	if (G2_FB_SET_BITS == flag) 	/* Set relative bits on given register */
		io->regs.val |= regvalue;

	writel(io->regs.val, fb->regs + io->regs.off);	/* write value to reg */
	DBGPRINT(1, "fb->regs=%p, io->regs.val=%x\n",
		fb->regs, readl(fb->regs + io->regs.off));
}

void cs752x_pl111_reg(struct fb_info *info, unsigned int flag, union iodata *io)
{
	struct clcd_fb *fb = to_clcd(info);

	if (G2_FB_GET_REG == flag) {
		DBGPRINT(1, "G2_FB_GET_REG: fb->regs=%p, io->regs.off=%x\n",
			fb->regs, io->regs.off);
		io->regs.val = readl(fb->regs + io->regs.off);
	}
	if (G2_FB_SET_REG == flag) {
		DBGPRINT(1, "G2_FB_SET_REG: fb->regs=%p, io->regs.off=%x\n",
			fb->regs, io->regs.off);
		writel(io->regs.val, fb->regs + io->regs.off);
	}
}

#if defined(CONFIG_FB_CS752X_CLCD_AUTOPLAY)
void cs752x_pingpong_autoplay(unsigned long arg)
{
	struct timer_list *tick = (struct timer_list *)arg;
	int ret;

	ret = mod_timer(tick, jiffies + TIMEOUT_VALUE);

	if ((0 == g2_lcd_toggled) && (0 == g2_lcd_mask_toggle)) {
		g2_lcd_toggled = 1;
		DBGPRINT(1, "<%s:%d>need toggle\n",__func__, __LINE__);
	}
}
#endif	/* CONFIG_FB_CS752X_CLCD_AUTOPLAY */

int cs752x_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	int ret=0;
	u32 val;
	union iodata io;

	DBGENTER(1);
	if (_IOC_TYPE(cmd) != G2_FB_IOCTL_MAGIC) {
		DBGPRINT(1, "-ENOTTY 1\n");
		DBGLEAVE(1);
		return -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		if (copy_from_user(&io, (union iodata *)arg, sizeof(io))){
			DBGPRINT(1, "-EFAULT 1\n");
			DBGLEAVE(1);
			return -EFAULT;
		}
	}

	switch (cmd) {
#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
	case G2_FB_TOGGLE_BASE_ADDR:	/* Toggle framebuffer base address */
		DBGPRINT(1, "G2_FB_TOGGLE_BASE_ADDR\n");
		if (0 == g2_lcd_mask_toggle) {
			g2_lcd_toggled = 1;
		}
#if !defined(CONFIG_FB_CS752X_CLCD_POLLING_FB)
		cs752x_toggle_base_addr(info, G2_FB_TOGGLE_BASE_ADDR, &io);
		DBGPRINT(1, "go sleeping...\n");
	interruptible_sleep_on(&g2_lcd_wait_q);
#endif	/* CONFIG_FB_CS752X_CLCD_POLLING_FB */
		break;
	case G2_FB_GET_BASE_ADDR:	/* GET framebuffer base address */
		DBGPRINT(1, "G2_FB_GET_BASE_ADDR\n");
		cs752x_toggle_base_addr(info, G2_FB_GET_BASE_ADDR, &io);
		break;
	case G2_FB_GET_TOGGLED:	/* Get base address toggled */
		DBGPRINT(1, "G2_FB_GET_TOGGLED\n");
		cs752x_toggle_base_addr(info, G2_FB_GET_TOGGLED, &io);
		break;
	case G2_FB_MASK_TOGGLE:	/* Mask the toggle of ping-pong buffer
				   operation and change the pointer to
				   current framebuffer. */
		DBGPRINT(1, "G2_FB_MASK_TOGGLE\n");
		g2_lcd_mask_toggle = 1;
		break;
	case G2_FB_UNMASK_TOGGLE:
		DBGPRINT(1, "G2_FB_UNMASK_TOGGLE\n");
		g2_lcd_mask_toggle = 0;
		break;
	case G2_FB_GET_MASK_TOGGLE:
		DBGPRINT(1, "G2_FB_GET_MASK_TOGGLE\n");
		cs752x_get_mask_toggle(info, G2_FB_GET_MASK_TOGGLE, &io);
		break;
#endif	/* CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER */
	case G2_FB_SET_REG:	/* SET CLCD controller registers */
		DBGPRINT(1, "G2_FB_SET_REG\n");
		cs752x_pl111_reg(info, G2_FB_SET_REG, &io);
		break;
	case G2_FB_GET_REG:	/* GET CLCD controller registers */
		DBGPRINT(1, "G2_FB_GET_REG\n");
		cs752x_pl111_reg(info, G2_FB_GET_REG, &io);
		break;
	case G2_FB_SET_BITS:	/* Set relative bits on given register */
		DBGPRINT(1, "G2_FB_SET_BITS\n");
		cs752x_pl111_bits(info, G2_FB_SET_BITS, &io);
		break;
	case G2_FB_CLR_BITS:	/* Reset relative bits on given register */
		DBGPRINT(1, "G2_FB_CLR_BITS\n");
		cs752x_pl111_bits(info, G2_FB_CLR_BITS, &io);
		break;
#if defined(CONFIG_FB_CS752X_CLCD_AUTOPLAY)
	case G2_FB_SET_AUTOPLAY:	/* Set CLCD driver auto play pictures */
		DBGPRINT(1, "G2_FB_SET_AUTOPLAY\n");
		if (0 == g2_lcd_autoplay) {
			g2_lcd_autoplay = 1;
			init_timer(&fb_tick);
			fb_tick.function = cs752x_pingpong_autoplay;
			fb_tick.data = (unsigned long)&fb_tick;
			fb_tick.expires = jiffies + TIMEOUT_VALUE;
			add_timer(&fb_tick);
			printk("G2_FB_SET_AUTOPLAY\n");
		}
		break;
	case G2_FB_RESET_AUTOPLAY:
		DBGPRINT(1, "G2_FB_RESET_AUTOPLAY\n");
		if (1 == g2_lcd_autoplay) {
			g2_lcd_autoplay = 0;
			del_timer_sync(&fb_tick);
			printk("G2_FB_RESET_AUTOPLAY\n");
		}
		break;
#endif	/* CONFIG_FB_CS752X_CLCD_AUTOPLAY */
#ifndef CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF
	case G2_FB_TURNON:
		DBGPRINT(1, "G2_FB_TURNON\n");
		val = readl(fb->regs + LCDControl_REG);
		val |= CNTL_LCDEN;
		writel(val, fb->regs + LCDControl_REG);	/* enable lcd */
		break;
	case G2_FB_TURNOFF:
		DBGPRINT(1, "G2_FB_TURNOFF\n");
		val = readl(fb->regs + LCDControl_REG);
		val &= ~CNTL_LCDEN;
		writel(val, fb->regs + LCDControl_REG);	/* disable lcd */
		break;
#endif	/* CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF */
	default:
		DBGLEAVE(1);
		return -ENOTTY;
	}
	if (_IOC_DIR(cmd) & _IOC_READ) {
		if (copy_to_user((void __user *)arg, &io, sizeof(io))){
			DBGPRINT(1, "-EFAULT 2\n");
			DBGLEAVE(1);
			return -EFAULT;
		}
	}
	DBGLEAVE(1);
	return ret;
}


static struct fb_ops cs752x_clcdfb_ops = {
	.owner		= THIS_MODULE,
	.fb_open	= cs752x_clcdfb_open,
	.fb_release	= cs752x_clcdfb_release,
	.fb_check_var	= cs752x_clcdfb_check_var,
	.fb_set_par	= cs752x_clcdfb_set_par,
	.fb_setcolreg	= cs752x_clcdfb_setcolreg,
	.fb_blank	= cs752x_clcdfb_blank,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_mmap	= cs752x_clcdfb_mmap,
	.fb_ioctl       = cs752x_fb_ioctl,
};

static int cs752x_clcdfb_register(struct clcd_fb *fb)
{
	int ret;
	int status;

	fb->fb.fix.mmio_start	= fb->dev->res.start;
	fb->fb.fix.mmio_len	= resource_size(&fb->dev->res);

	fb->regs = ioremap(fb->fb.fix.mmio_start, fb->fb.fix.mmio_len);
	if (!fb->regs) {
		printk(KERN_ERR "CLCD: unable to remap registers\n");
		ret = -ENOMEM;
		goto out;
	}

	fb->fb.fbops		= &cs752x_clcdfb_ops;
	fb->fb.flags		= FBINFO_FLAG_DEFAULT;
	fb->fb.pseudo_palette	= fb->cmap;

	strncpy(fb->fb.fix.id, cs752x_clcd_name, sizeof(fb->fb.fix.id));
	fb->fb.fix.type		= FB_TYPE_PACKED_PIXELS;
	fb->fb.fix.type_aux	= 0;
	fb->fb.fix.xpanstep	= 0;
	fb->fb.fix.ypanstep	= 0;
	fb->fb.fix.ywrapstep	= 0;
	fb->fb.fix.accel	= FB_ACCEL_NONE;

	fb->fb.var.xoffset	= 0;
	fb->fb.var.yoffset	= 0;
	fb->fb.var.xres		= fb->panel->mode.xres;
	fb->fb.var.yres		= fb->panel->mode.yres;
	fb->fb.var.xres_virtual	= fb->panel->mode.xres;
	fb->fb.var.yres_virtual	= fb->panel->mode.yres;
	fb->fb.var.bits_per_pixel = fb->panel->bpp;
	fb->fb.var.grayscale	= fb->panel->grayscale;
	fb->fb.var.pixclock	= fb->panel->mode.pixclock;
	fb->fb.var.left_margin	= fb->panel->mode.left_margin;
	fb->fb.var.right_margin	= fb->panel->mode.right_margin;
	fb->fb.var.upper_margin	= fb->panel->mode.upper_margin;
	fb->fb.var.lower_margin	= fb->panel->mode.lower_margin;
	fb->fb.var.hsync_len	= fb->panel->mode.hsync_len;
	fb->fb.var.vsync_len	= fb->panel->mode.vsync_len;
	fb->fb.var.sync		= fb->panel->mode.sync;
	fb->fb.var.vmode	= fb->panel->mode.vmode;
	fb->fb.var.activate	= FB_ACTIVATE_FORCE | FB_ACTIVATE_NOW;
	fb->fb.var.nonstd	= 0;
	fb->fb.var.height	= fb->panel->height;
	fb->fb.var.width	= fb->panel->width;
	fb->fb.var.accel_flags	= 0;

	fb->fb.monspecs.hfmin	= 0;
	fb->fb.monspecs.hfmax   = 100000;
	fb->fb.monspecs.vfmin	= 0;
	fb->fb.monspecs.vfmax	= 400;
	fb->fb.monspecs.dclkmin = 1000000;
	fb->fb.monspecs.dclkmax	= 100000000;

	DBGPRINT(1, "<%s:%d> 10\n", __func__,__LINE__);

	cs752x_clcdfb_set_bitfields(fb, &fb->fb.var);

	DBGPRINT(1, "<%s:%d> 20\n", __func__,__LINE__);

	ret = fb_alloc_cmap(&fb->fb.cmap, 256, 0);
	if (ret)
		goto unmap;

	writel(0, fb->regs + LCDIMSC_REG);	/* disable interrupt */

	DBGPRINT(1, "<%s:%d> 30\n", __func__,__LINE__);
	fb_set_var(&fb->fb, &fb->fb.var);

	printk(KERN_INFO "CLCD: %s display\n", fb->panel->mode.name);

	DBGPRINT(1, "<%s:%d> 40\n", __func__,__LINE__);
	ret = register_framebuffer(&fb->fb);
	if (ret == 0) {
#ifndef CONFIG_CORTINA_FPGA
		status = readl(IO_ADDRESS(GLOBAL_GPIO_MUX_4));
		status &= ~0x1FFFFFFF;	/* GPIO 4 Bit[28:0] */
		writel(status, IO_ADDRESS(GLOBAL_GPIO_MUX_4));
#endif
		goto out;
	}

	printk(KERN_ERR "CLCD: cannot register framebuffer (%d)\n", ret);
	fb_dealloc_cmap(&fb->fb.cmap);
 unmap:
	iounmap(fb->regs);
 out:
	return ret;
}

static int cs752x_clcdfb_probe(struct amba_device *dev, struct amba_id *id)
{
	int ret = 0;
	u32 val = 0;

	ret = amba_request_regions(dev, NULL);
	if (ret) {
		printk(KERN_ERR "CLCD: unable to reserve regs region\n");
		goto out;
	}

	fb = kzalloc(sizeof(struct clcd_fb), GFP_KERNEL);
	if (!fb) {
		printk(KERN_INFO "CLCD: could not allocate new clcd_fb struct\n");
		ret = -ENOMEM;
		goto free_region;
	}

	fb->dev = dev;

	ret = request_irq(dev->irq[0], cs752x_clcd_irq,
			IRQF_DISABLED, cs752x_clcd_name, &dev);

	ret = cs752x_clcd_setup(fb);
	if (ret)
		goto free_fb;

#if defined(CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER)
#if !defined(CONFIG_FB_CS752X_CLCD_POLLING_FB)
	init_waitqueue_head(&g2_lcd_wait_q);
#endif	/* CONFIG_FB_CS752X_CLCD_POLLING_FB */
	g2_lcd_toggled = 0;
	g2_lcd_mask_toggle = 0;
#endif	/* CONFIG_FB_CS752X_CLCD_PINGPONG_BUFFER */
#if defined(CONFIG_FB_CS752X_CLCD_AUTOPLAY)
	g2_lcd_autoplay = 0;
#endif	/* CONFIG_FB_CS752X_CLCD_AUTOPLAY */

	ret = cs752x_clcdfb_register(fb);
	if (ret == 0) {
		amba_set_drvdata(dev, fb);
		goto out;
	}

	cs752x_clcd_remove(fb);
 free_fb:
	kfree(fb);
 free_region:
	amba_release_regions(dev);
 out:
	cs752x_clcd_dbg(dev, cs752x_clcd_dbg_info, "%s: G2 color lcd driver register done.\n", __func__);
#if defined(CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF)
	val = readl(fb->regs + LCDControl_REG);
	val &= ~CNTL_LCDEN;
	writel(val, fb->regs + LCDControl_REG);
#endif	/* CONFIG_FB_CS752X_CLCD_AUTO_TURNOFF */
	return ret;
}

static int cs752x_clcdfb_remove(struct amba_device *dev)
{
	struct clcd_fb *fb = amba_get_drvdata(dev);

	DBGENTER(1);
	DBGPRINT(1, "<%s:%d> start\n",__func__,__LINE__);
	amba_set_drvdata(dev, NULL);

	cs752x_clcdfb_disable(fb);
	unregister_framebuffer(&fb->fb);
	if (fb->fb.cmap.len)
		fb_dealloc_cmap(&fb->fb.cmap);
	iounmap(fb->regs);
	cs752x_clcd_remove(fb);
	kfree(fb);
	amba_release_regions(dev);

	cs752x_clcd_dbg(dev, cs752x_clcd_dbg_info, "%s: G2 color lcd driver unregister done.\n", __func__);
	DBGLEAVE(1);
	DBGPRINT(1, "<%s:%d> stop\n",__func__,__LINE__);
	return 0;
}

static struct amba_id cs752x_clcdfb_id_table[] = {
	{
		.id	= 0x00041111,	/* PL111 lcd controller */
		.mask	= 0x000fffff,
	},
	{ 0, 0 },
};

static struct amba_driver cs752x_clcd_driver = {
	.drv 		= {
		.name	= "dev:clcd",
	},
	.probe		= cs752x_clcdfb_probe,
	.remove		= cs752x_clcdfb_remove,
	.id_table	= cs752x_clcdfb_id_table,
};

static int __init cs752x_clcdfb_init(void)
{
	int status;

	if (fb_get_options(cs752x_clcd_name, NULL))
		return -ENODEV;

	status = amba_driver_register(&cs752x_clcd_driver);
	g2_lcd_enable = 0;

	return status;
}

module_init(cs752x_clcdfb_init);

static void __exit cs752x_clcdfb_exit(void)
{
	int status;

#ifndef CONFIG_CORTINA_FPGA
	status = readl(IO_ADDRESS(GLOBAL_GPIO_MUX_4));
	status |= 0x1FFFFFFF;	/* GPIO 4 Bit[28:0] */
	writel(status, IO_ADDRESS(GLOBAL_GPIO_MUX_4));
#endif
	amba_driver_unregister(&cs752x_clcd_driver);
}

module_exit(cs752x_clcdfb_exit);

MODULE_DESCRIPTION("Cortina CS75XX LCD core driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe Hsu <Joe.Hsu@cortina-systems.com>");
