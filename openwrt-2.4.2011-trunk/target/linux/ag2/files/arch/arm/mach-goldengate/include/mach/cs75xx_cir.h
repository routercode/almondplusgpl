/*
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _CS752X_CIR_H
#define _CS752X_CIR_H

#include <linux/ioctl.h>

#define CS75XX_CIR_NAME		"cs75xx-cir"

#define VCR_KEY_POWER		0x613E609F
#define TV1_KEY_POWER		0x40040100
#define TV1_KEY_POWER_EXT	0xBCBD
#define DVB_KEY_POWER		0x38000000

#define VCR_H_ACT_PER		(16-1)
#define VCR_L_ACT_PER		(8-1)
#define VCR_DATA_LEN		(32-1)
#define TV1_H_ACT_PER		(8-1)
#define TV1_L_ACT_PER		(4-1)
#define TV1_DATA_LEN		(48-1)

#define VCR_BAUD		540 	/* us */
#define TV1_BAUD		430	/* us */
#define DVB_BAUD		830	/* us */

#ifdef CONFIG_CORTINA_FPGA
#define	EXT_CLK_SRC		104	/* MHz */
#define	EXT_CLK_DIV		12
#else
#define	EXT_CLK			24	/* MHz */
#endif

#define VCR_PROTOCOL		0x0	/* KOKA KUC-100 VCR-33 */
#define TV1_PROTOCOL		0x1	/* KOKA KUC-100 TV1-26 */
#define DVB_PROTOCOL		0x2	/* white brand DVB */

struct cir_ioctl_data {
	unsigned int data;
};
struct cir_ioctl_data48 {
	unsigned int timeout;
	unsigned int length;
	unsigned int data;
	unsigned int data_ext;
	unsigned char ret;
};
#define OLD_DATA			0
#define NEW_RECEIVE			1

#define CIR_IOCTL_BASE                  ('I'|'R')
#define CIR_SET_BAUDRATE                _IOW (CIR_IOCTL_BASE,  0, struct cir_ioctl_data)
#define CIR_SET_HIGH_PERIOD             _IOW (CIR_IOCTL_BASE,  1, struct cir_ioctl_data)
#define CIR_SET_LOW_PERIOD              _IOW (CIR_IOCTL_BASE,  2, struct cir_ioctl_data)
#define CIR_SET_RC5_EXTEND              _IOW (CIR_IOCTL_BASE,  3, struct cir_ioctl_data)
#define CIR_SET_RC5_STOPBIT             _IOW (CIR_IOCTL_BASE,  4, struct cir_ioctl_data)
#define CIR_SET_PROTOCOL                _IOW (CIR_IOCTL_BASE,  5, struct cir_ioctl_data)
#define CIR_SET_POSITIVE_POLARITY       _IOW (CIR_IOCTL_BASE,  6, struct cir_ioctl_data)
#define CIR_SET_ENABLE_DEMOD            _IOW (CIR_IOCTL_BASE,  7, struct cir_ioctl_data)
#define CIR_SET_POWER_KEY_HANDLE        _IOW (CIR_IOCTL_BASE,  8, struct cir_ioctl_data)
#define CIR_SET_ENABLE_COMPARE          _IOW (CIR_IOCTL_BASE,  9, struct cir_ioctl_data)
#define CIR_SET_DATA_LEN                _IOW (CIR_IOCTL_BASE, 10, struct cir_ioctl_data)
#define CIR_SET_POWER_KEY               _IOW (CIR_IOCTL_BASE, 11, struct cir_ioctl_data48)
#define CIR_SET_CONFIGURATION_SEL       _IOW (CIR_IOCTL_BASE, 12, struct cir_ioctl_data48)
#define CIR_GET_BAUDRATE                _IOR (CIR_IOCTL_BASE, 13, struct cir_ioctl_data)
#define CIR_GET_HIGH_PERIOD             _IOR (CIR_IOCTL_BASE, 14 ,struct cir_ioctl_data)
#define CIR_GET_LOW_PERIOD              _IOR (CIR_IOCTL_BASE, 15 ,struct cir_ioctl_data)
#define CIR_GET_RC5_EXTEND              _IOR (CIR_IOCTL_BASE, 16, struct cir_ioctl_data)
#define CIR_GET_RC5_STOPBIT             _IOR (CIR_IOCTL_BASE, 17, struct cir_ioctl_data)
#define CIR_GET_PROTOCOL                _IOR (CIR_IOCTL_BASE, 18, struct cir_ioctl_data)
#define CIR_GET_POSITIVE_POLARITY       _IOR (CIR_IOCTL_BASE, 19, struct cir_ioctl_data)
#define CIR_GET_ENABLE_DEMOD            _IOR (CIR_IOCTL_BASE, 20, struct cir_ioctl_data)
#define CIR_GET_POWER_KEY_HANDLE        _IOR (CIR_IOCTL_BASE, 21, struct cir_ioctl_data)
#define CIR_GET_ENABLE_COMPARE          _IOR (CIR_IOCTL_BASE, 22, struct cir_ioctl_data)
#define CIR_GET_DATA_LEN                _IOR (CIR_IOCTL_BASE, 23, struct cir_ioctl_data48)
#define CIR_GET_POWER_KEY               _IOR (CIR_IOCTL_BASE, 24, struct cir_ioctl_data48)
#define CIR_GET_DATA                    _IOWR (CIR_IOCTL_BASE, 25, struct cir_ioctl_data48)
#define CIR_WAIT_INT_DATA               _IOWR (CIR_IOCTL_BASE, 26, struct cir_ioctl_data48)
#define CIR_SET_PRECISION               _IOW (CIR_IOCTL_BASE, 27, struct cir_ioctl_data)
#define CIR_GET_PRECISION               _IOR (CIR_IOCTL_BASE, 28, struct cir_ioctl_data)

#endif /* _CS752X_CIR_H */
