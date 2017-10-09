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

#ifndef __CS_VTABLE_HASH_H__
#define __CS_VTABLE_HASH_H__

#include <linux/list.h>
#include "cs_fe_util_api.h"
#include "cs_gc.h"

#define GARBAGE_COLLECTOR_ENABLE 1

#define FE_FWD_HASH_SIZE	FE_FWD_RESULT_ENTRY_MAX
#define FE_QOS_HASH_SIZE	FE_QOS_RESULT_ENTRY_MAX

#ifdef GARBAGE_COLLECTOR_ENABLE
// struct cs_vtable_rslt_lookup_entry_s;
#endif

struct cs_vtable_hash_qos_entry_s;
/*
 * crc32, result_index, and mask_ptr are needed by hardware hash entry. 
 * State is for entry state, invalid, reserved or valid. 
 * gc_users is user count of garbage collection tables.
 * next points to next bucket of hash entry.
 * l2_result, l3_result, and vid_result_index for result lookup table indices. 
 * When a hash entry is invalidated, result lookup table entry should be 
 * updated (i.e., decrement user count).
 * qos_entry links to associated qos entries.
 */
typedef enum {
	CS_VTABLE_L2_RSLT_IN_DA,
	CS_VTABLE_L2_RSLT_IN_SA,
	CS_VTABLE_L2_RSLT_OUT_DA,
	CS_VTABLE_L2_RSLT_OUT_SA,
	CS_VTABLE_L2_RSLT_TYPE_MAX
} cs_vtable_l2_rslt_type_e;

typedef enum {
	CS_VTABLE_L3_RSLT_IN_IP_DA,
	CS_VTABLE_L3_RSLT_IN_IP_SA,
	CS_VTABLE_L3_RSLT_OUT_IP_DA,
	CS_VTABLE_L3_RSLT_OUT_IP_SA,
	CS_VTABLE_L3_RSLT_TYPE_MAX
} cs_vtable_l3_rslt_type_e;

typedef enum {
	CS_VTABLE_L4_RSLT_IN_DST_PORT,
	CS_VTABLE_L4_RSLT_IN_SRC_PORT,
	CS_VTABLE_L4_RSLT_OUT_DST_PORT,
	CS_VTABLE_L4_RSLT_OUT_SRC_PORT,
	CS_VTABLE_L4_RSLT_TYPE_MAX
} cs_vtable_l4_rslt_type_e;

typedef enum {
	CS_VTABLE_VLAN_RSLT_IN_OUTER_VID,
	CS_VTABLE_VLAN_RSLT_IN_INNER_VID,
	CS_VTABLE_VLAN_RSLT_OUT_OUTER_VID,
	CS_VTABLE_VLAN_RSLT_OUT_INNER_VID,
	CS_VTABLE_VLAN_RSLT_TYPE_MAX
} cs_vtable_vlan_rslt_type_e;

// FIXME!! need more clarification from CXC. do we really need reserved state?
typedef enum {
	CS_VHASH_STATE_INVALID,
	CS_VHASH_STATE_RESERVED,
	CS_VHASH_STATE_ACTIVE,
	CS_VHASH_STATE_MAX,
} cs_vtable_hash_state_e;

struct cs_vtable_hash_entry_s {
	// crc16 and crc32 are used to double check against values stored in hw_hash_entry.
	__u32		crc32;
	__u16		crc16;
	__u16		result_index;
	__u8		mask_ptr;

	__u8		state;
	__u16		hw_index;

	void		*table;
	atomic_t	gc_users;

	unsigned long	last_use;	/* in jiffies */
	unsigned int	lifetime;	/* in second */
	// next bucket
	struct cs_vtable_hash_entry_s	*next;
#if 0
	// if l2 pair, l2_sa is used.
	struct fe_rslt_lookup_entry_s	*l2_sa, *l2_da;
	struct fe_rslt_lookup_entry_s	*l3_sa, *l3_da;
	struct fe_rslt_lookup_entry_s	*l4_sp, *l4_dp;
	struct fe_rslt_lookup_entry_s	*sw_misc;
#endif
#if 0
#ifdef GARBAGE_COLLECTOR_ENABLE
	struct cs_vtable_rslt_lookup_entry_s *l2_result[CS_VTABLE_L2_RSLT_TYPE_MAX];
	struct cs_vtable_rslt_lookup_entry_s *l3_result[CS_VTABLE_L3_RSLT_TYPE_MAX];
	struct cs_vtable_rslt_lookup_entry_s *l4_result[CS_VTABLE_L4_RSLT_TYPE_MAX];
	struct cs_vtable_rslt_lookup_entry_s *vlan_result[CS_VTABLE_VLAN_RSLT_TYPE_MAX];
#endif
#endif
	// associated qos entry list
	struct cs_vtable_hash_qos_entry_s	*qos_entry;
};

/*
 * crc32, result_index, and mask_ptr are needed by hardware hash entry. 
 * State is for entry state, invalid, reserved or valid. 
 * gc_users is user count of garbage collection tables.
 * next points to next bucket of hash entry.
 * When a hash entry is invalidated, result lookup table entry should be 
 * updated (i.e., decrement user count).
 */
typedef struct cs_vtable_hash_qos_entry_s {
	__u32		crc32;
	__u16		crc16;
	__u16		result_index;
	__u8		mask_ptr;

	__u8		state;
	__u16		hw_index;

	void		*table;
	//atomic_t	gc_users;	/* Does QoS hash entry need to keep gc_users? */

	/* next bucket */
	struct cs_vtable_hash_qos_entry_s	*next;

	/* this qos hash entry might link to a given fwd hash */
	struct cs_vtable_hash_entry_s		*fwd_hash;
	/* Next_qos is used as another QoS entry of the SAME fwd entry. In one QoS 
	 * model, one FWD entry company with multiple QoS entries. */
	struct cs_vtable_hash_qos_entry_s	*next_qos;
} cs_vtable_hash_qos_entry_t;

#if 0	// FIXME!! what for?
typedef struct cs_vtable_hash_result_s {
		cs_vtable_hash_entry_t  entries[6];
} cs_vtable_hash_result_t;

typedef struct cs_hash_vtable_s {
	cs_vtable_hash_result_t *vtable[FE_VTABLE_SIZE];
	cs_uint8		vid;			// virtual_table_ID
	cs_uint8		type;
	cs_uint8		sdb_idx;
	cs_uint8		class_idx;
	cs_uint8		mcg_id;
	spinlock_t	  	lock;
} cs_hash_vtable_t;
#endif


/*
 * Assign allocated L2 result (lookup) entry to vtable hash entry. 
 * vtable hash entry state should be reserved. The function should be called 
 * while allocating result entries, but hash not enabled yet.
 */
int cs_vhash_set_l2_result(unsigned int l2_type, cs_vtable_hash_entry_t *hash, 
		__u16 index, __u8 *mac_addr);

/*
 * Assign allocated L3 result (lookup) entry to vtable hash entry.  
 * Vtable hash entry state should be reserved. The function should be called 
 * while allocating result entries, but hash not enabled yet.
 */
int cs_vhash_set_l3_result(unsigned int l3_type, cs_vtable_hash_entry_t *hash, 
		__u16 index, __u32 *ip_addr, bool is_v6);

/*
 * Assign allocated L4 result (lookup) entry to vtable hash entry.  
 * Vtable hash entry state should be reserved. The function should be called 
 * while allocating result entries, but hash not enabled yet.
 */
int cs_vhash_set_l4_result(unsigned int l4_type, cs_vtable_hash_entry_t *hash, 
		__u16 port);

/* 
 * Assign vid index to vtable hash entry.
 * vtable hash entry state should be reserved. The function should be called 
 * while allocating result entries, but hash not enabled yet.
 */
int cs_vhash_set_vid_result(unsigned int vlan_type, 
		cs_vtable_hash_entry_t *hash, __u16 flow_vlan_index, __u16 vid);

/* create swid to result lookup table */
int cs_vhash_set_swid_result(cs_vtable_hash_entry_t *hash, __u64 swid, 
		struct fe_rslt_lookup_entry_s **new_lookup);

/*
 * Set vtable hash entry state, reserved or valid. When an entry state is 
 * reserved, only who sets the state could change it .
 * When an entry state is set valid, it will enable hardware hash entry.
 */
int cs_vhash_set_state(cs_vtable_hash_entry_t *hash, __u8 state);

/*
 * Set vtable hash entry to invalid, and release associated l2, l3 result 
 * lookup entries, all QoS entries.
 * 
 * Set hash entry state to invalid, and hash table pointer to NULL.  
 * When garbage collection function calls, the invalid entry is ready to free.
 */
int cs_vhash_invalid_fwd_entry(cs_vtable_hash_entry_t *hash);

/* delete the vtable hash entry entity by invalidating it and all the 
 * vtable qos hash entries that link to it, and then removing its linkage 
 * to vtable as well */
int cs_vhash_del_fwd_entry(cs_vtable_hash_entry_t *hash);

/* Add a QoS hash entry to fwd hash entry */
int cs_vhash_add_qos_entry(cs_vtable_hash_entry_t *fwd_hash, 
		cs_vtable_hash_qos_entry_t *qos_hash);

/* set the vtable qos hash entry to invalid */
int cs_vhash_invalid_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash);

/* delete the vtable qos hash entry and all the hw entities that it uses */
int cs_vhash_del_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash);

/* remove a qos hash entry from fwd hash entry given the qos_hash */
int cs_vhash_remove_qos_from_fwd(cs_vtable_hash_qos_entry_t *qos_hash, 
		cs_vtable_hash_entry_t *fwd_hash);


/* Remove a QoS hash entry from Fwd hash entry by crc32 value. */
int cs_vhash_remove_qos_from_fwd_by_crc32(cs_vtable_hash_entry_t *fwd_hash, 
		__u32 qos_crc32);

/* compare two hash entries, see if they are the same */
bool cs_vhash_compare(cs_vtable_hash_entry_t *hash_a, 
		cs_vtable_hash_entry_t *hash_b);

int cs_vhash_update_last_use(u16 hw_idx, int status);
#if 0
/* update the last use of the vhash with the given hw_idx to the 
 * current jiffies */
int cs_vhash_update_last_use(__u16 hw_idx);
/* tell vhash that this hw_idx is not used durig the last period of 
 * low level hash table timer */
int cs_vhash_update_not_use(__u16 hw_idx);
#endif

int cs_vhash_init(void);

#endif /* __CS_VTABLE_HASH_H__ */
