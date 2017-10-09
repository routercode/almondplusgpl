/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_vtable_hash.c
 *
 * $Id: cs_vtable_hash.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * It contains the implementation of hash related APIs of vtable.
 * used for a specific type of traffic.
 */

#include <linux/jiffies.h>
#include "cs_fe.h"
#include "cs_vtable_hash.h"
#include "cs_gc.h"
#include "cs_vtable.h"

static cs_vtable_hash_entry_t *hash_link_table[FE_HASH_STATUS_ENTRY_MAX << 6];

static unsigned int hw_index_to_link_tbl_idx(__u16 hw_idx)
{
	if (IS_OVERFLOW_ENTRY(hw_idx))
		return ((HASH_INDEX_SW2HW(hw_idx) & 0x3f) + 
				((FE_HASH_STATUS_ENTRY_MAX - 1) << 6));
	else
		return (((hw_idx >> 4) * 6) + (hw_idx & 0x07));
} /* hw_index_to_link_tbl_idx */

int cs_vhash_update_last_use(u16 hw_idx, int status)
{
	unsigned int link_tbl_idx = hw_index_to_link_tbl_idx(hw_idx);
	cs_vtable_hash_entry_t *vhash = hash_link_table[link_tbl_idx];
	unsigned long curr_time = jiffies;

	if (vhash == NULL) {
		printk("%s::hw idx %x, vhash entry NULL!\n",
			__func__, hw_idx);
		return 0;
	}
	vhash->last_use = curr_time;

	if (status == 0) {
		vhash->state = CS_VHASH_STATE_INVALID;
		cs_fe_hash_del_hash(vhash->hw_index);
#if 0
		/* when to enable this? */
		curr_qos = hash->qos_entry;
		while (curr_qos != NULL) {
			cs_vhash_invalid_qos_entry(curr_qos);
			curr_qos = curr_qos->next_qos;
		}
#endif
		hash_link_table[link_tbl_idx] = NULL;
	}
#if 0
	printk("%s::status %d, hw_idx %x, vhash %p\n",
		__func__, status, hw_idx, vhash);
#endif	
	return 0;
}

int cs_vhash_invalid_fwd_entry(cs_vtable_hash_entry_t *hash)
{
	cs_vhash_update_last_use(hash->hw_index, 0);
	return 0;
}

int cs_vhash_add_qos_entry(cs_vtable_hash_entry_t *fwd_hash,
       cs_vtable_hash_qos_entry_t *qos_hash)
{
	return 0;
}

int cs_vhash_del_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash)
{
	return 0;
}

int cs_vhash_del_fwd_entry(cs_vtable_hash_entry_t *fwd_hash)
{
	unsigned int link_tbl_idx = hw_index_to_link_tbl_idx(fwd_hash->hw_index);
	cs_vtable_t *vtable = fwd_hash->table;
	cs_vtable_hash_entry_t *vtable_entry, *head, *prev;

	head = vtable_entry = vtable->fwd_hash[fwd_hash->hw_index >> 4];
#if 0
	printk("%s::hash %p, hw_idx %x, table %p, entry %p, vtable entry %p\n",
		__func__, fwd_hash, fwd_hash->hw_index, fwd_hash->table,
		hash_link_table[link_tbl_idx], vtable_entry);
#endif
	while ((vtable_entry != NULL) && (vtable_entry != fwd_hash)) {
		prev = vtable_entry;
		vtable_entry = vtable_entry->next;
	}

	if (head == fwd_hash)
		vtable->fwd_hash[fwd_hash->hw_index>>4] = head->next;
	else {
		prev->next = vtable_entry->next;
	}

	// shall we kfree(vtable_entry) here?
#if 0
	printk("\t%s::vtable entry %p\n",
		__func__, vtable->fwd_hash[fwd_hash->hw_index>>4]);
#endif
	return 0;
}

int cs_vhash_invalid_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash)
{
	return 0;
}
/*
 * We do not distinguish MAC SA, MAC DA, IP SA, IP DA in this implementation
 */

#if 0
int cs_vhash_set_l2_result(unsigned int l2_type, cs_vtable_hash_entry_t *hash, 
		__u16 index, __u8 *mac_addr)
{
	cs_vtable_rslt_lookup_entry_t *new_lookup;
	int ret;

	if (l2_type >= CS_VTABLE_L2_RSLT_TYPE_MAX) return -1;
	if ((hash == NULL) || (mac_addr == NULL)) return -1;
	if (hash->l2_result[l2_type] != NULL) return -1;

	ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_MAC, (void *)mac_addr, 
			&new_lookup);
	if (ret != 0) return ret;

	new_lookup->hw_index = index;
	hash->l2_result[l2_type] = new_lookup;

	return 0;
} /* cs_vhash_set_l2_result */
#endif
#if 0
int cs_vhash_set_l3_result(unsigned int l3_type, cs_vtable_hash_entry_t *hash, 
		__u16 index, __u32 *ip_addr, bool is_v6)
{
	cs_vtable_rslt_lookup_entry_t *new_lookup;
	int ret;

	if (l3_type >= CS_VTABLE_L3_RSLT_TYPE_MAX) return -1;
	if ((hash == NULL) || (ip_addr == NULL)) return -1;
	if (hash->l3_result[l3_type] != NULL) return -1;

	if (is_v6 == true)
		ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_IPV6, 
				(void *)ip_addr, &new_lookup);
	else
		ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_IPV4, 
				(void *)ip_addr, &new_lookup);
	if (ret != 0) return ret;

	new_lookup->hw_index = index;
	hash->l3_result[l3_type] = new_lookup;

	return 0;
} /* cs_vhash_set_l3_result */

int cs_vhash_set_l4_result(unsigned int l4_type, cs_vtable_hash_entry_t *hash, 
		__u16 port)
{
	cs_vtable_rslt_lookup_entry_t *new_lookup;
	int ret;

	if (l4_type >= CS_VTABLE_L4_RSLT_TYPE_MAX) return -1;
	if (hash == NULL) return -1;
	if (hash->l4_result[l4_type] != NULL) return -1;

	ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_PORT, (void *)&port, 
			&new_lookup);
	if (ret != 0) return ret;

	hash->l4_result[l4_type] = new_lookup;

	return 0;
} /* cs_vhash_set_l4_result */

int cs_vhash_set_vid_result(unsigned int vlan_type, 
		cs_vtable_hash_entry_t *hash, __u16 flow_vlan_index, __u16 vid)
{
	cs_vtable_rslt_lookup_entry_t *new_lookup;
	int ret;

	if (vlan_type >= CS_VTABLE_VLAN_RSLT_TYPE_MAX) return -1;
	if (hash == NULL) return -1;
	if (hash->vlan_result[vlan_type] != NULL) return -1;

	ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_VID, (void *)&vid, 
			&new_lookup);
	if (ret != 0) return ret;

	new_lookup->hw_index = flow_vlan_index;
	hash->vlan_result[vlan_type] = new_lookup;

	return 0;
} /* cs_vhash_set_vid_result */

int cs_vhash_set_swid_result(cs_vtable_hash_entry_t *hash, __u64 swid, 
		cs_vtable_rslt_lookup_entry_t **new_lookup)
{
	int ret;
	__u32 swid_array[2];

	if (hash == NULL) return -1;

	swid_array[0] = swid & 0x00000000ffffffff;
	swid_array[1] = swid >> 32;
	ret = cs_vtable_rslt_add_entry(CS_VTABLE_RSLT_TYPE_SWID64, 
			(void *)swid_array, new_lookup);
	return ret;
} /* cs_vhash_set_swid_result */
#endif

int cs_vhash_set_state(cs_vtable_hash_entry_t *hash, __u8 state)
{
	unsigned int link_tbl_idx;

	if (hash == NULL) return -1;
	if (state >= CS_VHASH_STATE_MAX) return -1;

	/* update the linking table entry:
	 * 1) get the table index from hash->hw_index;
	 * 2) act based on the state this hash is changing to */
	link_tbl_idx = hw_index_to_link_tbl_idx(hash->hw_index);
#if 0
	printk("%s::vhash %p, hw index %x, tbl_idx %x, state %d, use %x\n",
		__func__, hash, hash->hw_index, link_tbl_idx, state,
		atomic_read(&hash->gc_users));
#endif
	if (state == CS_VHASH_STATE_ACTIVE)
		hash_link_table[link_tbl_idx] = hash;
	else
		hash_link_table[link_tbl_idx] = NULL;

	hash->state = state;
	return 0;
} /* cs_vhash_set_state */

#if 0
int cs_vhash_invalid_fwd_entry(cs_vtable_hash_entry_t *hash)
{
	cs_vtable_hash_qos_entry_t *curr_qos;
	if (hash == NULL) return -1;

	cs_vhash_set_state(hash, CS_VHASH_STATE_INVALID);
	cs_fe_hash_del_hash(hash->hw_index);

	/* also invalidate all the qos hash related to this fwd hash */
	curr_qos = hash->qos_entry;
	while (curr_qos != NULL) {
		cs_vhash_invalid_qos_entry(curr_qos);
		curr_qos = curr_qos->next_qos;
	};

	return 0;
} /* cs_vhash_invalid_fwd_entry */

int cs_vhash_del_fwd_entry(cs_vtable_hash_entry_t *hash)
{
	cs_vtable_hash_qos_entry_t *curr_qos, *next_qos;

	if (hash == NULL) return -1;

	if (hash->state == CS_VHASH_STATE_ACTIVE)
		cs_vhash_invalid_fwd_entry(hash);

	if (hash->result_index != 0)
		cs_fe_fwdrslt_del_by_idx(hash->result_index);

	/* also remove all the qos hash related to this fwd hash */
	curr_qos = hash->qos_entry;
	while (curr_qos != NULL) {
		next_qos = curr_qos->next_qos;
		cs_vhash_del_qos_entry(curr_qos);
		kfree(curr_qos);
		curr_qos = next_qos;
	};

	cs_vtable_remove_hash_from_list(hash);

	/* note!! removing fwd_entry does not require to clean up 
	 * rslt_lookup_entry, because that should've been taken care by garbage 
	 * collector. */

	/* another note! we do not free the memory here. we will have the caller 
	 * do so. */

	return 0;
} /* cs_vhash_del_fwd_entry */

int cs_vhash_add_qos_entry(cs_vtable_hash_entry_t *fwd_hash, 
		cs_vtable_hash_qos_entry_t *qos_hash)
{
	cs_vtable_hash_qos_entry_t *prev_qos_list;

	if ((fwd_hash == NULL) || (qos_hash == NULL)) return -1;

	if (fwd_hash->qos_entry == NULL) {
		fwd_hash->qos_entry = qos_hash;
	} else {
		prev_qos_list = fwd_hash->qos_entry;
		while (prev_qos_list->next_qos != NULL) {
			prev_qos_list = prev_qos_list->next_qos;
		};
		prev_qos_list->next_qos = qos_hash;
	}
	qos_hash->fwd_hash = fwd_hash;
	return 0;
} /* cs_vhash_add_qos_entry */

int cs_vhash_invalid_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash)
{
	if (qos_hash == NULL) return -1;

	cs_fe_hash_del_hash(qos_hash->hw_index);
	qos_hash->state = CS_VHASH_STATE_INVALID;

	return 0;
} /* cs_vhash_invalid_qos_entry */

int cs_vhash_del_qos_entry(cs_vtable_hash_qos_entry_t *qos_hash)
{
	if (qos_hash == NULL) return -1;

	if (qos_hash->state == CS_VHASH_STATE_ACTIVE)
		cs_vhash_invalid_qos_entry(qos_hash);

	if (qos_hash->result_index != 0)
		cs_fe_table_del_entry_by_idx(FE_TABLE_QOSRSLT, qos_hash->result_index, 
				false);

	cs_vtable_remove_qos_hash_from_list(qos_hash);

	/* note: we do not free the memory here. we will have the caller do so */

	return 0;
} /* cs_vhash_del_qos_entry */

int cs_vhash_remove_qos_from_fwd(cs_vtable_hash_qos_entry_t *qos_hash, 
		cs_vtable_hash_entry_t *fwd_hash)
{
	cs_vtable_hash_qos_entry_t *prev, *curr;

	if ((qos_hash == NULL) || (fwd_hash == NULL)) return -1;

	prev = NULL;
	curr = fwd_hash->qos_entry;
	while (curr != NULL) {
		if (curr == qos_hash) {
			if (prev == NULL)
				fwd_hash->qos_entry = curr->next_qos;
			else
				prev->next_qos = curr->next_qos;
			curr->next_qos = NULL;
		}
		prev = curr;
		curr = curr->next_qos;
	};
	return 0;
} /* cs_vhash_remove_qos_from_fwd */

int cs_vhash_remove_qos_from_fwd_by_crc32(cs_vtable_hash_entry_t *fwd_hash, 
		__u32 qos_crc32)
{
	cs_vtable_hash_qos_entry_t *curr_qos, *prev_qos;

	if (fwd_hash == NULL) return -1;
	if (fwd_hash->qos_entry == NULL) return -1;

	/* this loop will remove all the QoS hash entry in the given fwd hash entry 
	 * with matching qos_crc32! */
	prev_qos = NULL;
	curr_qos = fwd_hash->qos_entry;
	while (curr_qos != NULL) {
		if (curr_qos->crc32 == qos_crc32) {
			if (prev_qos == NULL)
				fwd_hash->qos_entry = curr_qos->next_qos;
			else
				prev_qos->next_qos = curr_qos->next_qos;
			cs_vhash_del_qos_entry(curr_qos);
			kfree(curr_qos);
		}
	};

	return 0;
} /* cs_vhash_remove_qos_from_fwd_by_crc32 */

bool cs_vhash_compare(cs_vtable_hash_entry_t *hash_a, 
		cs_vtable_hash_entry_t *hash_b)
{
	if (hash_a->crc32 != hash_b->crc32) return false;
	if (hash_a->crc16 != hash_b->crc16) return false;
	if (hash_a->result_index != hash_b->result_index) return false;
	if (hash_a->mask_ptr != hash_b->mask_ptr) return false;
	if (hash_a->hw_index != hash_b->hw_index) return false;

	return true;
} /* cs_vhash_compare */

int cs_vhash_update_last_use(__u16 hw_idx)
{
	unsigned int link_tbl_idx = hw_index_to_link_tbl_idx(hw_idx);
	cs_vtable_hash_entry_t *vhash;
	int i;
	unsigned long curr_time = jiffies;

//	printk("%s::hw_idx %x\n", __func__, hw_idx);
	if (hash_link_table[link_tbl_idx] == NULL) return -1;

	vhash = hash_link_table[link_tbl_idx];
	vhash->last_use = curr_time;

	/* also update the GC that this hash entry is pointed to */
	for (i = 0; i < CS_VTABLE_L2_RSLT_TYPE_MAX; i++) {
		if (vhash->l2_result[i] != NULL)
			cs_gc_entry_update_timer(vhash->l2_result[i]->gc_entry, curr_time);
	}

	for (i = 0; i < CS_VTABLE_L3_RSLT_TYPE_MAX; i++) {
		if (vhash->l3_result[i] != NULL)
			cs_gc_entry_update_timer(vhash->l3_result[i]->gc_entry, curr_time);
	}

	for (i = 0; i < CS_VTABLE_L4_RSLT_TYPE_MAX; i++) {
		if (vhash->l4_result[i] != NULL)
			cs_gc_entry_update_timer(vhash->l4_result[i]->gc_entry, curr_time);
	}

	for (i = 0; i < CS_VTABLE_VLAN_RSLT_TYPE_MAX; i++) {
		if (vhash->vlan_result[i] != NULL)
			cs_gc_entry_update_timer(vhash->vlan_result[i]->gc_entry, curr_time);
	}

	return 0;
} /* cs_vhash_update_last_use */

int cs_vhash_update_not_use(__u16 hw_idx)
{
	unsigned int link_tbl_idx = hw_index_to_link_tbl_idx(hw_idx);
	cs_vtable_hash_entry_t *vhash;

//	printk("%s::hw_idx %x\n", __func__, hw_idx);
	if (hash_link_table[link_tbl_idx] == NULL) return -1;

	vhash = hash_link_table[link_tbl_idx];

	/* check if the vhash is expired or not, if so, invalidate it!! */
	if ((vhash->lifetime != 0) && (!time_in_range(jiffies, vhash->last_use, 
					(vhash->last_use + vhash->lifetime * HZ / 1000 )))) {
					//(vhash->last_use + vhash->lifetime * HZ))))
		printk("%s::invalid hash entry crc32 %x, crc16 %x\n",
			__func__, vhash->crc32, vhash->crc16);
		cs_vhash_invalid_fwd_entry(vhash);
	}

	return 0;
} /* cs_vhash_update_not_use */
#endif
int cs_vhash_init(void)
{
	memset(hash_link_table, 0x0, (FE_HASH_STATUS_ENTRY_MAX << 8));

	return 0;
} /* cs_vhash_init */
