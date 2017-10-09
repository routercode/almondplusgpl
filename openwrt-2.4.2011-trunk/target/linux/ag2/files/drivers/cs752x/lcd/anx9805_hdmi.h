/*
 *  linux/drivers/video/anx9805_hdmi.h
 *
 * Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
 *                Joe Hsu <joe.hsu@cortina-systems.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 *  ANX9805 HDMI Transmitter
 */
#ifndef __ANX9805_HDMI_H__
#define __ANX9805_HDMI_H__

#define ANX9805_HDMI_DRIVER_NAME	"anx9805_hdmi"

/* Select unused one. see Documentation/ioctl/ioctl-number.txt, group 32-63 */
/* Same magic number with g2_framebuffer */
#define CS75XX_HDMI_IOCTL_MAGIC   0xFF

#define ANX9805_SYS_GET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,64, unsigned int)	/* Read anx9805 system register */
#define ANX9805_SYS_SET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,65, unsigned int)	/* Write anx9805 system register */
#define ANX9805_SYS_SET_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,66, unsigned int)	/* Set anx9805 system reg bits */
#define ANX9805_SYS_CLR_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,67, unsigned int)	/* Clear anx9805 system reg bits */
#define ANX9805_DP_GET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,68, unsigned int)	/* Read anx9805 display port register */
#define ANX9805_DP_SET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,69, unsigned int)	/* Write anx9805 display port register */
#define ANX9805_DP_SET_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,70, unsigned int)	/* Set anx9805 display port reg bits */
#define ANX9805_DP_CLR_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,71, unsigned int)	/* Clear anx9805 display port reg bits */
#define ANX9805_HDMI_GET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,72, unsigned int)	/* Read anx9805 hdmi register */
#define ANX9805_HDMI_SET_REG	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,73, unsigned int)	/* Write anx9805 hdmi register */
#define ANX9805_HDMI_SET_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,74, unsigned int)	/* Set anx9805 hdmi reg bits */
#define ANX9805_HDMI_CLR_BITS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,75, unsigned int)	/* Clear anx9805 hdmi reg bits */
#define ANX9805_EDID_READ	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,76, unsigned int)	/* Read edid data structure */
#define ANX9805_EEDID_READ	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,78, unsigned int)	/* Read e-edid data structure */
#define ANX9805_REG_DELAY_MS	_IOWR(CS75XX_HDMI_IOCTL_MAGIC,80, unsigned int)	/* Set anx9805 reg delay ms */

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

#define ANX9805_MCLK	0x00
#define ANX9805_MODE	0x01
#define ANX9805_TRANS	0x02
#define ANX9805_CHA_VOL	0x03
#define ANX9805_CHB_VOL	0x04

#define	ANX9805_GPIO_INT	91	/* INT - GPIO group 2, bit 27 */

#ifndef HDMI_MINOR
#define HDMI_MINOR	242	/* Documents/devices.txt suggest to use 240~255 for local driver!! */
#endif

/******************************************************************************
 *	ANX9805 HDMI Debug
 ******************************************************************************/
#if 1
#ifdef CONFIG_HDMI_ANX9805_DEBUG
#define CS752X_HDMIDBG_LEVEL 10
#include <linux/spinlock.h>

#define MAX_DBG_INDENT_LEVEL	5
#define DBG_INDENT_SIZE		2
#define MAX_DBG_MESSAGES	0

#define dbg_print(level, format, arg...)                           \
       if (level <= CS752X_HDMIDBG_LEVEL) {                        \
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
#else	/* HDMI_ANX9805_DEBUG */
#define DBGPRINT(level, format, ...)
#endif
#endif

/******************************************************************************
 *	HDMI SPDIF sampling clock
 ******************************************************************************/
#define	HDMI_TX_N_32k	0x1000
#define	HDMI_TX_N_44k	0x1880
#define	HDMI_TX_N_48k	0x1800
#define	HDMI_TX_N_88k	0x3100
#define	HDMI_TX_N_96k	0x3000
#define	HDMI_TX_N_176k	0x6200
#define	HDMI_TX_N_192k	0x6000

/******************************************************************************
 *	Video ID Code Definitions
 ******************************************************************************/
#define	HDMI_TX_640x480p_60HZ		1

#define	HDMI_TX_720x480p_60HZ_4x3	2
#define	HDMI_TX_720x480p_60HZ_16x9	3
#define	HDMI_TX_720x576p_50HZ_4x3	17
#define	HDMI_TX_720x576p_50HZ_16x9	18

/* need repeat time */
#define	HDMI_TX_720x480i_60HZ_4x3	6
#define	HDMI_TX_720x480i_60HZ_16x9	7
#define	HDMI_TX_720x576i_50HZ_4x3	21
#define	HDMI_TX_720x576i_50HZ_16x9	22

/******************************************************************************
 *	ANX9805 HDMI Data Structures
 ******************************************************************************/
struct anx9805_infoframe
{
	u8 type;
	u8 version;
	u8 length;
	u8 pb_byte[28];
};

typedef enum
{
	hdmi_avi_infoframe,
	hdmi_audio_infoframe,
} ANX9805_HDMI_PACKET_TYPE;

#define HDMI_TX_SEL_AVI		0x01	/* select avi config */
#define HDMI_TX_SEL_AUDIO	0x02	/* select audio config */

enum ANX9805_HDMI_STATE
{
	HDMI_INITIAL = 0,
	HDMI_WAIT_HOTPLUG,
	HDMI_PARSE_EDID,	/* no state change. skip */
	HDMI_LINK_TRAINING,
	HDMI_CONFIG_VIDEO,
	HDMI_CONFIG_AUDIO,
	HDMI_CONFIG_PACKET,	/* no state change. skip */
	HDMI_HDCP_AUTH,		/* not support HDCP */
	HDMI_PLAY_BACK
};

union anx9805_ddc {
    unsigned int dat;
    struct {
        unsigned char off;
        unsigned char val;
        unsigned char len;
    } reg;
};

struct anx9805_info
{
	struct miscdevice	*miscdev;	/* miscdevice */

	const struct i2c_device_id *id;

	u32 state;
	u32 delay;

	//union anx9805_ddc ddc;
	//u8 *edid;

	struct {
		u8 blkzero[128];
		u8 *extblk;	/* max: 254/255? */
	} edid;

	struct i2c_client *cli_sys;	/* system: device address=0x72 */
	struct i2c_client *cli_hdmi;	/* hdmi:   device address=0x7a */
	struct i2c_client *cli_dp;	/* display port:   device address=0x70 */

	u8 packets_need_config;
	struct anx9805_infoframe avi_info;	/* Tbl 1-48, AVI InfoFrame */
						/* hdmi 8.2.1 */
	struct anx9805_infoframe audio_info;	/* Tbl 1-49, Audio InfoFrame */
						/* hdmi 8.2.2 */
#if defined(CONFIG_HDMI_ANX9805_GPIO_INTERRUPT)
	struct work_struct	work;	/* delay work */

	u32 int_status[8];	/* Interrupt status: F0-F7 */
#endif
};

static const unsigned char edid_v1_header[] = { 0x00, 0xff, 0xff, 0xff,
						0xff, 0xff, 0xff, 0x00
};

/******************************************************************************
 *	ANX9805 HDMI Device Addresses
 ******************************************************************************/
#define	ANX9805_SYSTEM_D0_ADDR	0x39	/* HDMI/DisplayPort Dev Addr 0 (default) */
#define	ANX9805_SYSTEM_D1_ADDR	0x3b	/* HDMI/DisplayPort Dev Addr 1 */
//#define	ANX9805_HDCP_D0_ADDR	0x70	/* HDCP Device Address 0 */
//#define	ANX9805_HDCP_D1_ADDR	0x78	/* HDCP Device Address 1 */
#define	ANX9805_DP_D0_ADDR	0x38	/* DisplayPort Dev Addr 0 */
#define	ANX9805_DP_D1_ADDR	0x3c	/* DisplayPort Dev Addr 1 */
#define	ANX9805_HDMI_D0_ADDR	0x3d	/* HDMI System Device Address 0 */
#define	ANX9805_HDMI_D1_ADDR	0x3f	/* HDMI System Device Address 1 */

/******************************************************************************
 *	ANX9805 System Registers
 ******************************************************************************/
// ANX9805_SYSTEM_D0_ADDR	0x39	/* HDMI/DisplayPort Dev Addr 0 (default) */
// ANX9805_SYSTEM_D1_ADDR	0x3b	/* HDMI/DisplayPort Dev Addr 1 */
	/* Vendor/Device ID Registers */
	#define	VENDOR_ID_L_REG	0x00	/* 0xAA, Vendor ID low byte */
	#define	VENDOR_ID_H_REG	0x01	/* 0xAA, Vendor ID high byte */
	#define	DEVICE_ID_L_REG	0x02	/* 0x05, Device ID low byte */
		#define	DEVICE_ID_L	0x05	/* 0x05, Device ID low */
	#define	DEVICE_ID_H_REG	0x03	/* 0x98, Device ID high byte */
		#define	DEVICE_ID_H	0x98	/* 0x98, Device ID high */
	#define	DEVICE_VER_REG	0x04	/* 0x01, Device Version */
	/* System Registers */
	#define	SYS_PD_REG	0x05	/* 0x82, Power Down Register */
		#define	PD_REG		0x80
		#define	PD_MISC		0x40
		#define	PD_IO		0x20
		#define	PD_AUDIO	0x10
		#define	PD_VIDEO	0x08
		#define	PD_LINK		0x04
		#define	PD_TOTAL	0x02
		#define	MODE_SEL	0x01
		#define	HDMI_MODE	0x01
	#define SYS_RST1_REG	0x06	/* 0x04, Reset Control 1 Register */
		#define	MISC_RST	0x80
		#define	VID_CAP_RST	0x40
		#define	VID_FIFO_RST	0x20
		#define	AUD_FIFO_RST	0x10
		#define	AUD_RST		0x08
		#define	HDCP_RST	0x04
		#define	SW_RST		0x02
		#define	HW_RST		0x01
	#define SYS_RST2_REG	0x07	/* 0x00, Reset Control 2 Register */
		#define	SSC_RST		0x80
		#define	AC_MODE		0x40
		#define	DDC_RST		0x10
		#define	TMDS_BIST_RST	0x08
		#define	AUX_RST		0x04
		#define	SERDES_FIFO_RST	0x02
		#define	I2C_REG_RST	0x01
	/* Video Configuration and Status Registers */
	#define VIDEO_CTRL1_REG	0x08	/* 0x07, Video Control 1 Register */
		#define	VIDEO_EN	0x80
		#define	VIDEO_MUTE	0x40
		#define	DE_GEN		0x20
		#define	DEMUX		0x10
		#define	BSEL		0x04
		#define	DDR_CTRL	0x02
		#define	EDGE		0x01	// FIXME: modify EDGE name
	#define VIDEO_CTRL2_REG	0x09	/* 0x10, Video Control 2 Register */
		#define	IN_BPC_8bit	0x1	/* Bit per color/component */
		#define	IN_BPC_10bit	0x2	/* Bit per color/component */
		#define	IN_BPC_12bit	0x3	/* Bit per color/component */
	#define VIDEO_CTRL3_REG	0x0a	/* 0x00, Video Control 3 Register */
		#define C_SWAP_RGB	0x00
	#define VIDEO_CTRL4_REG	0x0b	/* 0x00, Video Control 4 Register */
	#define VIDEO_CTRL5_REG	0x0c	/* 0x00, Video Control 5 Register */
	#define VIDEO_CTRL6_REG	0x0d	/* 0x00, Video Control 6 Register */
	#define VIDEO_CTRL7_REG	0x0e	/* 0x00, Video Control 7 Register */
	#define VIDEO_CTRL8_REG	0x0f	/* 0x20, Video Control 8 Register */
	#define VIDEO_CTRL9_REG	0x10	/* 0x00, Video Control 9 Register */
		#define	BRU_WIDTH8		0x10	/* Bus width per pixel component. 0x2=8bits */
	#define VIDEO_CTRL10_REG	0x11	/* 0x00, Video Control 10 Register */
	#define TOTAL_LINE_CFG_L_REG	0x12	/* 0x00, Total Line Config Reg Low */
	#define TOTAL_LINE_CFG_H_REG	0x13	/* 0x00, Total Line Config Reg High */
	#define	ACTIVE_LINE_CFG_L_REG	0x14	/* 0x00, Active Line Config Reg Low */
	#define	ACTIVE_LINE_CFG_H_REG	0x15	/* 0x00, Active Line Config Reg High */
	#define	V_F_PORCH_CFG_REG	0x16	/* 0x00, Vertical Front Porch Config Reg */
	#define	V_SYNC_CFG_REG	0x17	/* 0x00, Vertical Sync Width Config Reg */
	#define	V_B_PORCH_CFG	0x18	/* 0x00, Vertical Back Porch Config Reg */
	#define	TOTAL_PIXEL_CFG_L_REG	0x19	/* 0x00, Total Pixel Config Reg Low */
	#define	TOTAL_PIXEL_CFG_H_REG	0x1a	/* 0x00, Total Pixel Config Reg High */
	#define	ACTIVE_PIXEL_CFG_L_REG	0x1b	/* 0x00, Active Pixel Config Reg Low */
	#define	ACTIVE_PIXEL_CFG_H_REG	0x1c	/* 0x00, Active Pixel Config Reg High */
	#define	H_F_PORCH_CFG_L_REG	0x1d	/* 0x00, Horizontal Front Porch Reg Low */
	#define	H_F_PORCH_CFG_H_REG	0x1e	/* 0x00, Horizontal Front Porch Reg High */
	#define	H_SYNC_CFG_L_REG	0x1f	/* 0x00, Horizontal Sync Width Reg Low */
	#define	H_SYNC_CFG_H_REG	0x20	/* 0x00, Horizontal Sync Width Reg High */
	#define	H_B_PORCH_CFG_L_REG	0x21	/* 0x00, Horizontal Back Porch Reg Low */
	#define	H_B_PORCH_CFG_H_REG	0x22	/* 0x00, Horizontal Back Porch Reg High */
	#define	VIDEO_STS_REG		0x23	/* 0x03, Video Status Register */
	#define	TOTAL_LINE_STA_L_REG	0x24	/* 0x01, Total Line Status Reg Low */
	#define	TOTAL_LINE_STA_H_REG	0x25	/* 0x00, Total Line Status Reg High */
	#define	ACTIVE_LINE_STA_L_REG	0x26	/* 0x00, Active Line Status Reg Low */
	#define	ACTIVE_LINE_STA_H_REG	0x27	/* 0x00, Active Line Status Reg High */
	#define	V_F_PORCH_STA_REG	0x28	/* 0x01, Vertical front porch status */
	#define	V_SYNC_STA_REG		0x29	/* 0x00, Vertical sync width status */
	#define	V_B_PORCH_STA_REG	0x2a	/* 0x00, Vertical Back Porch Status */
	#define	TOTAL_PIXEL_STA_L_REG	0x2b	/* 0x00, Total pixel status low */
	#define	TOTAL_PIXEL_STA_H_REG	0x2c	/* 0x00, Total pixel status high */
	#define	ACTIVE_PIXEL_STA_L_REG	0x2d	/* 0x00, Active pixel status low */
	#define	ACTIVE_PIXEL_STA_H_REG	0x2e	/* 0x00, Active pixel status high */
	#define	H_F_PORCH_STA_L_REG	0x2f	/* 0x00, Horizontal front porch status low */
	#define	H_F_PORCH_STA_H_REG	0x30	/* 0x00, Horizontal front porch status high */
	#define	H_SYNC_STA_L_REG	0x31	/* 0x00, Horizontal sync status low */
	#define	H_SYNC_STA_H_REG	0x32	/* 0x00, Horizontal sync status high */
	#define	H_B_PORCH_STA_L_REG	0x33	/* 0x00, Horizontal back porch status low */
	#define	H_B_PORCH_STA_H_REG	0x34	/* 0x00, Horizontal back porch status high */
	#define	VIDEO_BIST_REG		0x35	/* 0x00, Video Interface BIST Register */
	// Audio Control Registers
	#define	SPDIF_AUDIO_CTL0_REG	0x36	/* 0x00, S/PDIF Audio Control Register 0 */
		#define	AUD_SPDIF_IN		0x80	/* 1=Enable SPDIF audio stream input */
		#define	SPDIF1_SEL		0x08	/* 0=Select SPDIF1 pin0; 1=pin1 */
	#define	SPDIF_AUDIO_CTL1_REG	0x37	/* 0x00, S/PDIF Audio Control Register 1 */
	#define	SPDIF_AUDIO_STA0_REG	0x38	/* 0x00, S/PDIF Audio Status Register 0 */
		#define	SPDIF_CLK_DET		0x80	/* 1=clock detected. Input SPDIF audio clock detected indicator */
		#define	SPDIF_DET		0x01	/* 1=input detected. Detect indicator of SPDIF audio input */
	#define	SPDIF_AUDIO_STA1_REG	0x39	/* 0x00, S/PDIF Audio Status Register 1 */
		#define	SPDIF_FS_FREQ_44	0x00	/* 44.1kHz */
		#define	SPDIF_FS_FREQ_48	0x20	/* 48kHz */
		#define	SPDIF_FS_FREQ_32	0x30	/* 32kHz */
		#define	SPDIF_FS_FREQ_96	0xa0	/* 96kHz */
	// Video Bit Control Registers
	#define	VIDEO_BIT_CTRL0_REG	0x40	/* 0x00, Video Bit Control Reg 0 */
	#define	VIDEO_BIT_CTRL1_REG	0x41	/* 0x01, Video Bit Control Reg 1 */
	#define	VIDEO_BIT_CTRL2_REG	0x42	/* 0x02, Video Bit Control Reg 2 */
	#define	VIDEO_BIT_CTRL3_REG	0x43	/* 0x03, Video Bit Control Reg 3 */
	#define	VIDEO_BIT_CTRL4_REG	0x44	/* 0x04, Video Bit Control Reg 4 */
	#define	VIDEO_BIT_CTRL5_REG	0x45	/* 0x05, Video Bit Control Reg 5 */
	#define	VIDEO_BIT_CTRL6_REG	0x46	/* 0x06, Video Bit Control Reg 6 */
	#define	VIDEO_BIT_CTRL7_REG	0x47	/* 0x07, Video Bit Control Reg 7 */
	/* InfoFrame Registers */
	#define	AVI_TYPE_REG		0x70	/* 0x00, AVI InfoFrame type code */
	#define	AVI_VER_REG		0x71	/* 0x00, AVI InfoFrame version code */
	#define	AVI_LEN_REG		0x72	/* 0x00, AVI InfoFrame length */
	#define	AVI_DATA0_REG		0x73	/* 0x00, AVI Checksum (used in HDMI mode only) */
	#define AVI_DATA1_REG		0x74	/* 0x00, AVI InfoFrame data bytes */
	/* */
	#define	AUDIO_TYPE_REG		0x83	/* 0x00, Audio InfoFrame type code */
		#define	AUDIO_INFOFRAME		0x84	/* Audio InfoFrame */
	#define	AUDIO_VER_REG		0x84	/* 0x00, Audio InfoFrame version code */
		#define	AUDIO_IF_VER1		0x01
	#define	AUDIO_LEN_REG		0x85	/* 0x00, Audio InfoFrame length */
		#define	AUDIO_IF_LEN		0x0A
	#define	AUDIO_DATA0_REG		0x86	/* 0x00, Audio Checksum (used in HDMI mode only) */
	#define AUDIO_DATA1_REG		0x87	/* 0x00, Audio InfoFrame data bytes */
	/* */
	#define	SPD_TYPE_REG		0x91	/* 0x00, SPD InfoFrame type code */
	#define	SPD_VER_REG		0x92	/* 0x00, SPD InfoFrame version code */
	#define	SPD_LEN_REG		0x93	/* 0x00, SPD InfoFrame length */
	#define	SPD_DATA0_REG		0x94	/* 0x00, SPD Checksum (used in HDMI mode only) */
	#define SPD_DATA1_REG		0x95	/* 0x00, SPD InfoFrame data bytes */
	/* */
	#define	MPEG_TYPE_REG		0xb0	/* 0x00, MPEG InfoFrame type code */
	#define	MPEG_VER_REG		0xb1	/* 0x00, MPEG InfoFrame version code */
	#define	MPEG_LEN_REG		0xb2	/* 0x00, MPEG InfoFrame length */
	#define	MPEG_DATA0_REG		0xb3	/* 0x00, MPEG Checksum (used in HDMI mode only) */
	#define MPEG_DATA1_REG		0xb4	/* 0x00, MPEG InfoFrame data bytes */
	//
	#define AUDIO_BIST1_REG		0xd0	/* 0x00, Audio BIST Chan Status Reg 1 */
	#define AUDIO_BIST2_REG		0xd1	/* 0x00, Audio BIST Chan Status Reg 2 */
	#define AUDIO_BIST3_REG		0xd2	/* 0x00, Audio BIST Chan Status Reg 3 */
	#define AUDIO_BIST4_REG		0xd3	/* 0x00, Audio BIST Chan Status Reg 4 */
	#define AUDIO_BIST5_REG		0xd4	/* 0x0b, Audio BIST Chan Status Reg 5 */

	#define	GPIO_CTL_REG		0xd6	/* 0x7?, GPIO Control Register */
	#define	LANE_MAP_REG		0xd7	/* 0xe4, Lane Map Register */
	#define	E_EDID_PTR_DA_REG	0xd9	/* 0x30, E_EDID Pointer Device Address Register */
	#define	EDID_DA_REG		0xda	/* 0x50, EDID Device Address Register */
	#define	MCCS_DA_REG		0xdb	/* 0x37, MCCS Device Address Register */

	// Interrupt Registers
	#define	INT_STATE_REG		0xf0	/* 0x00, Interrupt Status Register */
	#define INT_COM_STS1_REG	0xf1	/* 0x00, Common Interrupt Status Reg 1 */
	#define INT_COM_STS2_REG	0xf2	/* 0x00, Common Interrupt Status Reg 2 */
	#define INT_COM_STS3_REG	0xf3	/* 0x00, Common Interrupt Status Reg 3 */
	#define INT_COM_STS4_REG	0xf4	/* 0x00, Common Interrupt Status Reg 4 */
	#define INT_HDMI_STS1_REG	0xf5	/* 0x00, HDMI Interrupt Status Reg 1 */
	#define INT_HDMI_STS2_REG	0xf6	/* 0x00, HDMI Interrupt Status Reg 2 */
	#define INT_DP_STS1_REG		0xf7	/* 0x00, DP Interrupt Status Register 1 */
	#define INT_COM_MSK1_REG	0xf8	/* 0x00, Interrupt Common Mask Reg 1 */
	#define INT_COM_MSK2_REG	0xf9	/* 0x00, Interrupt Common Mask Reg 2 */
	#define INT_COM_MSK3_REG	0xfa	/* 0x00, Interrupt Common Mask Reg 3 */
	#define INT_COM_MSK4_REG	0xfb	/* 0x00, Interrupt Common Mask Reg 4 */
	#define INT_HDMI_MSK1_REG	0xfc	/* 0x00, Interrupt Hdmi Mask Reg 1 */
	#define INT_HDMI_MSK2_REG	0xfd	/* 0x00, Interrupt Hdmi Mask Reg 2 */
	#define INT_DP_MSK1_REG		0xfe	/* 0x00, Interrupt DP Mask Reg 1 */
	#define	INT_CTL_REG		0xff	/* 0x02, Interrupt Control Reg */

/******************************************************************************
 *	ANX9805 DP Mode Registers
 ******************************************************************************/
// ANX9805_HDCP_D0_ADDR		0x70	/* HDCP Device Address 0 */
// ANX9805_HDCP_D1_ADDR		0x78	/* HDCP Device Address 1 */
	/* Not support */
// ANX9805_DP_D0_ADDR		0x38	/* DisplayPort Device Address 0 */
// ANX9805_DP_D1_ADDR		0x3c	/* DisplayPort Device Address 1 */
	/* DisplayPort System Registers */
	#define	DP_SCTL1_REG	0x80	/* 0x00, DP System Control 1 Register */
	#define	DP_SCTL2_REG	0x81	/* 0x40, DP System Control 2 Register */
	#define	DP_SCTL3_REG	0x82	/* 0x00, DP System Control 3 Register */
		#define HPD_STATUS	0x40	/* Hot plug detect status */
		#define F_HPD		0x20	/* Force Hot plug detect */
		#define HPD_CTRL	0x10	/* Hot plug detect manual control */
	#define	DP_SCTL4_REG	0x83	/* 0x00, DP System Control 4 Register */

/******************************************************************************
 *	ANX9805 HDMI Mode Registers
 ******************************************************************************/
// ANX9805_HDMI_S0_ADDR		0x3d	/* HDMI System Device Address 0 */
// ANX9805_HDMI_S1_ADDR		0x3f	/* HDMI System Device Address 1 */
	/* HDMI System Registers */
	#define	HDMI_RST_REG	0x00	/* 0x00, HDMI Reset Register */
		#define	TMDS_CHNL_ALIGN		0x01	/* TMDS analog four channels clock alignment reset 1=reset */
	#define HDMI_SSTS_REG	0x01	/* 0x00, HDMI System Status Register */
		#define	TXPLL_MISC_LOCK	0x40	/* Lock detected indicator of TX PLL misc control 1=lock detected */
		#define	CLK_DET		0x02	/* Input video clock detected indicator 1=clock detected */
	#define	HDMI_SCTRL1_REG	0x02	/* 0x04, HDMI System Control Register 1 */
	/* HDMI Video Control Registers */
	#define	VH_SYNC_ALIGN1_EN_REG	0x03	/* 0x00, HDMI Video Control Register 1 */
	#define	VH_SYNC_ALIGN2_EN_REG	0x04	/* 0x00, HDMI Video Control Register 2 */
	#define	HDMI_CAP_CTL_REG	0x05	/* 0x00, HDMI Video Capture Control Register */
	#define	HDMI_LNKFMT1_OFF_REG	0x06	/* 0x00, HDMI Link Format Lines Offset Register 1 */
	#define	HDMI_LNKFMT2_OFF_REG	0x07	/* 0x00, HDMI Link Format Lines Offset Register 2 */
	#define	HDMI_AUDIO_CTL1_REG	0x08	/* 0x00, HDMI Audio Control Register 1 */
	/* HDMI Audio Control Register */
	#define	HDMI_AUDIO_CTL_REG	0x09	/* 0x40, HDMI Audio Control Register */
		#define	HDMI_AUD_EN		0x80	/* 1=enable HDMI audio stream on the HDMI link */
		#define	PD_RING_OSC		0x40	/* 1=Powerdown ring oscillator, 0=powerup */
	/* HDMI Link Control Registers */
	#define	HDMI_LNK1_CTL_REG	0x30	/* 0x00, HDMI Link Control Register 1 */
	#define	HDMI_LNK2_CTL_REG	0x31	/* 0x02, HDMI Link Control Register 2 */
	#define	HDMI_LNK_MUTE_EX_EN_REG	0x32	/* 0x00, HDMI Link Mute Exception Enable Register */
	#define	SERDES_TEST_PTRN0_REG	0x33	/* 0x00, TMDS SerDes Functional Test Pattern Register 1 [7:0] */
	#define	SERDES_TEST_PTRN1_REG	0x34	/* 0x00, TMDS SerDes Functional Test Pattern Register 2 [15:8] */
	#define	SERDES_TEST_PTRN2_REG	0x35	/* 0x00, TMDS SerDes Functional Test Pattern Register 3 [19:16] */
	#define	PLL_MISC_CTL1_REG	0x38	/* 0xb0, Chip PLL Miscellaneous Control Register 1 */
		#define	MISC_TIMER_SEL		0x10
		#define	MISC_RNGCHK_EN		0x01
	#define	PLL_MISC_CTL2_REG	0x39	/* 0x80, Chip PLL Miscellaneous Control Register 2 */
		#define	MISC_MODE_SEL		0x20
		#define	FORCE_PLLF_LOCK		0x02
		#define	MISC_TXPLL_MAN_RNG	0x01
	#define	VID_FREQ_CNT_REG	0x3a	/* 0x00, Video Input Clock Frequency Counter Register */
	#define	DDC_DEV_ADDR_REG	0x40	/* 0x00, DDC Slave Device Address Register */
	#define	DDC_SEG_ADDR_REG	0x41	/* 0x00, DDC Slave Device Segment Address Register */
	#define	DDC_OFFSET_ADDR_REG	0x42	/* 0x00, DDC Slave Device Offset Address Register */
	#define	DDC_ACCESS_CMD_REG	0x43	/* 0x00, DDC Access Command Register */
		#define DDC_I2C_RESET		0x06	/* 110=I2C reset command */
		#define CLR_DDC_DFIFO		0x05	/* 101=Clear DDC Data FIFO */
		#define ESEQ_BYTE_READ		0x04	/* 100=Enhanced DDC Sequential Read */
		#define DDC_HDCP_IOAR		0x03	/* 011=Implicit Offset Address Read (HDCP) */
		#define SEQ_BYTE_WRITE		0x02	/* 010=Sequential Byte Write */
		#define SEQ_BYTE_READ		0x01	/* 001=Sequential Byte Read */
		#define SINGLE_BYTE_READ	0x81	/* 001=Single Byte Read */
		#define DDC_ABORT_OP		0x00	/* 000=Abort current operation */
	#define	DDC_ACCESS_NUM0_REG	0x44	/* 0x00, DDC Access Number Register 0 B[7:0] */
	#define	DDC_ACCESS_NUM1_REG	0x45	/* 0x00, DDC Access Number Register 1 B[9:8] */
	#define	DDC_CH_STATUS_REG	0x46	/* 0x10, DDC Channel Status Register */
		#define DDC_ERROR		0x80	/* An error has occurred when accessing the DDC channel */
		#define DDC_OCCUPY		0x40	/* DDC channel is accessed by an external device */
		#define DDC_FIFO_FULL		0x20	/* Indicator of FIFO full status */
		#define DDC_FIFO_EMPTY		0x10	/* Indicator of FIFO empty status */
		#define DDC_NO_ACK		0x08	/* No acknowledge detection has occurred when accessing the DDC cahnnel */
		#define DDC_READ		0x04	/* Indicator of FIFO being read */
		#define DDC_WRITE		0x02	/* Indicator of FIFO being written */
		#define DDC_PROGRESS		0x01	/* Indicator of DDC operation in process */
	#define	DDC_FIFO_DATA_REG	0x47	/* 0x00, DDC FIFO Access Register */
	#define	DDC_FIFO_CNT_REG	0x48	/* 0x00, DDC FIFO ACount Register */
		#define DDC_FIFO_MAX_CNT	0x1f	/* The number of bytes in the DDC FIFO */
	#define	DBG_LINK_FSM1_REG	0x49	/* 0x00, Debug Shared/Chip Link FSM Debug Status Register 1 */
	#define	DBG_LINK_FSM2_REG	0x4a	/* 0x00, Debug Shared/Chip Link FSM Debug Status Register 2 */

	#define	HDMI_DBG_CTRL1_REG	0x4c	/* 0x00, HDMI Debug Control Register 1 */
	#define	TMDS_PD_REG		0x60	/* 0x00, System Power Down Register */
	#define	TMDS_CH_CFG1_REG	0x61	/* 0x0c, HDMI TMDS Channel Configuration Register 1 */
	#define	TMDS_CH_CFG2_REG	0x62	/* 0x0c, HDMI TMDS Channel Configuration Register 2 */
	#define	TMDS_CH_CFG3_REG	0x63	/* 0x0c, HDMI TMDS Channel Configuration Register 3 */
	#define	TMDS_CH_CFG4_REG	0x64	/* 0x0c, HDMI TMDS Channel Configuration Register 4 */
		#define	TMDS_CLK_MUTE_CTRL	0x40	/* 1=don't mute TMDS clock */
	#define	HDMI_CHIP_CTRL_REG	0x65	/* 0x02, HDMI Chip Control Register */
	#define	HDMI_CHIP_STS_REG	0x66	/* 0x0?, 000011?0b HDMI Chip Status Register */
	#define	HDMI_CHIP_DCTRL1_REG	0x67	/* 0x30, HDMI Chip Debug Control Register 1 */
		#define HDMI_FORCE_HOTPLUG	0x01	/* Force Hot plug detect. for debug use only */

	#define	IF_PKT_CTRL1_REG	0x70	/* 0x00, InfoFrame Packet Control Register 1 */
		#define	AVI_PKT_RPT		0x20	/* 1=enable control of AVI packet transmission in every VBLANK period */
		#define	AVI_PKT_EN		0x10	/* 1=enable control of AVI packet transmission */
		#define	GCP_PKT_RPT		0x08	/* 1=Enable control of Control Packet transmission in every VBLANK period */
		#define	GCP_PKT_EN		0x04	/* 1=Enable control of Control Packet transmission */
		#define	ACR_PKT_NEW		0x02	/* 1=only new CTS value packet is sent */
		#define	ACR_PKT_EN		0x01	/* 1=enable control of ACR packet transmission */
	#define	IF_PKT_CTRL2_REG	0x71	/* 0x00, InfoFrame Packet Control Register 2 */
		#define	AIF_PKT_RPT		0x02	/* 1=enable control of audio InfoFrame packet transmission in every VBLANK period */
		#define	AIF_PKT_EN		0x01	/* 1=enable control of audio InfoFrame packet transmission */
	#define	ACR_SVAL1_REG		0x72	/* 0x00, ACR N Software Value Register 1 */
	#define	ACR_SVAL2_REG		0x73	/* 0x00, ACR N Software Value Register 2 */
	#define	ACR_SVAL3_REG		0x74	/* 0x00, ACR N Software Value Register 3 */
	#define	ACR_CTS_SVAL1_REG	0x75	/* 0x00, ACR CTS Software Value Register 1 */
	#define	ACR_CTS_SVAL2_REG	0x76	/* 0x00, ACR CTS Software Value Register 2 */
	#define	ACR_CTS_SVAL3_REG	0x77	/* 0x00, ACR CTS Software Value Register 3 */
	#define	ACR_CTS_HVAL1_REG	0x78	/* 0x00, ACR CTS Hardware Value Register 1 */
	#define	ACR_CTS_HVAL2_REG	0x79	/* 0x00, ACR CTS Hardware Value Register 2 */
	#define	ACR_CTS_HVAL3_REG	0x7a	/* 0x00, ACR CTS Hardware Value Register 3 */
	#define	ACR_CTS_CTL_REG		0x7b	/* 0x10, ACR CTS Control Register */
	#define	GEN_CTL_PKT_REG		0x7c	/* 0x80, General Control Packet Register */
	#define	AUDIO_PKT_FL_CTL_REG	0x7d	/* 0x30, Audio Packet Flatline Control Register */
	#define	GEN_CTL_PKT_HID_REG	0x7e	/* 0x03, General Control Packet Header ID Register */
	#define	AUDIO_PKT_HID_REG	0x7f	/* 0x02, Audio Packet Header ID Register */

	#define	CEC_CTL_REG		0x80	/* 0x00, CEC Control Register */
	#define	CEC_REC_STS_REG		0x81	/* 0x10, CEC Receiver Status Register */
	#define	CEC_TXR0_STS_REG	0x82	/* 0x10, CEC Transmitter Status Register 0 */
	#define	CEC_TXR1_STS_REG	0x83	/* 0x00, CEC Transmitter Status Register 1 */
	#define	CEC_SPD_CTL_REG		0x84	/* 0x00, CEC Speed Control Register */
		/* 7:4: 0x0=28MHz crystal */

#endif		/* __ANX9805_HDMI_H__ */
