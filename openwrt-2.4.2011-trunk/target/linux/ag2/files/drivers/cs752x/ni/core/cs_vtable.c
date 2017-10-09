/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_vtable.c
 *
 * $Id: cs_vtable.c,v 1.2 2011/08/25 23:04:11 peebles Exp $
 *
 * It contains the implementation of vtable, which is abstract entity that 
 * is used to maintain several FE entities at once.  Also, each vtable is 
 * used for a specific type of traffic.
 */

#include <linux/spinlock.h>
#include "cs_vtable.h"
#include "cs_fe.h"
#include "cs_hw_accel_cb.h"
#include "cs_fe_table_api.h"
#include "cs_fe_util_api.h"
#include <linux/if_ether.h>

#define MCGID_VTABLE_TO_HW(aaa)		(aaa)		// FIXME!! implement this
#define BCAST_DEF_CPU_VOQ		CPU_PORT0_VOQ_BASE
#define MCAST_DEF_CPU_VOQ		CPU_PORT0_VOQ_BASE
#define UU_DEF_CPU_VOQ			CPU_PORT0_VOQ_BASE
#define ARP_DEF_VOQ			CPU_PORT1_VOQ_BASE
#define BCAST_DEF_VTABLE_PRIORITY	41
#define L2_MCAST_DEF_VTABLE_PRIORITY	30
#define L3_MCAST_DEF_VTABLE_PRIORITY	38
#define L2_FORWARD_DEF_VTABLE_PRIORITY	10
#define L3_FORWARD_DEF_VTABLE_PRIORITY	20
#define RE_SPECIFIC_DEF_VTABLE_PRIORITY	43
#define ARP_DEF_VTABLE_PRIORITY		60

static cs_vtable_t* vtable_list[FE_VTABLE_TYPE_MAX];

/* Hash mask table, one application type per hm entry, fixed. */
static __u8 app_hm_table[64];
//static __u8 app_hm_table[FE_APP_TYPE_MAX];

int cs_vtable_init(void)
{
	int i, ret;
	cs_vtable_t *new_table;

	cs_vhash_init();

	for (i = 0; i < FE_VTABLE_TYPE_MAX; i++)
		vtable_list[i] = NULL;

	for (i = 0; i < FE_APP_TYPE_MAX; i++)
		app_hm_table[i] = 0;

	ret = cs_vtable_new(FE_VTABLE_TYPE_BCAST, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default Broadcast Vtable\n", __func__, 
				__LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_L2_MCAST, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default L2 MCAST Vtable\n", __func__, 
				__LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_L3_MCAST_V4, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default IPv4 L3 Mcast Vtable\n", 
				__func__, __LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_L3_MCAST_V6, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default IPv6 L3 Mcast Vtable\n", 
				__func__, __LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_L2_FORWARD, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default L2 Forward Vtable\n", __func__, 
				__LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_L3_FORWARD, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default L3 Forward Vtable\n", __func__, 
				__LINE__);

	ret = cs_vtable_new(FE_VTABLE_TYPE_ARP, &new_table);
	if (ret != 0)
		printk("%s:%d:failed to create default ARP Vtable\n", __func__, 
				__LINE__);
	//cxc
	cs_gc_init();
	return 0;
} /* cs_vtable_init */

/* hash mask related APIs (all internal APIs)*/
static int set_hash_mask_idx_to_app_type(unsigned int fwd_app_type, 
		__u8 hash_mask_idx)
{
	if ((app_hm_table[fwd_app_type] != 0) && 
			(app_hm_table[fwd_app_type] != hash_mask_idx)) {
		printk("%s:%d:a previous hash mask ", __func__, __LINE__);
		printk("%i has been set to this ", app_hm_table[fwd_app_type]);
		printk("fwd_app_type %i\n", fwd_app_type);
		return -1;
	}
	app_hm_table[fwd_app_type] = hash_mask_idx;
	printk("%s::add hash mask type %d, index %d\n",
		__func__, fwd_app_type, hash_mask_idx);
	return 0;
} /* set_hash_mask_idx_to_app_type */

int get_hash_mask_idx_by_app_type(unsigned int fwd_app_type, 
		__u8 *p_hash_mask_idx)
{
#if 0
	printk("%s::fwd_app_type %d, hm_table index %d\n",
		__func__, fwd_app_type, app_hm_table[fwd_app_type]);
#endif
	if (app_hm_table[fwd_app_type] == 0) {
		printk("%s:%d:no hash mask has been set ", __func__, __LINE__);
		printk("to this fwd_app_type %i\n", fwd_app_type);
		return -1;
	}
	*p_hash_mask_idx = app_hm_table[fwd_app_type];
	return 0;
} /* get_hash_mask_idx_by_app_type */


static int vtable_setup_def_flow(cs_vtable_t *p_vtbl, fe_sdb_entry_t *p_sdb)
{
	unsigned int fwdrslt_idx = 0, voqpol_idx = 0;
	int ret;
	fe_fwd_result_entry_t fwdrslt_entry;
	fe_voq_pol_entry_t voqpol_entry;

	memset(&fwdrslt_entry, 0, sizeof(fwdrslt_entry));
	memset(&voqpol_entry, 0, sizeof(voqpol_entry));

	switch (p_vtbl->vtable_type) {
	case FE_VTABLE_TYPE_BCAST:
		voqpol_entry.voq_base = BCAST_DEF_CPU_VOQ;
		ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry, 
				&voqpol_idx);
		if (ret != 0) return ret;

		fwdrslt_entry.dest.voq_pol_table_index = voqpol_idx;
		ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwdrslt_entry, 
				&fwdrslt_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER, voqpol_idx, 
					false);
			return ret;
		}
		p_sdb->misc.bc_flowidx = fwdrslt_idx;
		p_vtbl->bcflow_idx = fwdrslt_idx;
		break;
	case FE_VTABLE_TYPE_L2_MCAST:
		voqpol_entry.voq_base = MCAST_DEF_CPU_VOQ;
		ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry, 
				&voqpol_idx);
		if (ret != 0) return ret;

		fwdrslt_entry.dest.voq_pol_table_index = voqpol_idx;
		ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwdrslt_entry, 
				&fwdrslt_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER, voqpol_idx, 
					false);
			return ret;
		}
		p_sdb->misc.um_flowidx = fwdrslt_idx;
		p_vtbl->umflow_idx = fwdrslt_idx;
		break;
	case FE_VTABLE_TYPE_L3_MCAST_V4:
	case FE_VTABLE_TYPE_L3_MCAST_V6:
	case FE_VTABLE_TYPE_L2_FORWARD:
	case FE_VTABLE_TYPE_L3_FORWARD:
	case FE_VTABLE_TYPE_RE0:
	case FE_VTABLE_TYPE_RE1:
		voqpol_entry.voq_base = UU_DEF_CPU_VOQ;
		ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry, 
				&voqpol_idx);
		if (ret != 0) return ret;

		fwdrslt_entry.dest.voq_pol_table_index = voqpol_idx;
		ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwdrslt_entry,
				&fwdrslt_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER,
					voqpol_idx, false);
			return ret;
		}
		p_sdb->misc.uu_flowidx = fwdrslt_idx;
		p_vtbl->uuflow_idx = fwdrslt_idx;
		break;
	case FE_VTABLE_TYPE_ARP:
		voqpol_entry.voq_base = ARP_DEF_VOQ;
		ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry,
				&voqpol_idx);
		if (ret != 0) return ret;

		fwdrslt_entry.dest.voq_pol_table_index = voqpol_idx;
		ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwdrslt_entry, 
				&fwdrslt_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER,
					voqpol_idx, false);
			return ret;
		}
		p_sdb->misc.uu_flowidx = fwdrslt_idx;
		p_sdb->misc.um_flowidx = fwdrslt_idx;
		p_sdb->misc.bc_flowidx = fwdrslt_idx;
		p_vtbl->uuflow_idx = fwdrslt_idx;
		p_vtbl->umflow_idx = fwdrslt_idx;
		p_vtbl->bcflow_idx = fwdrslt_idx;
		break;
	default:
		break;
	}
	return 0;
} /* vtable_setup_def_flow */

static int vtable_setup_def_class(cs_vtable_t *p_vtbl, 
		fe_class_entry_t *p_class)
{
	fe_eth_type_entry_t etype_entry;
	unsigned int etype_idx;
	int ret;

	switch (p_vtbl->vtable_type) {
	case FE_VTABLE_TYPE_BCAST:
		p_class->rule_priority = BCAST_DEF_VTABLE_PRIORITY;
		p_class->l2.bcast_da = 1;
		p_class->l2.bcast_da_mask = 0;
		break;
	case FE_VTABLE_TYPE_L2_MCAST:
		p_class->rule_priority = L2_MCAST_DEF_VTABLE_PRIORITY;
		p_class->l2.mcast_da = 1;
		p_class->l2.mcast_da_mask = 0;
		break;
	case FE_VTABLE_TYPE_L3_MCAST_V4:
		p_class->rule_priority = L3_MCAST_DEF_VTABLE_PRIORITY;
		p_class->l3.da[0] = 0xe0000000;
		p_class->l3.da[1] = 0;
		p_class->l3.da[2] = 0;
		p_class->l3.da[3] = 0;
		p_class->l3.ip_da_mask = 0x04;
		p_class->l3.ip_valid = 1;
		p_class->l3.ip_valid_mask = 0;
		p_class->l3.ip_ver = 0;
		p_class->l3.ip_ver_mask = 0;
		break;
	case FE_VTABLE_TYPE_L3_MCAST_V6:
		p_class->rule_priority = L3_MCAST_DEF_VTABLE_PRIORITY;
		p_class->l3.da[0] = 0;
		p_class->l3.da[1] = 0;
		p_class->l3.da[2] = 0;
		p_class->l3.da[3] = 0xff000000;
		p_class->l3.ip_da_mask = 0x08;
		p_class->l3.ip_valid = 1;
		p_class->l3.ip_valid_mask = 0;
		p_class->l3.ip_ver = 1;
		p_class->l3.ip_ver_mask = 0;
		break;
	case FE_VTABLE_TYPE_L2_FORWARD:
		p_class->rule_priority = L2_FORWARD_DEF_VTABLE_PRIORITY;
		p_class->l2.da_an_mac_hit = 0;
		p_class->l2.da_an_mac_hit_mask = 0;
		p_class->l2.da_an_mac_sel = 0xf;
		p_class->l2.da_an_mac_sel_mask = 1;
		break;
	case FE_VTABLE_TYPE_L3_FORWARD:
		p_class->rule_priority = L3_FORWARD_DEF_VTABLE_PRIORITY;
		p_class->l2.da_an_mac_hit = 1;
		p_class->l2.da_an_mac_hit_mask = 0;
		p_class->l2.da_an_mac_sel = 0xf;
		p_class->l2.da_an_mac_sel_mask = 1;
		break;
	case FE_VTABLE_TYPE_RE0:
		p_class->rule_priority = RE_SPECIFIC_DEF_VTABLE_PRIORITY;
		p_class->port.lspid = ENCRYPTION_PORT;
		p_class->port.lspid_mask = 0;
		break;
	case FE_VTABLE_TYPE_RE1:
		p_class->rule_priority = RE_SPECIFIC_DEF_VTABLE_PRIORITY;
		p_class->port.lspid = ENCAPSULATION_PORT;
		p_class->port.lspid_mask = 0;
		break;
	case FE_VTABLE_TYPE_ARP:
		p_class->rule_priority = ARP_DEF_VTABLE_PRIORITY;
		memset(&etype_entry, 0, sizeof(etype_entry));
		etype_entry.ether_type = ETH_P_ARP;
		etype_entry.valid = 1;
		ret = cs_fe_table_add_entry(FE_TABLE_ETYPE, &etype_entry,
				&etype_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_ETYPE, etype_idx,
					false);
			return ret;
		}
		p_class->l2.ethertype_enc = etype_idx + 1;
		p_class->l2.ethertype_enc_mask = 0;
		break;
	default:
		break;
	}
	return 0;
} /* vtable_setup_def_class */

static int vtable_link_table(cs_vtable_t *p_dst_vtbl, cs_vtable_t *p_src_vtbl, 
		fe_class_entry_t *p_dst_class)
{
	unsigned int fwdrslt_idx = 0, voqpol_idx = 0;
	int ret;
	fe_fwd_result_entry_t fwdrslt_entry;
	fe_voq_pol_entry_t voqpol_entry;
	fe_sdb_entry_t src_sdb_entry;

	/* the following tables do not support VTABLE chaining */
	if ((p_src_vtbl->vtable_type == FE_VTABLE_TYPE_RE0) || 
			(p_src_vtbl->vtable_type == FE_VTABLE_TYPE_RE1) || 
			(p_src_vtbl->vtable_type == FE_VTABLE_TYPE_NONE))
		return -1;

	/* need to assign MCGID, update p_src_vtbl's default flow forwarding */
	/* update p_dst_class for MCGID */

	/* next vtable's MCGID = prev's + 1 */
	p_dst_vtbl->mcgid = p_src_vtbl->mcgid + 1;
	// FIXME. mcgid management functions aren't complete yet

	/* create fwdrslt entry that does the multiplication to update 
	 * src_vtable's SDB */
	if (p_src_vtbl->sdb_index == 0) return -1;
	ret = cs_fe_table_get_entry(FE_TABLE_SDB, p_src_vtbl->sdb_index, 
			&src_sdb_entry);
	if (ret != 0) return ret;

	memset(&fwdrslt_entry, 0, sizeof(fwdrslt_entry));
	memset(&voqpol_entry, 0, sizeof(voqpol_entry));
	voqpol_entry.voq_base = ROOT_PORT_VOQ_BASE;
	ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry, 
			&voqpol_idx);
	if (ret != 0) return ret;

	fwdrslt_entry.dest.voq_pol_table_index = voqpol_idx;
	fwdrslt_entry.l2.mcgid_valid = 1;
	// FIXME. mcgid management functions aren't complete yet
	fwdrslt_entry.l2.mcgid = MCGID_VTABLE_TO_HW(p_dst_vtbl->mcgid);
	ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwdrslt_entry, 
			&fwdrslt_idx);
	if (ret != 0) goto FAIL_CLEAR_VOQPOL;

	p_dst_class->port.mcgid = MCGID_VTABLE_TO_HW(p_dst_vtbl->mcgid);

	p_src_vtbl->next = p_dst_vtbl;
	p_dst_vtbl->prev = p_src_vtbl;

	switch (p_src_vtbl->vtable_type) {
	case FE_VTABLE_TYPE_BCAST:
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER,
				src_sdb_entry.misc.bc_flowidx, false);
		if (ret != 0) goto FAIL_CLEAR_FWDRSLT;
		src_sdb_entry.misc.bc_flowidx = fwdrslt_idx;
		ret = cs_fe_table_set_entry(FE_TABLE_SDB, p_src_vtbl->sdb_index, 
				&src_sdb_entry);
		if (ret != 0) {
			p_src_vtbl->bcflow_idx = 0xffff;
			goto FAIL_CLEAR_FWDRSLT;
		}
		p_src_vtbl->bcflow_idx = fwdrslt_idx;
		break;
	case FE_VTABLE_TYPE_L2_MCAST:
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER,
				src_sdb_entry.misc.um_flowidx, false);
		if (ret != 0) goto FAIL_CLEAR_FWDRSLT;
		src_sdb_entry.misc.um_flowidx = fwdrslt_idx;
		ret = cs_fe_table_set_entry(FE_TABLE_SDB, p_src_vtbl->sdb_index, 
				&src_sdb_entry);
		if (ret != 0) {
			p_src_vtbl->umflow_idx = 0xffff;
			goto FAIL_CLEAR_FWDRSLT;
		}
		p_src_vtbl->umflow_idx = fwdrslt_idx;
		break;
	case FE_VTABLE_TYPE_L3_MCAST_V4:
	case FE_VTABLE_TYPE_L3_MCAST_V6:
	case FE_VTABLE_TYPE_L2_FORWARD:
	case FE_VTABLE_TYPE_L3_FORWARD:
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER,
				src_sdb_entry.misc.uu_flowidx, false);
		if (ret != 0) goto FAIL_CLEAR_FWDRSLT;
		src_sdb_entry.misc.uu_flowidx = fwdrslt_idx;
		ret = cs_fe_table_set_entry(FE_TABLE_SDB, p_src_vtbl->sdb_index, 
				&src_sdb_entry);
		if (ret != 0) {
			p_src_vtbl->uuflow_idx = 0xffff;
			goto FAIL_CLEAR_FWDRSLT;
		}
		p_src_vtbl->uuflow_idx = fwdrslt_idx;
		break;
	default:
		/* These vtables do not support Vtable chaining */
		goto FAIL_CLEAR_FWDRSLT;
		break;
	}
	return 0;

FAIL_CLEAR_FWDRSLT:
	cs_fe_table_del_entry_by_idx(FE_TABLE_FWDRSLT, fwdrslt_idx, false);

FAIL_CLEAR_VOQPOL:
	cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER, voqpol_idx, false);

	return ret;
} /* vtable_link_table */

inline void init_hashmask_entry(fe_hash_mask_entry_t *hm_entry)
{
	memset(hm_entry, 0xff, sizeof(fe_hash_mask_entry_t));
	hm_entry->keygen_poly_sel = 0;
	hm_entry->ip_sa_mask = 0;
	hm_entry->ip_da_mask = 0;
}

static int vtable_setup_def_hashmask(cs_vtable_t *p_vtbl, fe_sdb_entry_t *p_sdb)
{
	int ret;
	unsigned int hm_idx;
	fe_hash_mask_entry_t hm_entry;
	init_hashmask_entry(&hm_entry);
#define DEFAULT_HASH_KEYGEN_POLY		0
	switch (p_vtbl->vtable_type) {
	case FE_VTABLE_TYPE_L3_MCAST_V4:
	case FE_VTABLE_TYPE_L3_MCAST_V6:
//		memset((void*)&hm_entry, 0xff, sizeof(hm_entry));
#if 0
		hm_entry.keygen_poly_sel = DEFAULT_HASH_KEYGEN_POLY;
		hm_entry.ip_sa_mask = 0x0;
		hm_entry.ip_da_mask = 0x0;
		/* First Hash Mask: MAC_SA_MASK */
		hm_entry.mac_sa_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) return ret;
		p_sdb->sdb_tuple[0].mask_ptr = hm_idx;
		p_sdb->sdb_tuple[0].priority = 0x08;
		p_sdb->sdb_tuple[0].enable = 1;
		printk("%s::set hash mask for L3_MCAST vtable %d, apptype SA_CHECK %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_SA_CHECK, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_SA_CHECK, hm_idx);

		/* Second Hash Mask: MAC_SA_MASK, IP_VLD_MASK, IP_VER_MASK, IP_SA_MASK, 
		 * 					 IP_DA_MASK. */
		// FIXME!! WEN! on document MCIDX_MASK is turned on for IPv4 L3 MCAT 
		// vtable. but it doesn't make sense to me, so I ignore it for now.. 
		// double check w/cxc
		hm_entry.mac_sa_mask = 0;
		hm_entry.ip_vld_mask = 0;
		hm_entry.ip_ver_mask = 0;
		/* to combine IPv4 and IPv6 vtable initialization for Hash Mask,
		 * we can set ip_da_mask and ip_sa_mask to 0x080. which will netmask 
		 * all 128 bits in an IPv6 address and all 32 bits in an IPv4 addr, 
		 * even though 0x040 is good enough for IPv4 */
		hm_entry.ip_da_mask = 0x080;
		hm_entry.ip_sa_mask = 0x080;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[0].mask_ptr, false);
			p_sdb->sdb_tuple[0].enable = 0;
			p_sdb->sdb_tuple[0].priority = 0;
			p_sdb->sdb_tuple[0].mask_ptr = 0;
			return ret;
		}
		p_sdb->sdb_tuple[1].enable = 1;
		p_sdb->sdb_tuple[1].priority = 1;
		p_sdb->sdb_tuple[1].mask_ptr = hm_idx;
		printk("%s::set hash mask for L3_MCAST vtable %d, apptype L3_MCAST %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_L3_MCAST, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_L3_MCAST, hm_idx);
#endif
		break;
	case FE_VTABLE_TYPE_L2_FORWARD:
#if 0
		memset((void*)&hm_entry, 0xff, sizeof(hm_entry));
		hm_entry.keygen_poly_sel = DEFAULT_HASH_KEYGEN_POLY;
		hm_entry.ip_sa_mask = 0x0;
		hm_entry.ip_da_mask = 0x0;
#endif
#if 0
		/* Tuple 0: MAC SA check */
		/* First Hash Mask: MAC_SA_MASK */
		hm_entry.mac_sa_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) return ret;
		p_sdb->sdb_tuple[0].mask_ptr = hm_idx;
		p_sdb->sdb_tuple[0].priority = 0x08;
		p_sdb->sdb_tuple[0].enable = 1;
		printk("%s::set hash mask for L2_FORWARD, vtable %d, apptype SA_CHECK %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_SA_CHECK, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_SA_CHECK, hm_idx);
#endif
		/* Second Hash Mask: MAC_DA_MASK, ETHERTYPE_MASK, VID_1_MASK */
		init_hashmask_entry(&hm_entry);
		hm_entry.mac_da_mask = 0;
		hm_entry.mac_sa_mask = 0;
		hm_entry.ethertype_mask = 0;
		//hm_entry.vid_1_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[0].mask_ptr, false);
			p_sdb->sdb_tuple[0].enable = 0;
			p_sdb->sdb_tuple[0].priority = 0;
			p_sdb->sdb_tuple[0].mask_ptr = 0;
			return ret;
		}
		p_sdb->sdb_tuple[1].enable = 1;
		p_sdb->sdb_tuple[1].priority = 1;
		p_sdb->sdb_tuple[1].mask_ptr = hm_idx;
		printk("%s::set hash mask for L2_FORWARD, vtable %d, apptype L2_FLOW %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_L2_FLOW, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_L2_FLOW, hm_idx);

		/* Third Hash Mask, QoS mask: 8021P_1_MASK, DEI_1_MASK, IP_VLD_MASK, 
		 * 		  DSCP_MASK, and ECN_MASK */
		/* disable the previous enabled one */
#if 0
		hm_entry.mac_da_mask = 1;
		hm_entry.ethertype_mask = 1;
		hm_entry.vid_1_mask = 1;
		hm_entry._8021p_1_mask = 0;
		hm_entry.dei_1_mask = 0;
		hm_entry.ip_vld_mask = 0;
		hm_entry.dscp_mask = 0;
		hm_entry.ecn_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[0].mask_ptr, false);
			p_sdb->sdb_tuple[0].enable = 0;
			p_sdb->sdb_tuple[0].priority = 0;
			p_sdb->sdb_tuple[0].mask_ptr = 0;
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[1].mask_ptr, false);
			p_sdb->sdb_tuple[1].enable = 0;
			p_sdb->sdb_tuple[1].priority = 0;
			p_sdb->sdb_tuple[1].mask_ptr = 0;
			return ret;
		}
		p_sdb->sdb_tuple[6].enable = 1;
		p_sdb->sdb_tuple[6].priority = 4;
		p_sdb->sdb_tuple[6].mask_ptr = hm_idx;
		printk("%s::set hash mask for L2_FORWARD, vtable %d, apptype L2_QOS %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_QOS_L2_QOS_1, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_QOS_L2_QOS_1, hm_idx);
#endif
		break;
	case FE_VTABLE_TYPE_L3_FORWARD:
		memset((void*)&hm_entry, 0xff, sizeof(hm_entry));
		hm_entry.keygen_poly_sel = DEFAULT_HASH_KEYGEN_POLY;
		// skip SA check first, add this later
		hm_entry.ip_sa_mask = 0x0;
		hm_entry.ip_da_mask = 0x0;
#if 0
		/* First Hash Mask: MAC_SA_MASK */
		hm_entry.mac_sa_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) return ret;
		p_sdb->sdb_tuple[0].mask_ptr = hm_idx;
		p_sdb->sdb_tuple[0].priority = 0x08;
		p_sdb->sdb_tuple[0].enable = 1;

		set_hash_mask_idx_to_app_type(FE_APP_TYPE_SA_CHECK, hm_idx);
#endif
		/* Second Hash Mask: IP_VLD_MASK, IP_VER_MASK, IP_DA_MASK, IP_SA_MASK, 
		 * 		 IP_PROT_MASK. SPI_VLD_MASK, SPI_MASK, L4_VLD_MASK, 
		 * 		 L4_SP_MASK, and L4_DP_MASK */
		// FIXME!! WEN! I combined tuple#1, #3, and #4 into one tuple
//		hm_entry.mac_sa_mask = 1;	/* was enabled in the earlier hash mask */
		hm_entry.ip_vld_mask = 0;
		hm_entry.ip_ver_mask = 0;
		hm_entry.ip_da_mask = 0x080;	/* for both IPv4 and IPv6 addr */
		hm_entry.ip_sa_mask = 0x080;	/* for both IPv4 and IPv6 addr */
		hm_entry.ip_prot_mask = 0;
		hm_entry.ip_fragment_mask = 0;
		/*
		hm_entry.spi_vld_mask = 0;
		hm_entry.spi_mask = 0;
		*/
		hm_entry.l4_vld_mask = 0;
		hm_entry.l4_sp_mask = 0;
		hm_entry.l4_dp_mask = 0;
		hm_entry.l4_ports_rngd = 0;

		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[0].mask_ptr, false);
			p_sdb->sdb_tuple[0].enable = 0;
			p_sdb->sdb_tuple[0].priority = 0;
			p_sdb->sdb_tuple[0].mask_ptr = 0;
			return ret;
		}
		p_sdb->sdb_tuple[1].enable = 1;
		p_sdb->sdb_tuple[1].priority = 1;
		p_sdb->sdb_tuple[1].mask_ptr = hm_idx;
		printk("%s::set hash mask for L3_FORWARD, vtable %d, apptype L4_FLOW_NAT %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_L4_FLOW_NAT, hm_idx);
		//set_hash_mask_idx_to_app_type(FE_APP_TYPE_L3_FLOW_GENERIC, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_L4_FLOW_NAT, hm_idx);

		/* Third Hash Mask: IP_VLD_MASK, IP_VER_MASK, IP_DA_MASK, IP_SA_MASK, 
		 * 			IPV6_NDP_MASK, IPV6_HBH_MASK, IPV6_RH_MASK, and 
		 * 			IPV6_DOH_MASK */
		/* disable the previous enabled one */
#if 0
		memset(&hm_entry, 0xff, sizeof(hm_entry));
		hm_entry.ip_prot_mask = 1;
		hm_entry.spi_vld_mask = 1;
		hm_entry.spi_mask = 1;
		hm_entry.l4_vld_mask = 1;
		hm_entry.l4_sp_mask = 1;
		hm_entry.l4_dp_mask = 1;
		hm_entry.ipv6_ndp_mask = 0;
		hm_entry.ipv6_hbh_mask = 0;
		hm_entry.ipv6_rh_mask = 0;
		hm_entry.ipv6_doh_mask = 0;
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, &hm_entry, &hm_idx);
		if (ret != 0) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[0].mask_ptr, false);
			p_sdb->sdb_tuple[0].enable = 0;
			p_sdb->sdb_tuple[0].priority = 0;
			p_sdb->sdb_tuple[0].mask_ptr = 0;
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[1].mask_ptr, false);
			p_sdb->sdb_tuple[1].enable = 0;
			p_sdb->sdb_tuple[1].priority = 0;
			p_sdb->sdb_tuple[1].mask_ptr = 0;
			return ret;
		}
		p_sdb->sdb_tuple[2].enable = 1;
		p_sdb->sdb_tuple[2].priority = 4;
		p_sdb->sdb_tuple[2].mask_ptr = hm_idx;
		printk("%s::set hash mask for L3_FORWARD, vtable %d, apptype L3_IPV6_ROUTING %d, idx %d\n",
			__func__, p_vtbl->vtable_type, FE_APP_TYPE_L3_IPV6_ROUTING, hm_idx);
		set_hash_mask_idx_to_app_type(FE_APP_TYPE_L3_IPV6_ROUTING, hm_idx);
#endif
		break;
	case FE_VTABLE_TYPE_RE0:
	case FE_VTABLE_TYPE_RE1:
		// FIXME!! WEN! Implement later
		break;
	case FE_VTABLE_TYPE_L2_MCAST:
	case FE_VTABLE_TYPE_BCAST:
	default:
		break;
	}
	return 0;
} /* vtable_setup_def_hashmask */

static void vtable_clear_hm_in_sdb(fe_sdb_entry_t *p_sdb)
{
	int i;

	for (i = 0; i < 8; i++) {
		if (p_sdb->sdb_tuple[i].enable == 1) {
			cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
					p_sdb->sdb_tuple[i].mask_ptr, false);
		}
		p_sdb->sdb_tuple[i].enable = 0;
		p_sdb->sdb_tuple[i].mask_ptr = 0;
	}
} /* vtable_clear_hm_in_sdb */

int cs_vtable_new(unsigned int vtbl_type, cs_vtable_t **table)
{
	cs_vtable_t *new_table;
	int ret;
	cs_vtable_t *prev_vtbl = NULL;
	fe_sdb_entry_t sdb_entry;
	fe_class_entry_t class_entry;

	if ((table == NULL) || (*table == NULL)) return -1;

	new_table = kmalloc(sizeof(cs_vtable_t), GFP_ATOMIC);
	if (new_table == NULL)
		return -ENOMEM;

	memset(new_table, 0x0, sizeof(cs_vtable_t));
	new_table->vtable_type = vtbl_type;
	new_table->uuflow_idx = 0xffff;
	new_table->bcflow_idx = 0xffff;
	new_table->umflow_idx = 0xffff;
	spin_lock_init(&new_table->lock);

	if (vtable_list[vtbl_type] != NULL) {
		/* locate the very last vtable */
		prev_vtbl = vtable_list[vtbl_type];
		while (prev_vtbl->next != NULL) {
			prev_vtbl = prev_vtbl->next;
		};
	}

	/* set up sdb and class entry */
	memset((void *)&sdb_entry, 0x0, sizeof(sdb_entry));
	ret = vtable_setup_def_flow(new_table, &sdb_entry);
	if (ret != 0)
		goto EXIT_FREE_TABLE;

	memset((void *)&class_entry, 0xff, sizeof(class_entry));
	class_entry.entry_valid = 1;
	class_entry.l3.ip_sa_mask = 0x000;
	class_entry.l3.ip_da_mask = 0x000;
	class_entry.port.mcgid = 0;
	class_entry.port.mcgid_mask = 0;
	class_entry.parity = 0;
	class_entry.sdb_idx = 0;
	class_entry.rule_priority = 0;
	ret = vtable_setup_def_class(new_table, &class_entry);
	if (ret != 0)
		goto EXIT_FREE_TABLE;

	if (prev_vtbl != NULL) {
		ret = vtable_link_table(new_table, prev_vtbl, &class_entry);
		if (ret != 0)
			goto EXIT_FREE_TABLE;
	} else {
		/* only set up default hash mask when it's the first vtable in 
		 * the chain */
		ret = vtable_setup_def_hashmask(new_table, &sdb_entry);
		if (ret != 0)
			goto EXIT_FREE_TABLE;
	}

	/* add sdb and class entry into HW FE table */
	/* can't use add_entry, because it will find a matching one and use it. 
	 * We can force to create new entry by doing the following for sdb and 
	 * classifier. */
	ret = cs_fe_table_alloc_entry(FE_TABLE_SDB, &new_table->sdb_index, 0);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;
	ret = cs_fe_table_set_entry(FE_TABLE_SDB, new_table->sdb_index, &sdb_entry);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;
	ret = cs_fe_table_inc_entry_refcnt(FE_TABLE_SDB, new_table->sdb_index);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;

	class_entry.sdb_idx = new_table->sdb_index;
	ret = cs_fe_table_alloc_entry(FE_TABLE_CLASS, &new_table->class_index, 0);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;
	ret = cs_fe_table_set_entry(FE_TABLE_CLASS, new_table->class_index, 
			&class_entry);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;
	ret = cs_fe_table_inc_entry_refcnt(FE_TABLE_CLASS, new_table->class_index);
	if (ret != 0)
		goto EXIT_CLEAR_SDB;

	new_table->fwd_hash = kmalloc(4 << 12, GFP_ATOMIC);
	if (new_table->fwd_hash == NULL)
		goto EXIT_CLEAR_SDB;
	memset(new_table->fwd_hash, 0x0, 4 << 12);
	printk("\t**%s::vtable %p, fwd_hash %p\n",
		__func__, new_table, new_table->fwd_hash);

	new_table->qos_hash = kmalloc(4 << 12, GFP_ATOMIC);
	if (new_table->qos_hash == NULL)
		goto EXIT_CLEAR_SDB;
	memset(new_table->qos_hash, 0x0, 4 << 12);
	printk("\t**%s::vtable %p, qos_hash %p\n",
		__func__, new_table, new_table->qos_hash);
	*table = new_table;
	if (vtable_list[vtbl_type] == NULL)
		vtable_list[vtbl_type] = new_table;

	return 0;

EXIT_CLEAR_SDB:
	vtable_clear_hm_in_sdb(&sdb_entry);

EXIT_FREE_TABLE:
	cs_vtable_release(new_table);

	return ret;
} /* cs_vtable_new */

static int __cs_vtable_release(cs_vtable_t *table)
{
	cs_vtable_hash_entry_t *p_fwd_hash, *p_next_fwd_hash;
	cs_vtable_hash_qos_entry_t *p_qos_hash, *p_next_qos_hash;
	int ret, iii;
	unsigned long flags;

	spin_lock_irqsave(&table->lock, flags);
	if (table->class_index != 0) {
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_CLASS, table->class_index, 
				false);
		if (ret != 0)
			printk("%s:%d:failed at deleting CLASS entry\n", __func__, __LINE__);
	}

	if (table->sdb_index != 0) {
		fe_sdb_entry_t sdb_entry;

		ret = cs_fe_table_get_entry(FE_TABLE_SDB, table->sdb_index, &sdb_entry);
		if (ret != 0)
			printk("%s:%d:failed at getting CLASS entry\n", __func__, __LINE__);
		vtable_clear_hm_in_sdb(&sdb_entry);


		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_SDB, table->sdb_index, 
				false);
		if (ret != 0)
			printk("%s:%d:failed at deleting SDB entry\n", __func__, __LINE__);
		/* as for default uu/bc/um flow assigned in SDB, we will delete it later, 
		 * since vtable data structure actually keeps those info */
	}

	if (table->uuflow_idx != 0xffff) {
		ret = cs_fe_fwdrslt_del_by_idx(table->uuflow_idx);
		if (ret != 0)
			printk("%s:%d:failed to delete uuflow idx %d\n", __func__, 
					__LINE__, table->uuflow_idx);
	}

	if (table->bcflow_idx != 0xffff) {
		ret = cs_fe_fwdrslt_del_by_idx(table->bcflow_idx);
		if (ret != 0)
			printk("%s:%d:failed to delete bcflow idx %d\n", __func__, 
					__LINE__, table->bcflow_idx);
	}

	if (table->umflow_idx != 0xffff) {
		ret = cs_fe_fwdrslt_del_by_idx(table->umflow_idx);
		if (ret != 0)
			printk("%s:%d:failed to delete umflow idx %d\n", __func__, 
					__LINE__, table->umflow_idx);
	}

	if (table->mcgid != 0) {
		// FIXME!! MCGID management functions aren't complete yet.
	}

	/* we don't physically remove the vtable hash entry yet. we will let 
	 * garbage collector to do so */
	for (iii = 0; iii < FE_SW_HASH_ENTRY_MAX; iii++) {
		p_fwd_hash = table->fwd_hash[iii];
		while (p_fwd_hash != NULL) {
			p_next_fwd_hash = p_fwd_hash->next;
			cs_vhash_invalid_fwd_entry(p_fwd_hash);
			p_fwd_hash->next = NULL;
			p_fwd_hash = p_next_fwd_hash;
		};
		table->fwd_hash[iii] = NULL;
	}

	for (iii = 0; iii < FE_SW_HASH_ENTRY_MAX; iii++) {
		p_qos_hash = table->qos_hash[iii];
		while (p_qos_hash != NULL) {
			p_next_qos_hash = p_qos_hash->next;
			cs_vhash_invalid_qos_entry(p_qos_hash);
			p_qos_hash->next = NULL;
			p_qos_hash = p_next_qos_hash;
		};
		table->qos_hash[iii] = NULL;
	}

	// FIXME! voq_mgr

	spin_unlock_irqrestore(&table->lock, flags);
	return 0;
} /* __cs_vtable_release */

int cs_vtable_release(cs_vtable_t *table)
{
	int ret = 0;
	unsigned long flags;

	if (table == NULL) return -1;

	spin_lock_irqsave(&table->lock, flags);
	/* release all the tables chaining after this vtable */
	if (table->next != NULL) {
		cs_vtable_t *curr_vtbl, *prev_vtbl;
		curr_vtbl = table->next;
		while (curr_vtbl->next != NULL)
			curr_vtbl = curr_vtbl->next;
		do {
			prev_vtbl = curr_vtbl->prev;
			if (prev_vtbl == NULL) {
				ret = -1;
				goto EXIT_VTABLE_RELEASE;
			}

			ret = __cs_vtable_release(curr_vtbl);
			if (ret != 0) goto EXIT_VTABLE_RELEASE;

			prev_vtbl->next = NULL;
			curr_vtbl = prev_vtbl;
		} while (curr_vtbl != table);
	}

	/* if there is an existing previous vtable, make the default result 
	 * send to CPU */
	if (table->prev != NULL) {
		fe_sdb_entry_t sdb_entry;

		if (table->prev->sdb_index == 0) {
			ret = -1;
			goto EXIT_VTABLE_RELEASE;
		}

		/* get the sdb information for the previous vtable */
		ret = cs_fe_table_get_entry(FE_TABLE_SDB, table->prev->sdb_index, 
				&sdb_entry);
		if (ret != 0) goto EXIT_VTABLE_RELEASE;

		/* delete the current default flow (which performs multiplication).
		 * it could be anyone of the uuflow_idx, bcflow_idx, or umflow_idx */
		if (table->prev->uuflow_idx != 0xffff) {
			ret = cs_fe_fwdrslt_del_by_idx(table->prev->uuflow_idx);
			if (ret != 0)
				printk("%s:%d:failed to delete uuflow idx %d\n", __func__, 
						__LINE__, table->prev->uuflow_idx);
		}

		if (table->prev->bcflow_idx != 0xffff) {
			ret = cs_fe_fwdrslt_del_by_idx(table->prev->bcflow_idx);
			if (ret != 0)
				printk("%s:%d:failed to delete bcflow idx %d\n", __func__, 
						__LINE__, table->prev->bcflow_idx);
		}

		if (table->prev->umflow_idx != 0xffff) {
			ret = cs_fe_fwdrslt_del_by_idx(table->prev->umflow_idx);
			if (ret != 0)
				printk("%s:%d:failed to delete umflow idx %d\n", __func__, 
						__LINE__, table->prev->umflow_idx);
		}

		/* set up the default flow which forwards to CPU */
		ret = vtable_setup_def_flow(table->prev, &sdb_entry);
		if (ret != 0) goto EXIT_VTABLE_RELEASE;

		/* update the SDB entry */
		ret = cs_fe_table_set_entry(FE_TABLE_SDB, table->prev->sdb_index, 
				&sdb_entry);
		if (ret != 0) goto EXIT_VTABLE_RELEASE;
	}
EXIT_VTABLE_RELEASE:
	spin_unlock_irqrestore(&table->lock, flags);
	if (ret != 0) return ret;

	return __cs_vtable_release(table);
} /* cs_vtable_release */

int cs_vtable_get_default(unsigned int vtbl_type, cs_vtable_t **table)
{
	*table = vtable_list[vtbl_type];
	if (*table == NULL) return -1;
	else return 0;
} /* cs_vtable_get_default */

int cs_vtable_set_class_idx(cs_vtable_t *table, unsigned int class_idx)
{
	int ret = 0;
	unsigned long flags;

	if (table == NULL) return -1;
	if (class_idx == 0) return -1;
	if (table->class_index == class_idx) return 0;

	spin_lock_irqsave(&table->lock, flags);
	/* we assume the new class table entry has already had all those sdb 
	 * , refcnt, and stuff set up. all we need is deleting the original one, 
	 * and update the software entity */
	if (table->class_index != 0) {
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_CLASS, table->class_index, 
				false);
		if (ret != 0) goto EXIT_SET_CLASS_IDX;
	}

	table->class_index = class_idx;
EXIT_SET_CLASS_IDX:
	spin_unlock_irqrestore(&table->lock, flags);
	return ret;
} /* cs_vtable_set_class_idx */

int cs_vtable_get_class_idx(cs_vtable_t *table, unsigned int *class_idx)
{
	unsigned long flags;

	if (table == NULL) return -1;
	if (class_idx == NULL) return -1;
	spin_lock_irqsave(&table->lock, flags);
	*class_idx = table->class_index;
	spin_unlock_irqrestore(&table->lock, flags);
	if (*class_idx == 0) return -1;
	return 0;
} /* cs_vtable_get_class_idx */

int cs_vtable_set_sdb_idx(cs_vtable_t *table, unsigned int sdb_idx)
{
	fe_class_entry_t class_entry;
	int ret = 0;
	unsigned long flags;

	if (table == NULL) return -1;
	if (sdb_idx == 0) return -1;
	if (table->sdb_index == sdb_idx) return 0;

	spin_lock_irqsave(&table->lock, flags);
	/* release the original assigned SDB and modify the class's SDB index. */
	/* we assume the caller has already prepared and set up all the related 
	 * information for this sdb. */
	if (table->sdb_index != 0) {
		ret = cs_fe_table_del_entry_by_idx(FE_TABLE_SDB, table->sdb_index, 
				false);
		if (ret != 0) goto EXIT_SET_SDB_IDX;
	}

	if (table->class_index != 0) {
		ret = cs_fe_table_get_entry(FE_TABLE_CLASS, table->class_index, 
				&class_entry);
		if (ret != 0) goto EXIT_SET_SDB_IDX;
		class_entry.sdb_idx = sdb_idx;
		ret = cs_fe_table_set_entry(FE_TABLE_CLASS, table->class_index, 
				&class_entry);
		if (ret != 0) goto EXIT_SET_SDB_IDX;
	}

	table->sdb_index = sdb_idx;
EXIT_SET_SDB_IDX:
	spin_unlock_irqrestore(&table->lock, flags);
	return ret;
} /* cs_vtable_set_sdb_idx */

int cs_vtable_get_sdb_idx(cs_vtable_t *table, unsigned int *sdb_idx)
{
	unsigned long flags;

	if (table == NULL) return -1;
	if (sdb_idx == NULL) return -1;
	spin_lock_irqsave(&table->lock, flags);
	*sdb_idx = table->sdb_index;
	spin_unlock_irqrestore(&table->lock, flags);
	if (*sdb_idx == 0) return -1;
	return 0;
} /* cs_vtable_get_sdb_idx */

int cs_vtable_set_uuflow_idx(cs_vtable_t *table, unsigned int uuflow_idx)
{
	unsigned long flags;

	if (table == NULL) return -1;
	spin_lock_irqsave(&table->lock, flags);
	table->uuflow_idx = uuflow_idx;
	spin_unlock_irqrestore(&table->lock, flags);
	return 0;
} /* cs_vtable_set_uuflow_idx */

int cs_vtable_add_tuple(cs_vtable_t *table, fe_hash_mask_entry_t *hash_mask, 
		unsigned int priority, bool is_qos)
{
	unsigned int hash_mask_idx, iii;
	unsigned int start_idx, end_idx, check_start_idx, check_end_idx;
	fe_sdb_entry_t sdb_entry;
	bool f_new_hashmask = false;
	int ret = 0, avail_tuple_idx = -1;
	unsigned long flags;

	if ((table == NULL) || (hash_mask == NULL)) return -1;
	if (table->sdb_index == 0) return -1;

	spin_lock_irqsave(&table->lock, flags);
	/* first add this hash_mask to hash_mask table */
	ret = cs_fe_table_find_entry(FE_TABLE_HASH_MASK, hash_mask, 
			&hash_mask_idx, 0);
	if (ret == FE_TABLE_ENTRYNOTFOUND) {
		ret = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, hash_mask, 
				&hash_mask_idx);
		if (ret != 0) goto EXIT_ADD_TUPLE;
		f_new_hashmask = true;
	}
	if (ret != 0) goto EXIT_ADD_TUPLE;

	/* now get the SDB info that this vtable is using. updating it */
	ret = cs_fe_table_get_entry(FE_TABLE_SDB, table->sdb_index, &sdb_entry);
	if (ret != 0) goto EXIT_RELEASE_HASH_MASK;

	if (is_qos == true) {
		start_idx = 6;
		end_idx = 7;
		check_start_idx = 0;
		check_end_idx = 5;
	} else {
		start_idx = 0;
		end_idx = 5;
		check_start_idx = 6;
		check_end_idx = 7;
	}

	/* for tuple insertion, we have to take care of the case that when we are 
	 * sharing the hashmask entry with another user. we have to make sure the 
	 * shared user is not our own tuples of other type, such that same hash 
	 * mask setting used in both FWD tuple and QoS tuple should have different 
	 * hash_index. If that's the case, we will need to create a new one! */
	for (iii = check_start_idx; iii <= check_end_idx; iii++) {
		if ((f_new_hashmask == false) && 
				(sdb_entry.sdb_tuple[iii].enable == 1) && 
				(sdb_entry.sdb_tuple[iii].mask_ptr == hash_mask_idx)) {
			/* found a matching one. need to either find or create a new hash 
			 * mask. */
			ret = cs_fe_table_find_entry(FE_TABLE_HASH_MASK, hash_mask, 
					&hash_mask_idx, hash_mask_idx + 1);
			if (ret == FE_TABLE_ENTRYNOTFOUND) {
				ret = cs_fe_table_alloc_entry(FE_TABLE_HASH_MASK, 
					&hash_mask_idx, 0);
				if (ret != 0) goto EXIT_RELEASE_HASH_MASK;

				f_new_hashmask = true;
				ret = cs_fe_table_set_entry(FE_TABLE_HASH_MASK, 
						hash_mask_idx, (void*)hash_mask);
				if (ret != 0) goto EXIT_RELEASE_HASH_MASK;

				ret = cs_fe_table_inc_entry_refcnt(FE_TABLE_HASH_MASK, 
						hash_mask_idx);
				if (ret != 0) goto EXIT_RELEASE_HASH_MASK;
			}
		}
	}

	/* search through sdb tuple entry with respective start_idx and end_idx to 
	 * see if there is matching one. also, try to locate an avaible slot */
	for (iii = start_idx; iii <= end_idx; iii++) {
		if ((sdb_entry.sdb_tuple[iii].enable == 1) && 
				(sdb_entry.sdb_tuple[iii].mask_ptr == hash_mask_idx)) {
			/* found a matching one. just update the priority. */
			if (sdb_entry.sdb_tuple[iii].priority != priority) {
				sdb_entry.sdb_tuple[iii].priority = priority;
				ret = cs_fe_table_set_entry(FE_TABLE_SDB, table->sdb_index, 
						&sdb_entry);
				goto EXIT_COMPLETE;
			}
		}
		if ((avail_tuple_idx == -1) && 
				(sdb_entry.sdb_tuple[iii].enable == 0))
			avail_tuple_idx = iii;
	}

	/* getting here means no previous matching index is found */

	if (avail_tuple_idx == -1)	/* no available slot */
		goto EXIT_RELEASE_HASH_MASK;

	sdb_entry.sdb_tuple[avail_tuple_idx].mask_ptr = hash_mask_idx;
	sdb_entry.sdb_tuple[avail_tuple_idx].priority = priority;
	sdb_entry.sdb_tuple[avail_tuple_idx].enable = 1;

	ret = cs_fe_table_set_entry(FE_TABLE_SDB, table->sdb_index, &sdb_entry);
	if (ret != 0) goto EXIT_RELEASE_HASH_MASK;

EXIT_COMPLETE:
	if (f_new_hashmask == false)
		cs_fe_table_inc_entry_refcnt(FE_TABLE_HASH_MASK, hash_mask_idx);

	spin_unlock_irqrestore(&table->lock, flags);
	return ret;

EXIT_RELEASE_HASH_MASK:
	if (f_new_hashmask == true)
		cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, hash_mask_idx, false);

EXIT_ADD_TUPLE:
	spin_unlock_irqrestore(&table->lock, flags);
	return ret;
} /* cs_vtable_add_tuple */

int cs_vtable_del_tuple(cs_vtable_t *table, fe_hash_mask_entry_t *hash_mask, 
		bool is_qos)
{
	unsigned int hash_mask_idx, iii;
	unsigned int start_idx, end_idx;
	fe_sdb_entry_t sdb_entry;
	int ret = 0, offset = 0;
	unsigned long flags;

	if ((table == NULL) || (hash_mask == NULL)) return -1;
	if (table->sdb_index == 0) return -1;

	if (is_qos == true) {
		start_idx = 6;
		end_idx = 7;
	} else {
		start_idx = 0;
		end_idx = 5;
	}

	spin_lock_irqsave(&table->lock, flags);
	/* get the SDB info */
	ret = cs_fe_table_get_entry(FE_TABLE_SDB, table->sdb_index, &sdb_entry);
	if (ret != 0) goto EXIT_DEL_TUPLE;

	/* search through sdb tuple entry with respective start_idx and end_idx to 
	 * see if there is matching one. also, try to locate an avaible slot */
	do {
		ret = cs_fe_table_find_entry(FE_TABLE_HASH_MASK, hash_mask, 
				&hash_mask_idx, offset);
		if (ret == 0) {
			for (iii = start_idx; iii <= end_idx; iii++) {
				if ((sdb_entry.sdb_tuple[iii].enable == 1) && 
						(sdb_entry.sdb_tuple[iii].mask_ptr == hash_mask_idx)) {
					/* found a matching one.. need to remove it from SDB */
					sdb_entry.sdb_tuple[iii].enable = 0;
					sdb_entry.sdb_tuple[iii].mask_ptr = 0;
					sdb_entry.sdb_tuple[iii].priority = 0;
					ret = cs_fe_table_set_entry(FE_TABLE_SDB, table->sdb_index, 
							&sdb_entry);
					if (ret != 0) goto EXIT_DEL_TUPLE;

					/* delete this hash mask entry */
					ret = cs_fe_table_del_entry_by_idx(FE_TABLE_HASH_MASK, 
							hash_mask_idx, false);
					if (ret != 0) goto EXIT_DEL_TUPLE;
				}
			}
			offset = hash_mask_idx + 1;
		}
	} while (ret == 0);

EXIT_DEL_TUPLE:
	spin_unlock_irqrestore(&table->lock, flags);

	return ret;
} /* cs_vtable_del_tuple */

// FIXME!! need to wait for definition of tuple data structure
int cs_vtable_set_voq_mgr(cs_vtable_t *table, void* voq_mgr)
{
	return 0;
} /* cs_vtable_set_voq_mgr */

int cs_vtable_get_hash_mask_ptr(cs_vtable_t *table, 
		fe_hash_mask_entry_t *hash_mask, __u8 *hash_mask_ptr)
{
	fe_sdb_entry_t sdb_entry;
	unsigned int hm_idx = 0;
	int ret, i;
	unsigned long flags;

	if (table == NULL) return -1;
	if (table->sdb_index == 0) return -1;

	spin_lock_irqsave(&table->lock, flags);
	ret = cs_fe_table_get_entry(FE_TABLE_SDB, table->sdb_index, &sdb_entry);
	if (ret != 0) {
		spin_unlock_irqrestore(&table->lock, flags);
		return ret;
	}

	while (cs_fe_table_find_entry(FE_TABLE_HASH_MASK, hash_mask, &hm_idx, 
				hm_idx? hm_idx + 1: 0) == 0) {
		for (i = 0; i < 8; i++) {
			if ((sdb_entry.sdb_tuple[i].enable == 1) && 
					(sdb_entry.sdb_tuple[i].mask_ptr == hm_idx)) {
				*hash_mask_ptr = hm_idx;
				spin_unlock_irqrestore(&table->lock, flags);
				return 0;
			}
		}
	}
	spin_unlock_irqrestore(&table->lock, flags);

	return -1;
} /* cs_vtable_get_hash_mask_ptr */

#ifdef GARBAGE_COLLECTOR_ENABLE
#if 0
/* assume vhash has already created all the rslt_lookup_entry */
static int add_vhash_to_gc(cs_vtable_hash_entry_t *vhash, bool is_v6)
{
	int i, ret;
	cs_gc_entry_t *p_gc_entry;

	/* L2 GC */
	for (i = 0; i < CS_VTABLE_L2_RSLT_TYPE_MAX; i++) {
		if (vhash->l2_result[i] != NULL) {
			ret = cs_gc_table_add_entry(FE_GC_TYPE_MAC, 
					(void *)vhash->l2_result[i]->l2.mac_addr, 6, &p_gc_entry);
			if (ret != 0) return ret;

			if (p_gc_entry == NULL) return -1;
			ret = cs_vtable_rslt_set_gc_entry(vhash->l2_result[i], p_gc_entry);
			if (ret != 0) return ret;

			ret = cs_gc_entry_add_hash(p_gc_entry, vhash);
			if (ret != 0) return ret;

			atomic_inc(&vhash->gc_users);
		}
	}

	/* L3 GC */
	for (i = 0; i < CS_VTABLE_L3_RSLT_TYPE_MAX; i++) {
		if (vhash->l3_result[i] != NULL) {
			if (is_v6 == true)
				ret = cs_gc_table_add_entry(FE_GC_TYPE_IP, 
						(void *)vhash->l3_result[i]->ipv6.addr, 16, 
						&p_gc_entry);
			else
				ret = cs_gc_table_add_entry(FE_GC_TYPE_IP, 
						(void *)&vhash->l3_result[i]->ipv4.addr, 4, 
						&p_gc_entry);
			if (ret != 0) return ret;

			if (p_gc_entry == NULL) return -1;
			ret = cs_vtable_rslt_set_gc_entry(vhash->l3_result[i], p_gc_entry);
			if (ret != 0) return ret;

			ret = cs_gc_entry_add_hash(p_gc_entry, vhash);
			if (ret != 0) return ret;

			atomic_inc(&vhash->gc_users);
		}
	}

	/* L4 GC */
	for (i = 0; i < CS_VTABLE_L4_RSLT_TYPE_MAX; i++) {
		if (vhash->l4_result[i] != NULL) {
			ret = cs_gc_table_add_entry(FE_GC_TYPE_PORT, 
					(void *)&vhash->l4_result[i]->l4.port, 2, &p_gc_entry);
			if (ret != 0) return ret;

			if (p_gc_entry == NULL) return -1;
			ret = cs_vtable_rslt_set_gc_entry(vhash->l4_result[i], p_gc_entry);
			if (ret != 0) return ret;

			ret = cs_gc_entry_add_hash(p_gc_entry, vhash);
			if (ret != 0) return ret;

			atomic_inc(&vhash->gc_users);
		}
	}

	/* VLAN GC */
	for (i = 0; i < CS_VTABLE_VLAN_RSLT_TYPE_MAX; i++) {
		if (vhash->vlan_result[i] != NULL) {
			ret = cs_gc_table_add_entry(FE_GC_TYPE_VID, 
					(void *)&vhash->vlan_result[i]->vlan.vid, 2, &p_gc_entry);
			if (ret != 0) return ret;

			if (p_gc_entry == NULL) return -1;
			ret = cs_vtable_rslt_set_gc_entry(vhash->vlan_result[i], p_gc_entry);
			if (ret != 0) return ret;

			ret = cs_gc_entry_add_hash(p_gc_entry, vhash);
			if (ret != 0) return ret;

			atomic_inc(&vhash->gc_users);
		}
	}

	return 0;
} /* add_vhash_to_gc */
#endif

#if 0
/*
 * This function will add vtable_hash_entry to gc tables of ingress packet's
 * MAC, VID, IP, port, or SW ID. When deletion of MAC/VID/IP/port happens,
 * the vtable_hash_entry state will be set invalid and then remove.
 */
static int add_fwd_hash_info_to_gc(cs_vtable_fwd_hash_t *fwd_hash, 
		cs_vtable_hash_entry_t *vhash)
{
	int ret, i;
	fe_hash_mask_entry_t hm_entry;
	cs_swid_entry_t *swid_entry;
	cs_vtable_rslt_lookup_entry_t *new_lookup;
	cs_gc_entry_t *p_gc_entry;

	memset(&hm_entry, 0x0, sizeof(hm_entry));
	ret = cs_fe_table_get_entry(FE_TABLE_HASH_MASK, fwd_hash->key.mask_ptr_0_7,
		&hm_entry);
	if (ret != 0) return ret;

	/* L2 Rslt_lookup */
	if (hm_entry.mac_da_mask == 0) {	/* 0 means enabled */
		ret = cs_vhash_set_l2_result(CS_VTABLE_L2_RSLT_IN_DA, vhash, 0xffff, 
				fwd_hash->key.mac_da);
		printk("%s::add to result l2_da ret %x\n", __func__, ret);
		if (ret != 0) return ret;
	}
	if (hm_entry.mac_sa_mask == 0) {	/* 0 means enabled */
		ret = cs_vhash_set_l2_result(CS_VTABLE_L2_RSLT_IN_SA, vhash, 0xffff, 
				fwd_hash->key.mac_sa);
		printk("%s::add to result l2_sa ret %x\n", __func__, ret);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l2.mac_da_replace_en == 1) {
		ret = cs_vhash_set_l2_result(CS_VTABLE_L2_RSLT_OUT_DA, vhash, 
				fwd_hash->action.l2.l2_index, &fwd_hash->param.mac[0]);
		printk("%s::add to result new l2_da ret %x\n", __func__, ret);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l2.mac_sa_replace_en == 1) {
		ret = cs_vhash_set_l2_result(CS_VTABLE_L2_RSLT_OUT_SA, vhash, 
				fwd_hash->action.l2.l2_index, &fwd_hash->param.mac[6]);
		printk("%s::add to result new l2_sa ret %x\n", __func__, ret);
		if (ret != 0) return ret;
	}

	/* L3 Rslt_lookup */
	if (hm_entry.ip_da_mask != 0) {		/* not 0 means it's used */
		ret = cs_vhash_set_l3_result(CS_VTABLE_L3_RSLT_IN_IP_DA, vhash, 0xffff, 
				fwd_hash->key.da, fwd_hash->param.is_v6);
		if (ret != 0) return ret;
	}
	if (hm_entry.ip_sa_mask != 0) {		/* not 0 means it's used */
		ret = cs_vhash_set_l3_result(CS_VTABLE_L3_RSLT_IN_IP_DA, vhash, 0xffff, 
				fwd_hash->key.sa, fwd_hash->param.is_v6);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l3.ip_da_replace_en == 1) {
		ret = cs_vhash_set_l3_result(CS_VTABLE_L3_RSLT_OUT_IP_DA, vhash, 
				fwd_hash->action.l3.ip_da_index, fwd_hash->param.dst_ip, 
				fwd_hash->param.is_v6);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l3.ip_sa_replace_en == 1) {
		ret = cs_vhash_set_l3_result(CS_VTABLE_L3_RSLT_OUT_IP_SA, vhash, 
				fwd_hash->action.l3.ip_sa_index, fwd_hash->param.src_ip, 
				fwd_hash->param.is_v6);
		if (ret != 0) return ret;
	}

	/* L4 Rslt_lookup */
	if (hm_entry.l4_dp_mask == 0) {		/* 0 means enabled */
		//ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_IN_DST_PORT, vhash, 0, 
		ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_IN_DST_PORT, vhash,
				fwd_hash->key.l4_dp);
		if (ret != 0) return ret;
	}
	if (hm_entry.l4_sp_mask == 0) {		/* 0 means enabled */
		//ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_IN_SRC_PORT, vhash, 0, 
		ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_IN_SRC_PORT, vhash,
				fwd_hash->key.l4_sp);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l4.dp_replace_en == 1) {
		//ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_OUT_DST_PORT, vhash, 0, 
		ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_OUT_DST_PORT, vhash,
				fwd_hash->action.l4.dp);
		if (ret != 0) return ret;
	}
	if (fwd_hash->action.l4.sp_replace_en == 1) {
		//ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_OUT_SRC_PORT, vhash, 0, 
		ret = cs_vhash_set_l4_result(CS_VTABLE_L4_RSLT_OUT_SRC_PORT, vhash,
				fwd_hash->action.l4.sp);
		if (ret != 0) return ret;
	}
#if 0
	/* VLAN Rslt_lookup */
	if (hm_entry.vid_1_mask == 0) {		/* 0 means enabled */
		ret = cs_vhash_set_vid_result(CS_VTABLE_VLAN_RSLT_IN_OUTER_VID, vhash, 
				fwd_hash->action.l2.flow_vlan_index, fwd_hash->param.in_vid);
		if (ret != 0) return ret;
	}
#if 0	/* do not support double tagged VLAN yet */
	if (hm_entry.vid_2_mask == 0) {		/* 0 means enabled */
		ret = cs_vhash_set_vid_result(CS_VTABLE_VLAN_RSLT_IN_INNER_VID, vhash, 
				fwd_hash->key.vid_2);
		if (ret != 0) return ret;
	}
#endif
	if ((fwd_hash->action.l2.flow_vlan_op_en == 1) && 
			((fwd_hash->param.out_vid != 0) || (fwd_hash->param.in_vid != 0))) {
		ret = cs_vhash_set_vid_result(CS_VTABLE_VLAN_RSLT_OUT_OUTER_VID, vhash, 
				fwd_hash->action.l2.flow_vlan_index, fwd_hash->param.out_vid);
		if (ret != 0) return ret;
	}
#if 0	/* do not support double tagged VLAN yet */
	if ((fwd_hash->action.l2.flow_vlan_op_en == 1) && 
			((fwd_hash->param.out_vid != 0) || (fwd_hash->param.in_vid != 0))) {
		ret = cs_vhash_set_vid_result(CS_VTABLE_VLAN_RSLT_OUT_INNER_VID, vhash, 
				fwd_hash->param.out_vid);
		if (ret != 0) return ret;
	}
#endif
#endif
	/* sw_id */
	if ((fwd_hash->swid_info.num_swid != 0) && 
			(fwd_hash->swid_info.list != NULL)) {
		swid_entry = fwd_hash->swid_info.list;
		for (i = 0; i < fwd_hash->swid_info.num_swid; i++) {
			if (swid_entry != NULL) {
				ret = cs_vhash_set_swid_result(vhash, swid_entry->swid, 
						&new_lookup);
				if (ret != 0) return ret;
				if (new_lookup == NULL) return -1;

				ret = cs_gc_table_add_entry(FE_GC_TYPE_SWID64, 
						(void*)new_lookup->misc.swid, 8, &p_gc_entry);
				if (ret != 0) return ret;
				if (p_gc_entry == NULL) return -1;

				ret = cs_vtable_rslt_set_gc_entry(new_lookup, p_gc_entry);
				if (ret != 0) return ret;

				ret = cs_gc_entry_add_hash(p_gc_entry, vhash);
				if (ret != 0) return ret;

				atomic_inc(&vhash->gc_users);
				swid_entry = swid_entry->next;
			}
		}
	}

	return add_vhash_to_gc(vhash, fwd_hash->param.is_v6);
} /* add_fwd_hash_info_to_gc */
#endif
#endif

int cs_vtable_add_hash(cs_vtable_t *table, 
		cs_fwd_hash_t *fwd_hash_entry, 
		cs_qos_hash_t *qos_hash_entry)
{
	cs_vtable_hash_entry_t *p_vhash_fwd_entry = NULL;
	fe_voq_pol_entry_t voqpol_entry;
	fe_flow_vlan_entry_t fvlan_entry;
	cs_vtable_hash_qos_entry_t *p_vhash_qos_entry = NULL;

	int ret;
	bool f_free_fwdrslt = false, f_is_qos_free = false;
	/* cs_fe_table_add_entry() uses u32* as rslt_idx! */
	unsigned int result_index;

	if (fwd_hash_entry != NULL) {
		p_vhash_fwd_entry = kmalloc(sizeof(cs_vtable_hash_entry_t),
				GFP_ATOMIC);
		memset(p_vhash_fwd_entry, 0x0, sizeof(cs_vtable_hash_entry_t));

		atomic_set(&p_vhash_fwd_entry->gc_users, 0);
		/* get the mask ptr first by application type */
		ret = get_hash_mask_idx_by_app_type(fwd_hash_entry->fwd_app_type, 
				&p_vhash_fwd_entry->mask_ptr);

		if (ret != 0)
			return ret;
		fwd_hash_entry->key.mask_ptr_0_7 = p_vhash_fwd_entry->mask_ptr;

		p_vhash_fwd_entry->table = (void *)table;
		p_vhash_fwd_entry->lifetime = fwd_hash_entry->lifetime;
		/* get the result index */
		ret = 0;

		if ((fwd_hash_entry->action.l2.mac_sa_replace_en == 1) &&
				(fwd_hash_entry->action.l2.mac_da_replace_en == 1))
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac,
					L2_LOOKUP_TYPE_PAIR, p_vhash_fwd_entry,
					&fwd_hash_entry->action.l2.l2_index);
		else if (fwd_hash_entry->action.l2.mac_sa_replace_en == 1)
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac,
					L2_LOOKUP_TYPE_SA, p_vhash_fwd_entry,
					&fwd_hash_entry->action.l2.l2_index);
		else if (fwd_hash_entry->action.l2.mac_da_replace_en == 1)
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac,
					L2_LOOKUP_TYPE_DA, p_vhash_fwd_entry,
					&fwd_hash_entry->action.l2.l2_index);
#if 0
		if ((fwd_hash_entry->action.l2.mac_sa_replace_en == 1) && 
				(fwd_hash_entry->action.l2.mac_da_replace_en == 1))
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac, 
					L2_LOOKUP_TYPE_PAIR, 
					(unsigned int*)&fwd_hash_entry->action.l2.l2_index);
		else if (fwd_hash_entry->action.l2.mac_sa_replace_en == 1)
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac, 
					L2_LOOKUP_TYPE_SA, 
					(unsigned int*)&fwd_hash_entry->action.l2.l2_index);
		else if (fwd_hash_entry->action.l2.mac_da_replace_en == 1)
			ret = cs_fe_l2_result_alloc(fwd_hash_entry->param.mac, 
					L2_LOOKUP_TYPE_DA, 
					(unsigned int*)&fwd_hash_entry->action.l2.l2_index);
#endif
		if (ret != 0) return ret;

		if ((fwd_hash_entry->param.first_vlan_cmd != 0) || 
				(fwd_hash_entry->param.second_vlan_cmd != 0)) {
			memset(&fvlan_entry, 0, sizeof(fvlan_entry));
			fvlan_entry.first_vid = fwd_hash_entry->param.first_vid;
			fvlan_entry.first_vlan_cmd = fwd_hash_entry->param.first_vlan_cmd;
			fvlan_entry.first_tpid_enc = fwd_hash_entry->param.first_tpid_enc;
			fvlan_entry.second_vid = fwd_hash_entry->param.second_vid;
			fvlan_entry.second_vlan_cmd = fwd_hash_entry->param.second_vlan_cmd;
			fvlan_entry.second_tpid_enc = fwd_hash_entry->param.second_tpid_enc;

			ret = cs_fe_table_add_entry(FE_TABLE_FVLAN, &fvlan_entry,
					&result_index);
			fwd_hash_entry->action.l2.flow_vlan_index = (u16) result_index;

			if (ret != 0) goto QUIT_FREE_L2;
			fwd_hash_entry->action.l2.flow_vlan_op_en = 1;
		}
		if (fwd_hash_entry->action.l3.ip_sa_replace_en == 1) {
			ret = cs_fe_l3_result_alloc(fwd_hash_entry->param.src_ip, 
					fwd_hash_entry->param.is_v6, p_vhash_fwd_entry,
					&fwd_hash_entry->action.l3.ip_sa_index);
			if (ret != 0) goto QUIT_FREE_FVLAN;
		}

		if (fwd_hash_entry->action.l3.ip_da_replace_en == 1) {
			ret = cs_fe_l3_result_alloc(fwd_hash_entry->param.dst_ip, 
					fwd_hash_entry->param.is_v6, p_vhash_fwd_entry,
					&fwd_hash_entry->action.l3.ip_da_index);
			if (ret != 0) goto QUIT_FREE_L3_SRC;
		}
#if 0
		if (fwd_hash_entry->action.l3.ip_sa_replace_en == 1) {
			ret = cs_fe_l3_result_alloc(fwd_hash_entry->param.src_ip, 
					fwd_hash_entry->param.is_v6, 
					(unsigned int*)&fwd_hash_entry->action.l3.ip_sa_index);
			if (ret != 0) goto QUIT_FREE_FVLAN;
		}

		if (fwd_hash_entry->action.l3.ip_da_replace_en == 1) {
			ret = cs_fe_l3_result_alloc(fwd_hash_entry->param.dst_ip, 
					fwd_hash_entry->param.is_v6, 
					(unsigned int*)&fwd_hash_entry->action.l3.ip_da_index);
			if (ret != 0) goto QUIT_FREE_L3_SRC;
		}
#endif
		memset(&voqpol_entry, 0, sizeof(voqpol_entry));
		voqpol_entry.voq_base = fwd_hash_entry->param.voq_id;
		voqpol_entry.pol_base = fwd_hash_entry->param.pol_id;

		ret = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voqpol_entry,
				&result_index);
		fwd_hash_entry->action.dest.voq_pol_table_index = (u16)result_index;

		if (ret != 0)
			goto QUIT_FREE_L3;

		ret = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, &fwd_hash_entry->action,
				&result_index);
		p_vhash_fwd_entry->result_index = (u16)result_index;
		if (ret != 0)
			goto QUIT_FREE_VOQPOL;

		/* calculate the crc values */
		ret = cs_fe_hash_calc_crc(&fwd_hash_entry->key,
				&p_vhash_fwd_entry->crc32, &p_vhash_fwd_entry->crc16, 
				CRC16_CCITT);
		if (ret != 0)
			goto QUIT_FREE_FWDRSLT;
#if 1
		ret = cs_fe_hash_add_hash(p_vhash_fwd_entry->crc32, 
				p_vhash_fwd_entry->crc16, p_vhash_fwd_entry->mask_ptr, 
				p_vhash_fwd_entry->result_index, 
				&p_vhash_fwd_entry->hw_index);
#else
		/* hw_index is used by hash_link_table[] to get this vhash */
		p_vhash_fwd_entry->hw_index = p_vhash_fwd_entry->crc16 & 0x0fff;
		ret = FE_TABLE_EDUPLICATE;
#endif
		{
			cs_vtable_hash_entry_t *curr_vhash;
		/* if gets here, meaning hash entry has been create properly.  Now, it 
		 * is time to update the vtable hash entry list */

			/* add the entry to vtable list */
			if (table->fwd_hash == NULL)
				goto QUIT_FREE_FWDHASH;

			if (table->fwd_hash[p_vhash_fwd_entry->hw_index >> 4] == NULL) {
				table->fwd_hash[p_vhash_fwd_entry->hw_index >> 4] =
					p_vhash_fwd_entry;
			} else {
				curr_vhash = table->fwd_hash[p_vhash_fwd_entry->hw_index >> 4];
				while (curr_vhash->next != NULL)
					curr_vhash = curr_vhash->next;
				curr_vhash->next = p_vhash_fwd_entry;
			}
		}
		if (ret != 0) {
			/* it's duplicate.. so we need to delete the created fwdrslt. */
			cs_fe_fwdrslt_del_by_idx(p_vhash_fwd_entry->result_index);
			/* vhash has been added to gc_list. Invalidate and remove later. */
			p_vhash_fwd_entry->state = CS_VHASH_STATE_INVALID;
//			goto QUIT_FREE_FWDRSLT;
		} else {
			ret = cs_vhash_set_state(p_vhash_fwd_entry, CS_VHASH_STATE_ACTIVE);
			if (ret != 0)
				goto QUIT_FREE_FWDHASH;
		}
#if 0
#ifdef GARBAGE_COLLECTOR_ENABLE
			/* hash addition complete! now we are adding info to Garbage 
			 * collectors */
			ret = add_fwd_hash_info_to_gc(fwd_hash_entry, p_vhash_fwd_entry);
			if (ret != 0)
				goto QUIT_FREE_FWDHASH;
#endif
#endif
		/* add vhash to other gc_table entry? */
	}

	if (qos_hash_entry != NULL) {
		p_vhash_qos_entry = kmalloc(sizeof(cs_vtable_hash_qos_entry_t), 
				GFP_ATOMIC);
		memset(p_vhash_qos_entry, 0x0, sizeof(cs_vtable_hash_qos_entry_t));

		ret = get_hash_mask_idx_by_app_type(qos_hash_entry->fwd_app_type, 
				&p_vhash_qos_entry->mask_ptr);
		if (ret != 0) goto QUIT_FREE_FWDHASH;
		qos_hash_entry->key.mask_ptr_0_7 = p_vhash_qos_entry->mask_ptr;

		p_vhash_qos_entry->table = (void *)table;
		ret = cs_fe_table_add_entry(FE_TABLE_QOSRSLT, &qos_hash_entry->action,
				&result_index);
		p_vhash_qos_entry->result_index = (u16)result_index;
		if (ret != 0) goto QUIT_FREE_FWDHASH;

		ret = cs_fe_hash_calc_crc(&qos_hash_entry->key, 
				&p_vhash_qos_entry->crc32, &p_vhash_qos_entry->crc16, 
				CRC16_CCITT);
		if (ret != 0) goto QUIT_FREE_QOSRSLT;

		ret = cs_fe_hash_add_hash(p_vhash_qos_entry->crc32, 
				p_vhash_qos_entry->crc16, p_vhash_qos_entry->mask_ptr, 
				p_vhash_qos_entry->result_index, 
				&p_vhash_qos_entry->hw_index);
		if (ret == FE_TABLE_EDUPLICATE) {
			/* it's duplicate. so we need to delete the created qosrslt for 
			 * decrementing the user count */
			cs_fe_table_del_entry_by_idx(FE_TABLE_QOSRSLT, 
					p_vhash_qos_entry->result_index, false);
			/* we are not going to increment the user count of this QoS 
			 * hash entry, even though one QoS hash entry might be shared by 
			 * various number of FWD hash entry. We are letting the upper 
			 * software to handle this situation */
		} else if (ret != 0) goto QUIT_FREE_QOSRSLT;

		/* if gets here, meaning hash entry has been create properly.  Now, it 
		 * is time to update the vtable hash entry list */
		if (ret != FE_TABLE_EDUPLICATE) {
			cs_vtable_hash_qos_entry_t *curr_vhash;

			/* add the entry to vtable list */
			if (table->qos_hash == NULL) goto QUIT_FREE_QOSHASH;

			if (table->qos_hash[p_vhash_qos_entry->hw_index >> 4] == NULL)
				table->qos_hash[p_vhash_qos_entry->hw_index >> 4] = 
					p_vhash_qos_entry;
			else {
				curr_vhash = table->qos_hash[p_vhash_qos_entry->hw_index >> 4];
				while (curr_vhash->next != NULL) curr_vhash = curr_vhash->next;
				curr_vhash->next = p_vhash_qos_entry;
			}
		}
	}

	if ((fwd_hash_entry != NULL) && (qos_hash_entry != NULL)) {
		/* link this QoS hash entry to FWD hash entry */
		printk("%s::add qos entry %p, fwd_entry %p\n",
			__func__, p_vhash_qos_entry, p_vhash_fwd_entry);
		ret = cs_vhash_add_qos_entry(p_vhash_fwd_entry, p_vhash_qos_entry);
		if (ret != 0)
			goto QUIT_FREE_QOSHASH;
	}

	return 0;

QUIT_FREE_QOSHASH:
	printk("%s::quit_free_qoshash!\n", __func__);
	if ((qos_hash_entry != NULL) && (p_vhash_qos_entry != NULL)) {
		cs_vtable_remove_qos_hash_from_list(p_vhash_qos_entry);
		cs_vhash_del_qos_entry(p_vhash_qos_entry);
		kfree(p_vhash_qos_entry);
		f_is_qos_free = true;
	}

QUIT_FREE_QOSRSLT:
	printk("%s::quit_free_qosresult!\n", __func__);
	if ((qos_hash_entry != NULL) && (p_vhash_qos_entry != NULL) && 
			(f_is_qos_free == false)) {
		cs_fe_table_del_entry_by_idx(FE_TABLE_QOSRSLT, 
				p_vhash_qos_entry->result_index, false);
		kfree(p_vhash_qos_entry);
		f_is_qos_free = true;
	}

QUIT_FREE_FWDHASH:
	printk("%s::quit_free_fwdhash\n", __func__);

	if ((fwd_hash_entry != NULL) && (p_vhash_fwd_entry != NULL)) {
		cs_vtable_remove_hash_from_list(p_vhash_fwd_entry);
		cs_vhash_del_fwd_entry(p_vhash_fwd_entry);
		kfree(p_vhash_fwd_entry);
	}

	return ret;

QUIT_FREE_FWDRSLT:
	printk("%s::quite_free_fwdrslt\n", __func__);
	if (fwd_hash_entry != NULL)
		f_free_fwdrslt = true;

QUIT_FREE_VOQPOL:
	printk("%s::quite_free_voqpol\n", __func__);
	if (fwd_hash_entry != NULL)
		cs_fe_table_del_entry_by_idx(FE_TABLE_VOQ_POLICER, 
				fwd_hash_entry->action.dest.voq_pol_table_index, false);

QUIT_FREE_L3:
	printk("%s::quit_free_l3\n", __func__);
	if ((fwd_hash_entry != NULL) && 
			(fwd_hash_entry->action.l3.ip_da_replace_en == 1))
		cs_fe_l3_result_dealloc(fwd_hash_entry->action.l3.ip_da_index);

QUIT_FREE_L3_SRC:
	printk("%s::quit_free_l3_src\n", __func__);
	if ((fwd_hash_entry != NULL) && 
			(fwd_hash_entry->action.l3.ip_sa_replace_en == 1))
		cs_fe_l3_result_dealloc(fwd_hash_entry->action.l3.ip_sa_index);

QUIT_FREE_FVLAN:
	printk("%s::quit_free_fvlan\n", __func__);
	if ((fwd_hash_entry != NULL) && 
			(fwd_hash_entry->action.l2.flow_vlan_op_en == 1))
		cs_fe_table_del_entry_by_idx(FE_TABLE_FVLAN, 
				fwd_hash_entry->action.l2.flow_vlan_index, false);

QUIT_FREE_L2:
	printk("%s::quit_free_l2\n", __func__);
	if ((fwd_hash_entry != NULL) && 
			((fwd_hash_entry->action.l2.mac_sa_replace_en == 1) || 
			 (fwd_hash_entry->action.l2.mac_da_replace_en == 1)))
		cs_fe_l2_result_dealloc(fwd_hash_entry->action.l2.mac_da_replace_en,
				fwd_hash_entry->action.l2.mac_sa_replace_en,
				fwd_hash_entry->action.l2.l2_index);

	if ((f_free_fwdrslt == true) && (p_vhash_fwd_entry != NULL)) {
		cs_fe_table_del_entry_by_idx(FE_TABLE_FWDRSLT, 
				p_vhash_fwd_entry->result_index, false);
	}

	if ((fwd_hash_entry != NULL) && (p_vhash_fwd_entry != NULL))
		kfree(p_vhash_fwd_entry);

	return ret;
} /* cs_vtable_add_hash */

int cs_vtable_remove_hash_from_list(cs_vtable_hash_entry_t *vhash)
{
	cs_vtable_hash_entry_t *curr_hash, *prev_hash;
	cs_vtable_t *p_vtable;
	if (vhash == NULL) return -1;

	/* well.. there is no vtable related to this hash entry, then we don't 
	 * have to remove the entry from a vtable */
	if (vhash->table == NULL) return 0;
	p_vtable = (cs_vtable_t*)vhash->table;

	if (p_vtable->fwd_hash[vhash->hw_index >> 4] != NULL) {
		prev_hash = NULL;
		curr_hash = p_vtable->fwd_hash[vhash->hw_index >> 4];
		while (curr_hash != NULL) {
			if (curr_hash == vhash) {
				if (prev_hash == NULL)
					p_vtable->fwd_hash[vhash->hw_index >> 4] = 
						curr_hash->next;
				else
					prev_hash->next = curr_hash->next;
				vhash->table = NULL;
				return 0;
			}
			prev_hash = curr_hash;
			curr_hash = curr_hash->next;
		};
	}

	vhash->table = NULL;
	return 0;
} /* cs_vtable_remove_hash_from_list */

int cs_vtable_remove_qos_hash_from_list(cs_vtable_hash_qos_entry_t *vhash)
{
	cs_vtable_hash_qos_entry_t *curr_hash, *prev_hash;
	cs_vtable_t *p_vtable;

	/* well.. there is no vtable related to this hash entry, then we don't have 
	 * to remove the entry from a vtable */
	if (vhash->table == NULL) return 0;
	p_vtable = (cs_vtable_t*)vhash->table;

	if (p_vtable->qos_hash[vhash->hw_index >> 4] != NULL) {
		prev_hash = NULL;
		curr_hash = p_vtable->qos_hash[vhash->hw_index >> 4];
		while (curr_hash != NULL) {
			if (curr_hash == vhash) {
				if (prev_hash == NULL)
					p_vtable->qos_hash[vhash->hw_index >> 4]
						= curr_hash->next;
				else
					prev_hash->next = curr_hash->next;
				vhash->table = NULL;
				return 0;
			}
			prev_hash = curr_hash;
			curr_hash = curr_hash->next;
		};
	}

	vhash->table = NULL;
	return 0;
} /* cs_vtable_remove_qos_hash_from_list */

#ifdef CONFIG_CS752X_HW_ACCELERATION
int cs_vtable_set_fwd_hash_l2sa_check(cs_vtable_t *vtable,
	cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb)
{
	fe_sw_hash_t *key = &fwd_hash->key;
	fe_fwd_result_entry_t *action = &fwd_hash->action;
	fe_fwd_result_param_t *result = &fwd_hash->param;
	int i=0;
	
	for (i=0; i<6; i++)
		key->mac_sa[i] = cb->input.raw.sa[5-i];

	result->voq_id = CPU_PORT0_VOQ_BASE;
	return CS_OK;
}

int cs_vtable_set_fwd_hash_bridge(cs_vtable_t *vtable, 
	cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb)
{
	fe_sw_hash_t *key = &fwd_hash->key;
	fe_fwd_result_entry_t *action = &fwd_hash->action;
	fe_fwd_result_param_t *result = &fwd_hash->param;
	int i=0;

	for (i=0; i<6; i++) {
		//key->mac_da[i] = cb->input.raw.da[i];
		key->mac_da[i] = cb->input.raw.da[5-i];
	}
	for (i=0; i<6; i++) {
		//key->mac_da[i] = cb->input.raw.da[i];
		key->mac_sa[i] = cb->input.raw.sa[5-i];
	}
#if 0
	printk("%s::input key MAC DA: %x %x %x %x %x %x\n",
		__func__, key->mac_da[0], key->mac_da[1], key->mac_da[2],
		key->mac_da[3], key->mac_da[4], key->mac_da[5]);
	printk("%s::input key MAC SA: %x %x %x %x %x %x\n",
		__func__, key->mac_sa[0], key->mac_sa[1], key->mac_sa[2],
		key->mac_sa[3], key->mac_sa[4], key->mac_sa[5]);
#endif
	//key->eth_type = cb->input.raw.eth_protocol;
	key->eth_type = ntohs(cb->input.raw.eth_protocol);
#if 0
	printk("%s::cb %x, eth_type %x\n",
		__func__, cb->input.raw.eth_protocol, key->eth_type);
#endif
	// modify VLAN later...
	//key->vid_1 = cb->input.raw.vlan_id & VLAN_VID_MASK;
	if (cb->output_mask & CS_HM_MAC_DA_MASK) {
		for (i=0; i<6; i++)
			result->mac[i] = cb->output.raw.da[5-i];
		action->l2.mac_da_replace_en = CS_RESULT_ACTION_ENABLE;
	}
	if (cb->output_mask & CS_HM_MAC_SA_MASK) {
		for (i=0; i<6; i++)
			result->mac[6+i] = cb->output.raw.sa[5-i];
		action->l2.mac_sa_replace_en = CS_RESULT_ACTION_ENABLE;
	}
#if 0
	printk("%s::result MAC %x %x %x %x %x %x, %x %x %x %x %x %x\n",
		__func__, result->mac[0], result->mac[1], result->mac[2],
		result->mac[3], result->mac[4], result->mac[5],
		result->mac[6], result->mac[7], result->mac[8],
		result->mac[9], result->mac[10], result->mac[11]);
#endif
	result->voq_id = cb->action.voq_pol.d_voq_id;
	result->pol_id = cb->action.voq_pol.d_pol_id;
	return CS_OK;
}

#define TCP_CTRL_MASK	0x18	// check URG, Reset, SYN, FIN bits
int cs_vtable_set_fwd_hash_nat(cs_vtable_t *vtable,
	cs_fwd_hash_t *fwd_hash, CS_KERNEL_ACCEL_CB_T *cb)
{
	fe_sw_hash_t *key = &fwd_hash->key;
	fe_fwd_result_entry_t *action = &fwd_hash->action;
	fe_fwd_result_param_t *result = &fwd_hash->param;
	int i=0;

	/* Set keys. Fields should be fixed */
	key->ip_valid = 1;
	key->ip_version = 0;	// ipv4
	key->ip_frag = 0;

	key->ip_prot = cb->input.l3_nh.iph.protocol;
	key->sa[0] = ntohl(cb->input.l3_nh.iph.sip);
	key->da[0] = ntohl(cb->input.l3_nh.iph.dip);

	key->l4_valid = 1;
	if (key->ip_prot == IPPROTO_TCP) {
		key->l4_sp = ntohs(cb->input.l4_h.th.sport);
		key->l4_dp = ntohs(cb->input.l4_h.th.dport);
		key->tcp_ctrl_flags = TCP_CTRL_MASK;
	} else if (key->ip_prot == IPPROTO_UDP) {
		key->l4_sp = ntohs(cb->input.l4_h.uh.sport);
		key->l4_dp = ntohs(cb->input.l4_h.uh.dport);
	}

	// should we enable l3/l4 csum_error flags?

	/* Set result and actions. These are determined by kernel or cb */
	if (cb->output_mask & CS_HM_MAC_SA_MASK) {
		action->l2.mac_sa_replace_en = CS_RESULT_ACTION_ENABLE;
		for (i=0; i<6; i++)
			result->mac[6+i] = cb->output.raw.sa[5-i];
	}
	if (cb->output_mask & CS_HM_MAC_DA_MASK) {
		action->l2.mac_da_replace_en = CS_RESULT_ACTION_ENABLE;
		for (i=0; i<6; i++)
			result->mac[i] = cb->output.raw.da[5-i];
	}

	if (cb->output_mask & CS_HM_IP_SA_MASK) {
		action->l3.ip_sa_replace_en = CS_RESULT_ACTION_ENABLE;
		result->src_ip[0] = ntohl(cb->output.l3_nh.iph.sip);
	}
	if (cb->output_mask & CS_HM_IP_DA_MASK) {
		action->l3.ip_da_replace_en = CS_RESULT_ACTION_ENABLE;
		result->dst_ip[0] = ntohl(cb->output.l3_nh.iph.dip);
	}
	if (cb->output_mask & CS_HM_L4_SP_MASK) {
		action->l4.sp_replace_en = CS_RESULT_ACTION_ENABLE;
		if (key->ip_prot == IPPROTO_TCP)
			action->l4.sp = ntohs(cb->output.l4_h.th.sport);
		else if (key->ip_prot == IPPROTO_UDP)
			action->l4.sp = ntohs(cb->output.l4_h.uh.sport);
	}
	if (cb->output_mask & CS_HM_L4_DP_MASK) {
		action->l4.dp_replace_en = CS_RESULT_ACTION_ENABLE;
		if (key->ip_prot == IPPROTO_TCP)
			action->l4.dp = ntohs(cb->output.l4_h.th.dport);
		else if (key->ip_prot == IPPROTO_UDP)
			action->l4.dp = ntohs(cb->output.l4_h.uh.dport);
	}

	/* destination voq */
	result->voq_id = cb->action.voq_pol.d_voq_id;
	result->pol_id = cb->action.voq_pol.d_pol_id;
//	printk("%s::dest voq %x, pol %x\n",
//		__func__, result->voq_id, result->pol_id);
	return CS_OK;
}

int cs_vtable_set_qos_hash_nat(cs_vtable_t *vtable,
	cs_qos_hash_t *qos_hash,	CS_KERNEL_ACCEL_CB_T *cb)
{
	return CS_OK;
}
#endif



