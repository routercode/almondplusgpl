/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                Wen Hsu <wen.hsu@cortina-systems.com>
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

#ifndef __CS_VTABLE_H__
#define __CS_VTABLE_H__

#include <mach/cs75xx_fe_core_table.h>
#include <linux/spinlock.h>
#include <cs_fe_mc.h>
//#include "cs_hw_accel_util.h"	//FIXME!!! why do we need this header?
#include "cs_fe.h"
#include "cs_vtable_hash.h"

#define GARBAGE_COLLECTOR_ENABLE 1

#ifdef GARBAGE_COLLECTOR_ENABLE
#include "cs_gc.h"
#endif

//#define MULTIPLE_VTABLE 1	
#define VTABLE_FORCE_ID 1
#define VTABLE_DYN_ID 2
//#define VTABLE_DEFAULT_MCG_ID 	0
//#define VTABLE_L2_FLOW_MCG_ID 	1
//#define VTABLE_L3_RULE_MCG_ID 	2
//#define VTABLE_L3_FLOW_MCG_ID 	3

#define VTABLE_UINT8_INVALID	0xFF

#define VTABLE_ACT_DEF_TO_CPU   1
#define VTABLE_ACT_DEF_TO_VTBL  2
 
/*
 * Virtual Hash Table, Entry and Operations.
 * Vtable has same structure of hw hash table, each bucket has 3 entry with
 * 6 results total
 */
#define FE_VTABLE_SIZE  (1<<12)

typedef enum {
	FE_VTABLE_TYPE_NONE,
	FE_VTABLE_TYPE_BCAST,
	FE_VTABLE_TYPE_L2_MCAST,
	FE_VTABLE_TYPE_L3_MCAST_V4,
	FE_VTABLE_TYPE_L3_MCAST_V6,
	FE_VTABLE_TYPE_L2_FORWARD,
	FE_VTABLE_TYPE_L3_FORWARD,
	FE_VTABLE_TYPE_RE0,
	FE_VTABLE_TYPE_RE1,
	FE_VTABLE_TYPE_ARP,

	FE_VTABLE_TYPE_MAX,
} cs_vtable_type_e;

typedef enum {
	FE_APP_TYPE_NONE,
	FE_APP_TYPE_L2_MCAST,
	FE_APP_TYPE_L3_MCAST,
	FE_APP_TYPE_SA_CHECK,
	FE_APP_TYPE_L2_FLOW,
//	FE_APP_TYPE_L2_QOS_1,
//	FE_APP_TYPE_L2_QOS_2,
	FE_APP_TYPE_L2_RULE_1,	/* 5 */
	FE_APP_TYPE_L2_RULE_2,
	FE_APP_TYPE_L3_FLOW_GENERIC,
	FE_APP_TYPE_L3_IPV6_ROUTING,
	FE_APP_TYPE_L4_FLOW_NAT,
//	FE_APP_TYPE_L3_QOS_GENERIC,
//	FE_APP_TYPE_L4_QOS_NAT,
	FE_APP_TYPE_L3_RULE_1,	/* 10 */
	FE_APP_TYPE_L4_RULE_1,

	FE_APP_TYPE_MAX,
} cs_fe_fwd_app_type_e;

typedef enum {
	FE_APP_QOS_TYPE_NONE = FE_APP_TYPE_MAX,
	FE_APP_QOS_L2_QOS_1,	/* 13 */
	FE_APP_QOS_L2_QOS_2,
	FE_APP_QOS_L3_QOS_GENERIC,
	FE_APP_QOS_L4_QOS_NAT,

	FE_APP_QOS_TYPE_MAX,
} cs_fe_qos_app_type_e;


/* 
 * this data structure defines relationship of hash mask entry, application
 * type and corresponding entry index.
 *
 */
#if 0
typedef struct {
	cs_fe_fwd_app_type_e	app_type;
	__u8	hm_index;
	__u64	hm_flags;
	__u16	fwd_vtable_type;
} cs_fe_hm_app_t;
/*
 * The hm_flags may change later.
 */
static cs_fe_hm_app_t
cs_fe_hm_app_table[FE_APP_TYPE_MAX] = {
	{FE_APP_TYPE_NONE,		0,	0},
	{FE_APP_TYPE_L2_MCAST,		1,	0},
	{FE_APP_TYPE_L3_MCAST,		2,	CS_HM_MAC_SA_MASK |
						CS_HM_IP_VLD_MASK |
						CS_HM_IP_VER_MASK |
						CS_HM_IP_SA_MASK  |
						CS_HM_IP_DA_MASK  |
						CS_HM_MCIDX_MASK},

	{FE_APP_TYPE_SA_CHECK,		3,	CS_HM_MAC_SA_MASK},

	{FE_APP_TYPE_L2_FLOW,		4,	CS_HM_MAC_DA_MASK    |
						CS_HM_ETHERTYPE_MASK |
						CS_HM_VID_1_MASK},

	{FE_APP_TYPE_L2_QOS_1,		5,	CS_HM_8021P_1_MASK |
						CS_HM_DEI_1_MASK   |
						CS_HM_IP_VLD_MASK  |
						CS_HM_DSCP_MASK    |
						CS_HM_ECN_MASK},

	{FE_APP_TYPE_L2_QOS_2,		6,	0},

	{FE_APP_TYPE_L2_RULE_1,		7,	CS_HM_MAC_SA_MASK    |
						CS_HM_MAC_DA_MASK    |
						CS_HM_ETHERTYPE_MASK |
						CS_HM_VID_1_MASK},

	{FE_APP_TYPE_L2_RULE_2,		8,	0},

	{FE_APP_TYPE_L3_FLOW_GENERIC,	9,	CS_HM_IP_VLD_MASK   |
						CS_HM_IP_VER_MASK   |
						CS_HM_IP_DA_MASK    |
						CS_HM_IP_SA_MASK    |
						CS_HM_IP_PROT_MASK  |
						CS_HM_SPI_VLD_MASK  |
						CS_HM_SPI_MASK      |
						CS_HM_IPV6_NDP_MASK |
						CS_HM_IPV6_HBH_MASK |
						CS_HM_IPV6_RH_MASK  |
						CS_HM_IPV6_DOH_MASK},

	{FE_APP_TYPE_L4_FLOW_NAT,	10,	CS_HM_IP_VLD_MASK  |
						CS_HM_IP_VER_MASK  |
						CS_HM_IP_PROT_MASK |
						CS_HM_IP_DA_MASK   |
						CS_HM_IP_SA_MASK   |
						CS_HM_L4_VLD_MASK  |
						CS_HM_L4_SP_MASK   |
						CS_HM_L4_DP_MASK   |
						CS_HM_TCP_CTRL_MASK},

	{FE_APP_TYPE_L3_QOS_GENERIC,	11,	CS_HM_IP_VLD_MASK   |
						CS_HM_IP_VER_MASK   |
						CS_HM_IP_DA_MASK    |
						CS_HM_IP_SA_MASK    |
						CS_HM_IP_PROT_MASK  |
						CS_HM_SPI_VLD_MASK  |
						CS_HM_SPI_MASK      |
						CS_HM_IPV6_NDP_MASK |
						CS_HM_IPV6_HBH_MASK |
						CS_HM_IPV6_RH_MASK  |
						CS_HM_IPV6_DOH_MASK |
						CS_HM_8021P_1_MASK  |
						CS_HM_DEI_1_MASK    |
						CS_HM_DSCP_MASK     |
						CS_HM_ECN_MASK},

	{FE_APP_TYPE_L4_QOS_NAT,	12,	CS_HM_IP_VLD_MASK  |
						CS_HM_IP_VER_MASK  |
						CS_HM_IP_PROT_MASK |
						CS_HM_IP_DA_MASK   |
						CS_HM_IP_SA_MASK   |
						CS_HM_L4_VLD_MASK  |
						CS_HM_L4_SP_MASK   |
						CS_HM_L4_DP_MASK   |
						CS_HM_TCP_CTRL_MASK |
						CS_HM_8021P_1_MASK  |
						CS_HM_DEI_1_MASK    |
						CS_HM_DSCP_MASK     |
						CS_HM_ECN_MASK},

	{FE_APP_TYPE_L3_RULE_1,		13,	0},

	{FE_APP_TYPE_L4_RULE_1,		14,	0},
};
#endif	//#if 0


typedef struct cs_swid_entry_s {
	struct cs_swid_entry_s *next;
	__u64 swid;
} cs_swid_entry_t;

typedef struct cs_swid_param_s {
	unsigned int num_swid;
	cs_swid_entry_t *list;
} cs_swid_param_t;

/* when adding hash with only fwd_hash_entry or qos_hash_entry, it will 
 * be created by itself individually w/o being linking together. 
 * However, if adding fwd_hash_entry AND qos_hash_entry, they are linked 
 * together.  When one is deleted, the other will be forced to be delete */
typedef struct {
	fe_sw_hash_t key;
	unsigned int fwd_app_type;
	fe_fwd_result_entry_t action;
	fe_fwd_result_param_t param;
	cs_swid_param_t swid_info;
	unsigned int lifetime;	/* in second, 0 means forever */
} cs_fwd_hash_t;

typedef struct {
	fe_sw_hash_t key;
	unsigned int fwd_app_type;
	fe_qos_result_entry_t action;
	cs_swid_param_t swid_info;
} cs_qos_hash_t;
#if 0
int cs_vtable_set_fwd_hash(cs_vtable_fwd_hash_t *fwd_hash,
	CS_KERNEL_ACCEL_CB_T* cb);
int cs_vtable_set_qos_hash(cs_vtable_qos_hash_t *qos_hash,
	CS_KERNEL_ACCEL_CB_T* cb);
#endif
/*
 * One vtable maps to one type of application entity, by app_type. There could
 * be multiple vtables for one application, i.e. rule table.
 */

typedef struct cs_vtable_s {
	/* next is another vtable of same app_type, but it may have its own 
	 * classifier, SDB, etc. */
	struct cs_vtable_s	*next;

	/* prev is another vtable whose next is this vtable. */
	struct cs_vtable_s	*prev;

	/* service id tells what kind of application this vtable is about bcast, 
	 * mcast, L2 forwarding, L3 forwarding etc. */
	unsigned int	vtable_type;

	unsigned int	class_index;
	unsigned int	sdb_index;

	/* mcgid of this V-Table, usually distinguish from other V-Table of same 
	 * app_type. */
	unsigned int	mcgid;

	/* uu flow index to forwarding result table */
	unsigned int	uuflow_idx;
	unsigned int	bcflow_idx;
	unsigned int	umflow_idx;

	/* fwd hash table and qos hash table. Do we need qos hash table though? */
	cs_vtable_hash_entry_t		**fwd_hash;
	cs_vtable_hash_qos_entry_t	**qos_hash;

	/* VoQ Mgr specifies how to choose VoQ for each connection of this VTable */
	void*		voq_mgr;

	/* FIXME! do we need forwarding result table here? */

	spinlock_t	lock;
#if 0
	/* May associate set hash functions per app_fwd_type */
	int (*set_fwd_hash)(void *table, cs_vtable_fwd_hash_t *fwd_hash,
			CS_KERNEL_ACCEL_CB_T* cb);
	int (*set_qos_hash)(void *table, cs_vtable_qos_hash_t *qos_hash,
			CS_KERNEL_ACCEL_CB_T* cb);
#endif
} cs_vtable_t;

/*
 * Five default Vtables: broadcast, multicast, L2, L3, RE.
 */

int cs_vtable_init(void);

/* allocate a vtable with app_type */
int cs_vtable_new(unsigned int vtbl_type, cs_vtable_t **table);

/* release a vtable */
int cs_vtable_release(cs_vtable_t *table);

/* 
 * get one of the five default vtables, caller may get chained vtables
 */
int cs_vtable_get_default(unsigned int vtbl_type, cs_vtable_t **table);

int cs_vtable_set_class_idx(cs_vtable_t *table, unsigned int class_idx);
int cs_vtable_get_class_idx(cs_vtable_t *table, unsigned int *class_idx);
int cs_vtable_set_sdb_idx(cs_vtable_t *table, unsigned int sdb_idx);
int cs_vtable_get_sdb_idx(cs_vtable_t *table, unsigned int *sdb_idx);
int cs_vtable_set_uuflow_idx(cs_vtable_t *table, unsigned int uuflow_idx);

/*
 * Add and del tuple to SDB of the vtable.
 */
int cs_vtable_add_tuple(cs_vtable_t *table, fe_hash_mask_entry_t *hash_mask, 
		unsigned int priority, bool is_qos);
int cs_vtable_del_tuple(cs_vtable_t *table, fe_hash_mask_entry_t *hash_mask, 
		bool is_qos);


/* the role of voq_mgr is to decide the destination voq of connections */
int cs_vtable_set_voq_mgr(cs_vtable_t *table, void* voq_mgr);

int cs_vtable_get_hash_mask_ptr(cs_vtable_t *table, 
		fe_hash_mask_entry_t *hash_mask, __u8 *hash_mask_ptr);
int cs_vtable_add_hash(cs_vtable_t *table, 
		cs_fwd_hash_t *fwd_hash_entry, 
		cs_qos_hash_t *qos_hash_entry);

// FIXME!! do we need the below API?
#if 0
int cs_vtable_del_hash(cs_vtable_t *table, 
		cs_vtable_fwd_hash_t *fwd_hash_entry, 
		cs_vtable_qos_hash_t *qos_hash_entry);
#endif

int cs_vtable_remove_hash_from_list(cs_vtable_hash_entry_t *vhash);
int cs_vtable_remove_qos_hash_from_list(cs_vtable_hash_qos_entry_t *vhash);

#ifdef CONFIG_CS752X_HW_ACCELERATION
extern int get_hash_mask_idx_by_app_type(unsigned int fwd_app_type, 
		__u8 *p_hash_mask_idx);

extern int cs_vtable_set_fwd_hash_l2sa_check(cs_vtable_t *vtable,
		cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb);
extern int cs_vtable_set_fwd_hash_nat(cs_vtable_t *table,
		cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb);
extern int cs_vtable_set_fwd_hash_bridge(cs_vtable_t *table,
		cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb);
#endif
// FIXME!! define later
#if 0
/*
 * Connection status table that kernel module retrieves and updates
 */
typedef struct cs_fe_conn_status {

} cs_fe_conn_status_t;

/*
 * Generic MCGID manager
 * replication and re-circulation
 */
typedef struct cs_fe_mcgid_mgr {

} cs_fe_mcgid_mgr_t;


/*
 * Generic VoQ manager
 * Determine VoQ based on policer setting, etc.
 */
typedef struct cs_fe_voq_mgr {

} cs_fe_voq_mgr_t;
#endif	//#if 0

#if 0
int cs_vtable_to_forwarding_result(fe_hash_rslt_s *result, 
		cs_uint8 v_type, cs_uint8 vtable_fwd_act, cs_uint8 nxt_vtable_idx);
int cs_find_vtable_idx_by_type (cs_uint8 type);
cs_status cs_kernel_to_checkmem(CS_HASH_ENTRY_S *p_hash, 
        fe_hash_check_entry_t *pchkmem);
cs_uint8 cs_vtable_to_hash_mask(CS_HASH_ENTRY_S *hash_entry,
        fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type);
cs_uint8 cs_vtable_to_qos_hash_mask(CS_HASH_ENTRY_S *hash_entry, 
		fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type);
cs_status cs_vtable_add_sdb_tuple(cs_uint8 mask_ptr, fe_hash_vtable_s *vtable);		                		
fe_hash_vtable_s *cs_find_vtable_by_type (cs_uint8 type);
cs_status cs_vtable_add_sdb_qos_tuple(cs_uint8 mask_ptr, fe_hash_vtable_s *vtable);
cs_status cs_add_vtable_entry(cs_uint16 crc16, cs_uint32 crc32, 
		cs_uint8 mask_ptr, cs_uint16 result_index, cs_uint16 *return_idx, 
		cs_uint8 vid, cs_int16 timeout);
cs_uint32 cs_vtable_add_hash_by_cb(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 v_type);
cs_uint32 cs_vtable_add_hash_by_cb(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 v_type);
cs_uint8 cs_vtable_to_hash_mask_qos_only(CS_HASH_ENTRY_S *hash_entry,
        fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type);
cs_uint16 cs_vtable_to_qos_result(fe_hash_rslt_s *result, cs_uint8 v_type);
cs_status cs_alloc_vtable(cs_uint8 *vid, cs_uint8 type, int default_act, 
		int next_vtable_type, int forced, int forced_mcg_id, int priority); 
cs_uint32 cs_vtable_add_hash_by_skb(struct sk_buff *skb , cs_uint8 v_type);
#endif

#endif /* __CS_VTABLE_H__ */

