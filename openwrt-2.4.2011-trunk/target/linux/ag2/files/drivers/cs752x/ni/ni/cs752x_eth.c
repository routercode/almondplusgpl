/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/dma-mapping.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/crc32.h>
#include <linux/string.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include <asm/checksum.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <linux/in6.h>
#include <linux/udp.h>
#include <linux/phy.h>
#include <linux/workqueue.h>
#include <linux/ethtool.h>
#include "cs752x_eth.h"
#include "cs75xx_ethtool.h"
#include "cs752x_ioctl.h"
#include "cs75xx_phy.h"
#include "cs_fe.h"
#include "cs75xx_tm.h"
#include "cs752x_sch.h"
#include "cs75xx_ne_irq.h"
#include "cs_vtable.h"

#ifdef CONFIG_CS752X_PROC
#define DBG(x) {if (cs_ni_debug == DBG_NI || cs_ni_debug == DBG_NI_IRQ) x;}
#else
#define DBG(x) {}
#endif

static const u32 default_msg = (NETIF_MSG_DRV		| \
				NETIF_MSG_PROBE		| \
	 			NETIF_MSG_LINK		| \
	 			NETIF_MSG_TIMER		| \
	 			NETIF_MSG_IFDOWN	| \
	 			NETIF_MSG_IFUP		| \
	 			NETIF_MSG_RX_ERR	| \
	 			NETIF_MSG_TX_ERR);
static int debug = -1;	/* defaults above */
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");

#define CS752X_NI_NAPI 1

#ifdef CS752X_NI_NAPI
#define NETIF_RX(skb)	netif_receive_skb(skb)
#else
#define NETIF_RX(skb)	netif_rx(skb)
#endif

#ifndef CONFIG_INTR_COALESCING
#define CONFIG_INTR_COALESCING 1
#endif

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
#include "cs752x_virt_ni.h"
#endif /* CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE */

#ifdef CONFIG_CS752X_HW_ACCELERATION
#include "cs_hw_accel_core.h"
#endif /* CONFIG_CS752X_HW_ACCELERATION */

#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
#include "cs_hw_accel_util.h"
#include "cs_vtable.h"
#include "cs_hw_accel_qos.h"
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
#include "cs_hw_accel_ipsec.h"
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#endif /* 0 */

int cs_ni_debug_dump_packet = 0;
ni_info_t ni_private_data;
static int ne_initialized = 0;
static int ne_irq_register = 0;
static int active_dev = 0;
spinlock_t active_dev_lock;

spinlock_t mdio_lock;
volatile static u32 reg = 0;
u8 eth_mac[GE_PORT_NUM][6] = {{0x00, 0x50, 0xc2, 0x11, 0x22, 0x33},
				{0x00, 0x50, 0xc2, 0x44, 0x55, 0x66},
				{0x00, 0x50, 0xc2, 0x77, 0x88, 0x99}};
EXPORT_SYMBOL(eth_mac);

#define XRAM_PTR_WRAP_ARND(ptr, base, top_addr, new_value)	\
	if (((u32)ptr - (u32)base) >= ((top_addr + 1) << 3))	\
		(ptr = (u32*)new_value)

typedef enum {
	CPU_XRAM_CFG_PROFILE_SIMPLE_DEFAULT = 0,
	CPU_XRAM_CFG_PROFILE_MAX
} cpu_xram_config_profile_e;

unsigned int
	cpuxram_rx_addr_cfg_prof[CPU_XRAM_CFG_PROFILE_MAX][XRAM_RX_INSTANCE + 1]
		= {{581, 581, 581, 7, 7, 7, 7, 7, 56}};
unsigned int
	cpuxram_tx_addr_cfg_prof[CPU_XRAM_CFG_PROFILE_MAX][XRAM_TX_INSTANCE]
		= {{214, 0}};
#define XRAM_CFG_PROF	CPU_XRAM_CFG_PROFILE_SIMPLE_DEFAULT

#ifdef CS752X_MULTIQUEUE_SUPPORT
//static uint16_t ni_select_queue(struct net_device *dev, struct sk_buff *skb);
#endif
extern int cs_ne_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
extern int cs_ni_init_proc(void);
extern void cs_ni_fini_proc(void);
extern void sch_init(void);
static int cs_mdiobus_read(struct mii_bus *mii_bus, int phy_addr, int reg_addr);
static int cs_mdiobus_write(struct mii_bus *mii_bus, int phy_addr,
			    int reg_addr, u16 val);
extern int netif_rx_cs(struct sk_buff *skb);

static int sw_qm_total_count[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
spinlock_t sw_qm_cnt_lock;

static const struct port_cfg_info {
	u8 auto_nego;
	u16 speed;
	u8 full_duplex;
	u8 flowctrl;
	u8 phy_mode;
	u8 port_id;
	u8 phy_addr;
	u8 irq;
	u8 *mac_addr;
} port_cfg_infos[] = {
	[GE_PORT0_CFG] = {
		.auto_nego = AUTONEG_ENABLE,
#ifdef CONFIG_CORTINA_FPGA
		.speed = SPEED_100,
		.irq = IRQ_NET_ENG,
#else
		.speed = SPEED_1000,
		.irq = IRQ_NI_RX_XRAM0,
#endif
		.full_duplex = DUPLEX_FULL,
		.flowctrl =  FLOW_CTRL_TX | FLOW_CTRL_RX,
		.phy_mode = NI_MAC_PHY_RMII_100,
		.port_id = GE_PORT0,
		.phy_addr = GE_PORT0_PHY_ADDR,
		.mac_addr = (&(eth_mac[0][0])),
	},
	[GE_PORT1_CFG] = {
#ifdef CONFIG_CORTINA_FPGA
		.auto_nego = AUTONEG_ENABLE,
		.speed = SPEED_100,
		.flowctrl =  FLOW_CTRL_TX | FLOW_CTRL_RX,
		.phy_mode = NI_MAC_PHY_RGMII_100,
		.irq = IRQ_NET_ENG,
#else
		.auto_nego = AUTONEG_DISABLE,
		.speed = SPEED_1000,
		.flowctrl = 0,
		.phy_mode = NI_MAC_PHY_RGMII_1000,
		.irq = IRQ_NI_RX_XRAM1,
#endif
		.full_duplex = DUPLEX_FULL,
		.port_id = GE_PORT1,
		.phy_addr = GE_PORT1_PHY_ADDR,
		.mac_addr = (&(eth_mac[1][0])),
	},
	[GE_PORT2_CFG] = {
		.auto_nego = AUTONEG_ENABLE,
#ifdef CONFIG_CORTINA_FPGA
		.speed = SPEED_100,
		.irq = IRQ_NET_ENG,
#else
		.speed = SPEED_1000,
		.irq = IRQ_NI_RX_XRAM2,
#endif
		.full_duplex = DUPLEX_FULL,
		.flowctrl =  FLOW_CTRL_TX | FLOW_CTRL_RX,
		.phy_mode = NI_MAC_PHY_RGMII_100,
		.port_id = GE_PORT2,
		.phy_addr = GE_PORT2_PHY_ADDR,
		.mac_addr = (&(eth_mac[2][0])),
	}
};

extern struct cs_ne_irq_info cs_ne_global_irq_info;

static u32 calc_crc(u32 crc, u8 const *p, u32 len)
{
	int i;

	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
	}
	return crc;
}

#ifdef CS752X_MANAGEMENT_MODE
/* this internal API is used by cs_mfe_start_xmit(), the management mode
 * FE xmit function */
static int ni_cpu_tx_packet(struct sk_buff *skb, u16 queue_id)
{
	HEADER_A_T ni_header_a;
	u16 pkt_len;

	if (skb == NULL)
		return -1;

	pkt_len = skb->len;

	/* Create and populate header A */
	memset(&ni_header_a, 0, sizeof(HEADER_A_T));
	ni_header_a.pkt_size = pkt_len;
	ni_header_a.fwd_type = CS_FWD_BYPASS; /* Always bypass */
	ni_header_a.pspid = 3; /* CPU */
	ni_header_a.dvoq = queue_id;
	ni_header_a.flags = 0x800;
#if 0 /* the following is commented out, because they've been reset to 0 */
	ni_header_a.original_lspid = 0;
	/* Should be marked valid when pspid==Mcast or Mirror */
	ni_header_a.original_lspid_valid = 0;
	ni_header_a.recirc_idx = 0;
	ni_header_a.mc_grp_id = 0;
	ni_header_a.mc_index = 0;
	ni_header_a.cpu_ptp_flag = 0;
	ni_header_a.ts_flag = 0;
	ni_header_a.mark = 0;
	ni_header_a.addr_cnt = 0;
	ni_header_a.bypass_cos = 0;
	ni_header_a.no_crc = 0;
	ni_header_a.no_stuff = 0;
	ni_header_a.ni_port_id = 0;
	ni_header_a.replace_l4_chksum = 0;
#endif

	return ni_special_start_xmit(skb, &ni_header_a);
} /* ni_cpu_tx_packet */
#endif /* CS752X_MANAGEMENT_MODE */

/*
* Write MII register
* phy_addr -> physical address
* reg_addr -> register address
* value -> value to be write
*/
int ni_mdio_write(int phy_addr, int reg_addr, u16 value)
{
	u16 poll_time = 0;
	PER_MDIO_CFG_t cfg;
	PER_MDIO_ADDR_t addr;
	PER_MDIO_CTRL_t ctrl;
	PER_MDIO_WRDATA_t wrdata;
	//PER_GPIO0_CFG_t gpio_cfg;
	//PER_GPIO0_OUT_t gpio_out;

	if (phy_addr > PER_MDIO_MAX_PHY_ADDR) {
		printk("Invalid Phy Addr < 64");
		return -1;
	}

	if (reg_addr > PER_MDIO_MAX_REG_ADDR) {
		printk("Invalid MDIO Regiser Addr < 32");
		return -1;
	}

	cfg.wrd = readl(PER_MDIO_CFG);
	/* Set the MDIO mode as Manual */
	cfg.bf.mdio_manual = 1;
	writel(cfg.wrd, PER_MDIO_CFG);
#if 0
	/* Drive GPIO according to the Phy address passed */
	gpio_cfg.wrd = readl(PER_GPIO0_CFG);
	gpio_cfg.bf.iopmod |= 0x1; /* Configure GPIO[0] as output */
	writel(gpio_cfg.wrd, PER_GPIO0_CFG);
	gpio_out.wrd = readl(PER_GPIO0_OUT);
	if (phy_addr > PER_MDIO_SELECT_GPIO) { /* more then 32 Phy selected */
		/* GPIO 0 is used for MDIO select. 1-> Upper 32 Phys are
		 * selected */
		gpio_out.wrd |= 0x1; /* Drive GPIO[0] to high */
	} else {
		gpio_out.wrd &= ~0x1; /* Drive GPIO[0] to low */
	}

	writel(gpio_out.wrd, PER_GPIO0_OUT);
#endif				/* #if 0 */
	addr.wrd = readl(PER_MDIO_ADDR);
	/* Set the Operation as Write */
	addr.bf.mdio_rd_wr = mdio_rd_wr_WR;
	/* Set the MDIO Address */
	addr.bf.mdio_offset = reg_addr;
	/* Set the PHY Address */
	addr.bf.mdio_addr = phy_addr;
	writel(addr.wrd, PER_MDIO_ADDR);

	ctrl.wrd = readl(PER_MDIO_CTRL);
	wrdata.wrd = readl(PER_MDIO_WRDATA);
	wrdata.bf.mdio_wrdata = value;
	writel(wrdata.wrd, PER_MDIO_WRDATA);

	ctrl.bf.mdiodone = 0;
	/* Start the MDIO Operation */
	ctrl.bf.mdiostart = 1;
	writel(ctrl.wrd, PER_MDIO_CTRL);

	poll_time = PER_MDIO_POLL_TIME;
	do {
		ctrl.wrd = readl(PER_MDIO_CTRL);
		if (ctrl.bf.mdiodone)
			break;
	} while (poll_time--);
	if (!poll_time)
		return -1;

	/* Clear MDIO done */
	ctrl.bf.mdiodone = 1;
	writel(ctrl.wrd, PER_MDIO_CTRL);

	return 1;
}
EXPORT_SYMBOL(ni_mdio_write);

/*
* Read MII register
* phy_addr -> physical address
* reg_addr -> register address
*/
int ni_mdio_read(int phy_addr, int reg_addr)
{
	int value = -1;
	u16 poll_time = 0;
	PER_MDIO_CFG_t cfg;
	PER_MDIO_ADDR_t addr;
	PER_MDIO_CTRL_t ctrl;
	//PER_GPIO0_CFG_t gpio_cfg;
	//PER_GPIO0_OUT_t gpio_out;

	if (phy_addr > PER_MDIO_MAX_PHY_ADDR) {
		printk("Invalid Phy Addr < 64");
		return -1;
	}

	if (reg_addr > PER_MDIO_MAX_REG_ADDR) {
		printk("Invalid MDIO Regiser Addr < 32");
		return -1;
	}

	cfg.wrd = readl(PER_MDIO_CFG);
	/* Set the MDIO mode as Manual */
	cfg.bf.mdio_manual = 1;
	writel(cfg.wrd, PER_MDIO_CFG);
#if 0
	/* Drive GPIO according to the Phy address passed */
	gpio_cfg.wrd = readl(PER_GPIO0_CFG);
	gpio_cfg.bf.iopmod |= 0x1; /* Configure GPIO[0] as output */
	writel(gpio_cfg.wrd, PER_GPIO0_CFG);
	gpio_out.wrd = readl(PER_GPIO0_OUT);
	if (phy_addr > PER_MDIO_SELECT_GPIO) {
		/* more then 32 Phy selected */
		/* GPIO 0 is used for MDIO select. 1-> Upper 32 Phys are */
		/* selected */
		gpio_out.wrd |= 0x1; /* Drive GPIO[0] to high */
	} else {
		gpio_out.wrd &= ~0x1; /* Drive GPIO[0] to low */
	}

	writel(gpio_out.wrd, PER_GPIO0_OUT);
#endif				/* #if 0 */
	addr.wrd = readl(PER_MDIO_ADDR);
	/* Set the Operation as Read */
	addr.bf.mdio_rd_wr = mdio_rd_wr_RD;
	/* Set the MDIO Address */
	addr.bf.mdio_offset = reg_addr;
	/* Set the PHY Address */
	addr.bf.mdio_addr = phy_addr;
	writel(addr.wrd, PER_MDIO_ADDR);

	ctrl.wrd = readl(PER_MDIO_CTRL);

	//ctrl.bf.mdiodone = 0;
	//printk("ctrl.bf.mdiodone = 0x%X\n",ctrl.bf.mdiodone);
	/* Start the MDIO Operation */
	ctrl.bf.mdiostart = 1;
	writel(ctrl.wrd, PER_MDIO_CTRL);

	poll_time = PER_MDIO_POLL_TIME;
	do {
		ctrl.wrd = readl(PER_MDIO_CTRL);
		if (ctrl.bf.mdiodone)
			break;
	} while (poll_time--);
	if (!poll_time)
		return -1;

	value = readl(PER_MDIO_RDDATA) & 0xffff;

	/* Clear MDIO done */
	ctrl.bf.mdiodone = 1;
	writel(ctrl.wrd, PER_MDIO_CTRL);

	return value;
}
EXPORT_SYMBOL(ni_mdio_read);

static inline void cs_ni_alloc_linux_free_buffer(struct net_device *dev,
						 int qid, int cnt)
{
	int i;
	struct sk_buff *skb;
	volatile u32 phy_addr = 0;
	u32 hw_cnt;
#ifdef CONFIG_DEBUG_KMEMLEAK
	ni_skb_list_t *skb_list_entry;
#endif
	cs_qm_get_cpu_free_buffer_cnt(qid, &hw_cnt);

	cnt = min_t(u16, LINUX_FREE_BUF_LIST_SIZE - sw_qm_total_count[qid],
		    LINUX_FREE_BUF_LIST_SIZE - hw_cnt);
	DBG(printk("%s::alloc queue %d, buf cnt %d, hw_cnt %d, sw_ttl_cnt %d\n",
		 __func__, qid, cnt, hw_cnt, sw_qm_total_count[qid]));

	for (i = 0; i < cnt; i++) {
		skb = netdev_alloc_skb(dev, SKB_PKT_LEN + 0x100);
		/* 0x100 is reserve space */
		if (!skb) {
			printk(KERN_WARNING "%s: Could only allocate %d "
			       "receive skb(s).\n", dev->name, cnt);
			break;
		}

		/* first 256 bytes aligned address from skb->head */
		skb->data = (unsigned char *)((u32)
				(skb->head + 0x100) & 0xffffff00);
		REG32((u32)(skb->data-4)) = (unsigned long)skb;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
		skb->tail = skb->data - skb->head;
#else
		skb->tail = skb->data;
#endif
		/* for FPGA QM need strip MSB, ASIC don't need
		 * QM will jump to next 128 bytes aligned place for storing
		 * data */
		/* FIXME: support ACP need modify */
		phy_addr = virt_to_phys(skb->head);
		//phy_addr = dma_map_single(NULL, skb->head,
		//				SKB_PKT_LEN + 0x100,
		//				DMA_FROM_DEVICE);

		/* Write to SDRAM address, physical adress */
		/* Invalid inner cache */
#ifndef ACP_QM
		//dmac_flush_range(skb->data, skb->data + SKB_PKT_LEN + 0x100);
#if 0
		rx_dma_addr = dma_map_single(NULL, skb->head,
						SKB_PKT_LEN,
						DMA_BIDIRECTIONAL);
		dma_sync_single_for_device(dev, rx_dma_addr, SKB_PKT_LEN,
						DMA_FROM_DEVICE);
#endif
#endif
		/* Invalid outer cache */
		//outer_flush_range(__pa(skb), __pa(skb)+SKB_PKT_LEN);
#ifdef CONFIG_CORTINA_FPGA
		writel(phy_addr & 0x0FFFFFFF,
		       QM_CPU_PATH_LINUX0_SDRAM_ADDR + (qid * 4));
#else
#ifdef ACP_QM
		//writel(phy_addr | GOLDENGATE_ACP_BASE,
		//	QM_CPU_PATH_LINUX0_SDRAM_ADDR + (qid * 4));
		writel(phy_addr, QM_CPU_PATH_LINUX0_SDRAM_ADDR + (qid * 4));
#else
		writel(phy_addr, QM_CPU_PATH_LINUX0_SDRAM_ADDR + (qid * 4));
#endif
#endif
#ifdef CONFIG_DEBUG_KMEMLEAK
		skb_list_entry = kmalloc(sizeof(ni_skb_list_t), GFP_ATOMIC);
		memset(skb_list_entry, 0x0, sizeof(ni_skb_list_t));
		skb_list_entry->skb = skb;
		list_add(&skb_list_entry->list,
				&ni_private_data.ni_skb_list_head[qid]);
#endif
	}
	spin_lock(&sw_qm_cnt_lock);
	sw_qm_total_count[qid] += i;
	spin_unlock(&sw_qm_cnt_lock);
}

void write_reg(u32 data, u32 bit_mask, u32 address)
{
	volatile u32 reg_val;

	reg_val = (readl(address) & (~bit_mask)) | (data & bit_mask);
	writel(reg_val, address);
	return;
}

/*
 * Handles NE interrupts.
 */
static irqreturn_t ni_generic_interrupt(int irq, void *dev_instance)
{
	u32 status;

	status = readl(GLOBAL_NETWORK_ENGINE_INTERRUPT_0);
	writel(status, GLOBAL_NETWORK_ENGINE_INTERRUPT_0);

	if (status) {
#ifdef CONFIG_CS752X_PROC
		//if (cs_ni_debug == DBG_IRQ)
		//	printk(KERN_INFO "%s:: status %x\n", __func__, status);
#endif
		cs_ne_global_intr_handle(0, &cs_ne_global_irq_info, status);
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
}

#ifdef CS_UU_TEST
static u8 cs_ni_dvoq_to_pspid(u8 d_voq)
{
	u8 pspid;

	/* at this moment, we only have 1 state machine assigned to each
	 * GE ports */
	if ((d_voq >= CPU_PORT0_VOQ_BASE) && (d_voq < CPU_PORT1_VOQ_BASE))
		return GE_PORT0;
	else if ((d_voq >= CPU_PORT1_VOQ_BASE) && (d_voq < CPU_PORT2_VOQ_BASE))
		return GE_PORT1;
	else if ((d_voq == CPU_PORT2_VOQ_BASE) && (d_voq < CPU_PORT3_VOQ_BASE))
		pspid = GE_PORT2;
	else
		printk("Not come from GE-0, GE-1, GE-2 \n");

	return pspid;
}
#endif

static inline struct net_device *get_dev_by_cpu_hdr(CPU_HEADER0_T *cpu_hdr_0,
						    struct net_device *org_dev)
{
#ifdef CS_UU_TEST
	u8 o_pspid;
#endif

	if (cpu_hdr_0->bits.pspid > 2) {
		DBG(printk("%s::pspid %d\n", __func__, cpu_hdr_0->bits.pspid));
#ifdef CS_UU_TEST
		if (cpu_hdr_0->bits.pspid == 6) {
			/* 6: MCast */
			/*
			 * Note!! In the future, we will need to maintain a
			 * DVOQ to O_LSPIDmapping table.
			 */
			o_pspid = cs_ni_dvoq_to_pspid(cpu_hdr_0->bits.dst_voq);
			DBG(printk("%s::pspid %d, d_voq = %d, o_pspid = %d\n",
				   __func__, cpu_hdr_0->bits.pspid,
				   cpu_hdr_0->bits.dst_voq, o_pspid));
			return ni_private_data.dev[o_pspid];
		}
#endif /* CS_UU_TEST */
	} else if (ni_private_data.dev[cpu_hdr_0->bits.pspid] != NULL) {
		DBG(printk("%s::cpu_hdr_0->bits.pspid = %d",
			__func__, cpu_hdr_0->bits.pspid));
		return ni_private_data.dev[cpu_hdr_0->bits.pspid];
	}
	return org_dev;
}

#if 0 /* FIXME!! Wen! DONT REMOVE! Implementation for future requirement! */
/*
 * implement first, in the future when we increment the xram_ptr, we should
 * make sure it wraps around.  This applies to both RX and TX xram_ptr action.
 * Description:
 * curr_xram_ptr is the current xram_ptr points to.
 * inc_num is the number of increment.
 * start is the number written@NI_TOP_NI_CPUXRAM_ADRCFG_RX/TX_#.rx_base_addr
 * end is the number written@NI_TOP_NI_CPUXRAM_ADRCFG_RX/TX_#.rx_top_addr
 */
static u32 *xram_ptr_inc(u32 *curr_xram_ptr, unsigned short inc_num,
		u32 start, u32 end)
{
	u32 *rslt_xram_ptr = curr_xram_ptr + inc_num;
	u32 dif;

	if (((u32)rslt_xram_ptr - (u32)NI_XRAM_BASE) >= ((end + 1) << 3)) {
		dif = (u32)rslt_xram_ptr - (u32)NI_XRAM_BASE - ((end + 1) << 3);
		rslt_xram_ptr = (u32 *)NI_XRAM_BASE;
		rslt_xram_ptr += (start * 2) + dif;
	}
	return rslt_xram_ptr;
} /* xram_ptr_inc */
#endif

#ifdef CONFIG_DEBUG_KMEMLEAK
static int ni_remove_skb_from_list(u32 instance, struct sk_buff *skb)
{
	struct list_head *next;
	ni_skb_list_t *curr_skb_entry;

	list_for_each(next, &ni_private_data.ni_skb_list_head[instance]) {
		curr_skb_entry = (ni_skb_list_t *)list_entry(next,
				ni_skb_list_t, list);
		if (curr_skb_entry->skb == skb) {
			list_del(&curr_skb_entry->list);
			kfree(curr_skb_entry);
			return 0;
		}
	}
	return 1;
} /* ni_remove_skb_from_list */
#endif

/*
 * Receive packets on any CPU VoQs, and forward to dev per pspid.
 */
static int ni_complete_rx_instance(struct net_device *dev, u32 instance,
				   int budget, bool is_napi)
{
	HEADER_XR_T *hdr_x;
	HEADER_E_T xram_hdr_e;
	CPU_HEADER0_T xram_cpu_hdr0;
	CPU_HEADER1_T xram_cpu_hdr1;
	volatile u32 hw_wr_ptr, next_link;
	volatile u32 *xram_ptr = (u32 *)NI_XRAM_BASE;
	struct sk_buff *skb, *tmp_skb = NULL, *tail_skb = NULL;
	u32 rx_base_addr, rx_top_addr;
	volatile u32 reg;
	u32 linux_2byte_align = 0;
	u32 seg_len;
	mac_info_t *tp = NULL;
	volatile u32 tmp_data, *tmp_ptr;
	u32 jumbo_pkt_index = 0, refill_cnt = 0;
	int pkt_len, done = 0;
	unsigned long flags;
	struct net_device *in_dev = dev;
	dma_addr_t rx_dma_addr;

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	int err_virt_ni = 0;
#endif

	/* do we check ptp queue/port though? */
	reg = readl(NI_TOP_NI_CPUXRAM_ADRCFG_RX_0 + (instance * 4));

	rx_base_addr = reg & RX_BASE_ADDR_LOC;
	rx_top_addr = (reg >> 16) & 0x7FF;

	hw_wr_ptr = (readl(NI_TOP_NI_CPUXRAM_CPU_STA_RX_0 + (instance * 24)));
	hw_wr_ptr &= HW_WR_PTR;
	next_link = (readl(NI_TOP_NI_CPUXRAM_CPU_CFG_RX_0 + (instance * 24)));
	next_link &= SW_RD_PTR;
	xram_ptr += next_link * 2;

	while ((next_link != hw_wr_ptr) && (done < budget)) {
		hdr_x = (HEADER_XR_T *)(xram_ptr + 1);
		if (hdr_x->bits.ownership != 0) {
			printk("%s::Ownership by HW!\n", __func__);
			break;
		}
		next_link = hdr_x->bits.next_link;
		/*
		 * WARNING *(xram_ptr+offset) may fail if xram instance size is
		 * not 56 bytes aligned, when CPU header is on!
		 * i.e., if RE sends a packet to NI w/FE_BYPASS mode, we will
		 * only see 40 bytes xram instead of 56 bytes, thus the
		 * alignment is gone.
		 */
		xram_hdr_e.bits32[0] = *(xram_ptr + 3);
		xram_hdr_e.bits32[1] = *(xram_ptr + 2);

		if (xram_hdr_e.bits.cpu_header != 0) {
			xram_cpu_hdr0.bits32[0] = *(xram_ptr + 5);
			xram_cpu_hdr0.bits32[1] = *(xram_ptr + 4);
			xram_cpu_hdr1.bits32[0] = *(xram_ptr + 7);
			xram_cpu_hdr1.bits32[1] = *(xram_ptr + 6);
		}

		tmp_data = ((*(xram_ptr + 10)) & 0xfffff000) + 0x100 - 4;
		tmp_ptr = (u32 *)(phys_to_virt(tmp_data + GOLDENGATE_DRAM_BASE));
		skb = (struct sk_buff *)(*tmp_ptr);
		if (skb == NULL) {
			printk("%s:%d:something is not right! skb@0x%x, "
					"tmp_ptr@0x%x\n", __func__, __LINE__,
					(u32)skb, (u32)tmp_ptr);
		}
#ifdef CONFIG_DEBUG_KMEMLEAK
		/* buffer is used.. remove it from the list */
		ni_remove_skb_from_list(instance, skb);
#endif
		pkt_len = xram_hdr_e.bits.pkt_size;
		/* FIXME: change mtu got problem */
		//dma_map_single(NULL, skb->data, pkt_len + 0x100,
		//		DMA_FROM_DEVICE);
#ifndef ACP_QM
		rx_dma_addr = dma_map_single(NULL, skb->data,
						pkt_len + 16,
						DMA_BIDIRECTIONAL);
		dma_sync_single_for_device(dev, rx_dma_addr, pkt_len + 16,
						DMA_FROM_DEVICE);
		//dma_map_single(NULL, skb->data, pkt_len, DMA_FROM_DEVICE);
#endif
		if (hdr_x->bits.error_flag != 0) {
			/*
			 * error happens with this packet, jump to next
			 * packet!
			 */
			dev_kfree_skb(skb);
			/* update read pointer */
			hw_wr_ptr = (readl(NI_TOP_NI_CPUXRAM_CPU_STA_RX_0 +
						(instance * 24))) & HW_WR_PTR;
			writel(next_link, NI_TOP_NI_CPUXRAM_CPU_CFG_RX_0 +
					(instance * 24));
			xram_ptr = (u32 *)NI_XRAM_BASE + next_link * 2;
			refill_cnt++;
			continue;
		}

		skb->data += sizeof(HEADER_E_T);
		if (xram_hdr_e.bits.cpu_header != 0) {
			skb->data += sizeof(CPU_HEADER0_T);
			skb->data += sizeof(CPU_HEADER1_T);
			pkt_len -= 16;
		}

		linux_2byte_align = readl(QM_CPU_PATH_CONFIG_1) & 0x4;
		if (linux_2byte_align != 0)
			skb->data += 2;

		/* decrease pkt len by CRC length */
		pkt_len -= 4;

		seg_len = SKB_PKT_LEN - ((u32)skb->data & 0xff);
		DBG(printk("%s::seg len %d\n", __func__,
			min_t(u16, pkt_len, seg_len)));
#ifdef NET_SKBUFF_DATA_USES_OFFSET
		skb->tail = skb->data - skb->head;
#else
		skb->tail = skb->data;
#endif
		seg_len = min_t(u16, pkt_len, seg_len);
		skb_put(skb, seg_len);
		skb->len = pkt_len;
		pkt_len -= min_t(u16, pkt_len, seg_len);
		skb->data_len = pkt_len;
		jumbo_pkt_index = 0;
		refill_cnt++;
		while (pkt_len > 0) {
			tmp_data = ((*(xram_ptr + 11 + jumbo_pkt_index)) &
			    0xfffff000) + 0x100 - 4;
			tmp_ptr = (u32 *)(phys_to_virt(tmp_data
						+ GOLDENGATE_DRAM_BASE));
			tmp_skb = (struct sk_buff *)(*tmp_ptr);
			seg_len = min_t(u16, SKB_PKT_LEN, pkt_len);
#ifndef ACP_QM
			//dma_map_single(NULL, tmp_skb->data, seg_len,
			//	       DMA_FROM_DEVICE);
			rx_dma_addr = dma_map_single(NULL, skb->data,
						seg_len + 16,
						DMA_BIDIRECTIONAL);
			dma_sync_single_for_device(dev, rx_dma_addr, seg_len + 16,
						DMA_FROM_DEVICE);
#endif
			tmp_skb->len = 0;
			skb_put(tmp_skb, seg_len);
			tmp_skb->next = NULL;
			tmp_skb->prev = NULL;
			if (jumbo_pkt_index == 0) {
				skb_shinfo(skb)->frag_list = tmp_skb;
				tail_skb = tmp_skb;
			} else {
				tail_skb->next = tmp_skb;
				tail_skb = tmp_skb;
			}
			pkt_len -= seg_len;
			jumbo_pkt_index++;
			refill_cnt++;
		}

		skb->next = NULL;
		skb->prev = NULL;
		if (xram_hdr_e.bits.cpu_header != 0)
			skb->dev = get_dev_by_cpu_hdr(&xram_cpu_hdr0, dev);
		else
			skb->dev = dev;
		skb->ip_summed = CHECKSUM_NONE;
		tp = netdev_priv(skb->dev);
#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
		/* in this mode checksum should not be enabled, because FE is
		 * not capable to locate the header offset properly */
		err_virt_ni = cs_ni_virt_ni_process_rx_skb(skb);
		/* check if the packet is dropped by the internal process */
		if (err_virt_ni < 0) {
			//printk("%s:%d:unable to process this skb!\n",
			//		__func__, __LINE__);
			skb->dev = in_dev;
			dev_kfree_skb(skb);
			/* update read pointer */
			hw_wr_ptr = (readl(NI_TOP_NI_CPUXRAM_CPU_STA_RX_0 +
						(instance * 24))) & HW_WR_PTR;
			writel(next_link, NI_TOP_NI_CPUXRAM_CPU_CFG_RX_0 +
					(instance * 24));
			xram_ptr = (u32 *)NI_XRAM_BASE + next_link * 2;
			continue;
		}
#endif

#if 0
		/* Default RX HW checksum enable */
		if (tp->rx_checksum == CS_ENABLE) {
			if (!((xram_cpu_hdr1.bits.ipv4_csum_err == CHECKSUM_ERROR) ||
			      (xram_cpu_hdr1.bits.l4_csum_err == CHECKSUM_ERROR)))
				skb->ip_summed = CHECKSUM_UNNECESSARY;
			else {
				if (xram_cpu_hdr1.bits.ipv4_csum_err == CHECKSUM_ERROR) {
					tp->ifStatics.rx_errors++;
					printk("IP CSUM error!\n");
				}
				if (xram_cpu_hdr1.bits.l4_csum_err == CHECKSUM_ERROR) {
					/*
					 * FIXME: HW BUG#26229. SW workaround. Do we
					 * need change? ask Alan Carr.
					 */
					skb->ip_summed = CHECKSUM_NONE;
					DBG(printk("L4 CSUM error!\n"));
				}
			}
		}
#endif
		skb->protocol = eth_type_trans(skb, skb->dev);

#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
		{
			CS_KERNEL_ACCEL_CB_T *cs_cb;
			cs_cb = (CS_KERNEL_ACCEL_CB_T *) CS_KERNEL_SKB_CB(skb);
			// FIXME: pspid <--> lspid one to one map
                        //cs_cb->hdrE = hdr_e;
                        //cs_cb->cpu_hdr0 = cpu_hdr0;
                        //cs_cb->cpu_hdr1 = cpu_hdr1;
			cs_cb->key_misc.orig_lspid = xram_hdr_e.bits.lspid;
			//cs_cb->key_misc.orig_lspid = tp->port_id;
			/* FIXME: Don't need enable before cut to kernel
			 * adapter tree. CH */
			//cs_kernel_input_set_cb(skb);
		}
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#endif
		skb->pkt_type = PACKET_HOST;
#ifdef CONFIG_CS752X_PROC
		if (cs_ni_debug == DBG_NI_DUMP_RX) {
				//printk("%s:: RX: XRAM packet pkt_len %d, len %d, data_len %d, head %p, data %p, tail %p, end %p\n",
				//	__func__, pkt_len, skb->len, skb->data_len, skb->head, skb->data,
				//	skb->tail, skb->end);
				printk("%s:: RX, seg_len %d\n", __func__, seg_len);
				ni_dm_byte((u32)skb->data - 14, seg_len + 14);
		}
#endif

		if (tp != NULL) {
			int extra_padding = 0;
#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
#ifdef CONFIG_CS752X_VIRTUAL_NI_CPUTAG
			if (err_virt_ni)
				extra_padding = RTL_CPUTAG_LEN;
#else
			if (err_virt_ni)
				extra_padding = VLAN_HLEN;
#endif
#endif
			if (pkt_len > (dev->mtu + ETH_HLEN + extra_padding)) {
				tp->ifStatics.rx_errors++;
				tp->ifStatics.rx_bytes += pkt_len;
				printk("%s:: RX [GMAC %d] pkt_len %d > %d\n",
				       __func__, tp->port_id, pkt_len,
				       dev->mtu + ETH_HLEN + extra_padding);
			} else {
				tp->ifStatics.rx_packets++;
				tp->ifStatics.rx_bytes += skb->len;
			}
		}
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
		if ((xram_hdr_e.bits.cpu_header) &&
		    ((xram_cpu_hdr0.bits.dst_voq == 51) ||
		     (xram_cpu_hdr0.bits.dst_voq == 59))) {
			cs_hw_accel_ipsec_handle_rx(skb, tp->port_id,
						    xram_cpu_hdr0.bits.dst_voq,
						    xram_cpu_hdr1.bits.
						    sw_action);
		} else
#endif

#ifdef CONFIG_RPS
		/* a special tuned netif_rx routine designed for CS752x
		 * SMP processor.  It's with some easy CPU load balance */
		netif_rx_cs(skb);
#else
		NETIF_RX(skb);
#endif

		/* Update the software read pointer */
		hw_wr_ptr = (readl(NI_TOP_NI_CPUXRAM_CPU_STA_RX_0 +
				   (instance * 24))) & HW_WR_PTR;

		writel(next_link, NI_TOP_NI_CPUXRAM_CPU_CFG_RX_0 +
		       (instance * 24));
		done++;
		xram_ptr = (u32 *)NI_XRAM_BASE + next_link * 2;
#if 0
		{
			volatile u32 status = readl(NI_TOP_NI_INTERRUPT_0);
			if (status & NI_TOP_INTR_STAT_ERR) {
				volatile u32 cpuxram_err_status =
				    readl(NI_TOP_NI_CPUXRAM_ERR_INTERRUPT_0);
				if (cpuxram_err_status) {
					writel(cpuxram_err_status,
					       NI_TOP_NI_CPUXRAM_ERR_INTERRUPT_0);
					//printk("%s::CPU Error status %x\n",
					//      __func__, cpuxram_err_status);
					//while(1);
				}
			}
		}
#endif
	} /* end of while loop */

	/* FIXME!! which linux queue does QM fetch for PTP packet? */
	/* only refill when we are dealing with instance#0~7.
	 * Ignore instance#8 (PTP). */
	if (instance < LINUX_FREE_BUF) {
		if (is_napi == true) {
			spin_lock(&sw_qm_cnt_lock);
			sw_qm_total_count[instance] -= refill_cnt;
			spin_unlock(&sw_qm_cnt_lock);
		} else {
			spin_lock_irqsave(&sw_qm_cnt_lock, flags);
			sw_qm_total_count[instance] -= refill_cnt;
			spin_unlock_irqrestore(&sw_qm_cnt_lock, flags);
		}
		if (refill_cnt != 0)
			cs_ni_alloc_linux_free_buffer(dev, instance, refill_cnt);
	}

	return done;
}

static int ni_complete_rx(struct net_device *dev, int budget, bool is_napi)
{
	int i = 0, curr_rx_pkt, total_rx_pkt = 0;

	/*
	 * We can implement some sorts of software QoS of receiving packets
	 * from different instance; however, since we don't have any QoS
	 * defined here, we have a simple loop from the first instance to
	 * the last one and retrieve a packet from it if there is.
	 */
	for (i = 0; i <= XRAM_RX_INSTANCE; i++) {
		curr_rx_pkt = ni_complete_rx_instance(dev, i, budget, is_napi);
		total_rx_pkt += curr_rx_pkt;
		if (curr_rx_pkt != 0)
			DBG(printk("%s::instance %d, pkts %d\n", __func__, i,
				   curr_rx_pkt));
	}

	return total_rx_pkt;
}

static int cs_ni_poll(struct napi_struct *napi, int budget)
{
	int i, received_pkts = 0;
	unsigned long flags;

	received_pkts = ni_complete_rx(ni_private_data.dev[0], budget, true);
	if (received_pkts == 0) {
	//if (received_pkts < budget) {
		/*
		 * other driver has received_pkts == 0 to double confirm all
		 * the queues are empty, but in our current logic, when
		 * received_pkts is less than budget, it basically means
		 * there is no more packet in any of the queue.
		 */
		napi_complete(napi);
		spin_lock_irqsave(&ni_private_data.rx_lock, flags);
		for (i = 0; i <= XRAM_RX_INSTANCE; i++)
			writel(1, NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 +
			       (i << 3));
		spin_unlock_irqrestore(&ni_private_data.rx_lock, flags);
		return 0;
	}
	return 1;
}

struct net_device *ni_get_device(unsigned char port_id)
{
	struct net_device *dev;

	if (port_id > 2)
		return NULL;

	dev = ni_private_data.dev[port_id];
	if (dev == NULL)
		return NULL;
	else
		return dev;
}
EXPORT_SYMBOL(ni_get_device);

int ni_special_start_xmit(struct sk_buff *skb, HEADER_A_T *header_a)
{
	u16 hw_rd_ptr, sw_wr_ptr;
	int pad = 0;
	u32 *ni_xram_base, *tx_xram_base;
	volatile u32 *xram_ptr, *tx_start;
	register u32 *data_ptr;
	int loop, free_space;
	u32 tx_base_addr = 0, tx_top_addr = 0, hdr_xt = 0, crc32;
	u8 valid_bytes = 0;
	u16 next_link = 0;
	cs_reg reg_val;
	u32 last_value1, temp_value, last_value2;
	u32 *helper_ptr;
	u8 *helper_ptr_t;
	u16 pkt_len;
	unsigned long flags;

	if ((skb == NULL) || (header_a == NULL))
		return -1;

	pkt_len = skb->len;
	if ((pkt_len > 2032))
		return -1;
	header_a->pkt_size = pkt_len;

	ni_xram_base = (u32 *)NI_XRAM_BASE;
	/* We are only using TX XRAM#0.  Not TX XRAM#1. If we want to use #1,
	 * then we will need to read/write to NI_TOP_NI_CPUXRAM_ADRCFG_TX_1 and
	 * NI_TOP_NI_CPUXRAM_CPU_CFG_TX_1 */
	reg_val = readl(NI_TOP_NI_CPUXRAM_ADRCFG_TX_0);
	tx_top_addr = (reg_val >> 16) & 0x7FF;
	tx_base_addr = reg_val & 0x7FF;
	spin_lock_irqsave(&ni_private_data.tx_lock, flags);
	hw_rd_ptr = readl(NI_TOP_NI_CPUXRAM_CPU_STAT_TX_0);
	hw_rd_ptr &= 0x7FF;
	sw_wr_ptr = readl(NI_TOP_NI_CPUXRAM_CPU_CFG_TX_0);
	sw_wr_ptr &= 0x7FF;

	pad = MIN_PKT_SIZE - (pkt_len + 4);
	pad = (pad < 0) ? 0 : pad;
	pkt_len += pad;

	if (hw_rd_ptr > sw_wr_ptr)
		free_space = (hw_rd_ptr - sw_wr_ptr);
	else
		free_space = ((tx_top_addr - sw_wr_ptr + 1) +
			      (hw_rd_ptr - tx_base_addr));

	if (free_space < (((pkt_len + 4) / 8) + ((pkt_len + 4) % 8 ? 1 : 0))) {
		spin_unlock_irqrestore(&ni_private_data.tx_lock, flags);
		return -1;
	}

	/* Calculate the CRC32 */
	crc32 = ~(calc_crc(~0, (u8 *)skb->data, pkt_len));
	memcpy(((u8 *)skb->data + pkt_len), (u8 *)(&crc32), sizeof(crc32));
	pkt_len += 4;

	tx_xram_base = (u32 *)(ni_xram_base + (tx_base_addr * 2));
	valid_bytes = (pkt_len % 8);
	next_link = (sw_wr_ptr + (pkt_len / 8)) + 1; /* for headr XT */
	/* Extra location for the trailing bytes */
	if (valid_bytes != 0)
		next_link++;

	valid_bytes = valid_bytes ? valid_bytes : 8;

	/* Set header A bit in hdr_xt and increment the next chunk */
	hdr_xt |= (1 << 30);
	next_link++;
	next_link++;

	/* Wrap around if required */
	if (next_link > tx_top_addr)
		next_link = tx_base_addr + (next_link - (tx_top_addr + 1));

	hdr_xt |= 0x80000000 | ((valid_bytes & 0xF) << 12) | next_link;

	/* Go the correct absolute location */
	xram_ptr = (u32 *)ni_xram_base;
	xram_ptr += sw_wr_ptr * 2;

	/* Leave first 4 bytes before copying the headerXT */
	xram_ptr++;

	/* Little endian byte order */
	writel(hdr_xt, xram_ptr);

	tx_start = xram_ptr;
	xram_ptr++;
	XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr, tx_xram_base);

	data_ptr = (u32 *)skb->data;

	/* Now to copy the data . The first byte on the line goes first */
	for (loop = 0; loop < pkt_len / 8; loop++) {
		writel(*data_ptr, xram_ptr);
		xram_ptr++;
		data_ptr++;
		XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr,
				   tx_xram_base);

		writel(*data_ptr, xram_ptr);
		xram_ptr++;
		data_ptr++;
		XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr,
				   tx_xram_base);
	}
	if (valid_bytes != 8) {	/* extra location is there */
		last_value1 = 0;
		temp_value = 0;
		last_value2 = 0;
		helper_ptr_t = (u8 *)data_ptr;
		loop = 0;
		while (valid_bytes) {
			temp_value |= (*(helper_ptr_t++) << (8 * loop));
			valid_bytes--;
			if (loop == 3) { /* wrap around for the next word */
				last_value1 = temp_value;
				loop = 0;
				temp_value = 0;
				continue;
			}
			loop++;
		}
		last_value2 = last_value1 ? temp_value : last_value1;
		last_value1 = (0 == last_value1) ? temp_value : last_value1;
		writel(last_value1, xram_ptr);
		xram_ptr++;
		XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr,
				   tx_xram_base);

		writel(last_value2, xram_ptr);
		xram_ptr++;
		XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr,
				   tx_xram_base);
	}

#if 0	/* OLD one.. archive for now.. for future reference. remove later */
	/* Create and populate header A */
	memset(&ni_header_a, 0, sizeof(HEADER_A_T));
	ni_header_a.pkt_size = pkt_len;
	ni_header_a.fwd_type = CS_FWD_NORMAL;
	ni_header_a.pspid = pspid;
	ni_header_a.dvoq = voq_id;
	ni_header_a.recirc_idx = recirc_idx;
	ni_header_a.flags = 0x800;
#if 0 /* the following is commented out, because they've been reset to 0 */
	ni_header_a.original_lspid = 0;
	ni_header_a.original_lspid_valid = 0;
	ni_header_a.mc_grp_id = 0;
	ni_header_a.mc_index = 0;
	ni_header_a.cpu_ptp_flag = 0;
	ni_header_a.ts_flag = 0;
	ni_header_a.mark = 0;
	ni_header_a.addr_cnt = 0;
	ni_header_a.bypass_cos = 0;
	ni_header_a.no_crc = 0;
	ni_header_a.no_stuff = 0;
	ni_header_a.ni_port_id = 0;
	ni_header_a.replace_l4_chksum = 0;
#endif
#endif
	/* Copy the header A */
	helper_ptr_t = (u8 *)header_a;
	helper_ptr = (u32 *)helper_ptr_t;

	helper_ptr += 2;
	writel(*helper_ptr, xram_ptr);
	xram_ptr++; /* Bits 64-95 */
	XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr, tx_xram_base);

	helper_ptr -= 1;
	writel(*helper_ptr, xram_ptr);
	xram_ptr++; /* Bits 32-63 */
	XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr, tx_xram_base);

	helper_ptr -= 1;
	writel(*helper_ptr, xram_ptr);
	xram_ptr++; /* Bits 0-31 */
	XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr, tx_xram_base);

	/* Put debugging statement here. */
	/* The current+1 or next+1 pointer should be equal to next_link
	 * Reset the next header XT */
	xram_ptr = (u32 *)ni_xram_base;
	xram_ptr += next_link * 2;
	XRAM_PTR_WRAP_ARND(xram_ptr, ni_xram_base, tx_top_addr, tx_xram_base);

	/* The next Header XT */
	writel(0, xram_ptr);
	xram_ptr++;
	writel(0, xram_ptr);
	xram_ptr++;

	/* Publish the write pointer */
	writel(next_link, NI_TOP_NI_CPUXRAM_CPU_CFG_TX_0);
	/* TX state machine 0 */

	/* enable CPUXRAM TXPKT interrupt */
	//writel(0x1, NI_TOP_NI_CPUXRAM_TXPKT_INTENABLE_0);

	spin_unlock_irqrestore(&ni_private_data.tx_lock, flags);

	dev_kfree_skb_any(skb);
	//dev_kfree_skb(skb);
	return 0;
}

void ni_dm_byte(u32 location, int length)
{
	u8 *start_p, *end_p, *curr_p;
	u8 *datap, data;
	int i;

	start_p = datap = (u8 *)location;
	end_p = (u8 *)start_p + length;
	curr_p = (u8 *)((u32)location & 0xfffffff0);

	while (curr_p < end_p) {
		u8 *p1, *p2;
		printk("0x%08x: ", (u32)curr_p & 0xfffffff0);
		p1 = curr_p;
		p2 = datap;
		/* dump data */
		for (i = 0; i < 16; i++) {
			if (curr_p < start_p || curr_p >= end_p)
				printk("   ");	/* print space x 3 for padding */
			else {
				data = *datap;
				printk("%02X ", data);
				datap++;
			}
			if (i == 7)
				printk("- ");
			curr_p++;
		}

		/* dump ascii */
		curr_p = p1;
		datap = p2;
		for (i = 0; i < 16; i++) {
			if (curr_p < start_p || curr_p >= end_p)
				printk(".");
			else {
				data = *datap;
				if ((data < 0x20) || (data > 0x7f) ||
				    (data == 0x25))
					printk(".");
				else
					printk("%c", data);
				datap++;
			}
			curr_p++;
		}
		printk("\n");
	}
}
EXPORT_SYMBOL(ni_dm_byte);

void ni_dm_short(u32 location, int length)
{
	u16 *start_p, *curr_p, *end_p;
	u16 *datap, data;
	int i;

	/* start address should be a multiple of 2 */
	start_p = datap = (u16 *)(location & 0xfffffffe);
	end_p = (u16 *)location + length;
	curr_p = (u16 *)((u32)location & 0xfffffff0);

	while (curr_p < end_p) {
		printk("0x%08x: ", (u32)curr_p & 0xfffffff0);
		for (i = 0; i < 8; i++) {
			if (curr_p < start_p || curr_p >= end_p)
				printk("     "); /* print space x 5 for padding */
			else {
				data = *datap;
				printk("%04X ", data);
				datap++;
			}
			if (i == 3)
				printk("- ");
			curr_p++;
		}
		printk("\n");
	}
}
EXPORT_SYMBOL(ni_dm_short);

void ni_dm_long(u32 location, int length)
{
	u32 *start_p, *curr_p, *end_p;
	u32 *datap, data;
	int i;

	/* start address should be a multiple of 4 */
	start_p = datap = (u32 *)(location & 0xfffffffc);
	end_p = (u32 *)location + length;
	curr_p = (u32 *)((u32)location & 0xfffffff0);

	while (curr_p < end_p) {
		printk("0x%08x: ", (u32)curr_p & 0xfffffff0);
		for (i = 0; i < 4; i++) {
			if (curr_p < start_p || curr_p >= end_p)
				printk("         "); /* print space x 9 for padding */
			else {
				data = *datap;
				printk("%08X ", data);
				datap++;
			}
			if (i == 1)
				printk("- ");
			curr_p++;
		}
		printk("\n");
	}
}
EXPORT_SYMBOL(ni_dm_long);

#ifdef CS752X_MULTIQUEUE_SUPPORT
static uint16_t ni_select_queue(struct net_device *dev, struct sk_buff *skb)
{
	/* FIXME. WEN. more implementation later */
	/* This API can be used to set the default TX queue */
#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
	cs_qos_set_voq_id_to_skb_cs_cb(skb, dev, 7);
#endif
#endif
	return 7;
}
#endif

u8 cs_ni_get_port_id(struct net_device * dev)
{
	mac_info_t *tp = netdev_priv(dev);
	return tp->port_id;
}
EXPORT_SYMBOL(cs_ni_get_port_id);

void ni_set_ne_enabled(u8 enabled)
{
	ni_info_t *ni = &ni_private_data;
	ni->neEnabled = enabled;
	if (enabled != 0)
		printk("%s: NE enabled\n", __FILE__);
	return;
}
EXPORT_SYMBOL(ni_set_ne_enabled);

static void cs_ni_reset(void)
{
	u32 reg_val = 0;

	/* Apply resets */
	reg_val = readl(NI_TOP_NI_INTF_RST_CONFIG);
	reg_val = reg_val | (RESET_NI);	/* ->1 */
	writel(reg_val, NI_TOP_NI_INTF_RST_CONFIG);
	/* Remove resets */
	reg_val = readl(NI_TOP_NI_INTF_RST_CONFIG);
	reg_val = reg_val & (~(RESET_NI)); /* -> 0 */
	writel(reg_val, NI_TOP_NI_INTF_RST_CONFIG);
}

int cs_ni_set_port_calendar(u16 table_address, u8 pspid_ts)
{
	NI_TOP_NI_RX_PORT_CAL_ACCESS_t rx_port_cal_acc, rx_port_cal_acc_mask;
	NI_TOP_NI_RX_PORT_CAL_DATA_t rx_port_cal_data, rx_port_cal_data_mask;
	int access;

	rx_port_cal_data.wrd = 0;
	rx_port_cal_data_mask.wrd = 0;

	rx_port_cal_data.bf.pspid_ts = pspid_ts;
	rx_port_cal_data_mask.bf.pspid_ts = 0x7;
	write_reg(rx_port_cal_data.wrd, rx_port_cal_data_mask.wrd,
		  NI_TOP_NI_RX_PORT_CAL_DATA);

	rx_port_cal_acc.wrd = 0;
	rx_port_cal_acc_mask.wrd = 0;

	rx_port_cal_acc.bf.access = 1;
	rx_port_cal_acc.bf.rxpc_page = 0;
	rx_port_cal_acc.bf.cpu_page = 0;
	rx_port_cal_acc.bf.rbw = CS_WRITE; /* 1: Write */
	rx_port_cal_acc.bf.address = table_address;

	rx_port_cal_acc_mask.bf.access = 1;
	rx_port_cal_acc_mask.bf.rxpc_page = 1;
	rx_port_cal_acc_mask.bf.cpu_page = 1;
	rx_port_cal_acc_mask.bf.rbw = 1;
	rx_port_cal_acc_mask.bf.address = 0x7F;
	write_reg(rx_port_cal_acc.wrd, rx_port_cal_acc_mask.wrd,
		  NI_TOP_NI_RX_PORT_CAL_ACCESS);

	do {
		access = (readl(NI_TOP_NI_TX_VOQ_LKUP_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);
	return 0;
}
EXPORT_SYMBOL(cs_ni_set_port_calendar);

/*
 * Initialize the NI port calendar
 * 	Goldengate supports 96 (maximum) time slot port calendar.
 * 	The source port numbering is as follows:
 * 	0 - GE0
 * 	1 - GE1
 * 	2 - GE2
 * 	3 - CPU
 * 	4 - Crypto
 * 	5 - Encap
 * 	6 - Mcast
 * 	7 - Mirror
 * 	GigE ports, 1.2 Gbps/100mbps = 12 slots * 3 ports = 36 slots
 * 	Encap and crypto ports each need 1.2 Gbps/100mbps =
 * 			12 slots * 2 ports = 24 slots
 * 	Multicast port needs 1.2Gbps/100mbps = 12 slots * 1 port = 12 slots
 * 	CPU port needs 1.2 Gbps/100 mbps=12 slots * 1 port = 12 slots
 * 	Mirror port  needs 1.2 Gbps/100 mbps=12 slots * 1 port = 12 slots
 */

static void cs_ni_port_cal_cfg(void)
{
	u8 i, j;
	int slot_per_port = NI_MAX_PORT_CALENDAR / NI_PORT_NUM;

	for (i = 0; i < slot_per_port; i++) {
		for (j = 0; j < NI_PORT_NUM; j++) {
			cs_ni_set_port_calendar((i * NI_PORT_NUM) + j, j);
		}
	}
	return;
}

int ni_get_port_calendar(u16 table_address)
{
	u8 access;
	u32 cal_data;
	NI_TOP_NI_RX_PORT_CAL_ACCESS_t rx_port_cal_acc, rx_port_cal_acc_mask;

	rx_port_cal_acc.wrd = 0;
	rx_port_cal_acc_mask.wrd = 0;

	rx_port_cal_acc.bf.access = 1;
	rx_port_cal_acc.bf.rxpc_page = 0;
	rx_port_cal_acc.bf.cpu_page = 0;
	rx_port_cal_acc.bf.rbw = CS_READ; /* 1: Write, 0:Read */
	rx_port_cal_acc.bf.address = table_address;

	rx_port_cal_acc_mask.bf.access = 1;
	rx_port_cal_acc_mask.bf.rxpc_page = 1;
	rx_port_cal_acc_mask.bf.cpu_page = 1;
	rx_port_cal_acc_mask.bf.rbw = 1;
	rx_port_cal_acc_mask.bf.address = 0x7F;
	write_reg(rx_port_cal_acc.wrd, rx_port_cal_acc_mask.wrd,
		  NI_TOP_NI_RX_PORT_CAL_ACCESS);

	do {
		access = (readl(NI_TOP_NI_RX_PORT_CAL_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);

	/* read data */
	cal_data = readl(NI_TOP_NI_RX_PORT_CAL_DATA) & 0x7;
	printk("RX Port Cal Table\t %3d :\t  %d\n", table_address, cal_data);

	return 0;
}
EXPORT_SYMBOL(ni_get_port_calendar);

static void cs_ni_init_xram_mem(void)
{
	int i;
	unsigned int next_start;
	ni_info_t *ni;
	u32 *ni_xram_base;
	NI_TOP_NI_CPUXRAM_ADRCFG_RX_0_t xram_rx_addr, xram_rx_addr_mask;
	NI_TOP_NI_CPUXRAM_ADRCFG_TX_0_t xram_tx_addr, xram_tx_addr_mask;

	ni = &ni_private_data;
	ni_xram_base = (u32 *)NI_XRAM_BASE;

	/* clear XRAM */
	for (i = 0; i <= XRAM_RX_INSTANCE; i++)
		writel(0, NI_TOP_NI_CPUXRAM_ADRCFG_RX_0 + (i * 4));

	for (i = 0; i < XRAM_TX_INSTANCE; i++)
		writel(0, NI_TOP_NI_CPUXRAM_ADRCFG_TX_0 + (i * 4));

	xram_rx_addr.wrd = 0;
	xram_tx_addr_mask.wrd = 0;
	xram_tx_addr.wrd = 0;
	xram_tx_addr_mask.wrd = 0;

	/* for RX XRAM CFG */
	xram_rx_addr_mask.bf.rx_base_addr = 0x7FF;
	xram_rx_addr_mask.bf.rx_top_addr = 0x7FF;
	next_start = 0;
	for (i = 0; i <= XRAM_RX_INSTANCE; i++) {
		if (i != XRAM_RX_INSTANCE)
			ni->xram_desc_base[i] = (u32)ni_xram_base;

		if (cpuxram_rx_addr_cfg_prof[XRAM_CFG_PROF][i] != 0) {
			xram_rx_addr.bf.rx_base_addr = next_start;
			next_start += cpuxram_rx_addr_cfg_prof[
					XRAM_CFG_PROF][i];

			xram_rx_addr.bf.rx_top_addr = next_start - 1;
		} else {
			xram_rx_addr.bf.rx_base_addr = 0;
			xram_rx_addr.bf.rx_top_addr = 0;
		}

		write_reg(xram_rx_addr.wrd, xram_rx_addr_mask.wrd,
				NI_TOP_NI_CPUXRAM_ADRCFG_RX_0 + (i * 4));
	}

	/* for TX XRAM CFG */
	xram_tx_addr_mask.bf.tx_base_addr = 0x7FF;
	xram_tx_addr_mask.bf.tx_top_addr = 0x7FF;
	for (i = 0; i < XRAM_TX_INSTANCE; i++) {
		ni->xram_desc_base[XRAM_RX_INSTANCE + i] = (u32)ni_xram_base;

		if (cpuxram_tx_addr_cfg_prof[XRAM_CFG_PROF][i] != 0) {
			xram_tx_addr.bf.tx_base_addr = next_start;
			next_start += cpuxram_tx_addr_cfg_prof[
					XRAM_CFG_PROF][i];
			xram_tx_addr.bf.tx_top_addr = next_start - 1;
		} else {
			xram_tx_addr.bf.tx_base_addr = 0;
			xram_tx_addr.bf.tx_top_addr = 0;
		}

		write_reg(xram_tx_addr.wrd, xram_tx_addr_mask.wrd,
				NI_TOP_NI_CPUXRAM_ADRCFG_TX_0);
	}
}

static int cs_ni_set_xram_cfg(xram_inst_t xram_inst,
			      xram_direction_t xram_direction)
{
	NI_TOP_NI_CPUXRAM_CFG_t ni_xram_cfg, ni_xram_cfg_mask;

	ni_xram_cfg.wrd = 0;
	ni_xram_cfg_mask.wrd = 0;

	switch (xram_direction) {
	case XRAM_DIRECTION_TX:
		if (xram_inst > XRAM_INST_1) {
			printk("Only 0 and 1 instances in TX direction\n");
			return -1;
		}

		switch (xram_inst) {
		case XRAM_INST_0:
			ni_xram_cfg.bf.tx_0_cpu_pkt_dis = 0;	/* 0: enable */
			ni_xram_cfg_mask.bf.tx_0_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_1:
			ni_xram_cfg.bf.tx_1_cpu_pkt_dis = 0;
			ni_xram_cfg_mask.bf.tx_1_cpu_pkt_dis = 1;
			break;
		default:
			printk("Only 0 and 1 instances in TX direction\n");
			return -1;
		}
		write_reg(ni_xram_cfg.wrd, ni_xram_cfg_mask.wrd,
			  NI_TOP_NI_CPUXRAM_CFG);
		break;
	case XRAM_DIRECTION_RX:
		switch (xram_inst) {
		case XRAM_INST_0:
			ni_xram_cfg.bf.rx_0_cpu_pkt_dis = 0; /* 0:enable */
			ni_xram_cfg_mask.bf.rx_0_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_1:
			ni_xram_cfg.bf.rx_1_cpu_pkt_dis = 0; /* 0:enable */
			ni_xram_cfg_mask.bf.rx_1_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_2:
			ni_xram_cfg.bf.rx_2_cpu_pkt_dis = 0; /* 0:enable */
			ni_xram_cfg_mask.bf.rx_2_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_3:
			ni_xram_cfg.bf.rx_3_cpu_pkt_dis = 0; /* 0:enable */
			ni_xram_cfg_mask.bf.rx_3_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_4:
			ni_xram_cfg.bf.rx_4_cpu_pkt_dis = 0; /* 0:enable */
			ni_xram_cfg_mask.bf.rx_4_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_5:
			ni_xram_cfg.bf.rx_5_cpu_pkt_dis = 0;
			ni_xram_cfg_mask.bf.rx_5_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_6:
			ni_xram_cfg.bf.rx_6_cpu_pkt_dis = 0;
			ni_xram_cfg_mask.bf.rx_6_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_7:
			ni_xram_cfg.bf.rx_7_cpu_pkt_dis = 0;
			ni_xram_cfg_mask.bf.rx_7_cpu_pkt_dis = 1;
			break;
		case XRAM_INST_8:
			ni_xram_cfg.bf.rx_8_cpu_pkt_dis = 0;
			ni_xram_cfg_mask.bf.rx_8_cpu_pkt_dis = 1;
			break;
		default:
			printk("Only 0 - 8 instances in RX direction\n");
			return -1;
		} /* switch(xram_inst) */
		write_reg(ni_xram_cfg.wrd, ni_xram_cfg_mask.wrd,
			  NI_TOP_NI_CPUXRAM_CFG);
		break;
	} /* switch(xram_direction) */

	return 0;
}

static void cs_init_tx_dma_lso(void)
{
	int i;
	ni_info_t *ni;
	dma_addr_t txq_paddr[NI_DMA_LSO_TXQ_NUM];
	dma_txdesc_t *tx_desc[NI_DMA_LSO_TXQ_NUM];
	u32 rptr_addr, wptr_addr, dma_tx_base_addr;
	DMA_DMA_LSO_RXDMA_CONTROL_t rxdma_control, rxdma_control_mask;
	DMA_DMA_LSO_TXDMA_CONTROL_t txdma_control, txdma_control_mask;
	DMA_DMA_LSO_TXQ0_CONTROL_t txq0_control, txq0_control_mask;
	DMA_D_AXI_CONFIG_t axi_config, axi_config_mask;
#ifdef CONFIG_CS752X_VIRTUAL_NI_DBLTAG
	DMA_DMA_LSO_VLAN_TAG_TYPE0_t vlan_tag_type0, vlan_tag_type0_mask;
#endif

	ni = &ni_private_data;
	/* RX DMA LSO */
	rxdma_control.wrd = 0;
	rxdma_control_mask.wrd = 0;
	rxdma_control.bf.rx_dma_enable = 0; /* Disable RX DMA */
	rxdma_control.bf.rx_check_own = 0;
	rxdma_control.bf.rx_burst_len = 3; /* Sanders's suggestion 3 */
	rxdma_control_mask.bf.rx_dma_enable = 1;
	rxdma_control_mask.bf.rx_check_own = 0;
	rxdma_control_mask.bf.rx_burst_len = 3;	/* 8 * 64 bits */
	write_reg(rxdma_control.wrd, rxdma_control_mask.wrd,
		  DMA_DMA_LSO_RXDMA_CONTROL);

	/* TX DMA LSO */
	txdma_control.wrd = 0;
	txdma_control_mask.wrd = 0;
	txdma_control.bf.tx_dma_enable = 1; /* Enable RX DMA */
	txdma_control.bf.tx_check_own = 0;
	txdma_control.bf.tx_burst_len = 3; /* Sanders's suggestion 3 */
	txdma_control_mask.bf.tx_dma_enable = 1;
	txdma_control_mask.bf.tx_check_own = 0;
	txdma_control_mask.bf.tx_burst_len = 3;	/* 8 * 64 bits */
	write_reg(txdma_control.wrd, txdma_control_mask.wrd,
		  DMA_DMA_LSO_TXDMA_CONTROL);

	/* enable TXQ 0~7 w/TXQ#6 and #7 in Round Robin mode */
	/* FIXME: TXQ 6 and 7 are different for memory copy */
	txq0_control.wrd = 0;
	txq0_control_mask.wrd = 0;
	txq0_control.bf.txq0_en = 1;
	txq0_control_mask.bf.txq0_en = 1;
	for (i = 0; i < NI_DMA_LSO_TXQ_NUM; i++)
		write_reg(txq0_control.wrd, txq0_control_mask.wrd,
				DMA_DMA_LSO_TXQ0_CONTROL + (i * 4));

	dma_tx_base_addr = DMA_DMA_LSO_TXQ0_BASE_DEPTH;
	rptr_addr = DMA_DMA_LSO_TXQ0_RPTR;
	wptr_addr = DMA_DMA_LSO_TXQ0_WPTR;

	for (i = 0; i < NI_DMA_LSO_TXQ_NUM; i++) {
		tx_desc[i] = dma_alloc_coherent(NULL,
		              NI_DMA_LSO_TXDESC_NUM * sizeof(dma_txdesc_t),
		              &txq_paddr[i], GFP_KERNEL);
		ni->swtxq[i].rptr_reg = rptr_addr;
		ni->swtxq[i].wptr_reg = wptr_addr;
		ni->swtxq[i].total_desc_num = NI_DMA_LSO_TXDESC_NUM;
		spin_lock_init(&ni->swtxq[i].lock);

		ni->swtxq[i].desc_base = tx_desc[i];

		if (!tx_desc[i]) {
			printk("%s::TX %d dma_alloc_coherent fail !\n",
			       __func__, i);
			dma_free_coherent(NULL,
				  NI_DMA_LSO_TXDESC_NUM * sizeof(dma_txdesc_t),
				  tx_desc,
				  txq_paddr[i]);
			return;
		}
		writel((txq_paddr[i] & DMA_BASE_MASK)
			| NI_DMA_LSO_TXDESC, dma_tx_base_addr);
		dma_tx_base_addr += 4;
		rptr_addr += 8;
		wptr_addr += 8;
	}

	/* for AXI debug purpose, check with Gordon */
	writel(0xFFFC0000, DMA_D_AXI_READ_TIMEOUT_THRESHOLD);
	writel(0xFFFC0000, DMA_D_AXI_WRITE_TIMEOUT_THRESHOLD);

	/* Sanders's suggestion */
	axi_config.wrd = 0;
	axi_config_mask.wrd = 0;
	axi_config.bf.axi_write_outtrans_nums = 3;
	axi_config_mask.bf.axi_write_outtrans_nums = 3;

	axi_config.bf.axi_read_channel0_arbscheme = 1;
	axi_config_mask.bf.axi_read_channel0_arbscheme = 1;
	axi_config.bf.axi_read_channel1_arbscheme = 1;
	axi_config_mask.bf.axi_read_channel1_arbscheme = 1;
	axi_config.bf.axi_read_channel2_arbscheme = 1;
	axi_config_mask.bf.axi_read_channel2_arbscheme = 1;

	axi_config.bf.axi_write_channel0_arbscheme = 1;
	axi_config_mask.bf.axi_write_channel0_arbscheme = 1;
	axi_config.bf.axi_write_channel1_arbscheme = 1;
	axi_config_mask.bf.axi_write_channel1_arbscheme = 1;
	axi_config.bf.axi_write_channel2_arbscheme = 1;
	axi_config_mask.bf.axi_write_channel2_arbscheme = 1;
	write_reg(axi_config.wrd, axi_config_mask.wrd, DMA_D_AXI_CONFIG);

	writel(0xFFFFFFFF, DMA_D_AXI_READ_CHANNEL_0_3_DRR_WEIGHT);
	writel(0x0000FFFF, DMA_D_AXI_READ_CHANNEL_4_7_DRR_WEIGHT);

	writel(0xFFFFFFFF, DMA_D_AXI_WRITE_CHANNEL_0_3_DRR_WEIGHT);
	writel(0x0000FFFF, DMA_D_AXI_WRITE_CHANNEL_4_7_DRR_WEIGHT);
#ifdef CONFIG_CS752X_VIRTUAL_NI_DBLTAG
	vlan_tag_type0.wrd = 0;
	vlan_tag_type0_mask.wrd = 0;
	vlan_tag_type0.bf.enable = 1;
	vlan_tag_type0_mask.bf.enable = 1;
	vlan_tag_type0.bf.value = ETH_P_8021AD;
	vlan_tag_type0_mask.bf.value = 0xffff;
	write_reg(vlan_tag_type0.wrd, vlan_tag_type0_mask.wrd,
			DMA_DMA_LSO_VLAN_TAG_TYPE0);
#endif
}

static void cs_ni_set_voq_map(u16 voq_number, u8 voq_did, u8 disable_crc)
{
	NI_TOP_NI_TX_VOQ_LKUP_ACCESS_t voq_lkup_access, voq_lkup_access_mask;
	NI_TOP_NI_TX_VOQ_LKUP_DATA0_t voq_lkup_data0, voq_lkup_data0_mask;
	u8 access;

	/* Prepare data */
	voq_lkup_data0.wrd = 0;
	voq_lkup_data0_mask.wrd = 0;
	voq_lkup_data0.bf.txem_voq_did = voq_did;
	voq_lkup_data0.bf.txem_discrc = disable_crc;
	voq_lkup_data0.bf.txem_crcstate = 0;
	voq_lkup_data0_mask.bf.txem_voq_did = 0xF;
	voq_lkup_data0_mask.bf.txem_discrc = 1;
	voq_lkup_data0_mask.bf.txem_crcstate = 0x7FFFFFF;
	//printk("00--> voq_did = %d, voq_lkup_data0.wrd =0x%X\n",
	//              voq_did,voq_lkup_data0.wrd);
	write_reg(voq_lkup_data0.wrd, voq_lkup_data0_mask.wrd,
		  NI_TOP_NI_TX_VOQ_LKUP_DATA0);
	writel(0, NI_TOP_NI_TX_VOQ_LKUP_DATA1);

	voq_lkup_access.wrd = 0;
	voq_lkup_access_mask.wrd = 0;
	voq_lkup_access.bf.access = 1;
	/* access address is the same as the voq_number, range from 0 to 127 */
	voq_lkup_access.bf.address = voq_number;
	voq_lkup_access.bf.debug_mode = NORMAL_MODE; /* NORMAL_MODE = 0 */
	voq_lkup_access.bf.rbw = CS_WRITE; /* 0:read, 1:write */

	voq_lkup_access_mask.bf.access = 1;
	voq_lkup_access_mask.bf.address = 0x7F;
	voq_lkup_access_mask.bf.debug_mode = 1;
	voq_lkup_access_mask.bf.rbw = 1;
	write_reg(voq_lkup_access.wrd, voq_lkup_access_mask.wrd,
		  NI_TOP_NI_TX_VOQ_LKUP_ACCESS);
	do {
		access = (readl(NI_TOP_NI_TX_VOQ_LKUP_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);
}

#if 0 /* debug use */
static void cs_ni_get_voq_map(u16 voq_number)
{
	NI_TOP_NI_TX_VOQ_LKUP_ACCESS_t voq_lkup_access, voq_lkup_access_mask;
	NI_TOP_NI_TX_VOQ_LKUP_DATA0_t voq_lkup_data0;
	u8 access;
	u32 voq_lkup_data;

	voq_lkup_access.wrd = 0;
	voq_lkup_access_mask.wrd = 0;

	voq_lkup_access.bf.access = 1;
	/* access address is the same as the voq_number, range from 0 to 127 */
	voq_lkup_access.bf.address = voq_number;
	voq_lkup_access.bf.debug_mode = 0; /* 0 = Normal mode of operation */
	voq_lkup_access.bf.rbw = CS_READ; /* 0:read, 1:write */
	voq_lkup_data0.bf.txem_crcstate = 0;

	voq_lkup_access_mask.bf.access = 1;
	voq_lkup_access_mask.bf.address = 0x7F;
	voq_lkup_access_mask.bf.debug_mode = 1;
	voq_lkup_access_mask.bf.rbw = 1;
	voq_lkup_data0.bf.txem_crcstate = 0x7FFFFFF;

	write_reg(voq_lkup_access.wrd, voq_lkup_access_mask.wrd,
		  NI_TOP_NI_TX_VOQ_LKUP_ACCESS);

	do {
		access = (readl(NI_TOP_NI_TX_VOQ_LKUP_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);

	/* read data 0, don't care txem_crcstate */
	voq_lkup_data = readl(NI_TOP_NI_TX_VOQ_LKUP_DATA0) & 0x1F;
	printk("Tx Voq Lkup Table\t %3d :\t  0x%02X\n", voq_number,
	       voq_lkup_data);
}
#endif

static void cs_ni_voq_map_cfg(void)
{
	int i;

#ifdef CS752X_MANAGEMENT_MODE
	/* for management port only */
	/* GE1: 0x0001 */
	for (i = 0; i <= 7; i++)
		cs_ni_set_voq_map(i, NI_VOQ_DID_GE1, 0);

#else
	for (i = 0; i <= 7; i++) {
		/* GE0, Voq: 0 ~ 7 */
		cs_ni_set_voq_map(i + GE_PORT0_VOQ_BASE, NI_VOQ_DID_GE0, 1);
		/* GE1, Voq: 8 ~ 15 */
		cs_ni_set_voq_map(i + GE_PORT1_VOQ_BASE, NI_VOQ_DID_GE1, 1);
		/* GE2, Voq: 16 ~ 23 */
		cs_ni_set_voq_map(i + GE_PORT2_VOQ_BASE, NI_VOQ_DID_GE2, 1);
		/* Encap and Crypto Core, Voq: 24 ~ 31 & 32 ~ 39 */
		cs_ni_set_voq_map(i + ENCRYPTION_VOQ_BASE, NI_VOQ_DID_CRYPTO,
				  1);
		cs_ni_set_voq_map(i + ENCAPSULATION_VOQ_BASE, NI_VOQ_DID_ENCAP,
				  1);
		/* for Multicast: 40 ~ 47 */
		cs_ni_set_voq_map(i + ROOT_PORT_VOQ_BASE, NI_VOQ_DID_MC, 1);
		/* for CPU 0 Voq: 48 ~ 55 */
		cs_ni_set_voq_map(i + CPU_PORT0_VOQ_BASE, NI_VOQ_DID_CPU0, 1);
		/* for CPU 1 Voq: 56 ~ 63 */
		cs_ni_set_voq_map(i + CPU_PORT1_VOQ_BASE, NI_VOQ_DID_CPU1, 1);
		/* for CPU 2 Voq: 64 ~ 71 */
		cs_ni_set_voq_map(i + CPU_PORT2_VOQ_BASE, NI_VOQ_DID_CPU2, 1);
		/* for CPU 3 Voq: 72 ~ 79 */
		cs_ni_set_voq_map(i + CPU_PORT3_VOQ_BASE, NI_VOQ_DID_CPU3, 1);
		/* for CPU 4 Voq: 80 ~ 87 */
		cs_ni_set_voq_map(i + CPU_PORT4_VOQ_BASE, NI_VOQ_DID_CPU4, 1);
		/* for CPU 5 Voq: 88 ~ 95 */
		cs_ni_set_voq_map(i + CPU_PORT5_VOQ_BASE, NI_VOQ_DID_CPU5, 1);
		/* for CPU 6 Voq: 96 ~ 103 */
		cs_ni_set_voq_map(i + CPU_PORT6_VOQ_BASE, NI_VOQ_DID_CPU6, 1);
		/* for CPU 7 Voq: 104 ~ 111 */
		cs_ni_set_voq_map(i + CPU_PORT7_VOQ_BASE, NI_VOQ_DID_CPU7, 1);
	}

#if 0 /* comment it out.. no need to print this all the times */
	printk("\t 	VoQ MAP: \n");
	printk("\t	0-23: Ports 0-2\n");
	printk("\t	24-31: Encryption Port\n");
	printk("\t	32-39: Encapsulation Port\n");
	printk("\t	40-47: Root Port\n");
	printk("\t	48-55: CPU 0 VoQs\n");
	printk("\t	56-63: CPU 1 VoQs\n");
	printk("\t	64-71: CPU 2 VoQs\n");
	printk("\t	72-79: CPU 3 VoQs\n");
	printk("\t	80-87: CPU 4 VoQs\n");
	printk("\t	88-95: CPU 5 VoQs\n");
	printk("\t	96-103: CPU 6 VoQs\n");
	printk("\t	104-111: CPU 7 VoQs\n");
	printk("***************************\n");
	printk("Tx Voq Lkup Table\t\"Table Name\":\t\"CRC_E & Dest VoQ id\"\n");
	for (i = 0; i < 111; i++)
		cs_ni_get_voq_map(i);
#endif
#endif
}

void cs_ni_set_mc_table(u8 mc_index, u16 mc_vec)
{
	NI_TOP_NI_MC_INDX_LKUP_ACCESS_t mc_lkup_access, mc_lkup_access_mask;
	NI_TOP_NI_MC_INDX_LKUP_DATA_t mc_lkup_data, mc_lkup_data_mask;
	u8 access;

	mc_lkup_data.wrd = 0;
	mc_lkup_data_mask.wrd = 0;
	mc_lkup_data.bf.mc_vec = mc_vec;
	mc_lkup_data_mask.bf.mc_vec = 0xFFFF;
	write_reg(mc_lkup_data.wrd, mc_lkup_data_mask.wrd,
		  NI_TOP_NI_MC_INDX_LKUP_DATA);

	mc_lkup_access.wrd = 0;
	mc_lkup_access_mask.wrd = 0;

	mc_lkup_access.bf.access = 1;
	mc_lkup_access.bf.address = mc_index;
	mc_lkup_access.bf.rbw = CS_WRITE;

	mc_lkup_access_mask.bf.access = 1;
	mc_lkup_access_mask.bf.rbw = 1;
	mc_lkup_access_mask.bf.address = 0xFF;
	write_reg(mc_lkup_access.wrd, mc_lkup_access_mask.wrd,
		  NI_TOP_NI_MC_INDX_LKUP_ACCESS);
	do {
		access = (readl(NI_TOP_NI_MC_INDX_LKUP_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);
}

#if defined(MULTIPLE_VTABLE) || defined(CS_UU_TEST)
static void cs_ni_get_mc_table(u8 mc_index)
{
	NI_TOP_NI_MC_INDX_LKUP_ACCESS_t mc_lkup_access, mc_lkup_access_mask;
	u8 access;
	u32 mc_table_data;

	mc_lkup_access.wrd = 0;
	mc_lkup_access_mask.wrd = 0;

	mc_lkup_access.bf.access = 1;
	mc_lkup_access.bf.address = mc_index;
	mc_lkup_access.bf.rbw = CS_READ;

	mc_lkup_access_mask.bf.access = 1;
	mc_lkup_access_mask.bf.address = 0xFF;
	mc_lkup_access_mask.bf.rbw = 1;

	write_reg(mc_lkup_access.wrd, mc_lkup_access_mask.wrd,
		  NI_TOP_NI_MC_INDX_LKUP_ACCESS);

	do {
		access = (readl(NI_TOP_NI_TX_VOQ_LKUP_ACCESS) & 0x80000000);
		udelay(1);
	} while (access == 0x80000000);

	mc_table_data = readl(NI_TOP_NI_MC_INDX_LKUP_DATA) & 0xFFFF;
	printk("MC Lkup Table\t %3d :\t  0x%04X\n", mc_index, mc_table_data);
}
#endif

static u32 cs_ni_get_phy_vendor(int phy_addr)
{
	u32 reg_val;
	reg_val = (ni_mdio_read(phy_addr, 0x02) << 16) +
		ni_mdio_read(phy_addr, 0x03);

	return reg_val;
}

static void cs_ne_init_cfg(void)
{
	int i;
	ni_info_t *ni;
	NI_TOP_NI_CPUXRAM_CFG_t ni_xram_cfg, ni_xram_cfg_mask;
#ifdef BYPASS_FE
	NI_TOP_NI_RX_CNTRL_CONFIG1_0_t rx_ctl_cfg1, rx_ctl_cfg1_mask;
#endif
	NI_TOP_NI_RX_CNTRL_CONFIG0_0_t rx_ctl_cfg0, rx_ctl_cfg0_mask;
	NI_TOP_NI_PKT_LEN_CONFIG_t pkt_len_cfg, pkt_len_cfg_mask;
	NI_TOP_NI_ETH_MGMT_PT_CONFIG_t mgmt_pt_cfg, mgmt_pt_cfg_mask;
	NI_TOP_NI_MISC_CONFIG_t misc_config, misc_config_mask;
	PER_MDIO_CFG_t mdio_cfg, mdio_cfg_mask;
	NI_TOP_NI_RX_AGG_CONFIG_t rx_agg_config, rx_agg_config_mask;
#ifndef CONFIG_CORTINA_FPGA
	GLOBAL_IO_DRIVE_STRENGTH_t driver_strength, driver_strength_mask;
#endif
	cs_ni_reset();

	/*
	 * acarr - Mar-10-2011
	 * For FPGA reduce ready signal threshold from NI-->SCH.
	 * This is because the NI will get an TX FIFO Overflow if too much data
	 * is dequeued from the scheduler.
	 * Default value was: 0x082a00f8
	 * New value is:      0x08298078
	 * This will make the:
	 * rdy_low_thld_ge=>0xc0
	 * rdy_high_thld_ge=>0x78
	 */
#ifdef CONFIG_CORTINA_FPGA
	{
		// acarr - Mar-10-2011
		NI_TOP_NI_SCH_BP_THLD_ETH_t ni_sch_bp_thld_eth_cfg;
		NI_TOP_NI_SCH_BP_THLD_ETH_t ni_sch_bp_thld_eth_mask;
		ni_sch_bp_thld_eth_cfg.wrd = 0x08298078;
		ni_sch_bp_thld_eth_mask.wrd = 0xffffffff;
		write_reg(ni_sch_bp_thld_eth_cfg.wrd, ni_sch_bp_thld_eth_mask.wrd,
			  NI_TOP_NI_SCH_BP_THLD_ETH);
	}
#else
	/* ASIC use default value */
	writel(0x082a00f8, NI_TOP_NI_SCH_BP_THLD_ETH);
#endif

	/* Hold all macs in reset */
	for (i = 0; i < GE_PORT_NUM; i++)
		writel(0xc0100800, NI_TOP_NI_ETH_MAC_CONFIG0_0 + (i * 12));

	if (ne_initialized == 0) {
		ne_initialized = 1;
		/* initli private data */
		ni = &ni_private_data;
		memset((void *)ni, 0, sizeof(ni_info_t));
		ni->neEnabled = 1; /* Default enable NetEngine */

		/* FIXME: We may need different value in ASIC */
		mdio_cfg.wrd = 0;
		mdio_cfg_mask.wrd = 0;
		mdio_cfg.bf.mdio_pre_scale = 0x20; /* MDC clock */
		mdio_cfg_mask.bf.mdio_pre_scale = 0xFFFF;
		write_reg(mdio_cfg.wrd, mdio_cfg_mask.wrd, PER_MDIO_CFG);
	}

#ifndef CONFIG_CORTINA_FPGA
	/* Engineer board adjust driver current to 12mA */
	driver_strength.wrd = 0;
	driver_strength_mask.wrd = 0;
	driver_strength.bf.gmac_ds = 3;
	driver_strength_mask.bf.gmac_ds = 3;
	write_reg(driver_strength.wrd, driver_strength.wrd,
		  GLOBAL_IO_DRIVE_STRENGTH);
#endif

	/* Receive port calendar is enabled. */
	rx_agg_config.wrd = 0;
	rx_agg_config_mask.wrd = 0;
	rx_agg_config.bf.rx_port_cal_dis = 0;
	rx_agg_config_mask.bf.rx_port_cal_dis = 1;
	write_reg(rx_agg_config.wrd, rx_agg_config_mask.wrd,
		  NI_TOP_NI_RX_AGG_CONFIG);
#ifdef CS752X_LINUX_MODE
	/* config port calendar */
	cs_ni_port_cal_cfg();
	//printk("Ni Rx Port Cal Table Map:\n");
	//printk("0 - GE0\n");
	//printk("1 - GE1\n");
	//printk("2 - GE2\n");
	//printk("3 - CPU\n");
	//printk("4 - Crypto\n");
	//printk("5 - Encap\n");
	//printk("6 - Mcast\n");
	//printk("7 - Mirror\n");
	//printk("*************************\n");
	//for (i = 0; i < NI_MAX_PORT_CALENDAR; i++)
	//	ni_get_port_calendar(i);
#endif

	/* initial QM */
	cs_qm_init_cfg();

	/* initial NI XRAM */
	cs_ni_init_xram_mem();

	for (i = 0; i < XRAM_RX_INSTANCE; i++)
		cs_ni_set_xram_cfg(i, XRAM_DIRECTION_RX);

	for (i = 0; i < XRAM_TX_INSTANCE; i++)
		cs_ni_set_xram_cfg(i, XRAM_DIRECTION_TX);

	ni_xram_cfg.wrd = 0x3ff;
	ni_xram_cfg_mask.wrd = 0x3ff;

	/* The xram resets needs a toggle to load the pointers correctly */
	write_reg(ni_xram_cfg.wrd, ni_xram_cfg_mask.wrd, NI_TOP_NI_CPUXRAM_CFG);
	ni_xram_cfg.wrd = 0x0;
	write_reg(ni_xram_cfg.wrd, ni_xram_cfg_mask.wrd, NI_TOP_NI_CPUXRAM_CFG);

#ifdef CS752X_LINUX_MODE
	cs_qm_init_cpu_path_cfg();
#endif

#ifdef CS752x_DMA_LSO_MODE
	/* Initial TX DMA LSO */
	cs_init_tx_dma_lso();
#endif

#ifdef BYPASS_FE
	/* By pass FE */
	rx_ctl_cfg1.wrd = 0;
	rx_ctl_cfg1_mask.wrd = 0;
	rx_ctl_cfg1.bf.rxctrl_byp_en = 1;
	rx_ctl_cfg1_mask.bf.rxctrl_byp_en = 1;
	rx_ctl_cfg1.bf.rxctrl_byp_cpuptp = 1;
	rx_ctl_cfg1_mask.bf.rxctrl_byp_cpuptp = 1;

	/* 8 instances, By pass FE */
	rx_ctl_cfg1.wrd = 0;
	rx_ctl_cfg1_mask.wrd = 0;
	for (i = 0; i < XRAM_RX_INSTANCE; i++)
		write_reg(rx_ctl_cfg1.wrd, rx_ctl_cfg1_mask.wrd,
			  NI_TOP_NI_RX_CNTRL_CONFIG1_0 + (i * 8));
#else
	for (i = 0; i < XRAM_RX_INSTANCE; i++)
		writel(0, NI_TOP_NI_RX_CNTRL_CONFIG1_0 + (i * 8));

	/* Allow NI send small size packet from CPU port */
	rx_ctl_cfg0.wrd = 0;
	rx_ctl_cfg0_mask.wrd = 0;
	rx_ctl_cfg0.bf.runt_drop_dis = 1;
	rx_ctl_cfg0_mask.bf.runt_drop_dis = 1;
	/* For GE0, GE1, GE2, MCAST,
	   MIRROR ports you should have a value of 0x00400400.
	   Only CPU, Crypto, Encap ports should have 0x00400440
	   0 - GE0
	   1 - GE1
	   2 - GE2
	   3 - CPU
	   4 - Crypto
	   5 - Encap
	   6 - Mcast
	   7 - Mirror
	*/
	for (i = 3; i < 6; i++)
		write_reg(rx_ctl_cfg0.wrd, rx_ctl_cfg0_mask.wrd,
			NI_TOP_NI_RX_CNTRL_CONFIG0_0 + (i * 8));
#endif /* BYPASS_FE */

	/* NI_SCH Back Pressure Reg, set for eth0,1,2 */
	for (i = 0; i < 3; i++) {
		NI_TOP_NI_CPUXRAM_SCH_BP_CFG_0_t sch_bp_reg;
		sch_bp_reg.bf.xram_sch_rdy_mode = 1;
		sch_bp_reg.bf.xram_sch_rdy_free_thld = 30;
		writel(sch_bp_reg.wrd, NI_TOP_NI_CPUXRAM_SCH_BP_CFG_0 + i * 4);
	}

#if 0
	{
		/* NI XRAM DEBUG MODE */
		NI_TOP_NI_CPUXRAM_CFG_t xram_cfg_reg;

		xram_cfg_reg.wrd = readl(NI_TOP_NI_CPUXRAM_CFG_dft);
//              xram_cfg_reg.bf.xram_cntr_debug_mode = 1;
		writel(xram_cfg_reg.wrd, NI_TOP_NI_CPUXRAM_CFG);
	};
#endif

	pkt_len_cfg.wrd = 0;
	pkt_len_cfg_mask.wrd = 0;
	/* Maximum allowed Receive Packet size */
	pkt_len_cfg.bf.max_pkt_size = MAX_FRAME_SIZE;
	/* Minimum allowed Receive Packet size */
	pkt_len_cfg.bf.min_pkt_size = MIN_FRAME_SIZE;

	pkt_len_cfg_mask.bf.max_pkt_size = 0x3FFF;
	pkt_len_cfg_mask.bf.min_pkt_size = 0x3FF;
	write_reg(pkt_len_cfg.wrd, pkt_len_cfg_mask.wrd,
		  NI_TOP_NI_PKT_LEN_CONFIG);

	/* GE 1 assigned as management port */
	mgmt_pt_cfg.wrd = 0;
	mgmt_pt_cfg_mask.wrd = 0;
#ifdef CS752X_MANAGEMENT_MODE
	mgmt_pt_cfg.bf.port_to_cpu = GE_PORT1;
#else
	mgmt_pt_cfg.bf.port_to_cpu = 3;
#endif /* CS752X_MANAGEMENT_MODE */
	/* management port data is sent to XRAM only */
	mgmt_pt_cfg.bf.mgmt_pt_to_fe_also = 0;
	mgmt_pt_cfg_mask.bf.port_to_cpu = 0x3;
	mgmt_pt_cfg_mask.bf.mgmt_pt_to_fe_also = 1;
	write_reg(mgmt_pt_cfg.wrd, mgmt_pt_cfg_mask.wrd,
		  NI_TOP_NI_ETH_MGMT_PT_CONFIG);

	/*
	 * Tx/RX MIB counters counts statistics from good and bad frames,
	 * for debug purpose.
	 * */
	misc_config.wrd = 0;
	misc_config_mask.wrd = 0;
	misc_config.bf.rxmib_mode = 1;
	misc_config.bf.txmib_mode = 1;
#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
	/* FIXME: for vtable multicast, if NI have problem, check this bits,
	 * for debug */
#ifdef MULTIPLE_VTABLE
	misc_config.bf.mc_accept_all = 1;
#endif /* MULTIPLE_VTABLE */
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#endif /* #if 0 */
	misc_config_mask.bf.rxmib_mode = 1;
	misc_config_mask.bf.txmib_mode = 1;
	misc_config_mask.bf.mc_accept_all = 1;
	write_reg(misc_config.wrd, misc_config_mask.wrd, NI_TOP_NI_MISC_CONFIG);
#ifdef BYPASS_FE
	/* By pass  bypass_pr, bypass_pe for debugging QM linux mode */
	writel(0x800010d8, FETOP_FE_PE_CONFIG);
#endif /* BYPASS_FE */

#ifdef CS752X_LINUX_MODE
	/* Insert VoQ 48 ~ 111 CPU header for debugging QM linux mode */
#define  PE_SET_MIN_CYCLE_PPARSER 0xC000
#define  PE_SEND_ALL_CPU_HDR 0x3FF
	writel(PE_SEND_ALL_CPU_HDR | PE_SET_MIN_CYCLE_PPARSER,
	       FETOP_FE_PE_CONFIG_1);

	cs_ni_voq_map_cfg();
	//FIXME: for debug temp disab RX flow control
	//for(i = 0; i < 3 ; i++) {
	//ni_disable_rx_flow_control(i);
	//}
#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
#ifdef CS_UU_TEST
#ifdef NEW_L2_DESIGN
#define UU_COPIES 0x2 /* 0x0010 */
#else
#define UU_COPIES 0x6 /* 0x0110 */
#endif
	//FIXME: how many copies, 2 times ?
	cs_ni_set_mc_table(CS_VTBL_ID_UU_FLOW, UU_COPIES);
	printk("MC Lkup Table\t\"Table Name\":\t\"MC vec number\"\n");
	//for(i = 0; i <= CS_VTBL_ID_UU_FLOW ; i++) {
	cs_ni_get_mc_table(CS_VTBL_ID_UU_FLOW);
	//}
#endif
#ifdef MULTIPLE_VTABLE
	cs_ni_set_mc_table(CS_VTBL_ID_L2_RULE, 1);
	cs_ni_set_mc_table(CS_VTBL_ID_L2_FLOW, 1);
	cs_ni_set_mc_table(CS_VTBL_ID_L3_RULE, 1);
	cs_ni_set_mc_table(CS_VTBL_ID_L3_FLOW, 1);
	printk("MC Lkup Table\t\"Table Name\":\t\"MC vec number\"\n");
	for (i = 0; i <= CS_VTBL_ID_L3_FLOW; i++)
		cs_ni_get_mc_table(i);
#endif /* MULTIPLE_VTABLE */
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#endif
#endif /* CS752X_LINUX_MODE */
} /* cs_ne_init_cfg */

static inline void cs_ni_netif_stop(mac_info_t *tp)
{
	/*
	 * FIXME!! need to worry about this.. since all 3 NIs share the
	 * same NAPI, we might not want to disable NAPI just because one
	 * of the interface goes down.
	 */
#ifdef CS752X_NI_NAPI
	napi_disable(&ni_private_data.napi);
#endif

	netif_tx_disable(tp->dev);
	netif_carrier_off(tp->dev);
} /* cs_ni_netif_stop */

static inline void cs_ni_netif_start(mac_info_t *tp)
{
	netif_tx_wake_all_queues(tp->dev);

	/*
	 * FIXME!! need to check phy link status before calling the following
	 * line.
	 */
	netif_carrier_on(tp->dev);

	/*
	 * FIXME! Making sure the NAPI wasn't enabled before calling the
	 * following, because other NIs might still be up and running.
	 */
#ifdef CS752X_NI_NAPI
	napi_enable(&ni_private_data.napi);
#endif

	//cs_enable_ints(tp);	/* FIXME: not finished */

} /* cs_ni_netif_start */
#if 0
static int cs_ni_init_hw(mac_info_t *tp, int reset_phy)
{
	if (reset_phy == 1)
		cs_ni_phy_start(tp);

	/* FIXME!! more.. Implement */
	return 0;
} /* cs_ni_init_hw */
#endif
static void cs_ni_disable_interrupts(void)
{
	int i;
	writel(0, NI_TOP_NI_CPUXRAM_TXPKT_INTENABLE_0);

	for (i=0; i<= XRAM_INST_MAX; i++) {
		writel(0, NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + (i * 8));
	}
}

static inline void cs_ni_enable_interrupts(void)
{
	int i;
	// tx complete interrupts are not enabled
	for (i=0; i <= XRAM_INST_MAX; i++)
		writel(ni_private_data.intr_cpuxram_rxpkt_mask,
			NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + (i * 8));
}

/*
 * Reset all TX rings.
 */
static inline void cs_ni_reset_tx_ring(void)
{
	ni_info_t *ni = &ni_private_data;
	dma_swtxq_t *swtxq;
	volatile dma_rptr_t rptr_reg;
	volatile dma_wptr_t wptr_reg;
	u32 desc_count;
	dma_txdesc_t *curr_desc, *tmp_desc;
	dma_txdesc_0_t word0;
	int i, new_idx;

	for (i=0; i< NI_DMA_LSO_TXQ_NUM; i++) {
		swtxq = &ni->swtxq[i];
		// swtxq->intr_cnt++; // needed?
		rptr_reg.bits32 = readl(swtxq->rptr_reg);
		wptr_reg.bits32 = readl(swtxq->wptr_reg);

		while (wptr_reg.bits.wptr != swtxq->finished_idx) {
			curr_desc = swtxq->desc_base + swtxq->finished_idx;
			word0.bits32 = curr_desc->word0.bits32;
			// we do not check ownership at all as in reset
			desc_count = word0.bits.desc_cnt;
			if (desc_count > 1)
				new_idx = (swtxq->finished_idx + desc_count -1) &
					(swtxq->total_desc_num - 1);
			else
				new_idx = swtxq->finished_idx;
			tmp_desc = swtxq->desc_base + new_idx;

			if (swtxq->tx_skb[new_idx]) {
				dev_kfree_skb_any(swtxq->tx_skb[new_idx]);
				swtxq->tx_skb[new_idx] = NULL;
				swtxq->curr_finished_desc = tmp_desc;
				swtxq->total_finished ++; // or not update?
				swtxq->finished_idx = RWPTR_ADVANCE_ONE(new_idx,
						swtxq->total_desc_num);
			}
		}

		swtxq->finished_idx = rptr_reg.bits32;
		wptr_reg.bits32 = rptr_reg.bits32;	// set queue empty now
		writel(wptr_reg.bits32, swtxq->wptr_reg);
		DBG(printk("\t%s::queue %d, rptr %d, set wptr %d\n",
				__func__, i, rptr_reg.bits32, wptr_reg.bits32));
	}
}

static void cs_ni_reset_task(struct work_struct *work)
{
	mac_info_t *tp;
	struct net_device *dev;
	int i;

	rtnl_lock();
#ifdef CS752X_NI_NAPI
	napi_disable(&ni_private_data.napi);
#endif
	// reset tx ring
	cs_ni_reset_tx_ring();

	for (i=0; i<GE_PORT_NUM; i++) {
		dev = ni_private_data.dev[i];
		tp  = netdev_priv(dev);
		if (dev == NULL)
			continue;
#if 0
		if (tp->phydev)
			phy_start(tp->phydev);
		else {
			printk("%s::enable GE1 port?\n", __func__);
		}
#endif
		netif_tx_wake_all_queues(dev);	// enable tx
	}
#ifdef CS752X_NI_NAPI
	napi_enable(&ni_private_data.napi);
#endif
	cs_ni_enable_interrupts();

	rtnl_unlock();
#if 0
	mac_info_t *tp = container_of(work, mac_info_t, reset_task);

	rtnl_lock();
	if (!netif_running(tp->dev)) {
		printk("%s::\n", __func__);
		rtnl_unlock();
		return;
	}

	cs_ni_phy_stop(tp);

	cs_ni_netif_stop(tp);

	cs_ni_init_hw(tp, 1);

	cs_ni_netif_start(tp);
#endif
} /* cs_ni_reset_task */

#if 0 /* don't know why this code is not used anymore */
static void cs_ne_system_reset(void)
{
	GLOBAL_BLOCK_RESET_t global_rest, global_rest_mask;

	global_rest.wrd = 0;
	global_rest_mask.wrd = 0;
	global_rest.bf.reset_qm = 1;
	global_rest.bf.reset_sch = 1;
	global_rest.bf.reset_tm = 1;
	global_rest.bf.reset_fe = 1;
	global_rest.bf.reset_ni = 1;

	global_rest_mask.bf.reset_qm = 0x1;
	global_rest_mask.bf.reset_sch = 0x1;
	global_rest_mask.bf.reset_tm = 0x1;
	global_rest_mask.bf.reset_fe = 0x1;
	global_rest_mask.bf.reset_ni = 0x1;
	write_reg(global_rest.wrd, global_rest_mask.wrd, GLOBAL_BLOCK_RESET);

	printk("%s: Reset Global QM, SCH, TM, FE, NI\n", __func__);
	mdelay(100);
}
#endif

#ifdef CS752X_MANAGEMENT_MODE
static int cs_mfe_set_mac_address(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);
	u8 *dev_addr;
	u32 mac_fifth_byte, mac_sixth_byte, low_mac;
	NI_TOP_NI_ETH_MAC_CONFIG2_0_t mac_config2, mac_config2_mask;
	NI_TOP_NI_MAC_ADDR1_t mac_addr1, mac_addr1_mask;

	dev_addr = dev->dev_addr;

	low_mac = dev_addr[0] | (dev_addr[1] << 8) | (dev_addr[2] << 16) |
	    (dev_addr[3] << 24);
	mac_fifth_byte = dev_addr[4];
	mac_sixth_byte = dev_addr[5];

	printk("Mgmt Setting MAC address for GE-%d: ", tp->port_id);
	printk("%02x:%02x:", dev->dev_addr[0], dev->dev_addr[1]);
	printk("%02x:%02x:", dev->dev_addr[2], dev->dev_addr[3]);
	printk("%02x:%02x\n", dev->dev_addr[4], dev->dev_addr[5]);

	/*
	 * used as SA while sending pause frames. Also used to detect WOL magic
	 * packets.
	 */
	writel(low_mac, NI_TOP_NI_MAC_ADDR0);

	mac_addr1.wrd = 0;
	mac_addr1_mask.wrd = 0;
	mac_addr1.bf.mac_addr1 = mac_fifth_byte;
	mac_addr1_mask.bf.mac_addr1 = 0xFF;
	write_reg(mac_addr1.wrd, mac_addr1_mask.wrd, NI_TOP_NI_MAC_ADDR1);

	mac_config2.wrd = 0;
	mac_config2_mask.wrd = 0;
	mac_config2.bf.mac_addr6 = mac_sixth_byte;
	mac_config2_mask.bf.mac_addr6 = mac_sixth_byte;
	switch (tp->port_id) {
	case GE_PORT0:
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0);
		break;
	case GE_PORT1:
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0 + 12);
		break;
	case GE_PORT2:
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0 + 24);
		break;
	}

	return 0;
}
#endif /* CS752X_MANAGEMENT_MODE */

static void set_mac_swap_order(unsigned char *dest, unsigned char *src, int len)
{
	int i;
	for (i=0; i<len; i++)
		*(dest+len-1-i) = *(src+i);
}

static void cs_init_lpb_an_bng_mac(void)
{
	mac_info_t *tp;
	fe_lpb_entry_t lpb_entry;
	fe_an_bng_mac_entry_t abm_entry;
	struct net_device *dev;
	int i, ret;

	/* setting up AN BNG MAC first */
	memset(&abm_entry, 0x0, sizeof(abm_entry));
	abm_entry.sa_da = 0;
	abm_entry.valid = 1;
	abm_entry.pspid_mask = 0;
	for (i = 0; i < 3; i++) {
		dev = ni_private_data.dev[i];
		tp = netdev_priv(dev);
		abm_entry.pspid = tp->port_id;
		set_mac_swap_order(abm_entry.mac, (unsigned char*)tp->mac_addr, 6);
		//memcpy(abm_entry.mac, tp->mac_addr, 6);
		if (tp->an_bng_mac_idx == 0xffff)
			ret = cs_fe_table_add_entry(FE_TABLE_AN_BNG_MAC,
						    &abm_entry,
						    &tp->an_bng_mac_idx);
		else
			ret = cs_fe_table_set_entry(FE_TABLE_AN_BNG_MAC,
						    tp->an_bng_mac_idx,
						    &abm_entry);
		if (ret != 0) {
			printk
			    ("%s:%d:unable to set up MAC to FE for port#%d ret %d\n",
			     __func__, __LINE__, tp->port_id, ret);
			// FIXME! any debug method?
		} else {
//                      printk("%s:%d:Setting port#%d with an_bng_mac idx %d\n",
//                              __func__, __LINE__, tp->port_id,
//                              tp->an_bng_mac_idx);
		}
	}

	/* setting up LPB */
	memset(&lpb_entry, 0x0, sizeof(lpb_entry));
	lpb_entry.pvid = 4095;
	for (i = 0; i < FE_LPB_ENTRY_MAX; i++) {
		lpb_entry.lspid = i;
		ret = cs_fe_table_set_entry(FE_TABLE_LPB, i, &lpb_entry);
		if (ret != 0)
			printk("%s:error setting up LPB\n", __func__);
	}
	return;
}

static void cs_ne_global_interrupt(void)
{
	GLOBAL_NETWORK_ENGINE_INTENABLE_0_t global_interrupt,
	    global_interrupt_mask;

	/* enable golable NI, XRAM interrupt */
	global_interrupt.wrd = 0;
	global_interrupt_mask.wrd = 0;

#ifndef CONFIG_INTR_COALESCING
	global_interrupt.bf.NI_XRAM_RXe = 1;
	global_interrupt_mask.bf.NI_XRAM_RXe = 1;
#endif

	/* enable gloabl QM/SCH interrupt for QM linux mode */
	global_interrupt.bf.SCHe = 1;
	global_interrupt_mask.bf.SCHe = 1;
#ifdef CS752X_LINUX_MODE
	global_interrupt.bf.QMe = 0;
	global_interrupt_mask.bf.QMe = 1;
#endif	/* CS752X_LINUX_MODE */

	global_interrupt.bf.NI_XRAM_TXe = 1;
	global_interrupt_mask.bf.NI_XRAM_TXe = 1;
	global_interrupt.bf.NIe = 1;
	global_interrupt_mask.bf.NIe = 1;

	write_reg(global_interrupt.wrd, global_interrupt_mask.wrd,
		  GLOBAL_NETWORK_ENGINE_INTENABLE_0);
}

static void cs_ni_init_interrupt_cfg(void)
{
	int i;
	ni_info_t *ni;
#ifdef CONFIG_INTR_COALESCING
	NI_TOP_NI_CPUXRAM_INT_COLSC_CFG_0_t reg;
#endif

	ni = &ni_private_data;

	/* enable golable NI, XRAM interrupt */
	cs_ne_global_interrupt();

	/* Enable NI interrupt */
	for (i = 0; i < XRAM_INST_MAX; i++) {	/* not only 3, max is 8 */
		ni->intr_port_mask[i] = 0x1FF;
		writel(ni->intr_port_mask[i], NI_TOP_NI_PORT_0_INTENABLE_0
			+ (i * 8));
	}

	ni->intr_mask = 0xFFFFFFFF;
	ni->intr_txem_mask = 0x3FF;
#if 1
	ni->intr_rxfifo_mask = 0x0;
	ni->intr_cpuxram_cnt_mask = 0x0;
	ni->intr_cpuxram_err_mask = 0x0;
#else
	ni->intr_rxfifo_mask = 0x3FFF;
	ni->intr_cpuxram_cnt_mask = 0xFFFFFFFF;
	ni->intr_cpuxram_err_mask = 0xFFFFFF;
#endif
	//ni->intr_cpuxram_rxpkt_mask = 0x1FF;
      	//ni->intr_cpuxram_txpkt_mask = 0x3; //FIXME: if enable, need clear
	ni->intr_cpuxram_rxpkt_mask = 0x1;

	writel(ni->intr_mask, NI_TOP_NI_INTENABLE_0);
	writel(ni->intr_rxfifo_mask, NI_TOP_NI_RXFIFO_INTENABLE_0);
	writel(ni->intr_txem_mask, NI_TOP_NI_TXEM_INTENABLE_0);
	//writel(ni->intr_cpuxram_txpkt_mask, NI_TOP_NI_CPUXRAM_TXPKT_INTENABLE_0);

	writel(ni->intr_cpuxram_cnt_mask, NI_TOP_NI_CPUXRAM_CNTR_INTENABLE_0);
	/* Keep Xram interrupt disabled till device is registered */
	writel(ni->intr_cpuxram_err_mask, NI_TOP_NI_CPUXRAM_ERR_INTENABLE_0);

#ifdef CONFIG_INTR_COALESCING
	for (i = 0; i <= XRAM_INST_MAX; i++)
		writel(ni->intr_cpuxram_rxpkt_mask,
		       NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + (i * 8));
#else
	writel(ni->intr_cpuxram_rxpkt_mask,
	       NI_TOP_NI_CPUXRAM_RXPKT_INTENABLE_0);
#endif

#ifdef CS752X_LINUX_MODE
	/* Enable QM interrupt */
	ni->intr_qm_mask = 0x077703FF; /* enable all QM interrupt for debug */
	writel(ni->intr_qm_mask, QM_INTENABLE_0);
	/* Enable SCH interrupt */
	ni->intr_sch_mask = 0x1F; /* enable all SCH interrupt for debug */
	writel(ni->intr_sch_mask, SCH_INTENABLE_0);
#endif

#ifdef CS752x_DMA_LSO_MODE
	/* Enable DMA LSO interrupt */
	ni->intr_dma_lso_mask = 0x1FFFFFF; /* enable all LSO interrupt for debug */
	writel(ni->intr_sch_mask, DMA_DMA_LSO_DMA_LSO_INTENABLE_0);
	//ni->intr_dma_lso_desc_mask = 0x3;/* enable all LSO interrupt for debug */
	ni->intr_dma_lso_desc_mask = 0x0; /* enable all LSO interrupt for debug */
	writel(ni->intr_sch_mask, DMA_DMA_LSO_DESC_INTENABLE);

	for (i = 0; i < NI_DMA_LSO_RXQ_NUM; i++) {
		ni->intr_lso_rx_mask[i] = 0x3F;
		writel(ni->intr_lso_rx_mask[i],
		       DMA_DMA_LSO_RXQ0_INTENABLE + (i * 8));
	}

	for (i = 0; i < NI_DMA_LSO_TXQ_NUM; i++) {
		ni->intr_lso_rx_mask[i] = 0xF;
		writel(ni->intr_lso_rx_mask[i],
		       DMA_DMA_LSO_TXQ0_INTENABLE + (i * 8));
	}

	ni->intr_dma_lso_bmc_mask = 0x1;
	writel(ni->intr_dma_lso_bmc_mask, DMA_DMA_LSO_BMC0_INTENABLE);
#endif /* CS752x_DMA_LSO_MODE */

#ifdef CONFIG_INTR_COALESCING
	/* Interrupt on first packet and then number of packets */
	reg.bf.int_colsc_en = 0; /* disable as default */
	reg.bf.int_colsc_first_en = NI_INTR_COALESCING_FIRST_EN;
	reg.bf.int_colsc_pkt = NI_INTR_COALESCING_PKT;
	writel(reg.wrd, NI_TOP_NI_CPUXRAM_INT_COLSC_CFG_0);
#endif
}

static void ni_set_mac_tx_rx(u8 port, u8 flag)
{
	NI_TOP_NI_ETH_MAC_CONFIG0_0_t config0, config0_mask;
	u32 config_addr;

	config0.wrd = 0;
	config0_mask.wrd = 0;
	config0.bf.mac_tx_rst = 0; /* Normal operation */
	config0.bf.mac_rx_rst = 0; /* Normal operation */
	config0.bf.rx_en = flag; /* 0: disable Rx MAC */
	config0.bf.tx_en = flag; /* 0: disable Tx MAC */
	config0_mask.bf.mac_tx_rst = 1;
	config0_mask.bf.mac_rx_rst = 1;
	config0_mask.bf.rx_en = 1;
	config0_mask.bf.tx_en = 1;
	config_addr = NI_TOP_NI_ETH_MAC_CONFIG0_0;

	config_addr = (port * 12) + config_addr;

	write_reg(config0.wrd, config0_mask.wrd, config_addr);
}

void cs_ni_set_short_term_shaper(mac_info_t *tp)
{
	unsigned int shaper_rate;

	/* setting up short-term shaper in scheduler based on linkup sppeed */
	if (tp->link_config.speed == SPEED_10)
		shaper_rate = 20 * 1000000; /* 20 mbps */
	else if (tp->link_config.speed == SPEED_100)
		shaper_rate = 200 * 1000000; /* 200 mbps */
	else
		shaper_rate = 2000 * 1000000; /* 2 gbps */

	printk("%s:: GE-%d shaper_rate %d Mbps\n",
		__func__, tp->port_id, (shaper_rate/1000000));
	cs752x_sch_set_port_rate_st(tp->port_id, shaper_rate);
}

void cs_ni_flow_control(mac_info_t *tp, u8 rx_tx, u8 flag)
{
	NI_TOP_NI_ETH_MAC_CONFIG0_0_t mac_status, mac_status_mask;
	u32 config_addr;

	ni_set_mac_tx_rx(tp->port_id, CS_DISABLE);

	mac_status.wrd = 0;
	mac_status_mask.wrd = 0;

	if (rx_tx == NI_RX_FLOW_CTRL) {
		mac_status.bf.rx_flow_disable = flag;
		mac_status_mask.bf.rx_flow_disable = 0x1;
	}
	if (rx_tx == NI_TX_FLOW_CTRL) {
		mac_status.bf.tx_flow_disable = flag;
		mac_status_mask.bf.tx_flow_disable = 0x1;
	}

	config_addr = NI_TOP_NI_ETH_MAC_CONFIG0_0 + (tp->port_id * 12);

	write_reg(mac_status.wrd, mac_status_mask.wrd, config_addr);

	ni_set_mac_tx_rx(tp->port_id, CS_ENABLE);

	return;
}

void cs_ni_set_mac_speed_duplex(mac_info_t *tp, int mac_interface)
{
	NI_TOP_NI_ETH_MAC_CONFIG0_0_t mac_status, mac_status_mask;
	NI_TOP_NI_ETH_INT_CONFIG1_t eth_cfg1, eth_cfg1_mask;
	u32 config_addr;
	int speed, duplex;

	ni_set_mac_tx_rx(tp->port_id, CS_DISABLE);

	mac_status.wrd = 0;
	mac_status_mask.wrd = 0;

	if (tp->link_config.speed == SPEED_10)
		speed = 1;
	else
		speed = 0;

	if (tp->link_config.duplex == DUPLEX_FULL)
		duplex = 0;
	else
		duplex = 1;

	mac_status.bf.speed = speed;
	mac_status.bf.duplex = duplex;
	mac_status_mask.bf.speed = 0x1;
	mac_status_mask.bf.duplex = 0x1;

	config_addr = NI_TOP_NI_ETH_MAC_CONFIG0_0 + (tp->port_id * 12);

	write_reg(mac_status.wrd, mac_status_mask.wrd, config_addr);

	printk(KERN_INFO "%s: link up, speed %u Mb/s, %s duplex\n",
	      tp->dev->name, tp->link_config.speed,
	       tp->link_config.duplex ? "full" : "half");

	eth_cfg1.wrd = 0;
	eth_cfg1_mask.wrd = 0;
	if (tp->port_id == GE_PORT0) {
		eth_cfg1.bf.int_cfg_ge0 = mac_interface;
		eth_cfg1_mask.bf.int_cfg_ge0 = 0x7;
	}
	if (tp->port_id == GE_PORT1) {
		eth_cfg1.bf.int_cfg_ge1 = mac_interface;
		eth_cfg1_mask.bf.int_cfg_ge1 = 0x7;
	}
	if (tp->port_id == GE_PORT2) {
		eth_cfg1.bf.int_cfg_ge2 = mac_interface;
		eth_cfg1_mask.bf.int_cfg_ge2 = 0x7;
	}
	write_reg(eth_cfg1.wrd, eth_cfg1_mask.wrd, NI_TOP_NI_ETH_INT_CONFIG1);

	ni_set_mac_tx_rx(tp->port_id, CS_ENABLE);
}


void cs_ni_set_eth_cfg(mac_info_t * tp, int config)
{
	NI_TOP_NI_ETH_INT_CONFIG1_t eth_config1, eth_config1_mask;
	NI_TOP_NI_ETH_INT_CONFIG2_t eth_config2, eth_config2_mask;

	eth_config1.wrd = 0;
	eth_config1_mask.wrd = 0;
	eth_config2.wrd = 0;
	eth_config2_mask.wrd = 0;
	switch (tp->port_id) {
	case GE_PORT0:
		if (config == NI_CONFIG_1) {
			eth_config1.bf.int_cfg_ge0 = tp->phy_mode;
			eth_config1.bf.phy_mode_ge0 = NORMAL_MODE;
			eth_config1.bf.tx_use_gefifo_ge0 = 1;
			eth_config1.bf.rmii_clksrc_ge0 = 0;
			eth_config1_mask.bf.int_cfg_ge0 = 0x7;
			eth_config1_mask.bf.phy_mode_ge0 = 0x1;
			eth_config1_mask.bf.tx_use_gefifo_ge0 = 0x1;
			eth_config1_mask.bf.rmii_clksrc_ge0 = 0x1;
		} else {
			eth_config2.bf.inv_clk_in_ge0 = 0;
			eth_config2.bf.inv_rxclk_out_ge0 = 0;
			eth_config2.bf.power_dwn_rx_ge0 = 0;
			eth_config2.bf.power_dwn_tx_ge0 = 0;
			eth_config2.bf.tx_intf_lp_time_ge0 = 0;
			eth_config2_mask.bf.inv_clk_in_ge0 = 0x1;
			eth_config2_mask.bf.inv_rxclk_out_ge0 = 0x1;
			eth_config2_mask.bf.power_dwn_rx_ge0 = 0x1;
			eth_config2_mask.bf.power_dwn_tx_ge0 = 0x1;
			eth_config2_mask.bf.tx_intf_lp_time_ge0 = 0x1;
		}
		break;
	case GE_PORT1:
		if (config == NI_CONFIG_1) {
			eth_config1.bf.int_cfg_ge1 = tp->phy_mode;
			eth_config1.bf.phy_mode_ge1 = NORMAL_MODE;
			eth_config1.bf.tx_use_gefifo_ge1 = 1;
			eth_config1.bf.rmii_clksrc_ge1 = 0;
			eth_config1_mask.bf.int_cfg_ge1 = 0x7;
			eth_config1_mask.bf.phy_mode_ge1 = 0x1;
			eth_config1_mask.bf.tx_use_gefifo_ge1 = 0x1;
			eth_config1_mask.bf.rmii_clksrc_ge1 = 0x1;
		} else {
			eth_config2.bf.inv_clk_in_ge1 = 0;
			eth_config2.bf.inv_rxclk_out_ge1 = 0;
			eth_config2.bf.power_dwn_rx_ge1 = 0;
			eth_config2.bf.power_dwn_tx_ge1 = 0;
			eth_config2.bf.tx_intf_lp_time_ge1 = 0;
			eth_config2_mask.bf.inv_clk_in_ge1 = 0x1;
			eth_config2_mask.bf.inv_rxclk_out_ge1 = 0x1;
			eth_config2_mask.bf.power_dwn_rx_ge1 = 0x1;
			eth_config2_mask.bf.power_dwn_tx_ge1 = 0x1;
			eth_config2_mask.bf.tx_intf_lp_time_ge1 = 0x1;
		}
		break;
	case GE_PORT2:
		if (config == NI_CONFIG_1) {
			eth_config1.bf.int_cfg_ge2 = tp->phy_mode;
			eth_config1.bf.phy_mode_ge2 = NORMAL_MODE;
			eth_config1.bf.tx_use_gefifo_ge2 = 1;
			eth_config1.bf.rmii_clksrc_ge2 = 0;
			eth_config1_mask.bf.int_cfg_ge2 = 0x7;
			eth_config1_mask.bf.phy_mode_ge2 = 0x1;
			eth_config1_mask.bf.tx_use_gefifo_ge2 = 0x1;
			eth_config1_mask.bf.rmii_clksrc_ge2 = 0x1;
		} else {
			eth_config2.bf.inv_clk_in_ge2 = 0;
			eth_config2.bf.inv_rxclk_out_ge2 = 0;
			eth_config2.bf.power_dwn_rx_ge2 = 0;
			eth_config2.bf.power_dwn_tx_ge2 = 0;
			eth_config2.bf.tx_intf_lp_time_ge2 = 0;
			eth_config2_mask.bf.inv_clk_in_ge2 = 0x1;
			eth_config2_mask.bf.inv_rxclk_out_ge2 = 0x1;
			eth_config2_mask.bf.power_dwn_rx_ge2 = 0x1;
			eth_config2_mask.bf.power_dwn_tx_ge2 = 0x1;
			eth_config2_mask.bf.tx_intf_lp_time_ge2 = 0x1;
		}
		break;
	default:
		printk("%s:Unacceptable port id %d\n", __func__, tp->port_id);
		return;
	}
	if (config == NI_CONFIG_1)
		write_reg(eth_config1.wrd, eth_config1_mask.wrd,
		  	NI_TOP_NI_ETH_INT_CONFIG1);
	if (config == NI_CONFIG_2)
		write_reg(eth_config2.wrd, eth_config2_mask.wrd,
		  	NI_TOP_NI_ETH_INT_CONFIG2);

	return;
}

static void cs_ni_enable_xram_intr(int xm_sm, xram_direction_t xram_direction)
{
	ni_info_t *ni;
	int i;

	printk("%s!\n", __func__);
	ni = &ni_private_data;
	switch (xram_direction) {
	case XRAM_DIRECTION_TX:
		/* Not supported yet */
		break;
	case XRAM_DIRECTION_RX:
		ni->intr_cpuxram_rxpkt_mask |= (1 << xm_sm);
		printk("\t%s::mask %x\n", __func__,
		       ni->intr_cpuxram_rxpkt_mask);
#ifdef CONFIG_INTR_COALESCING
		for (i = 0; i <= XRAM_INST_MAX; i++)
			writel(ni->intr_cpuxram_rxpkt_mask,
				NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + (i * 8));
#else
		writel(ni->intr_cpuxram_rxpkt_mask,
		       NI_TOP_NI_CPUXRAM_RXPKT_INTENABLE_0);
#endif
		break;
	default:
		break;
	}
}

static void cs_ni_init_port(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);
	NI_TOP_NI_RX_CNTRL_CONFIG0_0_t rx_cntrl_config0, rx_cntrl_config0_mask;
	//int i;
#ifdef CONFIG_CORTINA_FPGA
	TM_TC_PAUSE_FRAME_PORT_t tm_pause_reg;
#endif

	printk("\t***** %s is INVOKED! ***** init %d\n", __func__,
	       ne_initialized);
	if (ne_initialized != 0)
		return;

	tp->dev = dev;
	tp->rx_xram_intr = 0;

	/* The max number of buffers allocated to the port. */
	rx_cntrl_config0.wrd = 0;
	rx_cntrl_config0_mask.wrd = 0;
	//rx_cntrl_config0.bf.buff_use_thrshld = CPU_XRAM_BUFFER_NUM * (3 / 4);
#ifdef CS_UU_TEST
	rx_cntrl_config0.bf.runt_drop_dis = 1;	//FIXME: CH
	rx_cntrl_config0_mask.bf.runt_drop_dis = 1;	//FIXME: CH
#endif
	rx_cntrl_config0_mask.bf.buff_use_thrshld = 0x3FF;
	switch (tp->port_id) {
	case GE_PORT0:
		write_reg(rx_cntrl_config0.wrd, rx_cntrl_config0_mask.wrd,
			  NI_TOP_NI_RX_CNTRL_CONFIG0_0);
		break;
	case GE_PORT1:
		write_reg(rx_cntrl_config0.wrd, rx_cntrl_config0_mask.wrd,
			  NI_TOP_NI_RX_CNTRL_CONFIG0_0 + 8);
		break;
	case GE_PORT2:
		write_reg(rx_cntrl_config0.wrd, rx_cntrl_config0_mask.wrd,
			  NI_TOP_NI_RX_CNTRL_CONFIG0_0 + 16);
		break;
	default:
		break;
	}

#ifdef CONFIG_CORTINA_FPGA
	/* Disable Pause frame */
	tm_pause_reg.wrd = readl(TM_TC_PAUSE_FRAME_PORT + tp->port_id * 4);
	tm_pause_reg.bf.bm_dying_gasp_enable = 0;
	writel(tm_pause_reg.wrd, TM_TC_PAUSE_FRAME_PORT + tp->port_id * 4);
#endif
}

/*
 * Handles RX interrupts.
 */
static irqreturn_t cs_ni_rx_interrupt(int irq, void *dev_instance)
{
	u32 status;
	int i, re_schedule = 0;
	unsigned long flags;

	spin_lock_irqsave(&ni_private_data.rx_lock, flags);
#ifndef CS752X_NI_NAPI
	for (i = 0; i <= XRAM_INST_MAX; i++) {
		status = readl(NI_TOP_NI_CPUXRAM_RXPKT_0_INTERRUPT_0 + i * 8);
		if (status != 0) {
			writel(0,
			       NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + i * 8);
			writel(status,
			       NI_TOP_NI_CPUXRAM_RXPKT_0_INTERRUPT_0 + i * 8);
			ni_complete_rx_instance(ni_private_data.dev[0], i, 16,
						false);
			writel(1,
			       NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + i * 8);
		}
	}
#else
	/* FIXME: don't need read 9 instanst */
	for (i = 0; i <= XRAM_INST_MAX; i++) {
		status = readl(NI_TOP_NI_CPUXRAM_RXPKT_0_INTERRUPT_0 + i * 8);
		if (status != 0) {
			writel(0,
			       NI_TOP_NI_CPUXRAM_RXPKT_0_INTENABLE_0 + i * 8);
			writel(status,
			       NI_TOP_NI_CPUXRAM_RXPKT_0_INTERRUPT_0 + i * 8);
			ni_private_data.napi_rx_status[i] = status;
			re_schedule = 1;
		}
	}
	if (re_schedule == 1)
		napi_schedule(&ni_private_data.napi);
#endif
	spin_unlock_irqrestore(&ni_private_data.rx_lock, flags);
	return IRQ_HANDLED;
}

int cs_ni_open(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);
	int retval = 0;
	unsigned int val;
	struct net_device *tmp_dev;
	int jj;
#ifdef CS752X_MULTIQUEUE_SUPPORT
	int i;
#endif

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	mac_info_t *virt_tp = netdev_priv(dev);
	if (virt_tp->dev != dev)	/* virtual device now */
		return cs_ni_virt_ni_open(virt_tp->port_id, dev);
#endif

	if (tp->phydev) {
		/* enable PHY */
		if (tp->ni_driver_state == 0) {
			tp->ni_driver_state = 1;
			val = cs_mdiobus_read(NULL, tp->phy_addr, 0);
			val = val & ~(1 << 11); /* disable power down */
			val |= (1<<9); /* restart re-autonegotiation */
			cs_mdiobus_write(NULL, tp->phy_addr, 0, val);
		}
		phy_start(tp->phydev);
	} else {
#ifdef CONFIG_CORTINA_REFERENCE
		if (tp->port_id == GE_PORT1) {
			/* we have switch connect to GMAC#1, which does not
			 * work with Linux Kernel Phy framework. We should
			 * just ignore this. */
			// FIXME!! any other stuff that we need to take care?
		} else {
			/* Only 2 GMACs (GMAC#0 connects to GPHY, and GMAC1
			 * connects to switch) are used on reference board */
			printk("%s::REF board, GMAC#%d does not have working "
					"phy\n", __func__, tp->port_id);
			netif_err(tp, drv, dev, "Open dev failure\n");
			return -EINVAL;
		}
#endif
#ifdef CONFIG_CORTINA_ENGINEERING
		if (tp->port_id == GE_PORT1) {
			/* we have switch connect to GMAC#1, which does not
			 * work with Linux Kernel Phy framework. We should
			 * just ignore this. */
			// FIXME!! any other stuff that we need to take care?
		} else {
			printk("%s::Eng board, GMAC#%d does not have working "
					"phy\n", __func__, tp->port_id);
			netif_err(tp, drv, dev, "Open dev failure\n");
			return -EINVAL;
		}
#endif
	}

	netif_carrier_off(dev);

	spin_lock(&active_dev_lock);
	active_dev++;
	spin_unlock(&active_dev_lock);

	/* Enable XRAM Rx interrupts at this point */
	cs_ni_enable_xram_intr(tp->port_id, XRAM_DIRECTION_RX);

	/* this cs_ni_init_port is not called at all? */
	cs_ni_init_port(dev);
	cs_ni_set_short_term_shaper(tp);

	if (ne_irq_register == 0) {
#ifdef CONFIG_CORTINA_FPGA
		retval += request_irq(dev->irq, ni_generic_interrupt,
				     IRQF_SHARED, dev->name,
				     (struct net_device *)&ni_private_data);
		/* temp hack, for irq coalescing and via irq_wol2 line */
		retval += request_irq(IRQ_WOL2, cs_ni_rx_interrupt,
				     IRQF_SHARED, dev->name,
				     (struct net_device *)&ni_private_data);
#else
#if 0	// FIXME!! Performance Tuning!
		retval += request_irq(IRQ_NET_ENG, ni_generic_interrupt,
				     IRQF_SHARED, dev->name,
				     (struct net_device *)&ni_private_data);
#endif
		for (jj = 0; jj < GE_PORT_NUM; jj++) {
			tmp_dev = ni_private_data.dev[jj];
			retval += request_irq(tmp_dev->irq, cs_ni_rx_interrupt,
					IRQF_SHARED, tmp_dev->name, tmp_dev);
		}
#endif
		if (retval != 0) {
//                      napi_disable(&tp->napi);
			printk("%s::Error !", __func__);
			return retval;
		}
#ifdef CS752X_NI_NAPI
		napi_enable(&ni_private_data.napi);
#endif

		printk("\t*** %s\n", __func__);
		smp_mb();
		ne_irq_register++;
	}
	tp->status = 1;
	netif_tx_wake_all_queues(dev);

#if defined(CONFIG_CORTINA_REFERENCE) || defined(CONFIG_CORTINA_ENGINEERING)
	/* We turn on the carrier for switch port  after all the related SW
	 * entities have been set up properly */
	if (tp->port_id == GE_PORT1)
		netif_carrier_on(dev);
#endif

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	cs_ni_virt_ni_set_phy_port_active(tp->port_id, true);
#endif

#if defined(CONFIG_CORTINA_REFERENCE) || defined(CONFIG_CORTINA_ENGINEERING)
	if (tp->port_id == GE_PORT1 && tp->ni_driver_state == 0)
		tp->ni_driver_state = 1;
#endif

	return 0;
}

int cs_ni_close(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	if (cs_ni_virt_ni_close(dev) <= 0)
		return 0;
#endif

	/* update counters before going down */
	//ni_update_counters(dev);/* add later */
	netif_tx_disable(dev);

	if (tp->phydev)
		phy_stop(tp->phydev);
	spin_lock(&active_dev_lock);
	if (active_dev > 0)
		active_dev--;
	spin_unlock(&active_dev_lock);

	if (active_dev == 0) {
		//napi_disable(&tp->napi);

		if (ne_irq_register != 0) {
			ne_irq_register = 0;
#ifdef CS752X_NI_NAPI
			napi_disable(&ni_private_data.napi);
#endif
#ifdef CONFIG_CORTINA_FPGA
			free_irq(dev->irq,
				 (struct net_device *)&ni_private_data);
			free_irq(IRQ_WOL2,
				 (struct net_device *)&ni_private_data);
#else
#if 0		// FIXME!! Performance Tuning!!
			free_irq(IRQ_NET_ENG,
				 (struct net_device *)&ni_private_data);
#endif
		{
			struct net_device *tmp_dev;
			int jj;
			for (jj = 0; jj < GE_PORT_NUM; jj++) {
				tmp_dev = ni_private_data.dev[jj];
				free_irq(tmp_dev->irq, tmp_dev);
			}
		}
#endif
		}
	}

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	cs_ni_virt_ni_set_phy_port_active(tp->port_id, false);
#endif

	tp->status = 0;
	return 0;
}

/**
 *  cs_ni_get_stats - Get GoldenGate read/write statistics
 *  @dev: The Ethernet Device to get statistics for
 *
 *  Get TX/RX statistics for GoldenGate
 */
static struct net_device_stats *cs_ni_get_stats(struct net_device *dev)
{
	mac_info_t *tp = (mac_info_t *) netdev_priv(dev);
	//unsigned long flags;

#if 0
	if (netif_running(dev)) {
#if 0
		spin_lock_irqsave(&tp->lock, flags);
		/* add later */
		spin_unlock_irqrestore(&tp->lock, flags);
#endif
	}
#endif
	return &tp->ifStatics;
}

static inline void cs_dma_tx_complete(mac_info_t * tp, int tx_qid,
				      struct net_device *dev, int interrupt)
{
	ni_info_t *ni;
	dma_txdesc_t *curr_desc, *tmp_desc;
	dma_txdesc_0_t word0;
	u32 desc_count;
	dma_swtxq_t *swtxq;
	dma_rptr_t rptr_reg;
	dma_wptr_t wptr_reg;
	unsigned int new_idx, free_count = 0;

	ni = &ni_private_data;
	/* get tx H/W completed descriptor virtual address */
	/* check tx status and accumulate tx statistics */

	swtxq = &ni->swtxq[tx_qid];
	swtxq->intr_cnt++;
	rptr_reg.bits32 = readl(swtxq->rptr_reg);
	wptr_reg.bits32 = readl(swtxq->wptr_reg);
	//smp_mb();

	while (wptr_reg.bits.wptr != swtxq->finished_idx) {
		curr_desc = swtxq->desc_base + swtxq->finished_idx;
		word0.bits32 = curr_desc->word0.bits32;
		if (word0.bits.own == HARDWARE)
			goto QUIT_FREE_TXQ;

		desc_count = word0.bits.desc_cnt;
		/* a workaround as sometimes hw sets desc_count to 0! */
		if (desc_count > 1)
			new_idx = (swtxq->finished_idx + desc_count - 1) &
			    (swtxq->total_desc_num - 1);
		else
			new_idx = swtxq->finished_idx;

		tmp_desc = swtxq->desc_base + new_idx;

		if (swtxq->tx_skb[new_idx]) {
			free_count++;
			dev_kfree_skb_any(swtxq->tx_skb[new_idx]);
			swtxq->tx_skb[new_idx] = NULL;
			swtxq->curr_finished_desc = tmp_desc;
			swtxq->total_finished++;
			swtxq->finished_idx = RWPTR_ADVANCE_ONE(new_idx,
								swtxq->
								total_desc_num);
		}
	}

      QUIT_FREE_TXQ:
	if (free_count != 0) {
		//smp_mb();
#ifdef CS752X_MULTIQUEUE_SUPPORT
		if (__netif_subqueue_stopped(dev, tx_qid))
			netif_wake_subqueue(dev, tx_qid);
#else
		if (netif_queue_stopped(dev)) {
			printk("%s:: \n",__func__);
			netif_wake_queue(dev);
		}
#endif
	}
	return;
}

static u32 set_desc_word3_calc_l4_chksum_ipv4(struct sk_buff *skb,
		u16 seg_size)
{
	u32 word3 = 0;
	struct iphdr *iph = ip_hdr(skb);
	struct udphdr *uh = udp_hdr(skb);
	__wsum csum = 0;

	if (iph == NULL)
		return 0;

	//FIXME: for IPv4, need consider vlan, pppoe, SNAP etc case:
	//segment_size = ip_skb_dst_mtu(skb);
	//word3 = segment_size & 0x00001FFF;

	/* obtain the udphdr location if the given value is NULL */
	if ((iph->protocol == IPPROTO_UDP) &&
			((uh == NULL) || ((u32)iph == (u32)uh)))
		uh = (struct udphdr *)((u32)iph + (iph->ihl << 2));

	/* Was the following conditional statement */
	//if (skb->len > ip_skb_dst_mtu(skb) &&
	//		(iph->protocol == IPPROTO_UDP)) {
	if ((skb->len <= seg_size) && (iph->protocol == IPPROTO_UDP)) {
		/* for example, Transport Stream packet = 188, no checksum */
		/*
		 * There might be case skb_transport_header()
		 * might not be assigned earlier; therefore, we
		 * need to perform the check and get a new uhp.
		 */
		if (uh->check != 0) {
			word3 |= LSO_UDP_CHECKSUM_EN | LSO_IPV4_FRAGMENT_EN;
		}
	} else if ((skb->len > seg_size) && (iph->protocol == IPPROTO_UDP)) {
		/* the above statement was as below: */
		//} else if (skb->len <= ip_skb_dst_mtu(skb) &&
		//              (iph->protocol == IPPROTO_UDP)) {
		word3 |= LSO_IPV4_FRAGMENT_EN;
		/*
		 * There might be case skb_transport_header()
		 * might not be assigned earlier; therefore, we
		 * need to perform the check and get a new uhp.
		 */
		//FIXME: SW do UDP checksum if length > MTU
		uh->check = csum_tcpudp_magic(uh->source, uh->dest, uh->len,
				IPPROTO_UDP, csum);
		if (uh->check == 0) {
			uh->check = CSUM_MANGLED_0;
			printk("Error: uh->check = 0x%X\n", uh->check);
		}
	} else if (iph->protocol == IPPROTO_TCP) {
		/* IPv4 TCP SEGMENT */
		word3 |= LSO_TCP_CHECKSUM_EN | LSO_IPV4_FRAGMENT_EN;
	} else if ((skb->len > seg_size) ||
			(iph->frag_off & htons(IP_MF | IP_OFFSET))) {
		word3 |= LSO_IPV4_FRAGMENT_EN; /* IPv4 IP FRAGMENT */
	}
	return word3;
} /* set_desc_word3_calc_l4_chksum_ipv4 */

static u32 set_desc_word3_calc_l4_chksum_ipv6(struct sk_buff *skb,
		struct net_device *dev, int total_len)
{
	u32 word3 = 0;
	struct ipv6hdr *ipv6h = ipv6_hdr(skb);
	struct tcphdr *th = tcp_hdr(skb);
	int found_rhdr = 0, ip6_frag = 0;
	struct udphdr *uh = udp_hdr(skb);
	__wsum csum = 0;

#if 0
	/* IPv6 FRAGMENT */
	if ((skb->len > ip6_skb_dst_mtu(skb) && !skb_is_gso(skb)) ||
			dst_allfrag(skb_dst(skb)))
		word3 |= LSO_IPV6_FREGMENT_EN | LSO_SEGMENT_EN;
#endif
	//if (skb->ip_summed != CHECKSUM_PARTIAL)
	//	return word3;

	do {
		if (ipv6h->nexthdr == NEXTHDR_TCP) {
			if ((found_rhdr != 0)) {
				/*
				 * FIXME:SW do tcp checksum, need verify packet
				 * length.
				 */
				//tcp_hdr(skb)->check =
				//	tcp_checksum_complete(skb);
				th->check = csum_tcpudp_magic(th->source,
						th->dest, skb->len - 38,
						IPPROTO_TCP, csum);
			} else {
				word3 |= LSO_TCP_CHECKSUM_EN |
					 LSO_IPV6_FREGMENT_EN;
			}
		} else if (ipv6h->nexthdr == NEXTHDR_UDP) {
			if ((found_rhdr != 0) || (total_len > dev->mtu)) {
				uh->check = csum_tcpudp_magic(uh->source,
					uh->dest, uh->len,
					IPPROTO_UDP, csum);
			} else {
				if (uh->check != 0)
					word3 |= LSO_UDP_CHECKSUM_EN |
				 		LSO_IPV6_FREGMENT_EN;
			}
#if 0
		} else if (ipv6h->nexthdr == NEXTHDR_ROUTING) {
		      found_rhdr = 1;
		} else if (ipv6h->nexthdr == NEXTHDR_FRAGMENT) {
		      ip6_frag = 1;
#endif
		} else {
			word3 |= LSO_IPV6_FREGMENT_EN;
		}
		if (ipv6h->nexthdr == NEXTHDR_ROUTING) {
			found_rhdr = 1;
			word3 &= (!LSO_IPV6_FREGMENT_EN | !LSO_TCP_CHECKSUM_EN |
					!LSO_UDP_CHECKSUM_EN);
		}
		if (ipv6h->nexthdr == NEXTHDR_FRAGMENT) {
			ip6_frag = 1;
			word3 &= (!LSO_IPV6_FREGMENT_EN | !LSO_TCP_CHECKSUM_EN |
					!LSO_UDP_CHECKSUM_EN);
		}
		/*
		 * FIXME! Wen! there isn't any header moving to the next
		 * IPv6 header?
		 */
	} while (ipv6h->nexthdr == NEXTHDR_NONE); /* do */
	return word3;
} /* set_desc_word3_calc_l4_chksum_ipv6 */

static u32 set_desc_word3_calc_l4_chksum(struct sk_buff *skb,
		struct net_device *dev, int tot_len)
{
	u32 word3;
	u32 network_loc, mac_loc;
	u16 seg_size;
	struct iphdr *iph = ip_hdr(skb);

	if ((skb == NULL) || (dev == NULL))
		return 0;

	skb_reset_mac_header(skb);
	network_loc = (u32)skb_network_header(skb);
	mac_loc = (u32)skb_mac_header(skb);

	if ((network_loc != 0) && ((network_loc - mac_loc) > 14))
		seg_size = dev->mtu + network_loc - mac_loc;
	else
		seg_size = dev->mtu + 14;

#ifdef CONFIG_CS752X_VIRTUAL_NI_CPUTAG
	// FIXME! need to see if this is really a CPU tagged packet
	seg_size += RTL_CPUTAG_LEN;
#endif

	word3 = LSO_SEGMENT_EN | seg_size;
	/* we don't want DMA LSO to modify packet len */

	if (skb->ip_summed != CHECKSUM_PARTIAL)
		return word3;

	if (iph == NULL)
		return word3;

	if (iph->version == 4)	/* IPv4 */
		word3 |= set_desc_word3_calc_l4_chksum_ipv4(skb, seg_size);
	else if (iph->version == 6) /* IPv6 */
		word3 |= set_desc_word3_calc_l4_chksum_ipv6(skb, dev, tot_len);

	return word3;
} /* set_desc_word3_calc_l4_chksum */

#define HDRA_CPU_PKT	0xc30001
static int get_dma_lso_txqid(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);
	int lso_tx_qid = 0;
#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	mac_info_t *phy_tp;
	cs_virt_ni_t *virt_ni_ptr;
#endif

	lso_tx_qid = tp->port_id;

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	/* This can be modified later for different port setting;
	 * however, since right now, VIRTUAL NETWORK Interface only works
	 * on Reference board where only DMA LSO TXQ#0 is being used by eth0,
	 * we can use the rest of TXQ#1~5 */
	virt_ni_ptr = cs_ni_get_virt_ni(tp->port_id, dev);
	if (virt_ni_ptr != NULL) {
		/* the current method to spread the load is per "switch port"
		 * based. different interfaces might still use the same LSO
		 * TXQ, but we try our best to spread out */
		lso_tx_qid &= 0x03;
		phy_tp = (mac_info_t *)netdev_priv(tp->dev);
		if (phy_tp != NULL)
			lso_tx_qid += phy_tp->port_id;
	}
#endif

	return lso_tx_qid;
} /* get_dma_lso_txqid */

static int cs_ni_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	ni_info_t *ni;
	struct net_device *xmit_dev = dev;
	mac_info_t *tp = netdev_priv(xmit_dev);
	dma_rptr_t rptr_reg;
	dma_wptr_t wptr_reg;
	dma_txdesc_t *curr_desc;
	int snd_pages = skb_shinfo(skb)->nr_frags + 1;
	int frag_id = 0, len, total_len, tx_qid = 0, lso_tx_qid;
	struct net_device_stats *isPtr;
	u32 free_desc, wptr, rptr;
	dma_addr_t word1;
	u32 word0, word2, word3, word4, word5;
	dma_swtxq_t *swtxq;
	ni_header_a_t ni_header_a;
	u16 tx_queue = 0;
	struct iphdr *iph;
	char *pkt_datap;
	//unsigned long flags;
	struct netdev_queue *txq;
#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	int err_virt_ni = 0;
#endif
#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
	CS_KERNEL_ACCEL_CB_T *cs_cb;
#endif
#endif

	ni = &ni_private_data;

	if (skb == NULL) {
		printk("%s:: skb == NULL\n", __func__);
		return 0;
	}

#ifdef CS752X_MULTIQUEUE_SUPPORT
	tx_qid = skb_get_queue_mapping(skb);
#endif /* CS752X_MULTIQUEUE_SUPPORT */
	txq = netdev_get_tx_queue(dev, tx_qid);

	lso_tx_qid = get_dma_lso_txqid(dev);

	if (lso_tx_qid >= NI_DMA_LSO_TXQ_NUM)
		return 0; /* FIXME! a better return value? or better handle? */

	/* DMA_DMA_LSO_DMA_LSO_INTERRUPT_0, interrupt first level
	 * We are using the same tx_qid as for DMA LSO queue */
	swtxq = &ni->swtxq[lso_tx_qid];

	//if (unlikely(spin_trylock_irqsave(&swtxq->lock, flags) == 0))
	//	return NETDEV_TX_BUSY;

	isPtr = (struct net_device_stats *)&tp->ifStatics;

	ni_header_a.bits32 = HDRA_CPU_PKT;
//	memset(&ni_header_a, 0, sizeof(ni_header_a_t));

	/* The following should be called with interrupt;
	 * do that when ASIC is back. */
	cs_dma_tx_complete(tp, lso_tx_qid, dev, 0);

	wptr_reg.bits32 = readl(swtxq->wptr_reg);
	//rptr_reg.bits32 = readl(swtxq->rptr_reg);
	//smp_mb();
	wptr = wptr_reg.bits.wptr;
	//rptr = rptr_reg.bits.rptr;

	if (wptr >= swtxq->finished_idx)
		free_desc =
		    swtxq->total_desc_num - wptr - 1 + swtxq->finished_idx;
	else
		free_desc = swtxq->finished_idx - wptr - 1;

	/* try to reserve 1 descriptor in case skb is extended in xmit
	 * function */
	if (free_desc <= snd_pages) {
#if 0
		int iii;
		volatile dma_txdesc_0_t word0_tmp;
		//isPtr->tx_dropped++;
		printk
		    ("No available descriptor! free desc = %d, snd_pages = %d\n",
		     free_desc, snd_pages);
		printk("wptr = %d, rptr = %d, finished_idx = %d\n", wptr, rptr,
		       swtxq->finished_idx);
		printk("ownership of txq#%d =", lso_tx_qid);
		for (iii = 0; iii < swtxq->total_desc_num; iii++) {
			if ((iii % 4) == 0)
				printk(" ");
			if ((iii % 16) == 0)
				printk("\n");
			curr_desc = swtxq->desc_base + iii;
			dma_map_single(NULL, (void *)curr_desc,
				       sizeof(dma_txdesc_t), DMA_TO_DEVICE);
			word0_tmp.bits32 = curr_desc->word0.bits32;
			printk("%1d", word0_tmp.bits.own);
			//if (word0_tmp.bits.own == HARDWARE) printk("0");
			//else printk("1");
		}
		printk("\n");
#endif
		//spin_unlock_irqrestore(&swtxq->lock, flags);
		if ((txq != NULL) && (!netif_tx_queue_stopped(txq))) {
			netif_tx_stop_queue(txq);
			/* This is a hard error, log it. */
			printk("BUG! Tx Ring full when queue awake!\n");
		}
		printk("%s:: NETDEV_TX_BUSY\n",__func__);
		return NETDEV_TX_BUSY;
	}

#if 0
#ifdef CONFIG_CS752X_HW_ACCELERATION
	/* to disable hw accelration. Do
	 * "echo 0 > /proc/driver/cs752x/ne/hw_accel_debug" */
	/* to enable hw accelration.  Do
	 * "echo 1 > /proc/driver/cs752x/ne/hw_accel_debug" */
	if ((ni->neEnabled != 0) && (cs_hw_accel_debug != 0)) {
		cs_cb = (CS_KERNEL_ACCEL_CB_T *) CS_KERNEL_SKB_CB(skb);

		cs_cb->common.pspid = tp->port_id;
	}
#endif /* CONFIG_CS752X_HW_ACCELERATION */
#endif

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
	err_virt_ni = cs_ni_virt_ni_process_tx_skb(skb, dev, txq);
	if (err_virt_ni < 0) {
		dev_kfree_skb(skb);
		//spin_unlock_irqrestore(&swtxq->lock, flags);
		return 0;
	}
	dev = skb->dev;
	tp = netdev_priv(dev);
	txq = netdev_get_tx_queue(dev, tx_qid);
	/* in case we introduce any new send_page */
	snd_pages = skb_shinfo(skb)->nr_frags + 1;
#endif
	total_len = skb->len;
	/* BUG#29162.Workaround. if packet length < 24, DMA LSO can not
	 * send packet out. */
	if (total_len < MIN_DMA_SIZE) {
		dev_kfree_skb(skb);
		//spin_unlock_irqrestore(&swtxq->lock, flags);
		return 0;
	}

	/* get the destination VOQ */
	switch (tp->port_id) {
	case GE_PORT0:
		tx_queue = GE_PORT0_VOQ_BASE;
		break;
	case GE_PORT1:
		tx_queue = GE_PORT1_VOQ_BASE;
		break;
	case GE_PORT2:
		tx_queue = GE_PORT2_VOQ_BASE;
		break;
	default:
		printk("%s:%d:Unacceptable port_id %d\n", __func__, __LINE__,
		       tp->port_id);
		break;
	}

#ifdef CS752X_MULTIQUEUE_SUPPORT
	tx_queue += tx_qid;
#endif

	while (snd_pages != 0) {
		curr_desc = swtxq->desc_base + wptr;
#if 0
		if (swtxq->tx_skb[wptr]) {
			printk("Error! Stop due to TX descriptor's buffer "
			       "is not freed!\n");
			while (1) ;	/* For dedug purpose */
			dev_kfree_skb(swtxq->tx_skb[wptr]);
			swtxq->tx_skb[wptr] = NULL;
		}
#endif

		if (frag_id == 0) {
			pkt_datap = skb->data;
			len = total_len - skb->data_len;
		} else {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[frag_id - 1];
			pkt_datap =
			    page_address(frag->page) + frag->page_offset;
			len = frag->size;
			if (len > total_len)
				printk("Fatal Error! Send Frag size %d > "
				       "Total Size %d!!\n", len, total_len);
		}

		//if (len < 64) len = 64;
		word0 = len;
		word1 = dma_map_single(NULL, (void *)pkt_datap, len,
					DMA_TO_DEVICE);
		word2 = 0;
		word3 = set_desc_word3_calc_l4_chksum(skb, dev, total_len);

		if (snd_pages == 1) {
			word0 |= EOF_BIT; /* EOF */
			if (total_len < 64)
				word3 |= LSO_IP_LENFIX_EN;
			swtxq->tx_skb[wptr] = skb;
		} else {
			swtxq->tx_skb[wptr] = NULL;
			/* FIXME: if packet length > 1514, there are fragment or
			   or segment, we need clean this bit */
			word3 &= ~LSO_IP_LENFIX_EN;
		}

		if (frag_id == 0) {
			word0 |= SOF_BIT; /* SOF */
			word2 = (total_len << 16) & 0xFFFF0000;
			/* Enable LSO Debug:
			 * "echo 4 > /proc/driver/cs752x/ne/ni/ni_debug" */
			/* Disable LSO Debug:
			 * "echo 0 > /proc/driver/cs752x/ne/ni/ni_debug" */
#ifdef CONFIG_CS752X_PROC
			if ((total_len > (dev->mtu + 14)) &&
				(cs_ni_debug == DBG_NI_LSO))
				printk
				    ("DMA LSO enable: MTU = %d, Packet Length %d\n",
				     (dev->mtu + 14), total_len);
#endif
		}

		tp->ifStatics.tx_packets++;
		tp->ifStatics.tx_bytes += len;

		//wmb();

		ni_header_a.bits.dvoq = tx_queue;
#if 0
		/* not using CS_FWD_NORMAL, because we assume packet coming out
		 * of CPU does not need to be processed by FE anymore. */
		ni_header_a.bits.fwd_type = CS_FWD_BYPASS;
		ni_header_a.bits.pspid = 3; /* CPU */
		ni_header_a.bits.cpu_ptp_flag = 0;
		ni_header_a.bits.mark = 0;
		ni_header_a.bits.bypass_cos = 0;
		ni_header_a.bits.dvoq = tx_queue;
		ni_header_a.bits.no_crc = 0;
		ni_header_a.bits.no_stuff = 0;
		//ni_header_a.bits.header_valid = 0;
		ni_header_a.bits.header_valid = 1;
#endif
		word4 = ni_header_a.bits32;
		word5 = 0;
		/* for debug purpose, temp add */
		//word0 |= ONE_BIT;
		curr_desc->word0.bits32 = word0;
		curr_desc->word1.bits32 = (u32)word1;
		curr_desc->word2.bits32 = word2;
		curr_desc->word3.bits32 = word3;
		curr_desc->word4.bits32 = word4;
		curr_desc->word5.bits32 = word5;

		/* ignore the "Own" bit ownership in TX descriptor */
		curr_desc->word0.bits.own = HARDWARE;

		swtxq->curr_tx_desc = (dma_txdesc_t *)curr_desc;

		free_desc--;
		wmb();

		//wptr_reg.bits32 = readl(swtxq->wptr_reg);

#ifdef CONFIG_CS752X_PROC
		if (cs_ni_debug == DBG_NI_DUMP_TX) {
			iph = ip_hdr(skb);
			printk("Word0:0x%08X, Word1:0x%08X, ", word0, word1);
			printk("Word2:0x%08X, Word3:0x%08X, ", word2, word3);
			printk("Word4:0x%08X, Word5:0x%08X, ", word4, word5);
			printk("iph->id = 0x%X\n", iph->id);
			printk("%s:: TX: DMA packet pkt_len %d, skb->data = 0x%p\n",
                                     __func__, len, skb->data);
			ni_dm_byte((u32)skb->data - 14, len + 14);
		}
#endif

		wptr = RWPTR_ADVANCE_ONE(wptr, swtxq->total_desc_num);
		frag_id++;
		snd_pages--;
	}
	//swtxq->total_sent++;
	writel(wptr, swtxq->wptr_reg);

#ifdef CONFIG_CS752X_PROC
	if (cs_ni_debug == DBG_NI_DUMP_TX) {
		rptr_reg.bits32 = readl(swtxq->rptr_reg);
		rptr = rptr_reg.bits.rptr;
		printk("%s::tx reg wptr 0x%08x, rptr 0x%08x\n",
		       __func__, wptr, rptr_reg.bits32);
	}
#endif
	//SET_WPTR(swtxq->wptr_reg, wptr);/* need check every swtxq->wptr_reg */
	/* according to include/linux/netdevice.h, trans_start is expensive for
	 * high speed device on SMP, please use netdev_queue->trans_start */
	//dev->trans_start = jiffies;
	txq_trans_update(txq);

	//spin_unlock_irqrestore(&swtxq->lock, flags);
	return 0;
} /* cs_ni_start_xmit */

#ifdef CS752X_MANAGEMENT_MODE
static int cs_mfe_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);
	ni_info_t *ni;
	unsigned long flags;
	int tx_qid = 0;
	u16 tx_queue = 0;
	dma_swtxq_t *swtxq;

	ni = &ni_private_data;
	if (!skb) {
		printk("%s: Fatal Error SKB \n", __func__);
		return 0;
	}
#ifdef CS752X_MULTIQUEUE_SUPPORT
	tx_qid = skb_get_queue_mapping(skb);
#endif

	if (tp != NULL) {
		tp->ifStatics.tx_packets++;
		tp->ifStatics.tx_bytes += skb->len;
		skb->dev = tp->dev;
	}

	/* How to find out the destination queue  */
	swtxq = &ni->swtxq[tx_qid]; /* how to know which tx_qid ? */

	spin_lock_irqsave(&tp->lock, flags);

	if (tp->port_id == GE_PORT0)
		tx_queue = GE_PORT0_VOQ_BASE;
	else if (tp->port_id == GE_PORT1)
		tx_queue = GE_PORT1_VOQ_BASE;
	else if (tp->port_id == GE_PORT2)
		tx_queue = GE_PORT2_VOQ_BASE;

#ifdef CS752X_MULTIQUEUE_SUPPORT
	tx_queue += tx_qid;
#endif

	if (ni_cpu_tx_packet(skb, tx_queue) != 0) {
		spin_unlock_irqrestore(&tp->lock, flags);
		printk("%s: Error \n", __func__);
		return -EBUSY;
	}
	spin_unlock_irqrestore(&tp->lock, flags);
	dev->trans_start = jiffies;
	return 0;
}
#endif /* CS752X_MANAGEMENT_MODE */
#if 0
static void cs_ni_hw_reset(mac_info_t *tp)
{
	int i;

	/* Disable NI/WOL/LSO interrupts */
	// FIXME!! Implement

	/* tx/rx MAC disable */
	ni_set_mac_tx_rx(tp->port_id, CS_DISABLE);

	/* FIXME! Hold all macs in reset? how about just that specific device? */
	for (i = 0; i < GE_PORT_NUM; i++)
		writel(0xc0100800, NI_TOP_NI_ETH_MAC_CONFIG0_0 + (i * 12));
} /* cs_ni_hw_reset */
#endif
static void cs_ni_tx_timeout(struct net_device *dev)
{
	mac_info_t *tp;
	//mac_info_t *tp = netdev_priv(dev);
	int i;
	struct net_device *tmp_dev;

	printk("%s::\n", __func__);

	for (i=0; i<GE_PORT_NUM; i++) {
		tmp_dev = ni_private_data.dev[i];
		tp = netdev_priv(tmp_dev);
		if (tmp_dev == NULL)
			continue;
		netif_tx_stop_all_queues(tmp_dev);
#if 0
		if (tp->phydev) {
			printk("%s::%s phy stop!\n",
				__func__, tmp_dev->name);
			phy_stop(tp->phydev);
		} else {
			printk("%s::GE port1 switch reset?\n",
				__func__);
		}
#endif
	}

	cs_ni_disable_interrupts();
	//cs_ni_hw_reset(tp);
	/* FIXME!! not finish! what else needs to be done here? besides
	 * dumping out more precise error log? */

	printk("%s::schedule reset task\n", __func__);
	schedule_work(&ni_private_data.reset_task);
}

static int cs_ni_change_mtu(struct net_device *dev, int new_mtu)
{
	mac_info_t *tp = netdev_priv(dev);

	if ((new_mtu < MINIMUM_ETHERNET_FRAME_SIZE) ||
			(new_mtu > (MAX_CPU_PKT_LEN - (ENET_HEADER_SIZE +
						       ETHERNET_FCS_SIZE)))) {
		printk("%s::Invalid MTU setting, MTU= %d > Maximum mtu 9198\n",
			 __func__, new_mtu);
		return -EINVAL;
	}

	printk("%s changing MTU from %d to %d\n", dev->name, dev->mtu, new_mtu);

	dev->mtu = new_mtu;
	if (!netif_running(dev))
		goto out;

	if (tp->phydev != NULL) {
		netif_tx_disable(dev);
		phy_stop(tp->phydev);
		phy_start(tp->phydev);
		netif_carrier_off(dev);
		netif_tx_wake_all_queues(dev);
	}

out:
	return 0;
}

#define HDRA_CPU_PKT	0xc30001
static int cs_ni_set_mac_address(struct net_device *dev, void *p)
{
	mac_info_t *tp = netdev_priv(dev);
	fe_an_bng_mac_entry_t abm_entry;
	struct sockaddr *addr = p;
	NI_TOP_NI_ETH_MAC_CONFIG2_0_t mac_config2, mac_config2_mask;
	NI_TOP_NI_MAC_ADDR1_t mac_addr1, mac_addr1_mask;
	__u32 low_mac;
	int ret;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	set_mac_swap_order(abm_entry.mac, (unsigned char*)dev->dev_addr, dev->addr_len);
	//memcpy(abm_entry.mac, dev->dev_addr, dev->addr_len);
	abm_entry.sa_da = 0; /* 0: DA, 1: SA */
	abm_entry.pspid = tp->port_id;
	abm_entry.pspid_mask = 1;
	abm_entry.valid = 1;
	if (tp->an_bng_mac_idx == 0xffff)
		ret = cs_fe_table_add_entry(FE_TABLE_AN_BNG_MAC, &abm_entry,
					    &tp->an_bng_mac_idx);
	else
		ret =
		    cs_fe_table_set_entry(FE_TABLE_AN_BNG_MAC,
					  tp->an_bng_mac_idx, &abm_entry);
	if (ret != 0) {
		printk("%s:unable to set up MAC to FE for port#%d\n", __func__,
		       tp->port_id);
		// FIXME! any debug method?
	} else {
		/* Debug message.. can be removed later */
		printk("%s:setting up MAC address for Port#%d ", __func__,
		       tp->port_id);
		printk("using FE AN_BNG_MAC idx %d.\n", tp->an_bng_mac_idx);
	}

	printk
	    ("NI Setting MAC address for %s, %02x:%02x:%02x:%02x:%02x:%02x \n",
	     dev->name, dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
	     dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);

	/* used as SA while sending pause frames. Also used to detect WOL magic
	 * packets. */
	low_mac = dev->dev_addr[0] | (dev->dev_addr[1] << 8) |
	    (dev->dev_addr[2] << 16) | (dev->dev_addr[3] << 24);
	writel(low_mac, NI_TOP_NI_MAC_ADDR0);

	mac_addr1.wrd = 0;
	mac_addr1_mask.wrd = 0;
	mac_addr1.bf.mac_addr1 = dev->dev_addr[5];
	mac_addr1_mask.bf.mac_addr1 = 0xff;
	write_reg(mac_addr1.wrd, mac_addr1_mask.wrd, NI_TOP_NI_MAC_ADDR1);

	mac_config2.wrd = 0;
	mac_config2_mask.wrd = 0;
	mac_config2.bf.mac_addr6 = dev->dev_addr[6];
	mac_config2_mask.bf.mac_addr6 = dev->dev_addr[6];
	if (tp->port_id == GE_PORT0)
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0);
	if (tp->port_id == GE_PORT1)
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0 + 12);
	if (tp->port_id == GE_PORT2)
		write_reg(mac_config2.wrd, mac_config2_mask.wrd,
			  NI_TOP_NI_ETH_MAC_CONFIG2_0 + 24);

	return 0;
}

static int cs_ni_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	switch (cmd) {
	case SIOCDNEPRIVATE:
		return cs_ne_ioctl(dev, ifr, cmd);
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
	default:
		return -EOPNOTSUPP;
	}
}

void cs_ni_set_rx_mode(struct net_device *dev)
{
	u32 mc_filter[2];
	unsigned long flags;
	mac_info_t *tp = netdev_priv(dev);
	NI_TOP_NI_CPUXRAM_CFG_t rx_mode, rx_mode_mask;
	NI_TOP_NI_MISC_CONFIG_t misc_cfg, misc_cfg_mask;

	if (dev->flags & IFF_PROMISC) {
		/* Unconditionally log net taps. */
		netif_notice(tp, link, dev, "Promiscuous mode enabled\n");
		rx_mode.bf.xram_mgmt_promisc_mode = ACCEPTALLPACKET;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else if (dev->flags & IFF_ALLMULTI) {
		/* Too many to filter perfectly -- accept all multicasts. */
		rx_mode.bf.xram_mgmt_promisc_mode = ACCEPTMULTICAST;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else {
		struct netdev_hw_addr *ha;
		rx_mode.bf.xram_mgmt_promisc_mode =
		    ACCEPTBROADCAST;
		mc_filter[1] = mc_filter[0] = 0;
		netdev_for_each_mc_addr(ha, dev) {
			int bit_nr = ether_crc(ETH_ALEN, ha->addr) >> 26;
			mc_filter[bit_nr >> 5] |= 1 << (bit_nr & 31);
			rx_mode.bf.xram_mgmt_promisc_mode = ACCEPTMULTICAST;
		}
	}


	rx_mode.wrd = 0;
	rx_mode_mask.wrd = 0;
	misc_cfg.wrd = 0;
	misc_cfg_mask.wrd = 0;
	/* Valid only when GE port is directly connected to the XRAM
	 * (i.e. when NI_ETH_MGMT_PT_CONFIG.port_to_cpu[1:0] != 2'b11).
	 */
	rx_mode_mask.bf.xram_mgmt_promisc_mode = 0x3;
	/* Does not check the fwd_type to accept a packetfor multicast */
	misc_cfg.bf.mc_accept_all = 1;
	misc_cfg_mask.bf.mc_accept_all  = 0x1;

	spin_lock_irqsave(&tp->lock, flags);
	write_reg(rx_mode.wrd, rx_mode_mask.wrd, NI_TOP_NI_CPUXRAM_CFG);
	write_reg(misc_cfg.wrd, misc_cfg_mask.wrd, NI_TOP_NI_MISC_CONFIG);
	spin_unlock_irqrestore(&tp->lock, flags);
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling 'interrupt' - used by things like netconsole to send skbs
 * without having to re-enable interrupts. It's not called while
 * the interrupt routine is executing.
 */
static void cs_ni_poll_controller(struct net_device *dev)
{
	mac_info_t *tp = netdev_priv(dev);

	disable_irq(tp->irq);
	ni_generic_interrupt(tp->irq, dev);
#ifdef CONFIG_CORTINA_FPGA
	cs_ni_rx_interrupt(IRQ_WOL2, dev);
#else
	cs_ni_rx_interrupt(tp->irq, dev);
#endif
	enable_irq(tp->irq);
}
#endif	/* CONFIG_NET_POLL_CONTROLLER */

static const struct net_device_ops ni_netdev_ops = {
	.ndo_open = cs_ni_open,
	.ndo_stop = cs_ni_close,
	.ndo_get_stats = cs_ni_get_stats,
#ifdef CS752x_DMA_LSO_MODE
	.ndo_start_xmit = cs_ni_start_xmit,
#endif /* CS752x_DMA_LSO_MODE */
#ifdef CS752X_MANAGEMENT_MODE
	.ndo_start_xmit = cs_mfe_start_xmit,
#endif /* CS752X_MANAGEMENT_MODE */
	.ndo_tx_timeout = cs_ni_tx_timeout,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_change_mtu = cs_ni_change_mtu,
	.ndo_set_mac_address = cs_ni_set_mac_address,
	.ndo_do_ioctl = cs_ni_ioctl,
	.ndo_set_multicast_list = cs_ni_set_rx_mode,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller = cs_ni_poll_controller,
#endif
#ifdef CS752X_MULTIQUEUE_SUPPORT
	.ndo_select_queue = ni_select_queue,
#endif
};

static int cs_mdiobus_read(struct mii_bus *mii_bus, int phy_addr, int reg_addr)
{
	int ret;
	//unsigned long flags;

	spin_lock(&mdio_lock);
	ret = ni_mdio_read(phy_addr, reg_addr);
	spin_unlock(&mdio_lock);
	return ret;
}

static int cs_mdiobus_write(struct mii_bus *mii_bus, int phy_addr,
			    int reg_addr, u16 val)
{
	int ret = 0;
	//unsigned long flags;

	spin_lock(&mdio_lock);
	ret = ni_mdio_write(phy_addr, reg_addr, val);
	spin_unlock(&mdio_lock);
	return ret;
}

static int cs_mdio_reset(struct mii_bus *mii_bus)
{
	return 0;
}

static int cs_mdio_init(ni_info_t *ni, mac_info_t * tp)
{
	int i, err;

	if (tp->existed & CS_MDIOBUS_INITED)
		return 0;

	tp->mdio_bus = mdiobus_alloc();
	if (tp->mdio_bus == NULL) {
		err = -ENOMEM;
		goto err_out;
	}

	tp->mdio_bus->name = CS75XX_MDIOBUS_NAME;
	if (tp->port_id == GE_PORT0) {
		snprintf(tp->mdio_bus->id, MII_BUS_ID_SIZE, "%s", "0");
	}
	if (tp->port_id == GE_PORT1) {
		snprintf(tp->mdio_bus->id, MII_BUS_ID_SIZE, "%s", "1");
	}
	if (tp->port_id == GE_PORT2) {
		snprintf(tp->mdio_bus->id, MII_BUS_ID_SIZE, "%s", "2");
	}

	tp->mdio_bus->priv = tp;
	//tp->mdio_bus->parent = &tp->dev->dev;
	tp->mdio_bus->read = &cs_mdiobus_read;
	tp->mdio_bus->write = &cs_mdiobus_write;
	tp->mdio_bus->reset = &cs_mdio_reset;
	tp->mdio_bus->phy_mask = ~(1 << tp->phy_addr);
	tp->mdio_bus->irq = kmalloc(sizeof(int) * PHY_MAX_ADDR, GFP_KERNEL);

	if (!tp->mdio_bus->irq) {
		err = -ENOMEM;
		goto err_out_free_mdio_bus;
	}

	for (i = 0; i < PHY_MAX_ADDR; i++)
		tp->mdio_bus->irq[i] = PHY_POLL;

	/*
	 * The bus registration will look for all the PHYs on the mdio bus.
	 * Unfortunately, it does not ensure the PHY is powered up before
	 * accessing the PHY ID registers.
	 */
	err = mdiobus_register(tp->mdio_bus);
	if (err)
		goto err_out_free_mdio_irq;

	tp->existed |= CS_MDIOBUS_INITED;

	return 0;

err_out_free_mdio_irq:
	kfree(tp->mdio_bus->irq);
err_out_free_mdio_bus:
	mdiobus_free(tp->mdio_bus);
err_out:
	return err;
}

static unsigned long ascii2hex(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else
		return (0xffffffff);
}

static unsigned long string2hex(char *str_p)
{
	unsigned long i, result = 0;

	if (*str_p == '0' && toupper(*(str_p + 1)) == 'X')
		str_p += 2;

	while ((i = ascii2hex(*str_p)) != 0xffffffff) {
	      result = (result) * 16 + i;
	      str_p++;
	}

	while (*str_p == ' ' || *str_p == '.' || *str_p == ':')
		str_p++;

	return result;
}

static void ni_mac_parse(void)
{
	int i;
	char *ptr_0;

	ptr_0 = strstr(saved_command_line, "ethaddr0");
	if (ptr_0 == NULL) {
	        printk("%s: No GE MAC found in U-boot !!\n", __func__);
	        return;
	}
	ptr_0 += strlen("ethaddr0") + 1;
	for (i = 0; i < 6; i++) {
		while (*ptr_0 == ' ' || *ptr_0 == '.' || *ptr_0 == ':')
			ptr_0++;
		eth_mac[0][i] = (u8)string2hex(ptr_0);
		ptr_0 += 2;
	}

	printk("MAC0: %02x:%02x:", eth_mac[0][0], eth_mac[0][1]);
	printk("%02x:%02x:", eth_mac[0][2], eth_mac[0][3]);
	printk("%02x:%02x\n", eth_mac[0][4], eth_mac[0][5]);

#if defined(CONFIG_CORTINA_REFERENCE) || defined(CONFIG_CORTINA_ENGINEERING)
	memcpy(&eth_mac[1][0], &eth_mac[0][0], 6);
	eth_mac[1][3] += 0x10;
#else
	ptr_0 = strstr(saved_command_line, "ethaddr1");
	ptr_0 += strlen("ethaddr1") + 1;
	for (i = 0; i < 6; i++) {
		while (*ptr_0 == ' ' || *ptr_0 == '.' || *ptr_0 == ':')
			ptr_0++;

		eth_mac[1][i] = (u8)string2hex(ptr_0);
		ptr_0 += 2;
	}
#endif
	printk("MAC1: %02x:%02x:", eth_mac[1][0], eth_mac[1][1]);
	printk("%02x:%02x:", eth_mac[1][2], eth_mac[1][3]);
	printk("%02x:%02x\n", eth_mac[1][4], eth_mac[1][5]);

#if defined(CONFIG_CORTINA_REFERENCE) || defined(CONFIG_CORTINA_ENGINEERING)
	memcpy(&eth_mac[2][0], &eth_mac[0][0], 6);
	eth_mac[2][3] += 0x20;
#else
	ptr_0 = strstr(saved_command_line, "ethaddr2");
	ptr_0 += strlen("ethaddr2") + 1;
	for (i = 0; i < 6; i++) {
		while (*ptr_0 == ' ' || *ptr_0 == '.' || *ptr_0 == ':')
			ptr_0++;

		eth_mac[2][i] = (u8)string2hex(ptr_0);
		ptr_0 += 2;
	}
#endif
	printk("MAC2: %02x:%02x:", eth_mac[2][0], eth_mac[2][1]);
	printk("%02x:%02x:", eth_mac[2][2], eth_mac[2][3]);
	printk("%02x:%02x\n", eth_mac[2][4], eth_mac[2][5]);
}

static u64 ni_mib_access(u32 reg, u8 read_write, u8 op_code,
				     u8 port_id, u8 counter_id)
{
	NI_TOP_NI_RXMIB_ACCESS_t mib_access, mib_access_mask;
	u8 access_executed;
	u64 val_bottom, val_top;

	mib_access.wrd = 0;
	mib_access_mask.wrd = 0;
	mib_access.bf.rbw = read_write;
	mib_access.bf.op_code = op_code;
	mib_access.bf.port_id = port_id;
	mib_access.bf.counter_id = counter_id;
	mib_access.bf.access = 1;
	mib_access_mask.bf.rbw = 0x1;
	mib_access_mask.bf.op_code = 0x3;
	mib_access_mask.bf.port_id = 0x7;
	mib_access_mask.bf.counter_id = 0x1F;
	mib_access_mask.bf.access = 0x1;

	write_reg(mib_access.wrd, mib_access_mask.wrd, reg);

	do {
		access_executed = (readl(reg) & 0x80000000);
		udelay(10);
	} while (access_executed == 1);

	/* FIXME: need verify. which one is the top. */
	val_bottom = readl(reg + 8);
	val_top = readl(reg + 4);
	val_bottom |= val_top << 32;

	return val_bottom;
}

struct cs_ethtool_stats *cs_ni_update_stats(mac_info_t *tp)
{
	struct cs_ethtool_stats *stats = &tp->stats;
	unsigned long flags;
	u64 val = 0;

	spin_lock_irqsave(&tp->stats_lock, flags);

	/* RX mib */
	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXUCPKTCNT);
	tp->stats.rxucpktcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXMCFRMCNT);
	tp->stats.rxmcfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXBCFRMCNT);
	tp->stats.rxbcfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXOAMFRMCNT);
	tp->stats.rxoamfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXJUMBOFRMCNT);
	tp->stats.rxjumbofrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXPAUSEFRMCNT);
	tp->stats.rxpausefrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXUNKNOWNOCFRMCNT);
	tp->stats.rxunknownocfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXCRCERRFRMCNT);
	tp->stats.rxcrcerrfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXUNDERSIZEFRMCNT);
	tp->stats.rxundersizefrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXRUNTFRMCNT);
	tp->stats.rxruntfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXOVSIZEFRMCNT);
	tp->stats.rxovsizefrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXJABBERFRMCNT);
	tp->stats.rxjabberfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXINVALIDFRMCNT);
	tp->stats.rxinvalidfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM64OCT);
	tp->stats.rxstatsfrm64oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM65TO127OCT);
	tp->stats.rxstatsfrm65to127oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM128TO255OCT);
	tp->stats.rxstatsfrm128to255oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM256TO511OCT);
	tp->stats.rxstatsfrm256to511oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM512TO1023OCT);
	tp->stats.rxstatsfrm512to1023oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM1024TO1518OCT);
	tp->stats.rxstatsfrm1024to1518oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM1519TO2100OCT);
	tp->stats.rxstatsfrm1519to2100oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM2101TO9200OCT);
	tp->stats.rxstatsfrm2101to9200oct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXSTATSFRM9201TOMAXOCT);
	tp->stats.rxstatsfrm9201tomaxoct = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXBYTECOUNT_LO);
	tp->stats.rxbytecount_lo = val;

	val = ni_mib_access(NI_TOP_NI_RXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, RXBYTECOUNT_HI);
	tp->stats.rxbytecount_hi = val;

	/* TX mib */
	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXUCPKTCNT);
	tp->stats.txucpktcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXMCFRMCNT);
	tp->stats.txmcfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXBCFRMCNT);
	tp->stats.txbcfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXOAMFRMCNT);
	tp->stats.txoamfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXJUMBOFRMCNT);
	tp->stats.txjumbofrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXPAUSEFRMCNT);
	tp->stats.txpausefrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXCRCERRFRMCNT);
	tp->stats.txcrcerrfrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXOVSIZEFRMCNT);
	tp->stats.txovsizefrmcnt = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSINGLECOLFRM);
	tp->stats.txsinglecolfrm = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXMULTICOLFRM);
	tp->stats.txmulticolfrm = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXLATECOLFRM);
	tp->stats.txlatecolfrm = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXEXESSCOLFRM);
	tp->stats.txexesscolfrm = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM64OCT);
	tp->stats.txstatsfrm64oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM65TO127OCT);
	tp->stats.txstatsfrm65to127oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM128TO255OCT);
	tp->stats.txstatsfrm128to255oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM256TO511OCT);
	tp->stats.txstatsfrm256to511oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM512TO1023OCT);
	tp->stats.txstatsfrm512to1023oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM1024TO1518OCT);
	tp->stats.txstatsfrm1024to1518oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM1519TO2100OCT);
	tp->stats.txstatsfrm1519to2100oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM2101TO9200OCT);
	tp->stats.txstatsfrm2101to9200oct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXSTATSFRM9201TOMAXOCT);
	tp->stats.txstatsfrm9201tomaxoct = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXBYTECOUNT_LO);
	tp->stats.txbytecount_lo = val;

	val = ni_mib_access(NI_TOP_NI_TXMIB_ACCESS, CS_READ,
			PLAIN_READ, tp->port_id, TXBYTECOUNT_HI);
	tp->stats.txbytecount_hi = val;

	spin_unlock_irqrestore(&tp->stats_lock, flags);
	return stats;
}

static int __devinit cs_ni_init_module(void)
{
	int i, err = 0;
	mac_info_t *tp;
	struct net_device *dev;
	const struct port_cfg_info *cfg;
	ni_info_t *ni = &ni_private_data;
	u32 phy_vendor;
	FETOP_FE_PRSR_CFG_0_t fe_prsr_cfg0, fe_prsr_cfg0_mask;

	printk(KERN_INFO NI_DRIVER_NAME " built at %s %s\n", __DATE__,
	       __TIME__);

	ni_mac_parse();
	//cs_ne_system_reset();
	spin_lock_init(&mdio_lock);
	spin_lock_init(&sw_qm_cnt_lock);
	spin_lock_init(&active_dev_lock);

	cs_ne_init_cfg();

	cs_fe_init();

#ifdef CS752X_LINUX_MODE
	cs_qm_init();
	cs752x_sch_init();
#endif
	cs75xx_tm_init();
	cs_vtable_init();
	/* Hold all macs in Normal */
	for (i = 0; i < GE_PORT_NUM; i++)
		writel(0x00100800, NI_TOP_NI_ETH_MAC_CONFIG0_0 + (i * 12));

	for (i = 0; i < GE_PORT_NUM; i++) {
		cfg = &port_cfg_infos[i];
		tp = &ni_private_data.mac[i];
		//tp->dev = NULL;
		//if (tp->existed != CS_MAC_EXISTED_FLAG) continue;
#ifdef CS752X_MULTIQUEUE_SUPPORT
		dev = alloc_etherdev_mq(sizeof(*tp), NI_DMA_LSO_TXQ_NUM);
#else
		dev = alloc_etherdev(sizeof(*tp));
#endif
		if (dev == NULL) {
			printk(KERN_ERR
			       "Unable to alloc new ethernet device #%d .\n",
			       i);
			return -ENOMEM;
		}

		ni_private_data.dev[i] = dev;
		tp = netdev_priv(dev);
		tp->dev = dev;
		tp->link_config.advertising =
			(ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
		 	ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |
		 	ADVERTISED_1000baseT_Half | ADVERTISED_1000baseT_Full |
		 	ADVERTISED_Autoneg | ADVERTISED_MII);
		tp->link_config.link = 0; /* LINK_DOWN */
		tp->link_config.autoneg = cfg->auto_nego;
		tp->link_config.speed = cfg->speed;
		tp->link_config.duplex = cfg->full_duplex;
		tp->link_config.flowctrl = cfg->flowctrl;
		tp->phy_mode = cfg->phy_mode;
		tp->port_id = cfg->port_id;
		tp->phy_addr = cfg->phy_addr;
		tp->irq = cfg->irq;
		tp->mac_addr = (u32 *)cfg->mac_addr;
		//tp->rx_checksum = CS_ENABLE;
		tp->rx_checksum = CS_DISABLE;
		tp->an_bng_mac_idx = 0xffff;
		tp->msg_enable = netif_msg_init(debug, default_msg);
		tp->mdio_lock = &mdio_lock;

		dev->netdev_ops = &ni_netdev_ops;
		dev->irq = tp->irq;
		dev->watchdog_timeo = NI_TX_TIMEOUT;
		dev->features = NETIF_F_SG | NETIF_F_HW_CSUM |
				NETIF_F_TSO | NETIF_F_TSO6;
		//dev->features |= NETIF_F_UFO;
		//dev->base_addr = tp->base_addr;
		/* Get MAC address */
		memcpy(&dev->dev_addr[0], &eth_mac[tp->port_id][0],
		       dev->addr_len);
		memcpy(dev->perm_addr, dev->dev_addr, dev->addr_len);
		snprintf(dev->name, IFNAMSIZ, "eth%d", tp->port_id);
		spin_lock_init(&tp->stats_lock);
		spin_lock_init(&tp->lock);
		cs_ni_set_ethtool_ops(dev);


#if defined(CONFIG_CORTINA_REFERENCE) || defined(CONFIG_CORTINA_ENGINEERING)
		if (tp->phy_addr != GE_PORT1_PHY_ADDR) {
			/* ASIC will autodetect PHY status */
			err = cs_mdio_init(ni, tp);
			/* FIXME: need free net_device */
			if (err)
				return err;

			err = cs_phy_init(tp);
			/* FIXME: need free mdio, net_device */

			phy_vendor = cs_ni_get_phy_vendor(tp->phy_addr);
			printk("----> GE-%d: phy_id 0x%08x\n",
					tp->port_id, phy_vendor);
		} else {
			cs_ni_set_eth_cfg(tp, NI_CONFIG_1);
			cs_ni_set_mac_speed_duplex(tp, tp->phy_mode);
		}
#else	/* only work with FPGA now */
		/* ASIC will autodetect PHY status */
		err = cs_mdio_init(ni, tp);
		/* FIXME: need free net_device */
		if (err)
			return err;

		err = cs_phy_init(tp);
		/* FIXME: need free mdio, net_device */

		phy_vendor = cs_ni_get_phy_vendor(tp->phy_addr);
		printk("----> GE-%d: phy_id 0x%08x\n",
				tp->port_id, phy_vendor);
#endif

#ifndef CS752X_LINUX_MODE
		cs_mfe_set_mac_address(dev);
#endif

		err = register_netdev(dev);
		if (err) {
			printk(KERN_ERR
			       "%s: Cannot register net device, aborting.\n",
			       dev->name);
			if (tp->phydev)
				phy_disconnect(tp->phydev);
			free_netdev(dev);
			return err;
		}

#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
		err = cs_ni_virt_ni_create_if(i, dev, tp);
		if (err)
			return err;
#endif /* CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE */

		//spin_lock_init(&tp->link_lock);
	}
	INIT_WORK(&ni_private_data.reset_task, cs_ni_reset_task);

	if (tp->rx_checksum == CS_ENABLE) {
		/* l4_chksum_chk_enable */
		fe_prsr_cfg0.wrd = 0;
		fe_prsr_cfg0_mask.wrd = 0;
		fe_prsr_cfg0.bf.l4_chksum_chk_enable = 0;  /* 0: mean enable */
		fe_prsr_cfg0_mask.bf.l4_chksum_chk_enable = 1;
		write_reg(fe_prsr_cfg0.wrd, fe_prsr_cfg0_mask.wrd,
			  FETOP_FE_PRSR_CFG_0);
			  printk("%s::--> RX HW CHECKSUM Enable\n", __func__);
	}

//	for (i = 0; i < LINUX_FREE_BUF; i++) {
	for (i = 0; i < 2; i++) {
#ifdef CONFIG_DEBUG_KMEMLEAK
		INIT_LIST_HEAD(&ni_private_data.ni_skb_list_head[i]);
#endif
#ifdef CS752X_LINUX_MODE
		cs_ni_alloc_linux_free_buffer(ni_private_data.dev[0], i,
					      LINUX_FREE_BUF_LIST_SIZE);
#endif
	}

	cs_ni_init_interrupt_cfg();
#ifdef NI_WOL
	//if ((readl(NI_TOP_NI_ETH_MAC_CONFIG1_0) & WOL_PKT_DET_EN) == 1)
	//tp->features |= NI_FEATURE_WOL;
	//device_set_wakeup_enable(&dev->dev, tp->features & RTL_FEATURE_WOL);
#endif /* NI_WOL */
	//cs_ni_init_proc();
	spin_lock_init(&ni_private_data.rx_lock);
	spin_lock_init(&ni_private_data.tx_lock);
#ifdef CS752X_NI_NAPI
	netif_napi_add(ni_private_data.dev[0], &ni_private_data.napi,
		       cs_ni_poll, NI_NAPI_WEIGHT);
#endif

	cs_init_lpb_an_bng_mac();

	return 0;
}
module_init(cs_ni_init_module);

static void __exit cs_ni_cleanup_module(void)
{
	int i;
	ni_info_t *ni;
	mac_info_t *tp;

	ni = &ni_private_data;

	for (i = 0; i < GE_PORT_NUM; i++) {
#ifdef CONFIG_CS752X_VIRTUAL_NETWORK_INTERFACE
		cs_ni_virt_ni_remove_if(i);
#endif
		tp = &ni_private_data.mac[i];
		if (tp->existed & CS_MDIOBUS_INITED) {
			tp->existed &= ~CS_MDIOBUS_INITED;
			if (tp->phydev)
				phy_disconnect(tp->phydev);
			mdiobus_unregister(tp->mdio_bus);
			kfree(tp->mdio_bus->irq);
			mdiobus_free(tp->mdio_bus);
		}
		unregister_netdev(tp->dev);
	}

	//cs_ni_fini_proc();
	return;
}
module_exit(cs_ni_cleanup_module);

MODULE_LICENSE("GPL");
