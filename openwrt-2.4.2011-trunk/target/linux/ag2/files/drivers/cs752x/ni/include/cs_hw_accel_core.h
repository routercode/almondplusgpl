/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                CH Hsu <ch.hsu@cortina-systems.com>
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
#ifndef __CS_KERNEL_ACCEL_CORE_H__
#define __CS_KERNEL_ACCEL_CORE_H__
#if 0
#include "cs_hw_accel_util.h"
#include "uthash.h"
#include "cs_hw_accel_cb.h"

#define CS_FE_HASH_TIMER	1
//#define CS_UU_TEST	1	/* Enable Unknown Unicast */
//#define NEW_L2_DESIGN	1

//#define CS_NE_TEST_BY_IXIA	1
//#define CS_NE_TEST_NAT	1
// FIXME.. it's originally from ni/cs752x_eth.h
#define GE_PORT_NUM					3

#define CS_GUID_MAP_HASH_NUM_POWER	12
#define CS_GUID_MAP_HASH_NUM		(1 << CS_GUID_MAP_HASH_NUM_POWER)

#define CS_HASH_MAP_GUID_NUM_POWER	16	//32K
#define CS_HASH_MAP_GUID_NUM		(1 << CS_GUID_MAP_HASH_NUM_POWER)
#define CS_SUB_HW_HASH_NUM			12
#define CS_GUID_NUM					10
#define CS_VIRTUAL_TABLE_NUM		4
#define CS_VIRTUAL_TABLE_RESOURCE	3   /* clissifer, sdb, tuple */
#define HASH_TIMER_PERIOD		    30  /* seconds */

#define CS_RESULT_ACTION_ENABLE		1
#define CS_RESULT_ACTION_DISABLE	0
#define CS_ACL_ENABLE		1
#define CS_ACL_DISABLE		0
#define CS_DROP_ENABLE		1
#define CS_DROP_DISABLE		0
#define CS_HASH_MASK_ENABLE 0
#define CS_HASH_MASK_DISABLE 1

// SYSTEM TOPOLOGY INFORMATION
#define CS_VTBL_ID_L2_RULE  0x10
#define CS_VTBL_ID_L2_RULE_PRIORITY 0xA
#define CS_VTBL_ID_L2_FLOW  0x20
#define CS_VTBL_ID_L2_FLOW_PRIORITY 0x9
#define CS_VTBL_ID_L3_RULE  0x30
#define CS_VTBL_ID_L3_RULE_PRIORITY 0x8
#define CS_VTBL_ID_L3_FLOW  0x40
#define CS_VTBL_ID_L3_FLOW_PRIORITY 0x7
#define CS_VTBL_ID_IPSEC    0x50 //FIXME: How to arrange Vtable ID and priority ??
#define CS_VTBL_ID_IPSEC_PRIORITY   0x6
#define CS_VTBL_ID_UU_FLOW 0x80	//FIXME:CH
#define CS_VTBL_ID_UU_FLOW_PRIORITY 0x5

/* 0 = Dynamic allocation of the major device number */
#define CS_HW_ACCEL_MAJOR_N 0

/*
* CS Kernel Control Block
*	- Used by driver only
*	- Stores LAN-IN or WAN-IN information
*	- WAN-OUT and LAN-OUT driver use them to build up a hash entry
*	- NOTES: To update this data structure, MUST take care of alignment issue
*   -		 MUST make sure that the size of skbuff structure must
*            be larger than (64 + sizof(NAT_CB_T))
*/


#define _VIRTUAL_TABLE(CLASSIFIER, SDB, TUPLE_NUM) \
	{ .classifer_id = CLASSIFIER, .sdb_id = SDB, .tuple_number = TUPLE_NUM }

const static struct {
	cs_uint8 classifer_id;
	cs_uint8 sdb_id;
	int tuple_number;
} virtual_table_info[] = {
	_VIRTUAL_TABLE(0, 0, 6),
	_VIRTUAL_TABLE(1, 1, 6),
	_VIRTUAL_TABLE(2, 2, 6),
	_VIRTUAL_TABLE(3, 3, 6),
};

struct cs_core_hash {
	cs_uint16 hash_index; /* key */
	cs_uint8 vid;
	cs_uint8 guid_cnt;
	cs_uint64 guid[MODULE_GUID_NUM];
	UT_hash_handle hh; /* makes this structure hashable */
};

typedef struct hash_guid{
	cs_uint8 vid;
	cs_uint8 guid_cnt;
	cs_uint64 guid[MODULE_GUID_NUM];
	struct list_head guid_list;
} cs_hash_guid_s;

typedef struct {
	struct net_device *dev;
} NI_DEV_T;

typedef struct {
	cs_uint8 enabled;
	cs_uint8 l2_rule_vtable_id;
	cs_uint8 l2_flow_vtable_id;
	cs_uint8 l3_rule_vtable_id;
	cs_uint8 l3_flow_vtable_id;
	NI_DEV_T gmacDev[GE_PORT_NUM];
} CORE_CFG_T;

//struct timer_list fe_hash_timer_obj;
void cs_kernel_core_hw_fe_init(void);
cs_status cs_kernel_core_init_cfg(void);
void cs_kernel_core_hash_timer_func(cs_uint32 data);
void cs_kernel_core_fe_del_tuple(cs_uint8 tuple_idx);
cs_status cs_kernel_get_module_callback(cs_uint64 *p_guid_array,
		cs_uint8 guid_cnt, 	cs_uint16 status);

NI_DEV_T *cs_kernel_core_find_gmacdev(struct net_device *device);
cs_status cs_kernel_core_init_cfg(void);
cs_uint16 cs_kernel_to_forwarding_result(fe_hash_rslt_s *result);
cs_uint8 cs_kernel_to_hash_mask(CS_HASH_ENTRY_S *hash_entry, 
	fe_hash_mask_entry_t *mask_rule);

cs_uint32 cs_kernel_core_add_hash_cfg(struct sk_buff *skb);
cs_uint32 cs_kernel_core_add_hash_by_cb(CS_KERNEL_ACCEL_CB_T *cs_cb);
cs_uint32 cs_kernel_core_add_default_hash(cs_uint8 vid);
cs_status cs_kernel_core_allocate_vtable_resource(cs_uint8 vid, cs_uint8 type);

cs_status cs_add_qos_tuple7_entries(int8_t, int8_t, int8_t, int8_t);
#endif

/* New APIs */

#define CS_HW_ACCEL_MAJOR_N 0

ssize_t cs_hw_accel_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos);
int cs_hw_accel_open(struct inode *inode, struct file *filp);
int cs_hw_accel_release(struct inode *inode, struct file *filp);
int __init cs_hw_accel_init(void);
void __exit cs_hw_accel_cleanup(void);

cs_status cs_hw_accel_add_connections(struct sk_buff *skb);
cs_status cs_hw_accel_input_set_cb(struct sk_buff *skb);
#endif	/* __CS_KERNEL_ACCEL_CORE_H__ */
