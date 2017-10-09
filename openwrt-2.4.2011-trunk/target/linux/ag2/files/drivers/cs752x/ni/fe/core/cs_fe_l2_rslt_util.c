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

static fe_rslt_lookup_entry_t *fe_l2_rslt_sa_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_da_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];
static fe_rslt_lookup_entry_t *fe_l2_rslt_pair_lookup_base[FE_L2_ADDR_PAIR_ENTRY_MAX];

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


