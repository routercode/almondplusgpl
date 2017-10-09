/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/

#ifndef CS752X_PROC_H
#define CS752X_PROC_H

#include <linux/proc_fs.h>

#ifndef TRUE
#define TRUE	(1 == 1)
#endif

#ifndef FALSE
#define FALSE	(1 == 0)
#endif

#ifndef NULL
#define NULL	0
#endif

enum CS752X_DEBUG {
	CS752X_DISABLE = 0,
	CS752X_ENABLE = 1,
	CS752X_MAX = CS752X_ENABLE
};

enum CS752X_NI_DEBUG {
	DBG_DIASBLE = 0,
	DBG_NI,
	DBG_IRQ,
	DBG_NI_IRQ,
	DBG_NI_LSO,
	DBG_NI_DUMP_RX,
	DBG_NI_DUMP_TX,
	CS752X_NI_MAX = DBG_NI_DUMP_TX
};

enum CS752X_FE_DEBUG {
	CS752X_FE_ACL 		= 0x00000001,	/* offset 0 */
	CS752X_FE_AN_BNG_MAC 	= 0x00000002,
	CS752X_FE_CHECKMEM 	= 0x00000004,
	CS752X_FE_CLASS 	= 0x00000008,
	CS752X_FE_ETYPE 	= 0x00000010,	/* offset 4 */
	CS752X_FE_FWDRSLT 	= 0x00000020,
	CS752X_FE_HASH 		= 0x00000040,
	CS752X_FE_HW 		= 0x00000080,
	CS752X_FE_LLC_HDR 	= 0x00000100,	/* offset 8 */
	CS752X_FE_LPB 		= 0x00000200,
	CS752X_FE_LPM 		= 0x00000400,
	CS752X_FE_MC 		= 0x00000800,
	CS752X_FE_PE_VOQ_DRP 	= 0x00001000,	/* offset 12 */
	CS752X_FE_PKTLEN_RNGS 	= 0x00002000,
	CS752X_FE_PORT_RNGS 	= 0x00004000,
	CS752X_FE_QOSRSLT 	= 0x00008000,
	CS752X_FE_RSLT_FVLAN_TBL = 0x00010000,	/* offset 16 */
	CS752X_FE_RSLT_L2_TBL 	= 0x00020000,
	CS752X_FE_RSLT_L3_TBL 	= 0x00040000,
	CS752X_FE_SDB 		= 0x00080000,
	CS752X_FE_VLAN 		= 0x00100000,	/* offset 20 */
	CS752X_FE_VOQPOL 	= 0x00200000,
	CS752X_FE_MAX 		= 0x003FFFFF	/* the max. value when all flags are set */
};

enum CS752X_ADAPT_DEBUG {
	CS752X_ADAPT_8021Q 	= 0x00000001,	/* offset 0 */
	CS752X_ADAPT_BRIDGE 	= 0x00000002,
	CS752X_ADAPT_COMMON 	= 0x00000004,
	CS752X_ADAPT_CORE 	= 0x00000008,
	CS752X_ADAPT_IPSEC 	= 0x00000010,	/* offset 4 */
	CS752X_ADAPT_IPV6 	= 0x00000020,
	CS752X_ADAPT_MULTICAST 	= 0x00000040,
	CS752X_ADAPT_NETFILTER 	= 0x00000080,
	CS752X_ADAPT_PPPOE 	= 0x00000100,	/* offset 8 */
	CS752X_ADAPT_QOS 	= 0x00000200,
	CS752X_ADAPT_MAX 	= 0x000003FF	/* the max. value when all flags are set */
};

enum CS752X_HW_ACCEL_DEBUG {
	CS752X_HW_ACCEL 	= 0x00000001,	/* offset 0 */
	CS752X_HW_ACCEL_BRIDGE 	= 0x00000002,
	CS752X_HW_ACCEL_NAT 	= 0x00000004,
	CS752X_HW_ACCEL_VLAN 	= 0x00000008,
	CS752X_HW_ACCEL_PPPOE 	= 0x00000010,	/* offset 4 */
	CS752X_HW_ACCEL_IPSEC 	= 0x00000020,
	CS752X_HW_ACCEL_MULTICAST = 0x00000040,
	CS752X_HW_ACCEL_IPV6_ROUTING = 0x00000080,
	CS752X_HW_ACCEL_QOS_INGRESS = 0x00000100,	/* offset 8 */
	CS752X_HW_ACCEL_QOS_ENGRESS = 0x00000200,
	CS752X_HW_ACCEL_DOUBLE_CHECK = 0x00000400,
	CS752X_HW_ACCEL_MAX 	= 0x000007FF	/* the max. value when all flags are set */
};

#ifdef CONFIG_CS752X_PROC

int cs752x_str_paser(char *src_str, int max_tok_num,
		     char *tok_idx_list[] /*output */ ,
		     int *tok_cnt /*output */ );
int cs752x_add_proc_handler(char *name, read_proc_t * hook_func_read,
			    write_proc_t * hook_func_write,
			    struct proc_dir_entry *parent);

#else				/* CONFIG_PROC_FS */

static inline int cs752x_str_paser(char *src_str, int max_tok_num,
				   char *tok_idx_list[] /*output */ ,
				   int *tok_cnt /*output */ )
{
}

static inline int cs752x_add_proc_handler(char *name,
					  read_proc_t * hook_func_read,
					  write_proc_t * hook_func_write,
					  struct proc_dir_entry *parent)
{
}

#endif				/* CONFIG_CS752X_PROC */

#endif				/* CS752X_PROC_H */
