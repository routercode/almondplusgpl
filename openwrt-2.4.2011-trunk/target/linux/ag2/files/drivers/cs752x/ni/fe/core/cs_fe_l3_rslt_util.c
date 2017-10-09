/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_fe_l3_rslt_util.c
 *
 * $Id: cs_fe_l3_rslt_util.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * It contains the assistance API for L3 IP result table.  The following
 * implementation perform crc16 on the value given for table search
 * efficiency.
 */
#include <linux/module.h>
#include <linux/spinlock.h>
#include "cs_fe.h"
#include "crc.h"

static fe_rslt_lookup_entry_t *fe_l3_rslt_lookup_table_base[FE_L3_ADDR_ENTRY_MAX << 2];
static spinlock_t fe_l3_rslt_lookup_table_lock;
// this is l3 result sw lookup index and hw entry index mapping table
//     15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//   | v6 |              |                      sw lookup index                      |
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
// v6: 0 - ipv4
//     1 - ipv6
#define L3_MAP_TBL_IP_MASK          0x8000
#define L3_MAP_TBL_IP_V4            0x0000
#define L3_MAP_TBL_IP_V6            0x8000
static __u16 fe_l3_rslt_lookup_and_entry_map_tbl[FE_L3_ADDR_ENTRY_MAX << 2];

int cs_fe_l3_result_alloc(__u32 *ip_addr, bool is_v6, unsigned int *return_idx)
{
	int status = FE_TABLE_ENTRYNOTFOUND;
	__u8 *data = (__u8*)ip_addr;
	unsigned int crc16 = 0, crc_idx, l3_idx, maptbl_isv6 = 0;
	__u32 ip_addr_tmp[4];
	int i = 0;
	fe_rslt_lookup_entry_t *entry;
	bool add_new_l3_rslt = false;
	unsigned long flags;

	memset(ip_addr_tmp, 0, sizeof(ip_addr_tmp));

	for (i = 0; i < 4; i++) {
		crc16 = update_crc_ccitt(crc16, *data);
		data++;
	}
	crc_idx = crc16 >> 4;
	ip_addr_tmp[0] = *ip_addr;

	if (is_v6 == true) {
		for (i = 0; i < 12; i++) {
			crc16 = update_crc_ccitt(crc16, *data);
			data++;
		}
		crc_idx = crc16 >> 4;
		maptbl_isv6 = L3_MAP_TBL_IP_V6;
		memcpy(ip_addr_tmp, ip_addr, sizeof(ip_addr_tmp));
	}

	spin_lock_irqsave(&fe_l3_rslt_lookup_table_lock, flags);
	entry = fe_l3_rslt_lookup_table_base[crc_idx];

	if (entry == NULL) {
		entry = kzalloc(sizeof(fe_rslt_lookup_entry_t), GFP_ATOMIC);
		if (entry == NULL) {
			status = FE_TABLE_ENOMEM;
			goto exit;
		}
		fe_l3_rslt_lookup_table_base[crc_idx] = entry;

		add_new_l3_rslt = true;
	}

	if ((atomic_read(&entry->users) == 0) || (add_new_l3_rslt == true)) {
		status = cs_fe_table_add_l3_ip(ip_addr, &l3_idx, is_v6);
		if (status != FE_TABLE_OK) goto exit;

		if (is_v6 == true)
			memcpy(entry->l3.ip_addr, ip_addr_tmp,
					sizeof(entry->l3.ip_addr));
		else
			entry->l3.ip_addr[0] = ip_addr_tmp[0];

		/* keep index and is_v6 */
		entry->crc16 = crc16;
		entry->rslt_index = l3_idx;
		entry->l3.is_v6 = is_v6;
		/* set mapping table */
		fe_l3_rslt_lookup_and_entry_map_tbl[l3_idx] =
			maptbl_isv6 | crc_idx;
		atomic_set(&entry->users, 1);
		*return_idx = l3_idx;
		status = FE_TABLE_OK;
		goto exit;
	}

	if ((entry->l3.is_v6 != is_v6) || (0 != memcmp (&(entry->l3.ip_addr),
			&ip_addr_tmp, sizeof(ip_addr_tmp)))) {
		status = FE_TABLE_ENTRYNOTFOUND;
		goto exit;
	}

	*return_idx = entry->rslt_index;
	atomic_inc(&entry->users);
	status = FE_TABLE_OK;

exit:
	spin_unlock_irqrestore(&fe_l3_rslt_lookup_table_lock, flags);
	return status;
}/* cs_fe_l3_result_alloc */

int cs_fe_l3_result_dealloc(__u16 l3_idx)
{
	unsigned int crc_idx = 0, maptbl_isv6 = 0;
	int  status = FE_TABLE_ENTRYNOTFOUND;
	fe_rslt_lookup_entry_t *entry = NULL;
	unsigned long flags;

	crc_idx = fe_l3_rslt_lookup_and_entry_map_tbl[l3_idx] & 0x0FFF;
	maptbl_isv6 = fe_l3_rslt_lookup_and_entry_map_tbl[l3_idx] &
		L3_MAP_TBL_IP_MASK;

	spin_lock_irqsave(&fe_l3_rslt_lookup_table_lock, flags);
	entry = fe_l3_rslt_lookup_table_base[crc_idx];
	if (entry == NULL) goto exit;

	if (atomic_dec_and_test(&entry->users)) {
		switch (maptbl_isv6) {
		case L3_MAP_TBL_IP_V4:
			status = cs_fe_table_del_l3_ip(l3_idx, false);
			if (status == FE_TABLE_OK) {
				fe_l3_rslt_lookup_and_entry_map_tbl[l3_idx] = 0;
				kfree(entry);
				fe_l3_rslt_lookup_table_base[crc_idx] = NULL;
			}
			break;
		case L3_MAP_TBL_IP_V6:
			status = cs_fe_table_del_l3_ip(l3_idx, true);
			if (status == FE_TABLE_OK) {
				fe_l3_rslt_lookup_and_entry_map_tbl[l3_idx] = 0;
				kfree(entry);
				fe_l3_rslt_lookup_table_base[crc_idx] = NULL;
			}
			break;
		}
	}

exit:
	spin_unlock_irqrestore(&fe_l3_rslt_lookup_table_lock, flags);
	return status;
}/* cs_fe_l3_result_dealloc */

int fe_l3_rslt_lookup_table_init(void)
{
	spin_lock_init(&fe_l3_rslt_lookup_table_lock);
	return FE_TABLE_OK;
} /* fe_l3_rslt_lookup_table_init */
EXPORT_SYMBOL(fe_l3_rslt_lookup_table_init);

//int cs_fe_l3_result_dealloc_all(void)
//{
//	int i;
//	spin_lock(&L3_IP_LOCK);
//	for (i=0; i<4096; i++) {
//		if (fe_l3_rslt_lookup_table_base[i])
//			kfree(fe_l3_rslt_lookup_table_base[i]);
//	}
//	spin_unlock(&L3_IP_LOCK);
//	return FE_STATUS_OK;
//}/* cs_fe_l3_result_dealloc_all */

