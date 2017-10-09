/*
 * FILE NAME cs75xx_cir.c
 *
 * BRIEF MODULE DESCRIPTION
 *  Driver for Cortina CS75XX Consumer Infrared (CIR) device.
 *
 *  Copyright 2010 Cortina , Corp.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/cs75xx_cir.h>


#define cs75xx_cir_read_reg(offset)		(readl(cir->base+offset))
#define cs75xx_cir_write_reg(offset, val)	(writel(val, cir->base+offset))


/* Register Map */
#define	CS75XX_CIR_ID			0x00
#define	CS75XX_CIR_CTRL0		0x04
#define	CS75XX_CIR_CTRL1		0x08
#define	CS75XX_CIR_INT_ST		0x0C
#define	CS75XX_CIR_INT_EN		0x10
#define	CS75XX_CIR_DATA			0x14
#define	CS75XX_CIR_DATAEXT		0x18
#define	CS75XX_CIR_POWER		0x1C
#define	CS75XX_CIR_POWEREXT		0x20
#define	CS75XX_PWR_CTRL1		0x28
#define CS75XX_PWR_INT_EN		0x30

#define CIR_CTRL0_NEC_MAX		16
#define	CIR_CTRL1_DATA_LEN_MIN		8
#define	CIR_CTRL1_DATA_LEN_MAX		48

#define CS75XX_CIR_DEV_ID		((cs75xx_cir_read_reg(CS75XX_CIR_ID) & 0x00FFFF00) >> 8)
#define CS75XX_CIR_REV_ID		(cs75xx_cir_read_reg(CS75XX_CIR_ID) & 0x000000FF)
#define CS75XX_CIR_DEV_ID_VAL		0x000104

struct cs75xx_cir {
	struct device	*dev;
	void __iomem	*base;
	int				irq;
	struct timer_list ntimer;
	unsigned short	precision;
	unsigned char	trigger;	/* if input is triggerable, unit of 100ms */
#ifdef CONFIG_CORTINA_FPGA
	unsigned char	system_power;
#endif
};

static struct platform_device *cs75xx_cir_dev;

unsigned int data = 0;
unsigned short data_ext = 0;
wait_queue_head_t cir_wait_q;

#ifndef CIR_MINOR
#define CIR_MINOR	240		/* Documents/devices.txt suggest to use 240~255 for local driver!! */
#endif

static void cs75xx_cir_config(struct cs75xx_cir *cir, u32 mode);

#ifdef CONFIG_CORTINA_FPGA
void cs75xx_cir_power_on(void)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_INT_ENABLE_t reg_int_en;

	reg_int_en.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_EN);
	reg_int_en.bf.dat_int_en = 1;
	reg_int_en.bf.repeat_int_en = 1;
	cs75xx_cir_write_reg(CS75XX_CIR_INT_EN, reg_int_en.wrd);

	cir->system_power = 1;
}
EXPORT_SYMBOL(cs75xx_cir_power_on);

void cs75xx_cir_power_off(void)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_INT_ENABLE_t reg_int_en;

	reg_int_en.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_EN);
	reg_int_en.bf.dat_int_en = 0;
	reg_int_en.bf.repeat_int_en = 0;
	cs75xx_cir_write_reg(CS75XX_CIR_INT_EN, reg_int_en.wrd);

	cir->system_power = 0;
}
EXPORT_SYMBOL(cs75xx_cir_power_off);
#endif

static void cs75xx_cir_set_baudrate(u32 baud)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.baud_div = baud;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_baudrate(u32 *baud_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*baud_p = reg.bf.baud_div;
}

static void cs75xx_cir_set_h_period(u32 period)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	if (period > CIR_CTRL0_NEC_MAX)
		period = CIR_CTRL0_NEC_MAX;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.head_hi_t = period - 1;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_h_period(u32 *period_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*period_p = reg.bf.head_hi_t + 1;
}

static void cs75xx_cir_set_l_period(u32 period)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	if (period > CIR_CTRL0_NEC_MAX)
		period = CIR_CTRL0_NEC_MAX;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.head_lo_t = period - 1;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_l_period(u32 *period_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*period_p = reg.bf.head_lo_t + 1;

}

static void cs75xx_cir_set_rc5_extend(u32 enable)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.rc5_extend = enable ? 1 : 0;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_rc5_extend(u32 *enable_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*enable_p = reg.bf.rc5_extend;
}

static void cs75xx_cir_set_rc5_stopbit(u32 enable)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.rc5_stopBit_en = enable ? 1 : 0;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_rc5_stopbit(u32 *enable_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*enable_p = reg.bf.rc5_stopBit_en;
}

static void cs75xx_cir_set_protocol(u32 protocol)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.cir_protocol = protocol ? 1 : 0;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_protocol(u32 *protocol_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*protocol_p = reg.bf.cir_protocol;
}

static void cs75xx_cir_set_pos(u32 pos)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.pos = pos ? 1 : 0;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_pos(u32 *pos_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*pos_p = reg.bf.pos;
}

static void cs75xx_cir_set_demodula(u32 enable)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	reg.bf.demod_en = enable ? 1 : 0;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg.wrd);
}

static void cs75xx_cir_get_demodula(u32 *enable_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL0_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);
	*enable_p = reg.bf.demod_en;
}

static void cs75xx_cir_set_datalen(u32 data_len)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	if (data_len < CIR_CTRL1_DATA_LEN_MIN)
		data_len = CIR_CTRL1_DATA_LEN_MIN;
	else if (data_len > CIR_CTRL1_DATA_LEN_MAX)
		data_len = CIR_CTRL1_DATA_LEN_MAX;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	reg.bf.data_len_b = data_len - 1;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg.wrd);
}

static void cs75xx_cir_get_datalen(u32 *data_len_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	*data_len_p = reg.bf.data_len_b + 1;
}

static void cs75xx_cir_set_powerkey_handle(u32 enable)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	reg.bf.pwrKeyIRQCpu = enable ? 1 : 0;;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg.wrd);
}

static void cs75xx_cir_get_powerkey_handle(u32 *enable_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	*enable_p = reg.bf.pwrKeyIRQCpu;
}

static void cs75xx_cir_set_compare(u32 enable)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	reg.bf.data_compare = enable ? 1 : 0;;
	cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg.wrd);
}

static void cs75xx_cir_get_compare(u32 *enable_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_RXCTRL1_t reg;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL1);
	*enable_p = reg.bf.data_compare;
}

static void cs75xx_cir_set_powerkey(u32 key, u32 key_ext)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_PWRKEY_t reg;
	CIR_PWRCTRL_CIR_PWRKEY_EXT_t reg_ext;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_POWER);
	reg.bf.pwr_code1 = key;
	cs75xx_cir_write_reg(CS75XX_CIR_POWER, reg.wrd);

	reg_ext.wrd = cs75xx_cir_read_reg(CS75XX_CIR_POWEREXT);
	reg_ext.bf.pwr_code2 = key_ext;
	cs75xx_cir_write_reg(CS75XX_CIR_POWEREXT, reg_ext.wrd);
}

static void cs75xx_cir_get_powerkey(u32 *key_p, u32 *key_ext_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);
	CIR_PWRCTRL_CIR_PWRKEY_t reg;
	CIR_PWRCTRL_CIR_PWRKEY_EXT_t reg_ext;

	reg.wrd = cs75xx_cir_read_reg(CS75XX_CIR_POWER);
	*key_p = reg.bf.pwr_code1;

	reg_ext.wrd = cs75xx_cir_read_reg(CS75XX_CIR_POWEREXT);
	*key_ext_p = reg_ext.bf.pwr_code2;
}

static void cs75xx_cir_set_precision(u32 precision)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);

	cir->precision = precision;
}

static void cs75xx_cir_get_precision(u32 *precision_p)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);

	*precision_p = cir->precision;
}

static void cs75xx_cir_ntrigger_on(unsigned long arg)
{
	struct cs75xx_cir *cir = (struct cs75xx_cir *)arg;

	cir->trigger = 1;
}

static void cs75xx_cir_ptrigger_on(unsigned long arg)
{
	struct cs75xx_cir *cir = (struct cs75xx_cir *)arg;
	CIR_PWRCTRL_CIR_INT_ENABLE_t reg_cir_int_en;

	reg_cir_int_en.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_EN);
	reg_cir_int_en.bf.pwrkey_int_en = 1;
	cs75xx_cir_write_reg(CS75XX_CIR_INT_EN, reg_cir_int_en.wrd);
}

static int cs75xx_cir_open(struct inode *inode, struct file *file)
{
	struct cs75xx_cir *cir = platform_get_drvdata(cs75xx_cir_dev);

	file->private_data = cir;

	return 0;
}

static int cs75xx_cir_release(struct inode *inode, struct file *file)
{
	return 0;
}

int cs75xx_cir_int_wait(u32 *data1, u32 *data2, u32 timeout)
{
	int ret = 0;

	ret = interruptible_sleep_on_timeout(&cir_wait_q, timeout*HZ);
	if (ret > 0) {
		*data1 = data;
		*data2 = data_ext;
	}
	else{
		*data1 = 0;
		*data2 = 0;
	}

	return ret;
}

static int cs75xx_cir_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct cs75xx_cir *cir = file->private_data;
	struct cir_ioctl_data para;
	struct cir_ioctl_data48 data48;

#ifdef CONFIG_CORTINA_FPGA
	if (cir->system_power == 0) {
		printk("System isn't power-on now!\n");
		return 0;
	}
#endif

	switch (cmd) {
	case CIR_SET_BAUDRATE:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_baudrate(para.data);
		break;
	case CIR_GET_BAUDRATE:
		cs75xx_cir_get_baudrate(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_HIGH_PERIOD:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_h_period(para.data);
		break;
	case CIR_GET_HIGH_PERIOD:
		cs75xx_cir_get_h_period(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_LOW_PERIOD:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_l_period(para.data);
		break;
	case CIR_GET_LOW_PERIOD:
		cs75xx_cir_get_l_period(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_RC5_EXTEND:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_rc5_extend(para.data);
		break;
	case CIR_GET_RC5_EXTEND:
		cs75xx_cir_get_rc5_extend(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_RC5_STOPBIT:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_rc5_stopbit(para.data);
		break;
	case CIR_GET_RC5_STOPBIT:
		cs75xx_cir_get_rc5_stopbit(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_PROTOCOL:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_protocol(para.data);
		break;
	case CIR_GET_PROTOCOL:
		cs75xx_cir_get_protocol(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_POSITIVE_POLARITY:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_pos(para.data);
		break;
	case CIR_GET_POSITIVE_POLARITY:
		cs75xx_cir_get_pos(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_POWER_KEY_HANDLE:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_powerkey_handle(para.data);
		break;
	case CIR_GET_POWER_KEY_HANDLE:
		cs75xx_cir_get_powerkey_handle(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_ENABLE_COMPARE:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_compare(para.data);
		break;
	case CIR_GET_ENABLE_COMPARE:
		cs75xx_cir_get_compare(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_ENABLE_DEMOD:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_demodula(para.data);
		break;
	case CIR_GET_ENABLE_DEMOD:
		cs75xx_cir_get_demodula(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_DATA_LEN:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_datalen(para.data);
		break;
	case CIR_GET_DATA_LEN:
		cs75xx_cir_get_datalen(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	case CIR_SET_POWER_KEY:
		if (copy_from_user(&data48, (struct cir_ioctl_data48 *)arg, sizeof(data48)))
			return -EFAULT;
		cs75xx_cir_set_powerkey(data48.data, data48.data_ext);
		break;
	case CIR_GET_POWER_KEY:
		cs75xx_cir_get_powerkey(&data48.data, &data48.data_ext);
		if (copy_to_user((struct cir_ioctl_data48 *)arg, &data48, sizeof(data48)))
			return -EFAULT;
		break;
	case CIR_GET_DATA:
		if (copy_from_user(&data48, (struct cir_ioctl_data48 *)arg, sizeof(data48)))
			return -EFAULT;
		data48.ret = OLD_DATA;
		data48.data = data;
		data48.data_ext = data_ext;
		if (copy_to_user((struct cir_ioctl_data48 *)arg, &data48, sizeof(data48)))
			return -EFAULT;
		break;
	case CIR_WAIT_INT_DATA:
		if (copy_from_user(&data48, (struct cir_ioctl_data48 *)arg, sizeof(data48)))
			return -EFAULT;
		if (data48.timeout < 5)
			data48.timeout = 5;
		else if (data48.timeout > 60)
			data48.timeout = 60;
		ret = cs75xx_cir_int_wait(&data48.data, &data48.data_ext, data48.timeout);
		data48.ret = (ret > 0) ? NEW_RECEIVE:OLD_DATA;
		if (copy_to_user((struct cir_ioctl_data48 *)arg, &data48, sizeof(data48)))
			return -EFAULT;
		break;
	case CIR_SET_CONFIGURATION_SEL:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_config(cir, para.data);
		break;
	case CIR_SET_PRECISION:
		if (copy_from_user(&para, (struct cir_ioctl_data *)arg, sizeof(para)))
			return -EFAULT;
		cs75xx_cir_set_precision(para.data);
		break;
	case CIR_GET_PRECISION:
		cs75xx_cir_get_precision(&para.data);
		if (copy_to_user((struct cir_ioctl_data *)arg, &para.data, sizeof(para.data)))
			return -EFAULT;
		break;
	default:
		return -1;
	}

	return 0;
}

static struct file_operations cs75xx_cir_fops =
{
	.owner          = THIS_MODULE,
	.open           = cs75xx_cir_open,
	.release        = cs75xx_cir_release,
	.unlocked_ioctl = cs75xx_cir_ioctl,
};

static struct miscdevice cs75xx_cir_miscdev =
{
	CIR_MINOR,
	CS75XX_CIR_NAME,
	&cs75xx_cir_fops
};

#ifdef CONFIG_CS752X_PROC
extern u32 cs_cir_debug;
#endif
static irqreturn_t cs75xx_cir_interrupt(int irq, void *dev_instance)
{
	struct cs75xx_cir *cir = (struct cs75xx_cir *)dev_instance;

	CIR_PWRCTRL_CIR_INT_STATUS_t reg_int_st;
	CIR_PWRCTRL_CIR_INT_ENABLE_t reg_int_en;
	u32 len;

	reg_int_en.wrd = 0;
	cs75xx_cir_write_reg(CS75XX_CIR_INT_EN, reg_int_en.wrd);

	reg_int_st.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_ST);
	if (reg_int_st.bf.repeat_sts) {
#ifdef CONFIG_CS752X_PROC
		if (cs_cir_debug)
			dev_info(cir->dev, "CIR RX:repeat key\n");
#endif
		;
	}
	if (reg_int_st.bf.cir_dat_int) {
#ifdef CONFIG_CS752X_PROC
		if (cs_cir_debug)
			dev_info(cir->dev, "CIR RX:0x");
#endif
		cs75xx_cir_get_datalen(&len);
		if (len > 32) {
			data_ext = cs75xx_cir_read_reg(CS75XX_CIR_DATAEXT);
			data = cs75xx_cir_read_reg(CS75XX_CIR_DATA);
#ifdef CONFIG_CS752X_PROC
			if (cs_cir_debug)
				dev_info(cir->dev, "%08x%04x\n", data, data_ext);
#endif
		}
		else {
			data = cs75xx_cir_read_reg(CS75XX_CIR_DATA);
#ifdef CONFIG_CS752X_PROC
			if (cs_cir_debug)
				dev_info(cir->dev, "%08x\n", data);
#endif
		}
	}
	if (reg_int_st.bf.pwrkey_int_sts) {
#ifdef CONFIG_CS752X_PROC
		if (cs_cir_debug)
			dev_info(cir->dev, "CIR RX:power key\n");
#endif
		cs75xx_cir_get_datalen(&len);
		if (len > 32) {
			data_ext = cs75xx_cir_read_reg(CS75XX_CIR_POWEREXT);
			data = cs75xx_cir_read_reg(CS75XX_CIR_POWER);
#ifdef CONFIG_CS752X_PROC
			if (cs_cir_debug)
				dev_info(cir->dev, "%08x%04x\n", data, data_ext);
#endif
		}
		else {
			data = cs75xx_cir_read_reg(CS75XX_CIR_POWER);
#ifdef CONFIG_CS752X_PROC
			if (cs_cir_debug)
				dev_info(cir->dev, "%08x\n", data);
#endif
		}
	}
	cs75xx_cir_write_reg(CS75XX_CIR_INT_ST, reg_int_st.wrd);

	if (cir->trigger == 1) {
		wake_up_interruptible(&cir_wait_q);

		cir->ntimer.expires = jiffies + cir->precision*(HZ/10);
		add_timer(&cir->ntimer);
		cir->trigger = 0;
	}

	reg_int_en.bf.pwrkey_int_en = 1;
	reg_int_en.bf.dat_int_en = 1;
	reg_int_en.bf.repeat_int_en = 1;
	cs75xx_cir_write_reg(CS75XX_CIR_INT_EN, reg_int_en.wrd);

	return IRQ_RETVAL(IRQ_HANDLED);
}

static void cs75xx_cir_config(struct cs75xx_cir *cir, u32 mode)
{
	CIR_PWRCTRL_CIR_RXCTRL0_t reg_ctrl0;
	CIR_PWRCTRL_CIR_RXCTRL1_t reg_ctrl1;
	CIR_PWRCTRL_CIR_PWRKEY_t reg_pwrkey;
	CIR_PWRCTRL_CIR_PWRKEY_EXT_t reg_pwrkey_ext;

	reg_ctrl0.wrd = cs75xx_cir_read_reg(CS75XX_CIR_CTRL0);

	switch (mode) {
	case VCR_PROTOCOL:
		reg_ctrl0.bf.demod_en       = 0;
		reg_ctrl0.bf.pos            = 0;
		reg_ctrl0.bf.cir_protocol   = 1;
		reg_ctrl0.bf.rc5_stopBit_en = 0;
		reg_ctrl0.bf.rc5_extend     = 0;
		reg_ctrl0.bf.head_lo_t      = VCR_L_ACT_PER;
		reg_ctrl0.bf.head_hi_t      = VCR_H_ACT_PER;
#ifdef CONFIG_CORTINA_FPGA
		reg_ctrl0.bf.baud_div       = (VCR_BAUD*EXT_CLK_SRC/EXT_CLK_DIV);
#else
		reg_ctrl0.bf.baud_div       = (VCR_BAUD*EXT_CLK);
#endif
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg_ctrl0.wrd);

		reg_ctrl1.bf.data_len_b     = VCR_DATA_LEN;
		reg_ctrl1.bf.data_compare   = 0;
		reg_ctrl1.bf.pwrKeyIRQCpu   = 0;
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg_ctrl1.wrd);

		reg_pwrkey.bf.pwr_code1     = VCR_KEY_POWER;
		cs75xx_cir_write_reg(CS75XX_CIR_POWER, reg_pwrkey.wrd);
		reg_pwrkey_ext.bf.pwr_code2 = 0;
		cs75xx_cir_write_reg(CS75XX_CIR_POWEREXT, reg_pwrkey_ext.wrd);

		break;

	case TV1_PROTOCOL:
		reg_ctrl0.bf.demod_en       = 0;
		reg_ctrl0.bf.pos            = 0;
		reg_ctrl0.bf.cir_protocol   = 1;
		reg_ctrl0.bf.rc5_stopBit_en = 0;
		reg_ctrl0.bf.rc5_extend     = 0;
		reg_ctrl0.bf.head_lo_t      = TV1_L_ACT_PER;
		reg_ctrl0.bf.head_hi_t      = TV1_H_ACT_PER;
#ifdef CONFIG_CORTINA_FPGA
		reg_ctrl0.bf.baud_div       = (TV1_BAUD*EXT_CLK_SRC/EXT_CLK_DIV);
#else
		reg_ctrl0.bf.baud_div	    = (TV1_BAUD*EXT_CLK);
#endif
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg_ctrl0.wrd);

		reg_ctrl1.bf.data_len_b     = TV1_DATA_LEN;
		reg_ctrl1.bf.data_compare   = 0;
		reg_ctrl1.bf.pwrKeyIRQCpu   = 0;
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg_ctrl1.wrd);

		reg_pwrkey.bf.pwr_code1     = TV1_KEY_POWER;
		cs75xx_cir_write_reg(CS75XX_CIR_POWER, reg_pwrkey.wrd);

		reg_pwrkey_ext.bf.pwr_code2 = TV1_KEY_POWER_EXT;
		cs75xx_cir_write_reg(CS75XX_CIR_POWEREXT, reg_pwrkey_ext.wrd);

		break;

	case DVB_PROTOCOL:
		reg_ctrl0.bf.demod_en       = 0;
		reg_ctrl0.bf.pos            = 0;
		reg_ctrl0.bf.cir_protocol   = 0;
		reg_ctrl0.bf.rc5_stopBit_en = 0;
		reg_ctrl0.bf.rc5_extend     = 1;
		reg_ctrl0.bf.head_lo_t      = 0;
		reg_ctrl0.bf.head_hi_t      = 0;
#ifdef CONFIG_CORTINA_FPGA
		reg_ctrl0.bf.baud_div       = (DVB_BAUD*EXT_CLK_SRC/EXT_CLK_DIV);
#else
		reg_ctrl0.bf.baud_div       = (DVB_BAUD*EXT_CLK);
#endif
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg_ctrl0.wrd);

		reg_ctrl1.bf.data_len_b     = VCR_DATA_LEN;
		reg_ctrl1.bf.data_compare   = 0;
		reg_ctrl1.bf.pwrKeyIRQCpu   = 0;
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL1, reg_ctrl1.wrd);

		reg_pwrkey.bf.pwr_code1     = DVB_KEY_POWER;
		cs75xx_cir_write_reg(CS75XX_CIR_POWER, reg_pwrkey.wrd);
		reg_pwrkey_ext.bf.pwr_code2 = 0;
		cs75xx_cir_write_reg(CS75XX_CIR_POWEREXT, reg_pwrkey_ext.wrd);

		break;

	default:
		printk("Not Support Mode %d\n", mode);
		return;
	}

	if (reg_ctrl0.bf.fst_por_ok == 0) {
		CIR_PWRCTRL_CIR_INT_STATUS_t reg_int_st;
		CIR_PWRCTRL_PWR_CTRL1_t reg_pwr_ctrl1;
		CIR_PWRCTRL_PWR_INT_ENABLE_t reg_pwr_inten;

		reg_ctrl0.bf.fst_por_ok = 1;
		cs75xx_cir_write_reg(CS75XX_CIR_CTRL0, reg_ctrl0.wrd);

		/* Clear CIR Interrupt */
		reg_int_st.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_ST);
		reg_int_st.bf.pwrkey_int_sts = 1;
		reg_int_st.bf.cir_dat_int = 1;
		reg_int_st.bf.repeat_sts = 1;
		cs75xx_cir_write_reg(CS75XX_CIR_INT_ST, reg_int_st.wrd);

		/* Clear PWR Interrupt */
		cs75xx_cir_write_reg(CS75XX_PWR_INT_EN, 0);

		reg_pwr_ctrl1.wrd = 0;
		reg_pwr_ctrl1.bf.pwr_int_clear = 1;
		cs75xx_cir_write_reg(CS75XX_PWR_CTRL1, reg_pwr_ctrl1.wrd);

		/* Turn-on Interrupt */
		reg_pwr_inten.wrd = cs75xx_cir_read_reg(CS75XX_PWR_INT_EN);
		reg_pwr_inten.bf.cir_pwr_on_en = 1;
		reg_pwr_inten.bf.rtc_wake_en = 1;
		reg_pwr_inten.bf.push_btn_wake_en = 1;
		cs75xx_cir_write_reg(CS75XX_PWR_INT_EN, reg_pwr_inten.wrd);

		/* To Shut-Down, PWR_CTRL1_INIT_FINISH and PWR_CTRL1_SHUT_DOWN
		   can't set at the same time */
		reg_pwr_ctrl1.wrd = 0;
		reg_pwr_ctrl1.bf.sysInitFinish = 1;
		cs75xx_cir_write_reg(CS75XX_PWR_CTRL1, reg_pwr_ctrl1.wrd);

		mdelay(2);	/* 2ms, Not Remove */

		reg_pwr_ctrl1.wrd = 0;
		reg_pwr_ctrl1.bf.swShutdnEn = 1;
		cs75xx_cir_write_reg(CS75XX_PWR_CTRL1, reg_pwr_ctrl1.wrd);

		while(1);	/* stop until the system power down */
	}
}

static int __devinit cs75xx_cir_probe(struct platform_device *pdev)
{
	int rc;
	struct cs75xx_cir *cir = NULL;
	struct resource *res_mem;
	CIR_PWRCTRL_CIR_INT_STATUS_t reg_int_st;

	dev_info(&pdev->dev, "Function: %s, pdev->name = %s\n", __func__, pdev->name);

	cir = kzalloc(sizeof(struct cs75xx_cir), GFP_KERNEL);
	if (!cir) {
		dev_err(&pdev->dev, "\nFunc: %s - can't allocate memory for %s device\n", __func__, "cir");
		rc = -ENOMEM;
		goto fail;
	}
	memset(cir, 0, sizeof(struct cs75xx_cir));
	cir->dev = &pdev->dev;

	/* get the module base address and irq number */
	res_mem = platform_get_resource_byname(pdev, IORESOURCE_IO, "cir");
	if (!res_mem) {
		dev_err(&pdev->dev, "\nFunc: %s - can't get resource %s\n", __func__, "cir");
		rc = ENXIO;
		goto fail;
	}
	cir->base = ioremap(res_mem->start, resource_size(res_mem));
	if (!cir->base) {
		dev_err(&pdev->dev, "\nFunc: %s - unable to remap %s %d memory\n",
		        __func__, "cir", resource_size(res_mem));
		rc = -ENOMEM;
		goto fail;
	}
	dev_info(&pdev->dev, "\tcir_base = 0x%08x, range = %d\n", (u32)cir->base,
	        resource_size(res_mem));

	cir->irq = platform_get_irq_byname(pdev, "irq_cir");
	if (cir->irq == -ENXIO) {
		dev_err(&pdev->dev, "Func: %s - can't get resource %s\n", __func__, "irq_cir");
		rc = ENXIO;
		goto fail;
	}
	dev_info(&pdev->dev, "\tirq_cir = %d\n", cir->irq);

	platform_set_drvdata(pdev, cir);
	cs75xx_cir_dev = pdev;


	/* init */
	dev_info(&pdev->dev, "CIR/PWR Device ID: %06x, rev: %02x\n", CS75XX_CIR_DEV_ID, CS75XX_CIR_REV_ID);
	if (CS75XX_CIR_DEV_ID != (CIR_PWRCTRL_CIR_ID_dft >> 8)) {
		dev_err(&pdev->dev, "CS75XX CIR and PWC Module Not Found!!\n");
		return -ENODEV;
	}

	printk("Cortina CS75XX CIR Initialization\n");

	reg_int_st.wrd = cs75xx_cir_read_reg(CS75XX_CIR_INT_ST);
	if (reg_int_st.bf.first_por) {
		/* First Power On, this section should be executed U-Boot, not
		   enter here in kernel driver loading */
		dev_info(&pdev->dev, "Please Press Power Button\n");
		cs75xx_cir_config(cir, TV1_PROTOCOL);	/* never return */

#ifdef CONFIG_CORTINA_FPGA
		cir->system_power = 0;
#endif
	} else {
		/* Clear INTERRUPT before request_irq */
		reg_int_st.bf.pwrkey_int_sts = 1;
		reg_int_st.bf.cir_dat_int = 1;
		reg_int_st.bf.repeat_sts = 1;
		cs75xx_cir_write_reg(CS75XX_CIR_INT_ST, reg_int_st.wrd);
	}

	if ((rc = request_irq(cir->irq, cs75xx_cir_interrupt, IRQF_DISABLED, "cs75xx_cir", cir)) != 0) {
		dev_err(&pdev->dev, "Error: Register IRQ for CS75XX CIR failed %d\n", rc);
		goto fail;
	}

	cir->precision = 5;		/* unit of 100 ms */
	init_timer(&cir->ntimer);	/* normal key timer */
	cir->ntimer.function = cs75xx_cir_ntrigger_on;
	cir->ntimer.data = (unsigned long)cir;
	cir->trigger = 1;

	init_waitqueue_head(&cir_wait_q);

	misc_register(&cs75xx_cir_miscdev);

	return 0;

fail:
	if (cir) {
		if (cir->base)
			iounmap(cir->base);

		kfree(cir);
	}

	return rc;
}

static int __devexit cs75xx_cir_remove(struct platform_device *pdev)
{
	struct cs75xx_cir *cir = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "Function: %s, pdev->name = %s\n", __func__, pdev->name);

	platform_set_drvdata(pdev, NULL);

	misc_deregister(&cs75xx_cir_miscdev);

	cs75xx_cir_dev = NULL;

	del_timer(&cir->ntimer);
	free_irq(cir->irq, cir);
	iounmap(cir->base);
	kfree(cir);

	return 0;
}

static struct platform_driver cs75xx_cir_platform_driver = {
	.probe	= cs75xx_cir_probe,
	.remove	= __devexit_p(cs75xx_cir_remove),
	.driver	= {
		.owner = THIS_MODULE,
		.name  = CS75XX_CIR_NAME,
	},
};

static int __init cs75xx_cir_init(void)
{
	printk("\n%s\n", __func__);

	return platform_driver_register(&cs75xx_cir_platform_driver);
}

static void __exit cs75xx_cir_exit(void)
{
	printk("\n%s\n", __func__);

	platform_driver_unregister(&cs75xx_cir_platform_driver);
}

module_init(cs75xx_cir_init);
module_exit(cs75xx_cir_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cortina CS75XX CIR driver");

