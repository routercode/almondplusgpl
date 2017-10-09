/*
 *  linux/drivers/video/anx9805_hdmi.c
 *
 * Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
 *                Joe Hsu <joe.hsu@cortina-systems.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 *  Reference:
 *	sound/oss/dac.c
 *	drivers/staging/msm/hdmi_sii9022.c
 *      sound/aoa/codecs/onyx.c
 *      .../rtc/rtc-m41t80.c
 *      drivers/net/sfc/falcon_boards.c
 *      drivers/misc/eeprom/at24.c
 *      drivers/hwmon/asb100.c
 *      drivers/video/sh_mobile_hdmi.c
 *      drivers/video/fbmon.c
 *      drivers/gpu/drm/drm_edid.c
 *
 *  ANX9805 HDMI Transmitter
 */
#include <linux/init.h>	
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/gpio.h>	
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/fb.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>

#include "anx9805_hdmi.h"
#include "edid.h"

static struct anx9805_info *pANX9805;
#ifdef CONFIG_HDMI_ANX9805_DEBUG
static int dbg_indent;
static int dbg_cnt;
static spinlock_t dbg_spinlock = SPIN_LOCK_UNLOCKED;
#endif


#if defined(CONFIG_HDMI_VIDEO_ANX9805)
void anx9805_hdmi_panel(struct fb_videomode *videomode)
{
	DBGPRINT(1, "<%s:%d> enter\n", __func__, __LINE__);
	/* mode */
	videomode->name = "anx9805-hdmi";
	videomode->refresh = 60;
	videomode->xres = 640;
	videomode->yres = 480;
	videomode->pixclock = 39700;
	videomode->left_margin = 134;
	videomode->right_margin = 16;
	videomode->upper_margin = 11;
	videomode->lower_margin = 32;
	videomode->hsync_len = 10;
	videomode->vsync_len = 2;
	videomode->sync = 0;
	videomode->vmode = FB_VMODE_NONINTERLACED;

	DBGPRINT(1, "<%s:%d> exit\n", __func__, __LINE__);
}
EXPORT_SYMBOL(anx9805_hdmi_panel);
#endif

static inline void anx9805_msleep(unsigned int ms)
{
	//if (in_atomic())
		mdelay(ms);
	//else
	//	msleep(ms);
}

static int anx9805_i2c_read(struct i2c_client *i2c_cli, u8 reg, u8 *value_p)
{
	struct i2c_msg msg[2] = {
		{
			.addr = i2c_cli->addr,
			.flags = 0,
			.buf = &reg,
			.len = 1,
		},
		{
			.addr = i2c_cli->addr,
			.flags = I2C_M_RD,
			.buf = value_p,
			.len = 1,
		}
	};

	return i2c_transfer(i2c_cli->adapter, msg, 2);
}

static int anx9805_i2c_write(struct i2c_client *i2c_cli, u8 reg, u8 value)
{
	struct i2c_msg msg;
	unsigned char buf[2];

	buf[0] = reg;
	buf[1] = value;

	msg.addr = i2c_cli->addr;
	msg.buf = buf;
	msg.len = 2;
	msg.flags = 0;

	return i2c_transfer(i2c_cli->adapter, &msg, 1);
}

static void anx9805_reg(struct anx9805_info *anx9805, unsigned int flag,
			  union iodata *io)
{
	u8 regvalue;
	int result;
	struct i2c_client *i2client;

	if ((ANX9805_HDMI_GET_REG == flag) || (ANX9805_HDMI_SET_REG == flag)) {
		i2client = anx9805->cli_hdmi;
	} else if ((ANX9805_DP_GET_REG == flag) || (ANX9805_DP_SET_REG == flag)) {
		i2client = anx9805->cli_dp;
	} else {
		i2client = anx9805->cli_sys;
	}

	if ((ANX9805_HDMI_GET_REG == flag) || (ANX9805_SYS_GET_REG == flag) || (ANX9805_DP_GET_REG == flag)) {
		// TODO: reg offset need boundary check!
		result = anx9805_i2c_read( i2client, io->regs.off, &regvalue);
		if (result < 0)
			printk("<%s:%d> err=%d\n", __func__, __LINE__, result);
		io->regs.val = regvalue;
	}

	if ((ANX9805_HDMI_SET_REG == flag) || (ANX9805_SYS_SET_REG == flag) || (ANX9805_DP_SET_REG == flag)) {
		regvalue = io->regs.val;
		result = anx9805_i2c_write( i2client, io->regs.off, regvalue);
		if (result < 0)
			printk("<%s:%d> err=%d\n", __func__, __LINE__, result);
	}

	if (anx9805->delay) {
		anx9805_msleep(anx9805->delay);
	}
}

static int anx9805_i2c_rreg(struct anx9805_info *anx9805, u32 flag, u8 regoff, u8 *regval)
{
	union iodata io;

	io.regs.off = regoff;
	io.regs.val = *regval;

	anx9805_reg(anx9805, flag, &io);

	*regval = io.regs.val;

	return 0;
}

static int anx9805_i2c_wreg(struct anx9805_info *anx9805, u32 flag, u8 regoff, u8 regval)
{
	union iodata io;

	io.regs.off = regoff;
	io.regs.val = regval;

	anx9805_reg(anx9805, flag, &io);

	return 0;
}

static void anx9805_bits(struct anx9805_info *anx9805, unsigned int flag,
			  union iodata *io)
{
	u8 regvalue;
	int result;
	struct i2c_client *i2client;

	if ((ANX9805_HDMI_CLR_BITS == flag) || (ANX9805_HDMI_SET_BITS == flag)) {
		i2client = anx9805->cli_hdmi;
	} else if ((ANX9805_DP_CLR_BITS == flag) || (ANX9805_DP_SET_BITS == flag)) {
		i2client = anx9805->cli_dp;
	} else {
		i2client = anx9805->cli_sys;
	}

	// TODO: reg offset need boundary check!
	result = anx9805_i2c_read( i2client, io->regs.off, &regvalue);	
	if (result < 0)
		printk("<%s:%d> err=%d\n", __func__, __LINE__, result);

	if (anx9805->delay) {
		anx9805_msleep(anx9805->delay);
	}

	if ((ANX9805_HDMI_CLR_BITS == flag) || (ANX9805_SYS_CLR_BITS == flag) || (ANX9805_DP_CLR_BITS == flag))
		regvalue &= ~io->regs.val;

	if ((ANX9805_HDMI_SET_BITS == flag) || (ANX9805_SYS_SET_BITS == flag) || (ANX9805_DP_SET_BITS == flag))
		regvalue |= io->regs.val;

	result = anx9805_i2c_write( i2client, io->regs.off, regvalue);
	if (result < 0)
		printk("<%s:%d> err=%d\n", __func__, __LINE__, result);

	if (anx9805->delay) {
		anx9805_msleep(anx9805->delay);
	}
}

static int anx9805_i2c_bits(struct anx9805_info *anx9805, u32 flag, u8 regoff, u8 regval)
{
	union iodata io;

	io.regs.off = regoff;
	io.regs.val = regval;

	anx9805_bits(anx9805, flag, &io);

	return 0;
}

static void hdmi_tx_video_enable(struct anx9805_info *anx9805)
{
	u8 value, cnt;

	anx9805_i2c_read( anx9805->cli_sys, VIDEO_CTRL1_REG, &value);	/* 72:08 */
	anx9805_i2c_write( anx9805->cli_sys, VIDEO_CTRL1_REG, value | VIDEO_EN);/* 72:08.7 */

	/* only support color8 */
	for (cnt=0; cnt<24; cnt++) {
		anx9805_i2c_write( anx9805->cli_sys, 0x40 + cnt, 0 + cnt);	/* 72:40-57 */
	}

	/* delay */
	/* msleep(10); */
	anx9805_i2c_write( anx9805->cli_sys, VIDEO_CTRL3_REG, 0x00);	/* 72:0a RGB mode */
}

#if 0
static void hdmi_tx_video_disable(struct anx9805_info *anx9805)
{
	u8 value;

	anx9805_i2c_read( anx9805->cli_sys, VIDEO_CTRL1_REG, &value);	
	value &= ~VIDEO_EN;
	anx9805_i2c_write(anx9805->cli_sys, VIDEO_CTRL1_REG, value);	/* 72:08.7 */
}
#endif

#if 0
static void
hdmi_tx_pwr_on(void)
{
	u8 value;

	anx9805_i2c_read( anx9805->cli_sys, SYS_PD_REG, &value);		/* 72:05 */
	value &= ~PD_TOTAL;
	anx9805_i2c_write(anx9805->cli_sys, SYS_PD_REG, value);
}
#endif

static int anx9805_i2c_client(struct anx9805_info *anx9805)
{
	struct i2c_client  *clisys = anx9805->cli_sys;
	struct i2c_adapter *adapter= clisys->adapter;

	/* anx9805 hdmi client */
	anx9805->cli_hdmi = i2c_new_dummy(adapter, ANX9805_HDMI_D0_ADDR);
	if (!anx9805->cli_hdmi) {
		printk("can't add anx9805 hdmi client at device address 0x%x\n", ANX9805_HDMI_D0_ADDR);
		return -EPERM;
	}

	/* anx9805 display port client */
	anx9805->cli_dp = i2c_new_dummy(adapter, ANX9805_DP_D0_ADDR);
	if (!anx9805->cli_dp) {
		printk("can't add anx9805 display port client at device address 0x%x\n", ANX9805_DP_D0_ADDR);
		return -EPERM;
	}

	return 0;
}

#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
static int anx9805_gpio_init(void)
{
	/* INT - GPIO group 2, bit 27 */
	if (gpio_request(ANX9805_GPIO_INT, "ANX9805_INT")) {
		printk("<%s:%d> Can't reserve GPIO %d for anx9805 interrupt\n", __func__, __LINE__, ANX9805_GPIO_INT);	// 36
		return -ENODEV;
	}
	gpio_direction_input(ANX9805_GPIO_INT);

	return 0;
}

static void anx9805_gpio_release(void)
{
	gpio_free(ANX9805_GPIO_INT);
}
#endif

static int anx9805_edid_cs_check(u8 *edid)
{
	u8 csum = 0, all_null = 0;
	int i, err = 0;

	for (i = 0; i < EDID_LENGTH; i++) {
	        csum += edid[i];
	        all_null |= edid[i];
	}

	if (csum == 0x00 && all_null) {
	        /* checksum passed, everything's good */
	        err = 1;
	} else {
		printk("edid fail checksum=0x%02x\n", csum);
	}

	return err;
}

static int anx9805_edid_check_header(u8 *edid)
{
	int i, err = 1;

	for (i = 0; i < 8; i++) {
		if (edid[i] != edid_v1_header[i])
			err = 0;
	}

	return err;
}

static int anx9805_parse_edid(struct anx9805_info *anx9805, u8 blk)
{
	u8 *edidbuf = NULL;
	u32 error = 0, blkbase = (u32)(blk-1) << 7;

	if (0 == blk) {	/* block zero */
		edidbuf = anx9805->edid.blkzero;
	} else {	/* extension block */
		edidbuf = anx9805->edid.extblk + blkbase;
	}

	if (edidbuf == NULL) {
		printk("<%s:%d> edid is null\n", __func__, __LINE__);
		error = -1;
		goto parse_edid_done;
	}
	if (!(anx9805_edid_cs_check(edidbuf))) {
		printk("<%s:%d> edid checksum fail\n", __func__, __LINE__);
		error = -1;
		goto parse_edid_done;
	}
	if (0 == blk) {
		if (!(anx9805_edid_check_header(edidbuf))) {
			printk("<%s:%d> edid check header fail\n", __func__, __LINE__);
			error = -1;
			goto parse_edid_done;
		}
	}
	printk("<%s:%d> edid (blk=%02d, edidbuf=%p, extblk=%p, blkbase=%04x) passed\n", __func__, __LINE__, blk, edidbuf, anx9805->edid.extblk, blkbase);

parse_edid_done:
	return error;
}

static int anx9805_dump_edid(struct anx9805_info *anx9805, u8 blk)
{
	u8 *edidbuf = NULL;
	u8 blkbase = (u32)(blk-1) << 7; 
	u32 cntx, cnty;

	if (0 == blk) {	/* block zero */
		edidbuf = anx9805->edid.blkzero;
	} else {	/* extension block */
		edidbuf = anx9805->edid.extblk + blkbase;
	}
	for (cntx=0; cntx<8; cntx++) {
		for (cnty=0; cnty<16; cnty++) {
			printk("%02x ", edidbuf[(cntx<<4)+cnty]);
		}
		printk("\n");
	}

	return 0;
}

static u32 ddc_edid_read128(struct anx9805_info *anx9805, u8 edidblk, u8 *edidbuf)
{
	u8  edid_len=EDID_LENGTH, edid_cnt=0, ddc_val=0, bytes_in_fifo=0, cnt, edidseg = edidblk >> 1;
	u32 timeout=100, error=0;

	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_SEG_ADDR_REG, edidseg);
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_OFFSET_ADDR_REG, edidblk << 7);
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_NUM0_REG, 0x80);	/* fixed to 0x80 bytes */
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_NUM1_REG, 0x00);
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, SEQ_BYTE_READ);

	while (edid_len) {
		/* checking DDC FIFO status */
		anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_CH_STATUS_REG, &ddc_val);
		if (ddc_val & DDC_ERROR) {
			printk("<%s:%d> ddc operation error, reg[%02x]=0x%02x\n", __func__, __LINE__, DDC_CH_STATUS_REG, ddc_val);
			error = -1;
			break;
		}
		if (ddc_val & DDC_OCCUPY) {
			printk("<%s:%d> ddc i2c bus is occupied by other device, reg[%02x]=0x%02x\n", __func__, __LINE__, DDC_CH_STATUS_REG, ddc_val);
			error = -1;
			break;
		}
		if (ddc_val & DDC_FIFO_FULL) {
			anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_CNT_REG, &bytes_in_fifo);
			bytes_in_fifo &= DDC_FIFO_MAX_CNT;	/* max: 31 bytes */
			edid_len -= bytes_in_fifo;

			/* accessing the FIFO data */
			printk("<%s:%d> edid_len=%02x\n", __func__, __LINE__, edid_len);
			for (cnt=0; cnt<bytes_in_fifo; cnt++) {
				anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_DATA_REG, &ddc_val);
				edidbuf[edid_cnt] = ddc_val;
				edid_cnt++;
			}
			continue;
		}
		if (ddc_val & DDC_PROGRESS) {
			timeout--;
			if (timeout) {
				continue;
			} else {
				printk("<%s:%d> ddc operation timeout, reg[%02x]=0x%02x\n", __func__, __LINE__, DDC_CH_STATUS_REG, ddc_val);
				error = -1;
				break;
			}
		} else {
			/* accessing the FIFO data */
			anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_CNT_REG, &bytes_in_fifo);
			bytes_in_fifo &= DDC_FIFO_MAX_CNT;	/* max: 31 bytes */
			edid_len -= bytes_in_fifo;

			/* accessing the FIFO data */
			printk("<%s:%d> edid_len=%02x\n", __func__, __LINE__, edid_len);
			for (cnt=0; cnt<bytes_in_fifo; cnt++) {
				anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_DATA_REG, &ddc_val);
				edidbuf[edid_cnt] = ddc_val;
				edid_cnt++;
			}
		}
	}
	return error;
}

static u8 ddc_i2c_cmd(struct anx9805_info *anx9805, int ddcmd, u8 edidblk, u8 edidoff, u8 *edidval)
{
	u8  value=0, edidseg, offset;
	u32 timeout, error=0;

	switch (ddcmd) {
	case SINGLE_BYTE_READ:	/* single edid byte read */
		edidseg = edidblk >> 1;
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_SEG_ADDR_REG, edidseg);
		offset = (edidblk << 7) + (edidoff & 0x7f);
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_OFFSET_ADDR_REG, offset);
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_NUM0_REG, 1);	/* single byte */
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, SEQ_BYTE_READ);
		timeout=100;
		do{
			anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_CH_STATUS_REG, &value);
			if (!(value & DDC_PROGRESS)) {
				break;
			}
		}while(timeout--);
		if (0 == timeout) {
			printk("<%s:%d> ddc operation timeout\n", __func__, __LINE__);
			error = -1;
		} else {
			anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_DATA_REG, edidval);
		}
		break;
	case CLR_DDC_DFIFO:	/* Clear DDC Data FIFO */
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, CLR_DDC_DFIFO);
		break;
#if 0
	case SEQ_BYTE_WRITE:	/* Sequential Byte Write, reserved */
		break;
	case DDC_I2C_RESET:	/* I2C reset command */
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, DDC_I2C_RESET);
		break;
	case DDC_ABORT_OP:	/* Abort current operation */
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, DDC_ABORT_OP);
		break;
	case ESEQ_BYTE_READ:	/* Enhanced DDC Sequential Read, reserved */
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_OFFSET_ADDR_REG, anx9805->ddc.reg.off);
		anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_CMD_REG, ESEQ_BYTE_READ);
		anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, DDC_FIFO_DATA_REG, &value);
	case DDC_HDCP_IOAR:	/* Implicit Offset Address Read (HDCP), reserved */
#endif
	default:
		printk("<%s:%d> unsupported operation\n", __func__, __LINE__);
		break;
	}

	return error;
}

static void hdmi_dump_reg(struct anx9805_info *anx9805, int i2cdev, int base, int len)
{
	u8 value;
	u32 cnt;

	printk("\n");

	for (cnt=base; cnt<(base+len); cnt++) {
		if (ANX9805_SYSTEM_D0_ADDR == i2cdev)
			anx9805_i2c_rreg(anx9805, ANX9805_SYS_GET_REG, cnt, &value);
		if (ANX9805_DP_D0_ADDR == i2cdev)
			anx9805_i2c_rreg(anx9805, ANX9805_DP_GET_REG, cnt, &value);
		if (ANX9805_HDMI_D0_ADDR == i2cdev)
			anx9805_i2c_rreg(anx9805, ANX9805_HDMI_GET_REG, cnt, &value);
		printk("anx9805 cli_hdmi_reg (%x) %02x: 0x%08x\n", i2cdev, cnt, value);
	}
}

static void anx9805_read_edid(struct anx9805_info *anx9805)
{
	u8 *edidbuf = NULL, edidblk, blkcnt;
	u32 error;

	/* force HDMI/DP hotplug during EDID reading to avoid glitch on HPD line. */
	anx9805_i2c_bits(anx9805, ANX9805_HDMI_SET_BITS, HDMI_CHIP_DCTRL1_REG, HDMI_FORCE_HOTPLUG);
	anx9805_i2c_bits(anx9805, ANX9805_DP_SET_BITS, DP_SCTL3_REG, F_HPD | HPD_CTRL);

	/* Init DDC Read channel: clear FIFO */
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_DEV_ADDR_REG, 0xa0);	/* assign i2c device address */
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG, DDC_ACCESS_NUM1_REG, 0);	/* always zero */
	ddc_i2c_cmd(anx9805, CLR_DDC_DFIFO, 0, 0, NULL);

	/* read and check edid block zero */
	edidbuf = anx9805->edid.blkzero;
	ddc_edid_read128(anx9805, 0, edidbuf);
	error = anx9805_parse_edid(anx9805, 0);
	if (error) {
		printk("<%s:%d> anx9805 edid checksum fail\n", __func__, __LINE__);
		goto edid_done;
	} else {
		anx9805_dump_edid(anx9805, 0);
	}

	/* read and determine the extension edid blocks */
	ddc_i2c_cmd(anx9805, SINGLE_BYTE_READ, 0, 0x7e, &edidblk);
	edidbuf = kzalloc(EDID_LENGTH * edidblk, GFP_KERNEL);
	if (!edidbuf) {
		printk("<%s:%d> anx9805 kzalloc failed\n", __func__, __LINE__);
		goto edid_done;
	}
	anx9805->edid.extblk = edidbuf;

	/* read and parse the extension edid blocks */
	for (blkcnt=1; blkcnt<=edidblk; blkcnt++) {
		edidbuf = anx9805->edid.extblk + ((blkcnt-1)<<7);
		ddc_edid_read128(anx9805, blkcnt, edidbuf);
		if (!anx9805_parse_edid(anx9805, blkcnt))
			anx9805_dump_edid(anx9805, blkcnt);
	}

	kfree(anx9805->edid.extblk);
	anx9805->edid.extblk = NULL;

edid_done:
	/* unforce HDMI/DP hotplyg detection */
	anx9805_i2c_bits(anx9805, ANX9805_HDMI_CLR_BITS, HDMI_CHIP_DCTRL1_REG, HDMI_FORCE_HOTPLUG);
	anx9805_i2c_bits(anx9805, ANX9805_DP_CLR_BITS, DP_SCTL3_REG, F_HPD | HPD_CTRL);
}

#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
static void anx9805_hdmi_work(struct work_struct *hdmi_work)
{
	struct anx9805_info *anx9805 = container_of(hdmi_work, struct anx9805_info, work);
	u8 value;
	u32 cnt;

	printk("<%s:%d>anx9805 hdmi workqueue, anx9805=%p\n", __func__, __LINE__, anx9805);

	for (cnt=0; cnt<8; cnt++) {
		anx9805_i2c_read( anx9805->cli_sys, INT_STATE_REG + cnt, &value);		/* 72:f0-f7 */
		anx9805->int_status[cnt] = (u32)value;
		printk("<%s:%d> hdmi interrupt status: [f0+%d]=0x%08x\n", __func__, __LINE__, cnt, anx9805->int_status[cnt]);
		anx9805_i2c_write(anx9805->cli_sys, INT_STATE_REG + cnt, value);
	}
}

static irqreturn_t anx9805_hdmi_irq(int irq, void *dev_id)
{
	struct anx9805_info *anx9805 = (struct anx9805_info *)dev_id;
	u32 cnt;

	printk("<%s:%d>anx9805 hdmi interrupt happened, anx9805=%p\n", __func__, __LINE__, anx9805);

	for (cnt=0; cnt<8; cnt++) {
		anx9805->int_status[cnt] = 0;
	}

	schedule_work(&anx9805->work);

	return IRQ_HANDLED;
}
#endif

static void anx9805_i2c_client_remove(struct anx9805_info *anx9805)
{
	if (anx9805->cli_hdmi)
		i2c_unregister_device(anx9805->cli_hdmi);
}

static u8 hdmi_tx_infoframe_checksum(struct anx9805_infoframe *p)
{
	u8 cnt, checksum = 0;

	checksum = p->type + p->length + p->version;
	for (cnt=1; cnt<=p->length; cnt++) {
		checksum += p->pb_byte[cnt];
	}
	checksum = ~checksum;
	checksum += 0x01;

	return checksum;
}

static void hdmi_tx_load_infoframe(struct anx9805_info *anx9805, ANX9805_HDMI_PACKET_TYPE packet, struct anx9805_infoframe *p)
{
	u8	addr[2] = {0x70, 0x83};	/* avi, audio, spd, mpeg, deleted */
	u8	cnt;

	/* head */
	anx9805_i2c_write(anx9805->cli_sys, addr[packet], p->type);	/* 72:InfoFrame */
	anx9805_i2c_write(anx9805->cli_sys, addr[packet]+1, p->version);/* 72:InfoFrame+1 */
	anx9805_i2c_write(anx9805->cli_sys, addr[packet]+2, p->length);	/* 72:InfoFrame+2 */

	/* checksum */
	p->pb_byte[0] = hdmi_tx_infoframe_checksum(p);

	/* data field */
	for (cnt=0; cnt <= p->length; cnt++) {
		anx9805_i2c_write(anx9805->cli_sys, addr[packet]+3+cnt, p->pb_byte[cnt]);/* 72:InfoFrame+datafield */
	}
}

#if defined(CONFIG_FB_CS752X_CLCD)
/* avi infoframe variables configuration */
static void hdmi_tx_avi_set_vars(struct anx9805_info *anx9805)
{
	//u32 cnt;

	/* Variable settings */
	anx9805->packets_need_config |= HDMI_TX_SEL_AVI; /* avi infoframes */
	/* AVI InfoFrame Packet Header, hdmi 8.2.1 */
	anx9805->avi_info.type	= 0x82;
	anx9805->avi_info.version	= 0x02;
	anx9805->avi_info.length	= 0x0d;

	/* Clear AVI InfoFrame packet contents */
	memset(&(anx9805->avi_info.pb_byte[1]), 0x00, anx9805->avi_info.length);
	anx9805->avi_info.pb_byte[1]= 0x01;
		/* S1S0=0x1: composed for an overscanned display, where some
		   active pixels and lines at the edges are not displayed. */
		/* B1B0=0x0: Bar Data not valid */
		/* A0=0: Active Format No Data */
		/* Y1Y0=0x0: RGB: default */
	anx9805->avi_info.pb_byte[2]= 0x08;
		/* R3R2R1R0=0x8: same as picture aspect ratio */
		/* M1M0=0x0: Picture Aspect Ratio: No Data */
		/* C1C0=0x0: Colorimetry: No Data */
}
#endif

#if defined(CONFIG_SOUND_CS75XX_SPDIF)
/* audio infoframe variables configuration */
static void hdmi_tx_audio_set_vars(struct anx9805_info *anx9805)
{
	u32 cnt;

	/* Variable settings */
	anx9805->packets_need_config |= HDMI_TX_SEL_AUDIO;/* audio infoframes */
	/* Audio InfoFrame Packet Header, hdmi 8.2.2 */
	anx9805->audio_info.type	= 0x84;
	anx9805->audio_info.version	= 0x01;
	anx9805->audio_info.length	= 0x0a;

	/* Clear Audio InfoFrame packet contents */
	memset(&(anx9805->audio_info.pb_byte[1]), 0x00, anx9805->audio_info.length);
}
#endif

static void hdmi_tx_set_state(struct anx9805_info *anx9805, u32 nextstate)
{
	switch (nextstate) {
	case HDMI_INITIAL:
		anx9805->state = HDMI_INITIAL;
		break;
	case HDMI_WAIT_HOTPLUG:
		anx9805->state = HDMI_WAIT_HOTPLUG;
		break;
	case HDMI_PARSE_EDID:
		anx9805->state = HDMI_PARSE_EDID;
		break;
	case HDMI_LINK_TRAINING:
		anx9805->state = HDMI_LINK_TRAINING;
		break;
	case HDMI_CONFIG_VIDEO:
		anx9805->state = HDMI_CONFIG_VIDEO;
		break;
	case HDMI_HDCP_AUTH:
		anx9805->state = HDMI_HDCP_AUTH;
		break;
	case HDMI_PLAY_BACK:
		anx9805->state = HDMI_PLAY_BACK;
		break;
	default:
		break;
	};
}

static void hdmi_tx_init(struct anx9805_info *anx9805)
{
	u8 value;
	u32 cnt;

	hdmi_tx_set_state(anx9805, HDMI_INITIAL);	/* Initialize the hdmi tx state */

#if defined(CONFIG_FB_CS752X_CLCD)
	hdmi_tx_avi_set_vars(anx9805);
#endif
#if defined(CONFIG_SOUND_CS75XX_SPDIF)
	hdmi_tx_audio_set_vars(anx9805);
#endif

/* system */
	/* software reset */
	anx9805_i2c_bits(anx9805, ANX9805_SYS_SET_BITS, SYS_RST1_REG, SW_RST);	/* 72:06.1 software reset */
	anx9805_i2c_bits(anx9805, ANX9805_SYS_CLR_BITS, SYS_RST1_REG, SW_RST);	/* 72:06.1 clear software reset */

	/* mask and disable the interrupts */
	for (cnt=0; cnt<7; cnt++) {
		anx9805_i2c_wreg(anx9805, ANX9805_SYS_SET_REG,  INT_COM_MSK1_REG + cnt, 0x00);	/* 72:f8-fe mask all interrupts */
		anx9805_i2c_wreg(anx9805, ANX9805_SYS_SET_REG,  INT_STATE_REG + cnt, 0xff);	/* 72:f0-f6 clear all suspend ints */
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
		anx9805->int_status[cnt] = 0;
#endif
	}
	anx9805_i2c_wreg(anx9805, ANX9805_SYS_SET_REG,  INT_DP_STS1_REG, 0xff);	/* 72:f7 clear DP interrupt flags */
	anx9805_i2c_wreg(anx9805, ANX9805_SYS_SET_REG,  INT_CTL_REG, 0x00);	/* 72:ff normal int output, active low */
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	anx9805->int_status[7] = 0;
#endif

	anx9805_i2c_bits(anx9805, ANX9805_SYS_CLR_BITS, VIDEO_CTRL1_REG, VIDEO_EN);	/* 72:08.7, disable video */
	anx9805_i2c_bits(anx9805, ANX9805_SYS_SET_BITS, VIDEO_CTRL1_REG, EDGE);	/* 72:08.0, negative edge latch */

	/* unknown registers setting */

/* hdmi */
/* hdcp */
	anx9805_i2c_wreg(anx9805, ANX9805_HDMI_SET_REG,  HDMI_SCTRL1_REG, 0x00);	/* 7a:02 clear hdcp */

// =============================================================================
	//anx9805_i2c_write(anx9805->cli_hdmi, HDMI_SCTRL1_REG, 0x00);	/* 7a:02 clear hdcp */

/* system related configuration */
	//anx9805_i2c_read( anx9805->cli_sys, SYS_PD_REG, &value);		/* 72:05 */
	//value = value & ~PD_REG | PD_AUDIO;	/* warning: need to be verified */
	//anx9805_i2c_write(anx9805->cli_sys, SYS_PD_REG, value);		/* 72:05.7=0, 4=1, 0=1 */
		/* 7: Power down cfg. 1=Power down mode, 0=Normal mode */
		/* 4: Power down audio clock domain. 1=power down mode, 0=Normal mode */
		/* 0: Mode select. 1=HDMI mode */

	/* software reset */
	//anx9805_i2c_read( anx9805->cli_sys, SYS_RST1_REG, &value);	/* 72:06 */
	//anx9805_i2c_write(anx9805->cli_sys, SYS_RST1_REG, value | SW_RST);
	// FIXME: does it need to delay a while?
	//anx9805_i2c_write(anx9805->cli_sys, SYS_RST1_REG, value & ~SW_RST);

	/* 12 bit SDR, negedge latch, and wait video stable */
	//anx9805_i2c_write(anx9805->cli_sys, VIDEO_CTRL1_REG, EDGE);	/* 72:08 */
		/* 0: Internal data latch, 1=negative edge latch */

	anx9805_i2c_write(anx9805->cli_sys, 0xe1, 0x19);	/* 72:e1 pll hi/low threadhold */
	anx9805_i2c_write(anx9805->cli_sys, 0xe6, 0xd9);	/* 72:e6 pll hi/low threadhold */

/* hdmi related configuration */
	anx9805_i2c_write(anx9805->cli_hdmi,PLL_MISC_CTL1_REG, MISC_TIMER_SEL);	/* 7a:38 pll range ctrl */
		/* 5: 0=Select long timer */
		/* 4: 1=Enable control of the PLL range check */
	anx9805_i2c_write(anx9805->cli_hdmi,PLL_MISC_CTL2_REG, MISC_MODE_SEL);	/* 7a:39 freq counter mode */
		/* 5: 1=Select frequency counter control  mode */

	/* disable ddc level shift */
	anx9805_i2c_write(anx9805->cli_hdmi,HDMI_CHIP_CTRL_REG, 0x00);	/* 7a:65 */

	/* serdes ac mode */
	anx9805_i2c_read( anx9805->cli_sys, SYS_RST2_REG, &value);	/* 72:07 */
	anx9805_i2c_write(anx9805->cli_sys, SYS_RST2_REG, value | AC_MODE);

	/* set channel output amplitude for dp phy cts */
	anx9805_i2c_write(anx9805->cli_hdmi,TMDS_CH_CFG1_REG, 0x10);	/* 7a:61, 540mV */
	anx9805_i2c_write(anx9805->cli_hdmi,TMDS_CH_CFG2_REG, 0x10);	/* 7a:62 */
	anx9805_i2c_write(anx9805->cli_hdmi,TMDS_CH_CFG3_REG, 0x10);	/* 7a:63 */
	anx9805_i2c_write(anx9805->cli_hdmi,TMDS_CH_CFG4_REG, 0x10);	/* 7a:64 */

	/* set termination */
	anx9805_i2c_write(anx9805->cli_sys, 0xdc, 0xf0);	/* 72:dc 30% off, low termination */

	/* set duty cycle */
	anx9805_i2c_write(anx9805->cli_sys, 0xde, 0x99);	/* 72:de duty cycle ctrl */

	anx9805_i2c_read( anx9805->cli_sys, 0xdf, &value);	/* 72:df */
	anx9805_i2c_write(anx9805->cli_sys, 0xdf, value | 0x2a);

	/* power down pll filter */
	anx9805_i2c_write(anx9805->cli_sys, 0xe2, 0x06);	/* 72:e2 pll tx out */

	hdmi_tx_set_state(anx9805, HDMI_WAIT_HOTPLUG);	/* change to waiting hotplug state */
}

static void hdmi_tx_wait_hotplug(struct anx9805_info *anx9805)
{
	//u8 value;

	hdmi_tx_set_state(anx9805, HDMI_PARSE_EDID);	/* change to parse edid state */
}

static void hdmi_tx_parse_edid(struct anx9805_info *anx9805)
{
	hdmi_tx_set_state(anx9805, HDMI_LINK_TRAINING);	/* change to link training state */
	anx9805_read_edid(anx9805);
}

static void hdmi_tx_link_train(struct anx9805_info *anx9805)
{
	//u8 value;

	hdmi_tx_set_state(anx9805, HDMI_CONFIG_VIDEO);	/* change to config video state */
}

void hdmi_tx_repeatime_set(struct anx9805_info *anx9805){
	u8 value;

	/* set input pixel repeat times */
	anx9805_i2c_read( anx9805->cli_sys, VIDEO_CTRL6_REG, &value);	/* 72:0d */
	//anx9805_i2c_write( anx9805->cli_sys, VIDEO_CTRL6_REG, value);	/* 72:0d.5:4 set repetition if need */
	
	/* set link pixel repeat times */
	anx9805_i2c_read( anx9805->cli_hdmi, VH_SYNC_ALIGN2_EN_REG, &value);	/* 7a:04 */
	//anx9805_i2c_write( anx9805->cli_hdmi, VH_SYNC_ALIGN2_EN_REG, value);	/* 7a:04.5:4 set repetition if need */
}

void hdmi_tx_csc_colordepth_set(struct anx9805_info *anx9805)
{
	u8 value;

	/* set 24 bit / RGB mode: default: RGB, 8bit per color */
	anx9805_i2c_read( anx9805->cli_sys, VIDEO_CTRL2_REG, &value);	/* 72:09 */
	anx9805_i2c_write( anx9805->cli_sys, VIDEO_CTRL2_REG, (value & 0x8c) | IN_BPC_8bit);	/* 72:09.6:4,1:0 */
}

#if defined(CONFIG_FB_CS752X_CLCD)
static void hdmi_tx_config_video(struct anx9805_info *anx9805)
{
	u8 value;

	anx9805_i2c_read( anx9805->cli_hdmi, HDMI_SSTS_REG, &value);	/* 7a:01 */
	if (value & CLK_DET) {
		printk("no clock\n");
		return;
	}
	if (value & TXPLL_MISC_LOCK) {
		anx9805_i2c_read(anx9805->cli_sys,  0xe4, &value);	/* 72:e4 */
		anx9805_i2c_write(anx9805->cli_sys, 0xe4, value & 0x7f);	/* 72:e4.7 TX pll pwr */
		printk("pll not lock\n");
		return;
	}
	hdmi_tx_repeatime_set(anx9805);	/* in case of repeating pixel */
	hdmi_tx_csc_colordepth_set(anx9805);	/* gcp, deep color arrangement */
	hdmi_tx_video_enable(anx9805);

	/* reset TMDS link to align 4 channels */
	anx9805_i2c_read(anx9805->cli_hdmi, HDMI_RST_REG, &value);	/* 7a:00.0 channel alignment */
	anx9805_i2c_write(anx9805->cli_hdmi, HDMI_RST_REG, value |  TMDS_CHNL_ALIGN);
	anx9805_i2c_write(anx9805->cli_hdmi, HDMI_RST_REG, value & ~TMDS_CHNL_ALIGN);

	/* enable TMDS clock output */
	anx9805_i2c_read(anx9805->cli_hdmi, TMDS_CH_CFG4_REG, &value);	/* 7a:64.6 */
	anx9805_i2c_write(anx9805->cli_hdmi, TMDS_CH_CFG4_REG, value | TMDS_CLK_MUTE_CTRL);

	/* enable Control Packet transmission */
	anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, &value);	/* 7a:70.3:2 */
	anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, value | (GCP_PKT_RPT | GCP_PKT_EN));

	hdmi_tx_set_state(anx9805, HDMI_CONFIG_AUDIO);	/* change to config audio state */
}
#endif

#if defined(CONFIG_SOUND_CS75XX_SPDIF)
static void hdmi_tx_config_audio(struct anx9805_info *anx9805)
{
	u8 value, fs;
	u32 acr_n;

	anx9805_i2c_read(anx9805->cli_sys,  SYS_PD_REG, &value);	/* 72:05.4 */
	anx9805_i2c_write(anx9805->cli_sys, SYS_PD_REG, value | PD_AUDIO);	/* powerup audio */

	anx9805_i2c_read(anx9805->cli_sys,  SPDIF_AUDIO_CTL0_REG, &value);	/* 72:36.7 */
	anx9805_i2c_write(anx9805->cli_sys, SPDIF_AUDIO_CTL0_REG, value | AUD_SPDIF_IN);	/* enable spdif input */

	/* hdmi needs to sw on first before detection */
	anx9805_i2c_read(anx9805->cli_hdmi, HDMI_AUDIO_CTL_REG, &value);	/* 7a:09 */
	anx9805_i2c_write(anx9805->cli_hdmi, HDMI_AUDIO_CTL_REG, (value & ~PD_RING_OSC) | HDMI_AUD_EN);

	/* FIXME: delay 3 ms for stabilizing the spdif circuit */

	/* check audio */
	anx9805_i2c_read(anx9805->cli_sys,  INT_COM_STS1_REG, &value);	/* 72:f1 */
	anx9805_i2c_write(anx9805->cli_sys, INT_COM_STS1_REG, value);	/* check interrupt status events */

	anx9805_i2c_read(anx9805->cli_sys,  SPDIF_AUDIO_STA0_REG, &value);	/* 72:38.7.0 */
	if (0 == (value & SPDIF_CLK_DET)) {
		printk("audio clock not detected\n");
	}

	if (0 == (value & SPDIF_DET)) {
		printk("audio not detected\n");
	}

	anx9805_i2c_write(anx9805->cli_sys, AUDIO_TYPE_REG, AUDIO_INFOFRAME);	/* 72:83 */
	anx9805_i2c_write(anx9805->cli_sys, AUDIO_VER_REG,  AUDIO_IF_VER1);	/* 72:84 */
	anx9805_i2c_write(anx9805->cli_sys, AUDIO_DATA0_REG,AUDIO_IF_LEN);	/* 72:85 */

	anx9805_i2c_read(anx9805->cli_sys,  SPDIF_AUDIO_STA1_REG, &value);	/* 72:39 */
	fs = value & 0xf0;
	switch (fs) {
	case SPDIF_FS_FREQ_44:
		acr_n = HDMI_TX_N_44k;
		break;
	case SPDIF_FS_FREQ_48:
		acr_n = HDMI_TX_N_48k;
		break;
	case SPDIF_FS_FREQ_32:
		acr_n = HDMI_TX_N_32k;
		break;
	case SPDIF_FS_FREQ_96:
		acr_n = HDMI_TX_N_96k;
		break;
	default:
		acr_n = HDMI_TX_N_44k;	/* 44.1kHz */
	}

	value = (u8)acr_n;
	anx9805_i2c_write(anx9805->cli_hdmi, ACR_SVAL1_REG, value);	// 7a:72 */
	value = (u8)(acr_n >> 8);
	anx9805_i2c_write(anx9805->cli_hdmi, ACR_SVAL2_REG, value);	// 7a:73 */
	anx9805_i2c_write(anx9805->cli_hdmi, ACR_SVAL3_REG, 0x00);	// 7a:74 */

	/* enable ACR packet */
	anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, &value);	// 7a:70.1:0 */
	anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, value | (ACR_PKT_NEW | ACR_PKT_EN));

	/* enable AIF InfoFrame packet */
	anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, &value);	/* 7a:71.1:0 */
	anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, value | (AIF_PKT_RPT | AIF_PKT_EN));

	hdmi_tx_set_state(anx9805, HDMI_CONFIG_PACKET);	/* change to config packet state */
}
#endif

static void hdmi_tx_config_packet(struct anx9805_info *anx9805)
{
	u8 value;

#if defined(CONFIG_FB_CS752X_CLCD)
	/* Configure avi InfoFrame packet */
	if (HDMI_TX_SEL_AVI & anx9805->packets_need_config) {
		/* disable the packet */
		anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, &value);	/* 7a:70.5 */
		anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, value & (~AVI_PKT_RPT));

		/* load InfoFrame packet data to regs */
		hdmi_tx_load_infoframe(anx9805, hdmi_avi_infoframe, &(anx9805->avi_info));

		/* enable the packet and repeater */
		anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, &value);	/* 7a:70.5:4 */
		anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL1_REG, value | (AVI_PKT_RPT | AVI_PKT_EN));

		/* complete avi packet */
		anx9805->packets_need_config &= ~HDMI_TX_SEL_AVI;
	}
#endif

#if defined(CONFIG_SOUND_CS75XX_SPDIF)
	/* Configure audio InfoFrame packet */
	if (HDMI_TX_SEL_AUDIO & anx9805->packets_need_config) {
		/* disable the packet */
		anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, &value);	/* 7a:71.1 */
		anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, value & (~AIF_PKT_RPT));

		/* check enabling the packet */
		anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, &value);	/* 7a:71 */
		if (value & AIF_PKT_EN) {
			return;
		}

		/* load InfoFrame packet data to regs */
		hdmi_tx_load_infoframe(anx9805, hdmi_audio_infoframe, &(anx9805->audio_info));

		/* enable the packet and repeater */
		anx9805_i2c_read(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, &value);	/* 7a:71.1:0 */
		anx9805_i2c_write(anx9805->cli_hdmi, IF_PKT_CTRL2_REG, value | (AIF_PKT_RPT | AIF_PKT_EN));

		/* complete audio packet */
		anx9805->packets_need_config &= ~HDMI_TX_SEL_AUDIO;
	}
#endif

	hdmi_tx_set_state(anx9805, HDMI_PLAY_BACK);	/* change to playback state */
}

static void hdmi_tx_playback(struct anx9805_info *anx9805)
{
}

static int anx9805_init(struct anx9805_info *anx9805)
{
	u8 dataL=0, dataH=0;
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	int gpio_ret = 0, irq_ret = 0;

	gpio_ret = anx9805_gpio_init();
	if (gpio_ret)
		goto error_gpio;
#endif

/* Check anx9805 chip id */
	anx9805_i2c_rreg(anx9805, ANX9805_SYS_GET_REG,  DEVICE_ID_L_REG, &dataL);	/* 72:02: 0x05 */
	anx9805_i2c_rreg(anx9805, ANX9805_SYS_GET_REG,  DEVICE_ID_H_REG, &dataH);	/* 72:03: 0x98 */
	if ((DEVICE_ID_L != dataL) || (DEVICE_ID_H != dataH)) {
		printk("<%s:%d> anx9805 hdmi transmitter device not found (ID=%02x:%02x)\n", __func__, __LINE__, dataH, dataL);
		goto error_id;	/* anx9805 device not found */
	}
	printk("<%s:%d> anx9805 hdmi transmitter device: anx9805=%p (ID=%02x:%02x)\n", __func__, __LINE__, anx9805, dataH, dataL);

	/* power on the hdmi device */
	anx9805_i2c_wreg(anx9805, ANX9805_SYS_SET_REG,  SYS_PD_REG, PD_MISC | PD_IO | PD_LINK |
#ifndef CONFIG_FB_CS752X_CLCD
							PD_VIDEO |
#endif
#ifndef CONFIG_SOUND_CS75XX_SPDIF
							PD_AUDIO |	/* 0x75 */
#endif
							HDMI_MODE);
	anx9805_msleep(100);	/* sleep 2000 ms. wait for HDMI RX/TX stable */

/* Configure the hdmi transmitter */
	hdmi_tx_init(anx9805);
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	hdmi_dump_reg(anx9805, ANX9805_SYSTEM_D0_ADDR, SYS_PD_REG, 1);
#endif

#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	irq_ret = request_irq(gpio_to_irq(ANX9805_GPIO_INT), anx9805_hdmi_irq, IRQF_SHARED, ANX9805_HDMI_DRIVER_NAME, anx9805);
	if (irq_ret) {
		printk("<%s:%d> failed to request anx9805 hdmi interrupt (error=%d, irqno=%d).\n", __func__, __LINE__, irq_ret, gpio_to_irq(ANX9805_GPIO_INT));
		goto error_irq;
	}
#endif

        hdmi_tx_wait_hotplug(anx9805);
        hdmi_tx_parse_edid(anx9805);
        hdmi_tx_link_train(anx9805);
#if defined(CONFIG_FB_CS752X_CLCD)
        hdmi_tx_config_video(anx9805);
#endif
#if defined(CONFIG_SOUND_CS75XX_SPDIF)
        hdmi_tx_config_audio(anx9805);
#endif
        hdmi_tx_config_packet(anx9805);
        hdmi_tx_playback(anx9805);

	return 0;

#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
error_irq:
	if (0 == irq_ret)
		free_irq(gpio_to_irq(ANX9805_GPIO_INT), anx9805);
#endif

error_id:
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	if (0 == gpio_ret)
		gpio_free(ANX9805_GPIO_INT);
error_gpio:
#endif
	return -EPERM;
}

static int anx9805_release(struct anx9805_info *anx9805)
{
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	free_irq(gpio_to_irq(ANX9805_GPIO_INT), anx9805);
	gpio_free(ANX9805_GPIO_INT);
#endif

	return 0;
}

static int anx9805_open(struct inode *inode, struct file *file)
{
	printk("%s open\n", ANX9805_HDMI_DRIVER_NAME);

	//inode->i_private = inode;
	file->private_data = pANX9805;
	return 0;
}

static int anx9805_close(struct inode *inode, struct file *file)
{
	printk("%s close\n", ANX9805_HDMI_DRIVER_NAME);

	//inode->i_private = NULL;
	file->private_data = NULL;
	return 0;
}

static long anx9805_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct anx9805_info *anx9805 = file->private_data;
	int ret=0;
	union iodata io;

	if (_IOC_TYPE(cmd) != CS75XX_HDMI_IOCTL_MAGIC) {
		return -ENOTTY;
	}
	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		if (copy_from_user(&io, (union iodata *)arg, sizeof(io))){
			return -EFAULT;
		}
	}

	switch (cmd) {
	case ANX9805_SYS_GET_REG:
	case ANX9805_SYS_SET_REG:
	case ANX9805_HDMI_GET_REG:
	case ANX9805_HDMI_SET_REG:
	case ANX9805_DP_GET_REG:
	case ANX9805_DP_SET_REG:
		anx9805_reg(anx9805, cmd, &io);	/* TODO: need boundary check */
		break;
	case ANX9805_SYS_CLR_BITS:
	case ANX9805_SYS_SET_BITS:
	case ANX9805_HDMI_CLR_BITS:
	case ANX9805_HDMI_SET_BITS:
	case ANX9805_DP_CLR_BITS:
	case ANX9805_DP_SET_BITS:
		anx9805_bits(anx9805, cmd, &io);	/* TODO: need boundary check */
		break;
	case ANX9805_REG_DELAY_MS:
		anx9805->delay = (u32)io.dat;	/* TODO: need boundary check */
		break;
	default:
		ret = -ENOTTY;
		break;
	}

	if (_IOC_DIR(cmd) & _IOC_READ) {
		if (copy_to_user((void __user *)arg, &io, sizeof(io))){
			return -EFAULT;
		}
	}

	return ret;
}

struct file_operations anx9805_fops = {
	.open    = anx9805_open,
	.release = anx9805_close,
	.owner   = THIS_MODULE,
	.compat_ioctl = anx9805_ioctl,	
};

static struct miscdevice anx9805_hdmi_miscdev =
{
	HDMI_MINOR,
	"anx9805",
	&anx9805_fops
};

static int register_anx9805(const struct file_operations *fops, struct anx9805_info *anx9805)
{
	int misc_ret;

	misc_ret = misc_register(&anx9805_hdmi_miscdev);
	if (0 == misc_ret) {
		DBGPRINT(1, "%s driver installed.\n", ANX9805_HDMI_DRIVER_NAME);
		anx9805->miscdev  = &anx9805_hdmi_miscdev;
	}
	return misc_ret;
}

static void unregister_anx9805(const struct file_operations *fops, struct anx9805_info *anx9805)
{
	misc_deregister(&anx9805_hdmi_miscdev);
	printk(KERN_ALERT "%s driver cleanup.\n", ANX9805_HDMI_DRIVER_NAME);
}

static const struct i2c_device_id anx9805_id[] = {
	{ "anx9805", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, anx9805_id);

static int anx9805_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct anx9805_info *anx9805 = NULL;
	int i2c_ret = 0, reg_ret = 0, init_ret = 0;

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk("<%s:%d> i2c functionality check failed\n", __func__, __LINE__);
		return -ENODEV;
	}

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	anx9805 = kzalloc(sizeof(struct anx9805_info), GFP_KERNEL);
	if (!anx9805) {
		printk("<%s:%d> anx9805 kzalloc failed\n", __func__, __LINE__);
		return -ENOMEM;
	}

	pANX9805 = anx9805;
	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	anx9805->cli_sys = client;
	anx9805->id = id;
	anx9805->delay = 0;	/* delay 10ms per register access by default */

#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	INIT_WORK(&anx9805->work, anx9805_hdmi_work);
#endif

	i2c_set_clientdata(client, anx9805);

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	i2c_ret = anx9805_i2c_client(anx9805);
	if (i2c_ret)
		goto error;

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	reg_ret = register_anx9805(&anx9805_fops, anx9805);
	if (reg_ret)
		goto error;

	/* Init the anx9805 hdmi transmitter */
	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	init_ret = anx9805_init(anx9805);
	if (init_ret)
		goto error;

	printk("anx9805 hdmi transmitter created and attached\n");
	return 0;

error:
	if (0 == reg_ret)
		unregister_anx9805(&anx9805_fops, anx9805);

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	if (0 == i2c_ret)
		anx9805_i2c_client_remove(anx9805);

	DBGPRINT(1, "<%s:%d> probing\n", __func__, __LINE__);
	i2c_set_clientdata(client, NULL);
	kfree(anx9805);
	printk("<%s:%d> anx9805 hdmi transmitter probing failed\n", __func__, __LINE__);
	pANX9805 = NULL;
	return -ENODEV;
}

static int anx9805_i2c_remove(struct i2c_client *client)
{
	struct anx9805_info *anx9805 = i2c_get_clientdata(client);

	anx9805_release(anx9805);
	anx9805_i2c_client_remove(anx9805);
	unregister_anx9805(&anx9805_fops, anx9805);
	i2c_set_clientdata(client, NULL);
	kfree(anx9805);
	pANX9805 = NULL;

	printk("anx9805 hdmi transmitter cleanup\n");

	return 0;
}

static struct i2c_driver anx9805_i2c_driver = {
	.driver = {
		.name   = "anx9805",
		.owner  = THIS_MODULE,
	},
	.probe          = anx9805_i2c_probe,
	.remove         = anx9805_i2c_remove,
	.id_table       = anx9805_id,
};

// follow coding style
int __init anx9805_hdmi_init(void)
{
	int ret;

	ret = i2c_add_driver(&anx9805_i2c_driver);
	printk("anx9805 hdmi transmitter%s initialized\n", ret ? " fail":"");

	return ret;
}

void __exit anx9805_hdmi_exit(void)
{
	i2c_del_driver(&anx9805_i2c_driver);
}

module_init(anx9805_hdmi_init);
module_exit(anx9805_hdmi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe Hsu <joe.hsu@cortina-systems.com>");
MODULE_DESCRIPTION("ANX9805 HDMI Transmitter driver in CS75XX");
