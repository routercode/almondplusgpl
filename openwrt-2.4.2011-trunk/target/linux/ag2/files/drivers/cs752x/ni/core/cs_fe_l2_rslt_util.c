/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_fe_l2_rslt_util.c
 *
 * $Id: cs_fe_l2_rslt_util.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * It contains the assistance API for L2 MAC result table.  The
 * following implementation perform crc16 on the value given for table
 * search efficiency.
 */
#include <linux/module.h>
#include <linux/spinlock.h>
#include "cs_fe.h"
#include "crc.h"
#include "cs_fe_util_api.h"

#define FE_L2_ADDR_LOOKUP_ENTRY_MAX	4096

static fe_rslt_lookup_entry_t *fe_l2_rslt_sa_lookup_base[FE_L2_ADDR_LOOKUP_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_da_lookup_base[FE_L2_ADDR_LOOKUP_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_pair_lookup_base[FE_L2_ADDR_LOOKUP_ENTRY_MAX];

#if 0
static fe_rslt_lookup_entry_t *fe_l2_rslt_sa_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_da_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_pair_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];
#endif

static spinlock_t fe_l2_rslt_sa_lookup_table_lock;
static spinlock_t fe_l2_rslt_da_lookup_table_lock;
static spinlock_t fe_l2_rslt_pair_lookup_table_lock;

static unsigned int fe_l2_rslt_lookup_and_entry_map_tbl[FE_L2_ADDR_PAIR_ENTRY_MAX];

// this is l2 result sw lookup index and hw entry index mapping table
//     15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//   |                        |  type   |              sw lookup index               |
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
// type: 00 - fe_l2_rslt_pair_lookup_base
//       01 - fe_l2_rslt_da_lookup_base
//       10 - fe_l2_rslt_sa_lookup_base
#define L2_MAP_TBL_TYPE_MASK        0x0600
#define L2_MAP_TBL_TYPE_PAIR        0x0000
#define L2_MAP_TBL_TYPE_DA          0x0200
#define L2_MAP_TBL_TYPE_SA          0x0400

#if 1
int cs_fe_l2_result_dealloc(u8 da_en, u8 sa_en, u16 l2_idx)
{
	return cs_fe_table_del_l2_mac(l2_idx, sa_en, da_en);
}
#endif

int cs_fe_l2_add_sw_only(u8 type, u8 *mac_addr, void *hash)
{
	fe_rslt_lookup_entry_t **table, *entry, *prev;
	// do we need table_lock here? as the caller holds lock of pair table.
	// spinlock_t *table_lock;
	int i, status = FE_TABLE_OK;
	u16 crc16 = 0;
	if (type == L2_LOOKUP_TYPE_SA)
		table = fe_l2_rslt_sa_lookup_base;
	else
		table = fe_l2_rslt_da_lookup_base;

	for (i=0; i<6; i++)
		crc16 = update_crc_ccitt(crc16, *(mac_addr + i));

	entry = table[crc16 >> 4];
	prev = NULL;

	for (;;) {
		if (entry == NULL) {
			entry = kzalloc(sizeof(fe_rslt_lookup_entry_t), GFP_ATOMIC);
			if (entry == NULL) {
				status = FE_TABLE_ENOMEM;
				break;
			}
			if (prev == NULL)
				table[crc16 >> 4] = entry;
			else
				prev->next = entry;

		}
		if (entry->valid == 0) {
			entry->valid = 1;
			status = cs_gc_table_alloc_entry(FE_GC_TYPE_MAC,
					&entry->gc_entry);
			if (status != FE_TABLE_OK) {
				entry->valid = 0;
				break;
			}
			status = cs_gc_entry_add_hash(entry->gc_entry->data,
					hash);
			if (status != FE_TABLE_OK) {
				cs_gc_table_del_entry(FE_GC_TYPE_MAC,
					entry->gc_entry);
				entry->valid = 0;
				break;
			}
			cs_gc_entry_set_rslt_lookup_entry(entry->gc_entry->data,
					entry);
			memcpy(entry->mac.mac_addr, mac_addr, 6);
			atomic_set(&entry->users, 1);
			break;
		}

		if (memcmp(entry->mac.mac_addr, mac_addr, 6) != 0) {
			prev = entry;
			entry = entry->next;
		} else {
			status = cs_gc_entry_add_hash(entry->gc_entry->data,
					hash);
			if (status != FE_TABLE_OK)
				break;
			atomic_inc(&entry->users);
			break;
		}
	}
	return status;
}

/*
 * This function will check existing entry for given MAC (DA/SA/Pair) address.
 * If found, hw index (result table index) will be returned, vhash will be
 * added into corresponding gc_entry (MAC SA and DA). If not, then try to
 * allocate a new lookup entry etc.
 *
 * For mac pair, hw_index still refers to the hw table index. gc_entry is not
 * used. mac sa and mac da table entries will be used, with reference to
 * gc_entry. mac_sa and mac_da entry's sw_only is set. There are cases that
 * multiple MAC pairs use same MAC SA or MAC DA. Then we only keep one sw_only
 * mac_sa or mac_da entry for different pairs.
 */
int cs_fe_l2_result_alloc(unsigned char *mac_addr, unsigned char type,
	cs_vtable_hash_entry_t *vhash, u16 *return_idx)
{
	u16 crc16 = 0, idx = 0;
	bool f_sa, f_da;
	int status = FE_TABLE_ENTRYNOTFOUND;
	unsigned long flags;
	fe_rslt_lookup_entry_t	*entry, *prev, **table;
	u32 l2_idx = 0;
	unsigned int length = 0, i;
	unsigned char *mac_sa = NULL, *mac_da = NULL, *dest = NULL;
	spinlock_t *table_lock;

	switch (type) {
	case L2_LOOKUP_TYPE_PAIR:
		length = 12;
		table = fe_l2_rslt_pair_lookup_base;
		table_lock = &fe_l2_rslt_pair_lookup_table_lock;
		mac_da = mac_addr;
		mac_sa = mac_addr + 6;
		f_sa = f_da = true;
		break;
	case L2_LOOKUP_TYPE_SA:
		length = 6;
		table = fe_l2_rslt_sa_lookup_base;
		table_lock = &fe_l2_rslt_sa_lookup_table_lock;
		mac_sa = mac_addr;
		f_sa = true;
		f_da = false;
		break;
	case L2_LOOKUP_TYPE_DA:
		length = 6;
		table = fe_l2_rslt_da_lookup_base;
		table_lock = &fe_l2_rslt_da_lookup_table_lock;
		mac_da = mac_addr;
		f_da = true;
		f_sa = false;
		break;
	default:
		return FE_TABLE_EOPNOTSUPP;
	};

	for (i=0; i<length; i++) {
		crc16 = update_crc_ccitt(crc16, *(mac_addr + i));
	}

	idx = crc16 >> 4;
	entry = table[idx];
	prev = NULL;

	spin_lock_irqsave(table_lock, flags);
	for (;;) {
		if (entry == NULL) {
			/* result lookup entry is not freed until the entire
			 * table is cleaned. */
			entry = kzalloc(sizeof(fe_rslt_lookup_entry_t),	GFP_ATOMIC);
			if (entry == NULL) {
				status = FE_TABLE_ENOMEM;
				break;
			}
			if (prev == NULL)
				table[idx] = entry;
			else
				prev->next = entry;
		}

		if (type == L2_LOOKUP_TYPE_PAIR)
			dest = entry->mac_pair.mac_addr;
		else
			dest = entry->mac.mac_addr;

		if (entry->valid == 0) {
			entry->valid = 1;
			if (type != L2_LOOKUP_TYPE_PAIR) {
				status = cs_gc_table_alloc_entry(FE_GC_TYPE_MAC,
						&entry->gc_entry);
				if (status != FE_TABLE_OK) {
					entry->valid = 0;
					break;
				}
				status = cs_fe_table_add_l2_mac(mac_sa, mac_da,
						&l2_idx);
				entry->hw_index = l2_idx;
				if (status != FE_TABLE_OK) {
					cs_gc_table_del_entry(FE_GC_TYPE_MAC,
						entry->gc_entry);
					entry->valid = 0;
					break;
				}
				status = cs_gc_entry_add_hash(entry->gc_entry->data,
						vhash);
				if (status != FE_TABLE_OK) {
					status = cs_fe_table_del_l2_mac(
						entry->hw_index, f_sa, f_da);
					cs_gc_table_del_entry(FE_GC_TYPE_MAC,
						entry->gc_entry);
					entry->valid = 0;
					break;
				}
				cs_gc_entry_set_rslt_lookup_entry(entry->gc_entry->data,
						entry);
				memcpy(entry->mac.mac_addr, mac_addr, 6);
			} else {
				status = cs_fe_table_add_l2_mac(mac_sa, mac_da,
						&l2_idx);
				entry->hw_index = l2_idx;
				if (status != FE_TABLE_OK) {
					entry->valid = 0;
					break;
				}
				status = cs_fe_l2_add_sw_only(L2_LOOKUP_TYPE_SA,
					mac_sa, vhash);
				if (status != FE_TABLE_OK) {
					entry->valid = 0;
					cs_fe_table_del_l2_mac(entry->hw_index,
						f_sa, f_da);
					break;
				}
				status = cs_fe_l2_add_sw_only(L2_LOOKUP_TYPE_DA,
					mac_da, vhash);
				if (status != FE_TABLE_OK) {
					entry->valid = 0;
					cs_fe_table_del_l2_mac(entry->hw_index,
						f_sa, f_da);
					break;
				}
				memcpy(entry->mac_pair.mac_addr, mac_addr, 12);
			}
			atomic_set(&entry->users, 1);
			break;
		}

		if (memcmp(dest, mac_addr, length) != 0) {
			// not found
			prev = entry;
			entry = entry->next;
		} else {
			if (type == L2_LOOKUP_TYPE_PAIR) {
				status = cs_fe_l2_add_sw_only(L2_LOOKUP_TYPE_SA,
					mac_sa, vhash);
				if (status != FE_TABLE_OK) {
					printk("%s::SA sw only table fail!\n",
						__func__);
					break;
				}
				status = cs_fe_l2_add_sw_only(L2_LOOKUP_TYPE_DA,
					mac_da, vhash);
				if (status != FE_TABLE_OK) {
					printk("%s::DA sw only table fail!\n",
						__func__);
					break;
				}
			} else {
				status = cs_gc_entry_add_hash(entry->gc_entry->data,
						vhash);
				if (status != FE_TABLE_OK) {
					break;
				}
			}
			atomic_inc(&entry->users);
			*return_idx = entry->hw_index;
			break;
		}
	}
	spin_unlock_irqrestore(table_lock, flags);
	return status;
}


#if 0
int cs_fe_l2_result_alloc(unsigned char *mac_addr, unsigned char type,
		unsigned int *return_idx)
{
	unsigned int crc16 = 0, idx = 0, l2_idx, maptbl_type = 0;
	int status = FE_TABLE_ENTRYNOTFOUND;
	int i, length;
	unsigned char mac_combo[12], *mac_sa, *mac_da;
	spinlock_t *table_lock;
	fe_rslt_lookup_entry_t **table;
	fe_rslt_lookup_entry_t *entry;
	bool add_new_l2_rslt = false;
	unsigned long flags;

	memset(mac_combo, 0, sizeof(mac_combo));

	switch (type) {
	case L2_LOOKUP_TYPE_PAIR:
		length = 12;
		table = fe_l2_rslt_pair_lookup_base;
		table_lock = &fe_l2_rslt_pair_lookup_table_lock;
		memcpy(mac_combo, mac_addr, length);
		maptbl_type = L2_MAP_TBL_TYPE_PAIR;
		mac_sa = mac_addr + 6;
		mac_da = mac_addr;
		break;
	case L2_LOOKUP_TYPE_SA:
		length = 6;
		table = fe_l2_rslt_sa_lookup_base;
		table_lock = &fe_l2_rslt_sa_lookup_table_lock;
		memcpy(mac_combo, mac_addr, length);
		maptbl_type = L2_MAP_TBL_TYPE_SA;
		mac_sa = mac_addr;
		mac_da = NULL;
		break;
	case L2_LOOKUP_TYPE_DA:
		length = 6;
		table = fe_l2_rslt_da_lookup_base;
		table_lock = &fe_l2_rslt_da_lookup_table_lock;
		memcpy(mac_combo, mac_addr, length);
		maptbl_type = L2_MAP_TBL_TYPE_DA;
		mac_sa = NULL;
		mac_da = mac_addr;
		break;
	default:
		return FE_TABLE_EOPNOTSUPP;
	}

	for (i = 0; i < length; i++) {
		crc16 = update_crc_ccitt(crc16, *(mac_addr+i));
	}

	spin_lock_irqsave(table_lock, flags);
	idx = crc16 & (FE_L2_ADDR_PAIR_ENTRY_MAX - 1);
	entry = table[idx];

	if (entry == NULL) {
		entry = kzalloc(sizeof(fe_rslt_lookup_entry_t), GFP_ATOMIC);
		if (entry == NULL) {
			status = FE_TABLE_ENOMEM;
			goto exit;
		}
		table[idx] = entry;

		add_new_l2_rslt = true;
	}

	if ((atomic_read(&entry->users) == 0) || (add_new_l2_rslt == true)) {
		status = cs_fe_table_add_l2_mac(mac_sa, mac_da, &l2_idx);

		if (status != FE_TABLE_OK)
			goto exit;

		/* keep mac_da, mac_sa, type and index */
		entry->crc16 = crc16;
		entry->rslt_index = l2_idx;
		entry->l2.type = type;
		memcpy(entry->l2.mac_addr, mac_combo, length);
		/* set mapping table */
		fe_l2_rslt_lookup_and_entry_map_tbl[l2_idx] = maptbl_type | idx;
		atomic_set(&entry->users, 1);
		*return_idx = l2_idx;
		status = FE_TABLE_OK;
		goto exit;
	}

	if ((entry->l2.type != type) || (memcmp(&(entry->l2.mac_addr),
					&mac_combo, length) != 0)) {
		status = FE_TABLE_ENTRYNOTFOUND;
		goto exit;
	}

	*return_idx = entry->rslt_index;
	atomic_inc(&entry->users);
	status = FE_TABLE_OK;

exit:
	spin_unlock_irqrestore(table_lock, flags);
	return status;
}/* cs_fe_l2_result_alloc */


int cs_fe_l2_result_dealloc(unsigned int l2_idx)
{
	unsigned int crc_idx = 0, maptbl_type = 0;
	int status = FE_TABLE_ENTRYNOTFOUND;
	int length;
	spinlock_t *table_lock;
	fe_rslt_lookup_entry_t **table;
	fe_rslt_lookup_entry_t *entry = NULL;
	bool f_sa, f_da;
	unsigned long flags;

	crc_idx = fe_l2_rslt_lookup_and_entry_map_tbl[l2_idx] &
		(FE_L2_ADDR_PAIR_ENTRY_MAX - 1);
	maptbl_type = fe_l2_rslt_lookup_and_entry_map_tbl[l2_idx] &
		L2_MAP_TBL_TYPE_MASK;

	switch (maptbl_type) {
	case L2_MAP_TBL_TYPE_PAIR:
		length = 12;
		table = fe_l2_rslt_pair_lookup_base;
		table_lock = &fe_l2_rslt_pair_lookup_table_lock;
		f_sa = f_da = true;
		break;
	case L2_MAP_TBL_TYPE_SA:
		length = 6;
		table = fe_l2_rslt_sa_lookup_base;
		table_lock = &fe_l2_rslt_sa_lookup_table_lock;
		f_sa = true;
		f_da = false;
		break;
	case L2_MAP_TBL_TYPE_DA:
		length = 6;
		table = fe_l2_rslt_da_lookup_base;
		table_lock = &fe_l2_rslt_da_lookup_table_lock;
		f_sa = false;
		f_da = true;
		break;
	default:
		return FE_TABLE_EOPNOTSUPP;
	}

	spin_lock_irqsave(table_lock, flags);
	entry = table[crc_idx];

	if (entry == NULL)
		goto exit;

	if (atomic_dec_and_test(&entry->users)) {
		status = cs_fe_table_del_l2_mac(l2_idx, f_sa, f_da);
		if (status == FE_TABLE_OK) {
			fe_l2_rslt_lookup_and_entry_map_tbl[l2_idx] = 0;
			kfree(entry);
			table[crc_idx] = NULL;
		}
    }

exit:
	spin_unlock_irqrestore(table_lock, flags);
    return status;
}/* cs_fe_l2_result_dealloc */
#endif

int fe_l2_rslt_lookup_table_init(void)
{
	spin_lock_init(&fe_l2_rslt_sa_lookup_table_lock);
	spin_lock_init(&fe_l2_rslt_da_lookup_table_lock);
	spin_lock_init(&fe_l2_rslt_pair_lookup_table_lock);

	memset(fe_l2_rslt_lookup_and_entry_map_tbl, 0,
			FE_L2_ADDR_PAIR_ENTRY_MAX);

	return FE_STATUS_OK;
} /* fe_l2_rslt_lookup_table_init */
EXPORT_SYMBOL(fe_l2_rslt_lookup_table_init);

/* FIXME!! any release implementation is needed? */

//cs_status fe_rslt_lookup_table_release(fe_rslt_lookup_bucket_s* table_base[],
//	cs_uint16 size)
//{
//	int i;
//	for (i=0; i<size; i++) {
//		if(table_base[i])
//			kfree(table_base[i]);
//	}
//	return FE_STATUS_OK;
//}
//
//cs_status cs_fe_l2_result_dealloc_all(void)
//{
//	spin_lock(&fe_l2_rslt_sa_lookup_table_lock);
//	fe_rslt_lookup_table_release(fe_l2_rslt_sa_lookup_base, FE_L2_ADDR_PAIR_ENTRY_MAX);
//	spin_unlock(&fe_l2_rslt_sa_lookup_table_lock);
//
//	spin_lock(&fe_l2_rslt_da_lookup_table_lock);
//	fe_rslt_lookup_table_release(fe_l2_rslt_da_lookup_base, FE_L2_ADDR_PAIR_ENTRY_MAX);
//	spin_unlock(&fe_l2_rslt_da_lookup_table_lock);
//
//	spin_lock(&fe_l2_rslt_pair_lookup_table_lock);
//	fe_rslt_lookup_table_release(fe_l2_rslt_pair_lookup_base, FE_L2_ADDR_PAIR_ENTRY_MAX);
//	spin_unlock(&fe_l2_rslt_pair_lookup_table_lock);
//
//	return FE_STATUS_OK;
//}/* cs_fe_l2_result_dealloc_all */


