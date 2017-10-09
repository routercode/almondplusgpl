/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/

/***********************************************************************
 Tree View of PROC                           Description             
 Path prefix : proc/drivers/cs752x/ne
 File                 Type/Length    Definition              
 ni/ni_debug          uint32         0: disable NI and IRQ
 				     1: enable NI
 				     2: enable IRQ
 				     3: enable NI and IRQ	              
 qm/qm_debug          uint32         0: disable, 1: enable               
 tm/tm_debug          uint32         0: disable, 1: enable               
 sch/sch_debug        uint32         0: disable, 1: enable               
 switch/switch_debug  uint32         0: disable, 1: enable               
 switch/register      string/uint32  Sample codes for multiple parameters               
 fe/fe_debug          uint32         bitwise flags   0: disable, 1: enable           
                                      Offset     Short Name      Full Name   
                                      0          acl             ACL table   
                                      1          an_bng_mac      AN BNG MAC table    
                                      2          checkmem        Memory checking table   
                                      3          class           Classifier table    
                                      4          etype           Ethernet type table 
                                      5          fwdrslt         Forwarding hash result table    
                                      6          hash            HASH    
                                      7          hw              Hardware table  
                                      8          llc_hdr         LLC header table    
                                      9          lpb             LPB table   
                                      10         lpm             LPM module  
                                      11         mc              Multicast group ID  
                                      12         pe_voq_drp      Packet editor VoQ drop table    
                                      13         pktlen_rngs     Packet length range table   
                                      14         port_rngs       Port range table    
                                      15         qosrslt         QoS hash result table   
                                      16         rslt_fvlan_tbl  Flow VLAN table 
                                      17         rslt_l2_tbl     L2 result table 
                                      18         rslt_l3_tbl     L3 result table 
                                      19         sdb             SDB table   
                                      20         vlan            VLAN table  
                                      21         voqpol          VoQ policer table   
                                      
 adaptation/adaptation_debug    uint32  bitwise flags   0: disable, 1: enable           
                                      Offset     Short Name      Full Name   
                                      0          8021q           802.1q VLAN hooks   
                                      1          bridge          Bridge hooks    
                                      2          common          Basic functions 
                                      3          core            Core hooks  
                                      4          ipsec           IPSec hooks 
                                      5          ipv6            IPv6 hooks  
                                      6          multicast       IGMP hooks  
                                      7          netfilter       Netfilter hooks 
                                      8          pppoe           PPPoE hooks 
                                      9          qos             QoS hooks   
                                     
 hw_accel_debug                 uint32  bitwise flags   0: disable, 1: enable          
                                     Offset     Short Name              Full Name   
                                     0          hw_accel                HW acceleration                      
                                     1          hw_accel_bridge         Bridge acceleration              
                                     2          hw_accel_nat            NAT acceleration                 
                                     3          hw_accel_vlan           VLAN acceleration                
                                     4          hw_accel_pppoe          PPPoE acceleration               
                                     5          hw_accel_ipsec          IPSec acceleration               
                                     6          hw_accel_multicast      Multicast acceleration       
                                     7          hw_accel_ipv6_routing   IPv6 Routing acceleration
                                     8          hw_accel_qos_ingress    OoS Ingress acceleration 
                                     9          hw_accel_qos_engress    OoS Engress acceleration 
                                     10         hw_accel_double_check   Double check enable      

 Path prefix : proc/drivers/cs752x
 File                       Type/Length    Definition              
 pcie/pcie_debug            uint32         0: disable, 1: enable               
 sata/sata_debug            uint32         0: disable, 1: enable               
 sd/sd_debug                uint32         0: disable, 1: enable               
 spdif/spdif_debug          uint32         0: disable, 1: enable               
 ssp/ssp_debug              uint32         0: disable, 1: enable               
 ts/ts_debug                uint32         0: disable, 1: enable               
 usb_host/usb_host_debug    uint32         0: disable, 1: enable               
 usb_dev/usb_dev_debug      uint32         0: disable, 1: enable               
 fb/fb_debug                uint32         0: disable, 1: enable               
 crypto/crypto_debug        uint32         0: disable, 1: enable               
 pwr_ctrl/pwr_ctrl_debug    uint32         0: disable, 1: enable               
 cir/cir_debug              uint32         0: disable, 1: enable               
 ***********************************************************************/
#ifdef CONFIG_CS752X_PROC

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */

#include "cs752x_proc.h"	/* local definitions */
#include "cs_fe.h"

/*
 * Global Debug Flags
 */
u32 cs_ni_debug = 0;
u32 cs_qm_debug = 0;
u32 cs_tm_debug = 0;
u32 cs_sch_debug = 0;
u32 cs_switch_debug = 0;
u32 cs_fe_debug = 0;
u32 cs_adapt_debug = 0;
u32 cs_hw_accel_debug = 0xFFFFFFFF;

u32 cs_pcie_debug = 0;
u32 cs_sata_debug = 0;
u32 cs_sd_debug = 0;
u32 cs_spdif_debug = 0;
u32 cs_ssp_debug = 0;
u32 cs_ts_debug = 0;
u32 cs_usb_host_debug = 0;
u32 cs_usb_dev_debug = 0;
u32 cs_fb_debug = 0;
u32 cs_crypto_debug = 0;
u32 cs_pwr_ctrl_debug = 0;
u32 cs_cir_debug = 0;

EXPORT_SYMBOL(cs_ni_debug);
EXPORT_SYMBOL(cs_qm_debug);
EXPORT_SYMBOL(cs_tm_debug);
EXPORT_SYMBOL(cs_sch_debug);
EXPORT_SYMBOL(cs_switch_debug);
EXPORT_SYMBOL(cs_fe_debug);
EXPORT_SYMBOL(cs_adapt_debug);
EXPORT_SYMBOL(cs_hw_accel_debug);

EXPORT_SYMBOL(cs_pcie_debug);
EXPORT_SYMBOL(cs_sata_debug);
EXPORT_SYMBOL(cs_sd_debug);
EXPORT_SYMBOL(cs_spdif_debug);
EXPORT_SYMBOL(cs_ssp_debug);
EXPORT_SYMBOL(cs_ts_debug);
EXPORT_SYMBOL(cs_usb_host_debug);
EXPORT_SYMBOL(cs_usb_dev_debug);
EXPORT_SYMBOL(cs_fb_debug);
EXPORT_SYMBOL(cs_crypto_debug);
EXPORT_SYMBOL(cs_pwr_ctrl_debug);
EXPORT_SYMBOL(cs_cir_debug);

/* directory name */
#define CS752X                  "driver/cs752x"
#define CS752X_NE               "ne"
#define CS752X_NE_NI            "ni"
#define CS752X_NE_QM            "qm"
#define CS752X_NE_TM            "tm"
#define CS752X_NE_SCH           "sch"
#define CS752X_NE_SWITCH        "switch"
#define CS752X_NE_FE            "fe"
#define CS752X_NE_ADAPT         "adaptation"

#define CS752X_PCIE             "pcie"
#define CS752X_SATA             "sata"
#define CS752X_SD               "sd"
#define CS752X_SPDIF            "spdif"
#define CS752X_SSP              "ssp"
#define CS752X_TS               "ts"
#define CS752X_USB_HOST         "usb_host"
#define CS752X_USB_DEV          "usb_dev"
#define CS752X_FB               "fb"
#define CS752X_CRYPTO           "crypto"
#define CS752X_PWR_CTRL         "pwr_ctrl"
#define CS752X_CIR              "cir"

/* file name */
#define NI_DEBUG                "ni_debug"
#define QM_DEBUG                "qm_debug"
#define TM_DEBUG                "tm_debug"
#define SCH_DEBUG               "sch_debug"
#define SWITCH_DEBUG            "switch_debug"
#define FE_DEBUG                "fe_debug"
#define ADAPT_DEBUG             "adaptation_debug"
#define HW_ACCEL_DEBUG          "hw_accel_debug"

#define SWITCH_REG              "register"

#define PCIE_DEBUG             "pcie_debug"
#define SATA_DEBUG             "sata_debug"
#define SD_DEBUG               "sd_debug"
#define SPDIF_DEBUG            "spdif_debug"
#define SSP_DEBUG              "ssp_debug"
#define TS_DEBUG               "ts_debug"
#define USB_HOST_DEBUG         "usb_host_debug"
#define USB_DEV_DEBUG          "usb_dev_debug"
#define FB_DEBUG               "fb_debug"
#define CRYPTO_DEBUG           "crypto_debug"
#define PWR_CTRL_DEBUG         "pwr_ctrl_debug"
#define CIR_DEBUG              "cir_debug"

/* help message */
#define CS752X_HELP_MSG "READ Usage: cat %s\n" \
                        "WRITE Usage: echo [value] > %s\n" \
                        "value 0: Disable\n" \
                        "value 1: Enable\n"

#define CS752X_NI_HELP_MSG "READ Usage: cat %s\n" \
                        "WRITE Usage: echo [value] > %s\n" \
                        "value 0: Disable NI and IRQ DBG Message\n" \
                        "value 1: Enable NI DBG Message\n" \
                        "value 2: Enable IRQ DBG Message\n" \
                        "value 3: Enable NI and IRQ DBG Message\n" \
                        "value 4: Enable DMA LSO DBG Message\n" \
                        "value 5: Enable to dump RX packets\n" \
                        "value 6: Enable to dump TX packets\n"

#define FE_HELP_MSG     "READ Usage: cat %s\n" \
                        "WRITE Usage: echo [bitwise flag] > %s\n" \
                        "flag 0x00000001: ACL table                   \n" \
                        "flag 0x00000002: AN BNG MAC table            \n" \
                        "flag 0x00000004: Memory checking table       \n" \
                        "flag 0x00000008: Classifier table            \n" \
                        "flag 0x00000010: Ethernet type table         \n" \
                        "flag 0x00000020: Forwarding hash result table\n" \
                        "flag 0x00000040: HASH                        \n" \
                        "flag 0x00000080: Hardware table              \n" \
                        "flag 0x00000100: LLC header table            \n" \
                        "flag 0x00000200: LPB table                   \n" \
                        "flag 0x00000400: LPM module                  \n" \
                        "flag 0x00000800: Multicast group ID          \n" \
                        "flag 0x00001000: Packet editor VoQ drop table\n" \
                        "flag 0x00002000: Packet length range table   \n" \
                        "flag 0x00004000: Port range table            \n" \
                        "flag 0x00008000: QoS hash result table       \n" \
                        "flag 0x00010000: Flow VLAN table             \n" \
                        "flag 0x00020000: L2 result table             \n" \
                        "flag 0x00040000: L3 result table             \n" \
                        "flag 0x00080000: SDB table                   \n" \
                        "flag 0x00100000: VLAN table                  \n" \
                        "flag 0x00200000: VoQ policer table           \n"

#define ADAPT_HELP_MSG  "READ Usage: cat %s\n" \
                        "WRITE Usage: echo [bitwise flag] > %s\n" \
                        "flag 0x00000001: 802.1q VLAN hooks\n" \
                        "flag 0x00000002: Bridge hooks     \n" \
                        "flag 0x00000004: Basic functions  \n" \
                        "flag 0x00000008: Core hooks       \n" \
                        "flag 0x00000010: IPSec hooks      \n" \
                        "flag 0x00000020: IPv6 hooks       \n" \
                        "flag 0x00000040: IGMP hooks       \n" \
                        "flag 0x00000080: Netfilter hooks  \n" \
                        "flag 0x00000100: PPPoE hooks      \n" \
                        "flag 0x00000200: QoS hooks        \n"

#define HW_ACCEL_HELP_MSG  "READ Usage: cat %s\n" \
                        "WRITE Usage: echo [bitwise flag] > %s\n" \
                        "flag 0x00000000: Disable HW acceleration  \n" \
                        "flag 0x00000001: HW acceleration          \n" \
                        "flag 0x00000002: Bridge acceleration      \n" \
                        "flag 0x00000004: NAT acceleration         \n" \
                        "flag 0x00000008: VLAN acceleration        \n" \
                        "flag 0x00000010: PPPoE acceleration       \n" \
                        "flag 0x00000020: IPSec acceleration       \n" \
                        "flag 0x00000040: Multicast acceleration   \n" \
                        "flag 0x00000080: IPv6 Routing acceleration\n" \
                        "flag 0x00000100: OoS Ingress acceleration \n" \
                        "flag 0x00000200: OoS Engress acceleration \n" \
                        "flag 0x00000400: Double check enable      \n"

#define REG_HELP_MSG "Command: echo write [block] [subblock] [reg] [value] > %s\n" \
                        "Command: echo read [block] [subblock] [reg] > %s\n" \
                        "Write example: echo write 1 1 4 0x000300ff > %s\n" \
                        "Read example: echo read 1 1 4 > %s\n"

/* entry pointer */
struct proc_dir_entry *proc_driver_cs752x,
    *proc_driver_cs752x_ne,
    *proc_driver_cs752x_ne_ni,
    *proc_driver_cs752x_ne_qm,
    *proc_driver_cs752x_ne_tm,
    *proc_driver_cs752x_ne_sch,
    *proc_driver_cs752x_ne_switch,
    *proc_driver_cs752x_ne_fe, *proc_driver_cs752x_ne_adaptation;

struct proc_dir_entry *proc_driver_cs752x_pcie,
    *proc_driver_cs752x_sata,
    *proc_driver_cs752x_sd,
    *proc_driver_cs752x_spdif,
    *proc_driver_cs752x_ssp,
    *proc_driver_cs752x_ts,
    *proc_driver_cs752x_usb_host,
    *proc_driver_cs752x_usb_dev,
    *proc_driver_cs752x_fb,
    *proc_driver_cs752x_crypto,
    *proc_driver_cs752x_pwr_ctrl, *proc_driver_cs752x_cir;

EXPORT_SYMBOL(proc_driver_cs752x);
EXPORT_SYMBOL(proc_driver_cs752x_ne);
EXPORT_SYMBOL(proc_driver_cs752x_ne_ni);
EXPORT_SYMBOL(proc_driver_cs752x_ne_qm);
EXPORT_SYMBOL(proc_driver_cs752x_ne_tm);
EXPORT_SYMBOL(proc_driver_cs752x_ne_sch);
EXPORT_SYMBOL(proc_driver_cs752x_ne_switch);
EXPORT_SYMBOL(proc_driver_cs752x_ne_fe);
EXPORT_SYMBOL(proc_driver_cs752x_ne_adaptation);

EXPORT_SYMBOL(proc_driver_cs752x_pcie);
EXPORT_SYMBOL(proc_driver_cs752x_sata);
EXPORT_SYMBOL(proc_driver_cs752x_sd);
EXPORT_SYMBOL(proc_driver_cs752x_spdif);
EXPORT_SYMBOL(proc_driver_cs752x_ssp);
EXPORT_SYMBOL(proc_driver_cs752x_ts);
EXPORT_SYMBOL(proc_driver_cs752x_usb_host);
EXPORT_SYMBOL(proc_driver_cs752x_usb_dev);
EXPORT_SYMBOL(proc_driver_cs752x_fb);
EXPORT_SYMBOL(proc_driver_cs752x_crypto);
EXPORT_SYMBOL(proc_driver_cs752x_pwr_ctrl);
EXPORT_SYMBOL(proc_driver_cs752x_cir);

EXPORT_SYMBOL(cs752x_add_proc_handler);
EXPORT_SYMBOL(cs752x_str_paser);

/*
 * Wrapper Functions
 */

int cs752x_add_proc_handler(char *name,
			    read_proc_t * hook_func_read,
			    write_proc_t * hook_func_write,
			    struct proc_dir_entry *parent)
{
	struct proc_dir_entry *node;

	node = create_proc_entry(name, S_IRUGO | S_IWUGO, parent);
	if (node) {
		node->read_proc = hook_func_read;
		node->write_proc = hook_func_write;
	} else {
		printk(KERN_ERR "ERROR in creating proc entry (%s)! \n", name);
		return -EINVAL;
	}

	return 0;
}

/*
 * Purpose: Parse string into a list of tokens.
 *          Notice that the original string will be changed.
 *          All splitters will be replaced with '\0'. 
 * Params: 
 *          src_str:   original string. It will be modified after calling this function.
 *          max_tok_num: the max. number of tokens could be supported in output array.
 *          tok_idx_list[]: a list of pointers which point to each token found in the original string.
 *                      We don't allocate other buffers for tokens, and directly reuse the original one.
 *          tok_cnt:    The acutal number of tokens we found in the string.
 * Return:
 *          0:  success
 *          -1: fail
 */

int cs752x_str_paser(char *src_str, int max_tok_num,
		     char *tok_idx_list[] /*output */ ,
		     int *tok_cnt /*output */ )
{
	int tok_index = 0;
	char *curr, *tok_head;

	if (src_str == NULL ||
	    max_tok_num < 1 || tok_idx_list == NULL || tok_cnt == NULL) {
		return EINVAL;
	}

	/* split each token into token buffer */
	curr = tok_head = src_str;
	while ((tok_index < max_tok_num)) {
		/* when we still have empty token buffer  */
		switch (*curr) {
		case '\0':	/* exit when end of string */
			goto STR_PARSER_EXIT;

		case '\"':
		case '\n':
		case '\r':
		case '\t':
		case ' ':
		case ',':
			if (tok_head != curr) {
				/* there is at least one character in the token */
				*curr = '\0';
				break;
			} else
				tok_head++;
			/* go through default case */

		default:
			curr++;
			continue;
		}
		tok_idx_list[tok_index] = tok_head;
		tok_index++;
		curr++;
		tok_head = curr;
	}

      STR_PARSER_EXIT:
	*tok_cnt = tok_index;
	return 0;
}

/*
 * The proc filesystem: function to read and write entry
 */

/* file handler for cs_pcie_debug */
static int cs_pcie_debug_read_proc(char *buf, char **start, off_t offset,
				   int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, PCIE_DEBUG, PCIE_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", PCIE_DEBUG, cs_pcie_debug);
	*eof = 1;

	return len;
}

static int cs_pcie_debug_write_proc(struct file *file, const char *buffer,
				    unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto PCIE_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto PCIE_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto PCIE_INVAL_EXIT;

	cs_pcie_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", PCIE_DEBUG, cs_pcie_debug);

	return count;

      PCIE_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, PCIE_DEBUG, PCIE_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_sata_debug */
static int cs_sata_debug_read_proc(char *buf, char **start, off_t offset,
				   int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SATA_DEBUG, SATA_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", SATA_DEBUG, cs_sata_debug);
	*eof = 1;

	return len;
}

static int cs_sata_debug_write_proc(struct file *file, const char *buffer,
				    unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SATA_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SATA_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SATA_INVAL_EXIT;

	cs_sata_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SATA_DEBUG, cs_sata_debug);

	return count;

      SATA_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SATA_DEBUG, SATA_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_sd_debug */
static int cs_sd_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SD_DEBUG, SD_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", SD_DEBUG, cs_sd_debug);
	*eof = 1;

	return len;
}

static int cs_sd_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SD_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SD_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SD_INVAL_EXIT;

	cs_sd_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SD_DEBUG, cs_sd_debug);

	return count;

      SD_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SD_DEBUG, SD_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_spdif_debug */
static int cs_spdif_debug_read_proc(char *buf, char **start, off_t offset,
				    int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SPDIF_DEBUG, SPDIF_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", SPDIF_DEBUG, cs_spdif_debug);
	*eof = 1;

	return len;
}

static int cs_spdif_debug_write_proc(struct file *file, const char *buffer,
				     unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SPDIF_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SPDIF_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SPDIF_INVAL_EXIT;

	cs_spdif_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SPDIF_DEBUG, cs_spdif_debug);

	return count;

      SPDIF_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SPDIF_DEBUG, SPDIF_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_ssp_debug */
static int cs_ssp_debug_read_proc(char *buf, char **start, off_t offset,
				  int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SSP_DEBUG, SSP_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", SSP_DEBUG, cs_ssp_debug);
	*eof = 1;

	return len;
}

static int cs_ssp_debug_write_proc(struct file *file, const char *buffer,
				   unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SSP_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SSP_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SSP_INVAL_EXIT;

	cs_ssp_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SSP_DEBUG, cs_ssp_debug);

	return count;

      SSP_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SSP_DEBUG, SSP_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_ts_debug */
static int cs_ts_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, TS_DEBUG, TS_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", TS_DEBUG, cs_ts_debug);
	*eof = 1;

	return len;
}

static int cs_ts_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto TS_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto TS_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto TS_INVAL_EXIT;

	cs_ts_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", TS_DEBUG, cs_ts_debug);

	return count;

      TS_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, TS_DEBUG, TS_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_usb_host_debug */
static int cs_usb_host_debug_read_proc(char *buf, char **start, off_t offset,
				       int count, int *eof, void *data)
{
	u32 len = 0;

	len +=
	    sprintf(buf + len, CS752X_HELP_MSG, USB_HOST_DEBUG, USB_HOST_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", USB_HOST_DEBUG,
		    cs_usb_host_debug);
	*eof = 1;

	return len;
}

static int cs_usb_host_debug_write_proc(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto USB_HOST_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto USB_HOST_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto USB_HOST_INVAL_EXIT;

	cs_usb_host_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", USB_HOST_DEBUG,
	       cs_usb_host_debug);

	return count;

      USB_HOST_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, USB_HOST_DEBUG, USB_HOST_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_usb_dev_debug */
static int cs_usb_dev_debug_read_proc(char *buf, char **start, off_t offset,
				      int count, int *eof, void *data)
{
	u32 len = 0;

	len +=
	    sprintf(buf + len, CS752X_HELP_MSG, USB_DEV_DEBUG, USB_DEV_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", USB_DEV_DEBUG,
		    cs_usb_dev_debug);
	*eof = 1;

	return len;
}

static int cs_usb_dev_debug_write_proc(struct file *file, const char *buffer,
				       unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto USB_DEV_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto USB_DEV_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto USB_DEV_INVAL_EXIT;

	cs_usb_dev_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", USB_DEV_DEBUG,
	       cs_usb_dev_debug);

	return count;

      USB_DEV_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, USB_DEV_DEBUG, USB_DEV_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_fb_debug */
static int cs_fb_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, FB_DEBUG, FB_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", FB_DEBUG, cs_fb_debug);
	*eof = 1;

	return len;
}

static int cs_fb_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto FB_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto FB_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto FB_INVAL_EXIT;

	cs_fb_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", FB_DEBUG, cs_fb_debug);

	return count;

      FB_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, FB_DEBUG, FB_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_crypto_debug */
static int cs_crypto_debug_read_proc(char *buf, char **start, off_t offset,
				     int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, CRYPTO_DEBUG, CRYPTO_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", CRYPTO_DEBUG,
		    cs_crypto_debug);
	*eof = 1;

	return len;
}

static int cs_crypto_debug_write_proc(struct file *file, const char *buffer,
				      unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto CRYPTO_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto CRYPTO_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto CRYPTO_INVAL_EXIT;

	cs_crypto_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", CRYPTO_DEBUG,
	       cs_crypto_debug);

	return count;

      CRYPTO_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, CRYPTO_DEBUG, CRYPTO_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_pwr_ctrl_debug */
static int cs_pwr_ctrl_debug_read_proc(char *buf, char **start, off_t offset,
				       int count, int *eof, void *data)
{
	u32 len = 0;

	len +=
	    sprintf(buf + len, CS752X_HELP_MSG, PWR_CTRL_DEBUG, PWR_CTRL_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", PWR_CTRL_DEBUG,
		    cs_pwr_ctrl_debug);
	*eof = 1;

	return len;
}

static int cs_pwr_ctrl_debug_write_proc(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto PWR_CTRL_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto PWR_CTRL_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto PWR_CTRL_INVAL_EXIT;

	cs_pwr_ctrl_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", PWR_CTRL_DEBUG,
	       cs_pwr_ctrl_debug);

	return count;

      PWR_CTRL_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, PWR_CTRL_DEBUG, PWR_CTRL_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_cir_debug */
static int cs_cir_debug_read_proc(char *buf, char **start, off_t offset,
				  int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, CIR_DEBUG, CIR_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", CIR_DEBUG, cs_cir_debug);
	*eof = 1;

	return len;
}

static int cs_cir_debug_write_proc(struct file *file, const char *buffer,
				   unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto CIR_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto CIR_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto CIR_INVAL_EXIT;

	cs_cir_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", CIR_DEBUG, cs_cir_debug);

	return count;

      CIR_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, CIR_DEBUG, CIR_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_ni_debug */
static int cs_ni_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_NI_HELP_MSG, NI_DEBUG, NI_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", NI_DEBUG, cs_ni_debug);
	*eof = 1;

	return len;
}

static int cs_ni_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto NI_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto NI_INVAL_EXIT;

	//if (mask > CS752X_MAX )
	//      goto NI_INVAL_EXIT;

	cs_ni_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", NI_DEBUG, cs_ni_debug);

	return count;

      NI_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, NI_DEBUG, NI_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_qm_debug */
static int cs_qm_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, QM_DEBUG, QM_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", QM_DEBUG, cs_qm_debug);
	*eof = 1;

	return len;
}

static int cs_qm_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto QM_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto QM_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto QM_INVAL_EXIT;

	cs_qm_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", QM_DEBUG, cs_qm_debug);

	return count;

      QM_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, QM_DEBUG, QM_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_tm_debug */
static int cs_tm_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, TM_DEBUG, TM_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", TM_DEBUG, cs_tm_debug);
	*eof = 1;

	return len;
}

static int cs_tm_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto TM_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto TM_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto TM_INVAL_EXIT;

	cs_tm_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", TM_DEBUG, cs_tm_debug);

	return count;

      TM_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, TM_DEBUG, TM_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_sch_debug */
static int cs_sch_debug_read_proc(char *buf, char **start, off_t offset,
				  int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SCH_DEBUG, SCH_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", SCH_DEBUG, cs_sch_debug);
	*eof = 1;

	return len;
}

static int cs_sch_debug_write_proc(struct file *file, const char *buffer,
				   unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SCH_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SCH_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SCH_INVAL_EXIT;

	cs_sch_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SCH_DEBUG, cs_sch_debug);

	return count;

      SCH_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SCH_DEBUG, SCH_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_switch_debug */
static int cs_switch_debug_read_proc(char *buf, char **start, off_t offset,
				     int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, CS752X_HELP_MSG, SWITCH_DEBUG, SWITCH_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", SWITCH_DEBUG,
		    cs_switch_debug);
	*eof = 1;

	return len;
}

static int cs_switch_debug_write_proc(struct file *file, const char *buffer,
				      unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto SWITCH_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto SWITCH_INVAL_EXIT;

	if (mask > CS752X_MAX)
		goto SWITCH_INVAL_EXIT;

	cs_switch_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", SWITCH_DEBUG,
	       cs_switch_debug);

	return count;

      SWITCH_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING CS752X_HELP_MSG, SWITCH_DEBUG, SWITCH_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_fe_debug */
static int cs_fe_debug_read_proc(char *buf, char **start, off_t offset,
				 int count, int *eof, void *data)
{
	int i;
	u32 len = 0;

	len += sprintf(buf + len, FE_HELP_MSG, FE_DEBUG, FE_DEBUG);
	len += sprintf(buf + len, "\n%s = 0x%08x\n", FE_DEBUG, cs_fe_debug);
	*eof = 1;

#if 0
	//FIXME: move below code to through ioctl, CH
	//"echo 6 > /proc/driver/cs752x/ne/cs_fe_debug"
	if (cs_fe_debug == 6) {
		for (i = 0; i < 12288 * 2; i++) {
			cs_hash_invalid_hash_entry_by_index(i);
		}
		printk("Delete all of Hash entry\n");
	}
#endif
	return len;
}

static int cs_fe_debug_write_proc(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto FE_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto FE_INVAL_EXIT;

	if (mask > CS752X_FE_MAX)
		goto FE_INVAL_EXIT;

	cs_fe_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", FE_DEBUG, cs_fe_debug);

	return count;

      FE_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING FE_HELP_MSG, FE_DEBUG, FE_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_adapt_debug */
static int cs_adapt_debug_read_proc(char *buf, char **start, off_t offset,
				    int count, int *eof, void *data)
{
	u32 len = 0;

	len += sprintf(buf + len, ADAPT_HELP_MSG, ADAPT_DEBUG, ADAPT_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", ADAPT_DEBUG, cs_adapt_debug);
	*eof = 1;
	return len;
}

static int cs_adapt_debug_write_proc(struct file *file, const char *buffer,
				     unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto ADAPT_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto ADAPT_INVAL_EXIT;

	if (mask > CS752X_ADAPT_MAX)
		goto ADAPT_INVAL_EXIT;

	cs_adapt_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", ADAPT_DEBUG, cs_adapt_debug);

	return count;

      ADAPT_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING ADAPT_HELP_MSG, ADAPT_DEBUG, ADAPT_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* file handler for cs_hw_accel_debug */
static int cs_hw_accel_debug_read_proc(char *buf, char **start, off_t offset,
				       int count, int *eof, void *data)
{
	u32 len = 0;

	len +=
	    sprintf(buf + len, HW_ACCEL_HELP_MSG, HW_ACCEL_DEBUG,
		    HW_ACCEL_DEBUG);
	len +=
	    sprintf(buf + len, "\n%s = 0x%08x\n", HW_ACCEL_DEBUG,
		    cs_hw_accel_debug);
	*eof = 1;
	return len;
}

static int cs_hw_accel_debug_write_proc(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	char buf[32];
	unsigned long mask;
	ssize_t len;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto HW_ACCEL_INVAL_EXIT;

	buf[len] = '\0';
	if (strict_strtoul(buf, 0, &mask))
		goto HW_ACCEL_INVAL_EXIT;

	if (mask > CS752X_HW_ACCEL_MAX)
		goto HW_ACCEL_INVAL_EXIT;

	cs_hw_accel_debug = mask;
	printk(KERN_WARNING "Set %s as 0x%08x\n", HW_ACCEL_DEBUG,
	       cs_hw_accel_debug);

	return count;

      HW_ACCEL_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING HW_ACCEL_HELP_MSG, HW_ACCEL_DEBUG, HW_ACCEL_DEBUG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/* sample code to support multiple parameters in one proc file */

/* 
 * --------------Sample code--------------
 * Read switch register 
 */
static int cs_switch_reg_read_proc(char *buf, char **start, off_t offset,
				   int count, int *eof, void *data)
{
	u32 len = 0;

	len +=
	    sprintf(buf + len, REG_HELP_MSG, SWITCH_REG, SWITCH_REG, SWITCH_REG,
		    SWITCH_REG);
	*eof = 1;

	return len;
}

/*
 * --------------Sample code--------------
 * Switch register read/write (read from user space)
 * Command: echo "write [block] [subblock] [reg] [value]" > register
 *          echo "read  [block] [subblock] [reg]" > register
 * Params:  [block]:    [1-2]       
 *          [subblock]: [1-8]
 *          [reg]:      [0x1-0xFF]
 *          [value]:    [0-0xFFFFFFFF]
 */

#define MAX_BUF_SIZE            128
#define MAX_NUM_OF_PARAMETERS   10

#define MAX_BLOCK_INDEX         2
#define MIN_BLOCK_INDEX         1

#define MAX_SUBBLOCK_INDEX      8
#define MIN_SUBBLOCK_INDEX      1

#define MAX_REG_ADDR            0xFF
#define MIN_REG_ADDR            0x1

static int cs_switch_reg_write_proc(struct file *file, const char *buffer,
				    unsigned long count, void *data)
{
	char buf[MAX_BUF_SIZE];
	char *token_list[MAX_NUM_OF_PARAMETERS];
	int tok_cnt = 0;
	ssize_t len;
	const char *write_cmd = "write";
	const char *read_cmd = "read";
	u8 cmd_mode;		/* 0:read, 1:write */
	//int idx;
	int nTokLen;
	unsigned long block, sub_block, reg, value;

	len = min(count, (unsigned long)(sizeof(buf) - 1));
	if (copy_from_user(buf, buffer, len))
		goto REG_INVAL_EXIT;

	buf[len] = '\0';

	/* split each token */
	if (cs752x_str_paser(buf, MAX_NUM_OF_PARAMETERS, token_list, &tok_cnt)) {
		goto REG_INVAL_EXIT;
	}
#if 0				/* only for debug */
	for (idx = 0; idx < tok_cnt; idx++) {
		printk(KERN_WARNING "Token %d: \"%s\"\n", idx + 1,
		       token_list[idx]);
	}
#endif

	/* analyze meaning of each token */

	nTokLen = strlen(token_list[0]);
	if (!strncmp(write_cmd, token_list[0], nTokLen) && tok_cnt >= 5) {
		cmd_mode = 1;	/* write */
	} else if (!strncmp(read_cmd, token_list[0], nTokLen) && tok_cnt >= 4) {
		cmd_mode = 0;	/* read */
	} else
		goto REG_INVAL_EXIT;

	if (strict_strtoul(token_list[1], 0, &block) ||
	    block > MAX_BLOCK_INDEX || block < MIN_BLOCK_INDEX)
		goto REG_INVAL_EXIT;

	if (strict_strtoul(token_list[2], 0, &sub_block) ||
	    sub_block > MAX_SUBBLOCK_INDEX || sub_block < MIN_SUBBLOCK_INDEX)
		goto REG_INVAL_EXIT;

	if (strict_strtoul(token_list[3], 0, &reg) ||
	    reg > MAX_REG_ADDR || reg < MIN_REG_ADDR)
		goto REG_INVAL_EXIT;

	if (cmd_mode == 1) {
		if (strict_strtoul(token_list[4], 0, &value))
			goto REG_INVAL_EXIT;

		/* for debug only */
		printk
		    ("Write: (block, subblock, reg, value) -> (%ld, %ld, %ld, 0x%08lX)\n",
		     block, sub_block, reg, value);
	} else {
		/* for debug only */
		printk("Read: (block, subblock, reg) -> (%ld, %ld, %ld)\n",
		       block, sub_block, reg);
	}

	/* Execute the command here */

	/* End of the execution */

	return count;

      REG_INVAL_EXIT:
	printk(KERN_WARNING "Invalid argument\n");
	printk(KERN_WARNING REG_HELP_MSG, SWITCH_REG, SWITCH_REG, SWITCH_REG,
	       SWITCH_REG);
	return count;	/* if we return error code here, PROC fs may retry up to 3 times. */
}

/*
 * Actually create (and remove) the /proc file(s).
 */

void __exit cs752x_proc_cleanup_module(void)
{
	/* no problem if it was not registered */
	/* remove file entry */
	remove_proc_entry(PCIE_DEBUG, proc_driver_cs752x_pcie);
	remove_proc_entry(SATA_DEBUG, proc_driver_cs752x_sata);
	remove_proc_entry(SD_DEBUG, proc_driver_cs752x_sd);
	remove_proc_entry(SPDIF_DEBUG, proc_driver_cs752x_spdif);
	remove_proc_entry(SSP_DEBUG, proc_driver_cs752x_ssp);
	remove_proc_entry(TS_DEBUG, proc_driver_cs752x_ts);
	remove_proc_entry(USB_HOST_DEBUG, proc_driver_cs752x_usb_host);
	remove_proc_entry(USB_DEV_DEBUG, proc_driver_cs752x_usb_dev);
	remove_proc_entry(FB_DEBUG, proc_driver_cs752x_fb);
	remove_proc_entry(CRYPTO_DEBUG, proc_driver_cs752x_crypto);
	remove_proc_entry(PWR_CTRL_DEBUG, proc_driver_cs752x_pwr_ctrl);
	remove_proc_entry(CIR_DEBUG, proc_driver_cs752x_cir);
	remove_proc_entry(NI_DEBUG, proc_driver_cs752x_ne_ni);
	remove_proc_entry(QM_DEBUG, proc_driver_cs752x_ne_qm);
	remove_proc_entry(TM_DEBUG, proc_driver_cs752x_ne_tm);
	remove_proc_entry(SCH_DEBUG, proc_driver_cs752x_ne_sch);
	remove_proc_entry(SWITCH_DEBUG, proc_driver_cs752x_ne_switch);
	remove_proc_entry(SWITCH_REG, proc_driver_cs752x_ne_switch);
	remove_proc_entry(FE_DEBUG, proc_driver_cs752x_ne_fe);
	remove_proc_entry(ADAPT_DEBUG, proc_driver_cs752x_ne_adaptation);

	/* remove dir entry */
	remove_proc_entry(CS752X_PCIE, proc_driver_cs752x);
	remove_proc_entry(CS752X_SATA, proc_driver_cs752x);
	remove_proc_entry(CS752X_SD, proc_driver_cs752x);
	remove_proc_entry(CS752X_SPDIF, proc_driver_cs752x);
	remove_proc_entry(CS752X_SSP, proc_driver_cs752x);
	remove_proc_entry(CS752X_TS, proc_driver_cs752x);
	remove_proc_entry(CS752X_USB_HOST, proc_driver_cs752x);
	remove_proc_entry(CS752X_USB_DEV, proc_driver_cs752x);
	remove_proc_entry(CS752X_FB, proc_driver_cs752x);
	remove_proc_entry(CS752X_CRYPTO, proc_driver_cs752x);
	remove_proc_entry(CS752X_PWR_CTRL, proc_driver_cs752x);
	remove_proc_entry(CS752X_CIR, proc_driver_cs752x);
	remove_proc_entry(CS752X_NE_NI, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_QM, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_TM, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_SCH, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_SWITCH, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_FE, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE_ADAPT, proc_driver_cs752x_ne);
	remove_proc_entry(CS752X_NE, proc_driver_cs752x);
	remove_proc_entry(CS752X, NULL);
}

int __init cs752x_proc_init_module(void)
{
	proc_driver_cs752x = proc_mkdir(CS752X, NULL);
	proc_driver_cs752x_ne = proc_mkdir(CS752X_NE, proc_driver_cs752x);
	proc_driver_cs752x_ne_ni =
	    proc_mkdir(CS752X_NE_NI, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_qm =
	    proc_mkdir(CS752X_NE_QM, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_tm =
	    proc_mkdir(CS752X_NE_TM, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_sch =
	    proc_mkdir(CS752X_NE_SCH, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_switch =
	    proc_mkdir(CS752X_NE_SWITCH, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_fe =
	    proc_mkdir(CS752X_NE_FE, proc_driver_cs752x_ne);
	proc_driver_cs752x_ne_adaptation =
	    proc_mkdir(CS752X_NE_ADAPT, proc_driver_cs752x_ne);

	proc_driver_cs752x_pcie = proc_mkdir(CS752X_PCIE, proc_driver_cs752x);
	proc_driver_cs752x_sata = proc_mkdir(CS752X_SATA, proc_driver_cs752x);
	proc_driver_cs752x_sd = proc_mkdir(CS752X_SD, proc_driver_cs752x);
	proc_driver_cs752x_spdif = proc_mkdir(CS752X_SPDIF, proc_driver_cs752x);
	proc_driver_cs752x_ssp = proc_mkdir(CS752X_SSP, proc_driver_cs752x);
	proc_driver_cs752x_ts = proc_mkdir(CS752X_TS, proc_driver_cs752x);
	proc_driver_cs752x_usb_host =
	    proc_mkdir(CS752X_USB_HOST, proc_driver_cs752x);
	proc_driver_cs752x_usb_dev =
	    proc_mkdir(CS752X_USB_DEV, proc_driver_cs752x);
	proc_driver_cs752x_fb = proc_mkdir(CS752X_FB, proc_driver_cs752x);
	proc_driver_cs752x_crypto =
	    proc_mkdir(CS752X_CRYPTO, proc_driver_cs752x);
	proc_driver_cs752x_pwr_ctrl =
	    proc_mkdir(CS752X_PWR_CTRL, proc_driver_cs752x);
	proc_driver_cs752x_cir = proc_mkdir(CS752X_CIR, proc_driver_cs752x);

	cs752x_add_proc_handler(NI_DEBUG, cs_ni_debug_read_proc,
				cs_ni_debug_write_proc,
				proc_driver_cs752x_ne_ni);
	cs752x_add_proc_handler(QM_DEBUG, cs_qm_debug_read_proc,
				cs_qm_debug_write_proc,
				proc_driver_cs752x_ne_qm);
	cs752x_add_proc_handler(TM_DEBUG, cs_tm_debug_read_proc,
				cs_tm_debug_write_proc,
				proc_driver_cs752x_ne_tm);
	cs752x_add_proc_handler(SCH_DEBUG, cs_sch_debug_read_proc,
				cs_sch_debug_write_proc,
				proc_driver_cs752x_ne_sch);
	cs752x_add_proc_handler(SWITCH_DEBUG, cs_switch_debug_read_proc,
				cs_switch_debug_write_proc,
				proc_driver_cs752x_ne_switch);
	cs752x_add_proc_handler(FE_DEBUG, cs_fe_debug_read_proc,
				cs_fe_debug_write_proc,
				proc_driver_cs752x_ne_fe);
	cs752x_add_proc_handler(ADAPT_DEBUG, cs_adapt_debug_read_proc,
				cs_adapt_debug_write_proc,
				proc_driver_cs752x_ne_adaptation);
	cs752x_add_proc_handler(HW_ACCEL_DEBUG, cs_hw_accel_debug_read_proc,
				cs_hw_accel_debug_write_proc,
				proc_driver_cs752x_ne);

	cs752x_add_proc_handler(PCIE_DEBUG, cs_pcie_debug_read_proc,
				cs_pcie_debug_write_proc,
				proc_driver_cs752x_pcie);
	cs752x_add_proc_handler(SATA_DEBUG, cs_sata_debug_read_proc,
				cs_sata_debug_write_proc,
				proc_driver_cs752x_sata);
	cs752x_add_proc_handler(SD_DEBUG, cs_sd_debug_read_proc,
				cs_sd_debug_write_proc, proc_driver_cs752x_sd);
	cs752x_add_proc_handler(SPDIF_DEBUG, cs_spdif_debug_read_proc,
				cs_spdif_debug_write_proc,
				proc_driver_cs752x_spdif);
	cs752x_add_proc_handler(SSP_DEBUG, cs_ssp_debug_read_proc,
				cs_ssp_debug_write_proc,
				proc_driver_cs752x_ssp);
	cs752x_add_proc_handler(TS_DEBUG, cs_ts_debug_read_proc,
				cs_ts_debug_write_proc, proc_driver_cs752x_ts);
	cs752x_add_proc_handler(USB_HOST_DEBUG, cs_usb_host_debug_read_proc,
				cs_usb_host_debug_write_proc,
				proc_driver_cs752x_usb_host);
	cs752x_add_proc_handler(USB_DEV_DEBUG, cs_usb_dev_debug_read_proc,
				cs_usb_dev_debug_write_proc,
				proc_driver_cs752x_usb_dev);
	cs752x_add_proc_handler(FB_DEBUG, cs_fb_debug_read_proc,
				cs_fb_debug_write_proc, proc_driver_cs752x_fb);
	cs752x_add_proc_handler(CRYPTO_DEBUG, cs_crypto_debug_read_proc,
				cs_crypto_debug_write_proc,
				proc_driver_cs752x_crypto);
	cs752x_add_proc_handler(PWR_CTRL_DEBUG, cs_pwr_ctrl_debug_read_proc,
				cs_pwr_ctrl_debug_write_proc,
				proc_driver_cs752x_pwr_ctrl);
	cs752x_add_proc_handler(CIR_DEBUG, cs_cir_debug_read_proc,
				cs_cir_debug_write_proc,
				proc_driver_cs752x_cir);

	cs752x_add_proc_handler(SWITCH_REG, cs_switch_reg_read_proc,
				cs_switch_reg_write_proc,
				proc_driver_cs752x_ne_switch);
	return 0;
}

module_init(cs752x_proc_init_module);
module_exit(cs752x_proc_cleanup_module);
MODULE_AUTHOR("Eric Wang <eric.wang@cortina-systems.com>");
MODULE_LICENSE("GPL");

#endif				/* CONFIG_CS752X_PROC */
