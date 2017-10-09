/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *				CH Hsu <ch.hsu@cortina-systems.com>
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
/*
 * cs_hw_accel_core.c
 *
 * $Id: cs_hw_accel_base.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * This file contains the implementation for CS Forwarding Engine Offload
 * Kernel Module.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <mach/cs_types.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/if_vlan.h>
#include <cs_hw_accel_util.h>
#include "cs_fe.h"
#include <cs_hw_accel_core.h>
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
#include <cs_hw_accel_ipsec.h>
#endif
#include <cs_hw_accel_mc.h>

#ifdef NEW_L2_DESIGN
#define MAC_TABLE_CNT	512
#define PORT_CNT		3
cs_uint8 mac_record[PORT_CNT*MAC_TABLE_CNT][6];
#endif

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"

int fwdrslt_entry=0, no_avaiable_fwdrslt_entry = 0, no_l2_result=0, no_l3_result=0, no_voq_result=0, no_vlan_result=0, no_fwdrslt_result=0;
int l2_result=0, l3_result=0, voq_result=0, vlan_result=0, fwdrslt_result=0;
extern u32 cs_adapt_debug;
#endif /* CONFIG_CS752X_PROC */

#define DBG(x)  if(cs_adapt_debug & CS752X_ADAPT_CORE) x

extern cs_hash_guid_s cs_hash_map_guid[CS_HASH_MAP_GUID_NUM_POWER];
extern struct cs_core_hash *users;
extern cs_uint8	core_initialized;
extern CORE_CFG_T core_cfg;
extern int cs_hw_accel_major;

/* Adding an item to a hash. */
void core_add_user(cs_uint16 hash_index, cs_uint8 vtable_id,
		cs_uint8 guid_cnt, cs_uint64 *p_guid)
{
	struct cs_core_hash *s;
	int i;

	s = kmalloc(sizeof(struct cs_core_hash), GFP_ATOMIC);
	s->hash_index = hash_index;
	s->vid = vtable_id;
	s->guid_cnt = guid_cnt;
	for (i = 0; i < guid_cnt; i++) {
		s->guid[i] = *p_guid;
		p_guid++;
	}
	HASH_ADD_INT(users, hash_index, s); /* hash_index: name of key field */
}

/* Looking up an item in a hash. */
struct cs_core_hash *core_find_user(cs_uint64 guid)
{
	struct cs_core_hash *s;

	HASH_FIND_INT(users, &guid, s); /* s: output pointer */
	return s;
}

/* Deleting an item from a hash. */
void core_delete_user(struct cs_core_hash *s)
{
	HASH_DEL(users, s); /* user: pointer to delete */
	kfree(s);
}

/* Deleting all of hash. */
void core_delete_all(void)
{
	struct cs_core_hash *current_user;

	while (users) {
		current_user = users; /* grab pointer to first item */
		HASH_DEL(users, current_user); /* delete it (users advances to next) */
		kfree(current_user); /* free it */
	}
}

/* Print hash content */
void core_print_users(void)
{
	struct cs_core_hash *s;
	int i;

	for (s = users; s != NULL; s = s->hh.next) {
		//rdmsg ("hash_index %d, Vit %d, GUID \n", s->hash_index, s->vid);
		for (i = 0; i < s->guid_cnt; i++) {
			//printk("0x%llx \t", s->guid);
			//s->guid++;
		}
	}
}

/* Iterating over all the items in a hash */
void core_get_users(cs_uint64 guid, struct cs_core_hash **p_index)
{
	struct cs_core_hash *s;
	int i = 0;
	cs_uint8 guid_table[MODULE_GUID_NUM];
	cs_uint64 *destp;

	memset((void *)&guid_table, 0, sizeof(guid_table));
	destp = (cs_uint64 *)&guid_table[0];
	for (s = users; s != NULL; s=s->hh.next) {
		destp = (cs_uint64 *)&s->guid[i];
		for (i = 0; i < s->guid_cnt; i++) {
			if (*destp == guid) {
				*p_index = s;
				break;
			} else
				destp++;
		}
	}
}

cs_uint8 __cs_kernel_to_hash_mask(CS_HASH_ENTRY_S *hash_entry,
	fe_hash_mask_entry_t *mask_rule)
{
	unsigned int idx;

	memset((void *)mask_rule, 0xFF, sizeof(fe_hash_mask_entry_t));

	mask_rule->keygen_poly_sel = 0;//ccitt crc-16
	mask_rule->ip_sa_mask = 0x0;
	mask_rule->ip_da_mask = 0x0;

	if (hash_entry->mask & CS_HM_MAC_DA_MASK)
		mask_rule->mac_da_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_MAC_SA_MASK)
		mask_rule->mac_sa_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_ETHERTYPE_MASK)
		mask_rule->ethertype_mask = CS_HASH_MASK_ENABLE;

	/* VLAN MASKs */
	if (hash_entry->mask & CS_HM_TPID_ENC_1_MSB_MASK)
		mask_rule->tpid_enc_1_msb_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_TPID_ENC_1_LSB_MASK)
		mask_rule->tpid_enc_1_lsb_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_8021P_1_MASK)
		mask_rule->_8021p_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_1_MASK)
		mask_rule->dei_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_VID_1_MASK)
		mask_rule->vid_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_TPID_ENC_2_MSB_MASK)
		mask_rule->tpid_enc_2_msb_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_TPID_ENC_2_LSB_MASK)
		mask_rule->tpid_enc_2_lsb_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_8021P_2_MASK)
		mask_rule->_8021p_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_2_MASK)
		mask_rule->dei_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_VID_2_MASK)
		mask_rule->vid_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_DA_MASK)
		mask_rule->ip_da_mask = 0x080;	//FIXME:

	if (hash_entry->mask & CS_HM_IP_SA_MASK) {
		//FIXME: according to IP type to fill mask field
		mask_rule->ip_sa_mask = 0x080;//FIXME:IPv4 0x080
	}
	if (hash_entry->mask & CS_HM_IP_PROT_MASK)
		mask_rule->ip_prot_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_FRAGMENT_MASK)
		mask_rule->ip_fragment_mask = CS_HASH_MASK_ENABLE;

	// FIXME! what is this for?
	if (hash_entry->mask & CS_HM_KEYGEN_POLY_SEL)
		mask_rule->keygen_poly_sel = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_FLOW_LBL_MASK)
		mask_rule->ipv6_flow_lbl_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_VER_MASK)
		mask_rule->ip_ver_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_VLD_MASK)
		mask_rule->ip_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_SP_MASK) {
		// FIXME!!... port range implementation
		// Fix you now!
		// FIXME again,CH
		// 00 = Not ranged (exact matched).
		// 01 = Destination port is ranged but source port is exact matched.
		// 10 = Destination port is exact matched but source port is ranged.
		// 11 = Both destination and source are ranged.
		mask_rule->l4_ports_rngd = CS_HASH_MASK_ENABLE;
		mask_rule->l4_sp_mask = CS_HASH_MASK_ENABLE;
	}

	if (hash_entry->mask & CS_HM_L4_DP_MASK) {
		// FIXME!!... port range implementation
		// Fix you now!
		// FIXME again,CH
		// 00 = Not ranged (exact matched).
		// 01 = Destination port is ranged but source port is exact matched.
		// 10 = Destination port is exact matched but source port is ranged.
		// 11 = Both destination and source are ranged.
		mask_rule->l4_ports_rngd = CS_HASH_MASK_ENABLE;
		mask_rule->l4_dp_mask = CS_HASH_MASK_ENABLE;
	}

	if (hash_entry->mask & CS_HM_TCP_CTRL_MASK){
		mask_rule->tcp_ctrl_mask = 0x18;		/* FIXME: Hard code,Bypass SYN,FIN,RST,URG to SW */
		//mask_rule->FE_HM_TCP_CTRL_MASK = 0x38;	/* FIXME: Hard code,Bypass SYN,FIN and RST to SW */
		//mask_rule->FE_HM_TCP_CTRL_MASK = CS_HASH_MASK_ENABLE;
	}

	/* is this belonged to here? or it only happens in QoS tuple? */
	if (hash_entry->mask & CS_HM_TCP_ECN_MASK)
		mask_rule->tcp_ecn_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_VLD_MASK)
		mask_rule->l4_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_LSPID_MASK)
		mask_rule->lspid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_FWDTYPE_MASK)
		mask_rule->fwdtype_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPPOE_SESSION_ID_VLD_MASK)
		mask_rule->pppoe_session_id_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPPOE_SESSION_ID_MASK)
		mask_rule->pppoe_session_id_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_RECIRC_IDX_MASK)
		mask_rule->recirc_idx_mask = CS_HASH_MASK_ENABLE;
	
	if (hash_entry->mask & CS_HM_MCIDX_MASK)
		mask_rule->mcidx_mask = CS_HASH_MASK_ENABLE;
		
	// FIXME: mcgid but not multicast, using vtable
	//if (hash_entry->mask & CS_HM_MCGID_MASK & (!CS_HM_MC_DA_MASK)) {
	if (hash_entry->mask & CS_HM_MCGID_MASK) {
		mask_rule->mcgid_mask = 0x10f;//enable bits[7:4]
		//rdmsg ("%s:: vtable mask_rule->mcgid_mask = 0x%x\n", __func__,
		//		mask_rule->MCGID_MASK);
	}
	//rdmsg ("%s::==> mask_rule->MCGID_MASK = 0x%X\n", __func__,
	//		mask_rule->MCGID_MASK);

	// FIXME: multicast
	if (hash_entry->mask & CS_HM_MC_DA_MASK) {
		mask_rule->mcgid_mask = 0x1f0;
		//rdmsg ("%s:: multicast mask_rule->mcgid_mask = 0x%x\n",
		//		__func__, mask_rule->MCGID_MASK);
	}

	if (hash_entry->mask & CS_HM_DA_AN_MAC_SEL_MASK)
		mask_rule->da_an_mac_sel_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DA_AN_MAC_HIT_MASK)
		mask_rule->da_an_mac_hit_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_ORIG_LSPID_MASK)
		mask_rule->orig_lspid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_MASK)
		mask_rule->l7_field_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_VLD_MASK)
		mask_rule->l7_field_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_HDR_A_FLAGS_CRCERR_MASK)
		mask_rule->hdr_a_flags_crcerr_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L3_CHKSUM_ERR_MASK)
		mask_rule->l3_chksum_err_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_CHKSUM_ERR_MASK)
		mask_rule->l4_chksum_err_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_NOT_HDR_A_FLAGS_STSVLD_MASK)
		mask_rule->not_hdr_a_flags_stsvld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_HASH_FID_MASK)
		mask_rule->hash_fid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_SEL) {
		//mask_rule->l7_field_sel = CS_HASH_MASK_ENABLE;//2 bits
	}

	if (hash_entry->mask & CS_HM_SA_BNG_MAC_SEL_MASK)
		mask_rule->sa_bng_mac_sel_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SA_BNG_MAC_HIT_MASK)
		mask_rule->sa_bng_mac_hit_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SPI_VLD_MASK)
		mask_rule->spi_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SPI_MASK)
		mask_rule->spi_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_NDP_MASK)
		mask_rule->ipv6_ndp_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_HBH_MASK)
		mask_rule->ipv6_hbh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_RH_MASK)
		mask_rule->ipv6_rh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_DOH_MASK)
		mask_rule->ipv6_doh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPP_PROTOCOL_VLD_MASK)
		mask_rule->ppp_protocol_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPP_PROTOCOL_MASK)
		mask_rule->ppp_protocol_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK) {
		/* Note:
		 * This is to make sure that the Hash Mask is only enabled
		 * for the match vectors that are used.  This way, it does not
		 * cause issue when introducing a new pktlen match vector that
		 * overlaps with previously created one. */
		if (hash_entry->key.pktlen_rng_match_vector != 0)
			mask_rule->pktlen_rng_match_vector_mask = 
				~hash_entry->key.pktlen_rng_match_vector;
		else
			mask_rule->pktlen_rng_match_vector_mask = CS_HASH_MASK_ENABLE;
	}

	/* FIXME! does this function check for existing matched mask rule? */
	cs_fe_table_add_entry(FE_TABLE_HASH_MASK, mask_rule, &idx);
	//rdmsg ("---> mask_index = %d\n", idx);
	//idx = mask_rule->index = 1;//FIXME: debug cs_fe_hash_mask_ram_alloc()

	hash_entry->key.mask_ptr_0_7 = idx;
	//hash_entry->key.mask_ptr_0_7 = 1;//FIXME: need update this data

	return idx;
}

cs_uint32 __cs_kernel_core_add_hash_by_cb(CS_KERNEL_ACCEL_CB_T *cs_cb)
{
	CS_HASH_ENTRY_S *hash_entry, *qos_hash_entry;
	cs_uint8 hash_data[sizeof(CS_HASH_ENTRY_S)];
	cs_uint8 qos_hash_data[sizeof(CS_HASH_ENTRY_S)];
	cs_status status;
	cs_uint64 guid = 0;
	cs_uint16 module_id;
	cs_uint8 i, ipsec_existed = 0, mc_existed = 0;

	/* check if it is sw_only */
	if (cs_cb->common.sw_only == CS_SWONLY_DONTCARE ||
			cs_cb->common.sw_only == CS_SWONLY_STATE) {
		//rdmsg ("^^^^^^^^^^^^^^^^^^^^^^^^ SW ONLY ^^^^^^^^^^^^^^^^^^^^^^^^\n");
		return 0;
	}

	//printk("%s:: ====>  cs_cb->input_mask = 0x%llX\n", __func__, cs_cb->input_mask);
	//FIXME: IPSEC and MC priority ?
	for (i = 0; i < cs_cb->common.guid_cnt; i++) {
		guid = cs_cb->common.guid[i];
		module_id = (cs_uint16)(guid >> 48);
		if (module_id == CS_KERNEL_IPSEC_ID_TAG) {
			ipsec_existed = 1;
			break;
		}
		if (module_id == CS_KERNEL_MC_ID_TAG) {
			mc_existed = 1;
			break;
		}
	}

	//printk("%s::IN cs_cb->output_mask = 0x%llX, voq_id = %d\n", __func__, cs_cb->output_mask, cs_cb->action.voq_pol.d_voq_id);

	if (cs_cb->action.voq_pol.d_voq_id == CS_DEFAULT_VOQ) {
		if (cs_cb->common.pspid == GE_PORT0)
			cs_cb->action.voq_pol.d_voq_id = GE_PORT0_VOQ_BASE;//voq 0

		if (cs_cb->common.pspid == GE_PORT1)
			cs_cb->action.voq_pol.d_voq_id = GE_PORT1_VOQ_BASE;//voq 8

		if (cs_cb->common.pspid == GE_PORT2)
			cs_cb->action.voq_pol.d_voq_id = GE_PORT2_VOQ_BASE;//voq 23
	}
	//rdmsg ("%s:cs_cb->action.voq_pol.d_voq_id = %d\n", __func__, cs_cb->action.voq_pol.d_voq_id);

#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
	/* IPsec create hash entry */
	if (cs_cb->common.module_mask & CS_MOD_MASK_IPSEC || ipsec_existed) {
		cs_hw_accel_ipsec_create_hash(cs_cb, guid);
		return 0;
	}
#endif

	/* Multicast create hash entry */
	if ((cs_cb->common.module_mask & CS_MOD_MASK_IPV4_MULTICAST) ||
			(cs_cb->common.module_mask & CS_MOD_MASK_IPV6_MULTICAST) ||
			 mc_existed) {
		//FIXME:
		DBG(printk("%s:: Multicast \n",__func__ ));
		//cs_hw_accel_mc_create_hash(cs_cb, guid);
		return 0;
	}

	hash_entry = (CS_HASH_ENTRY_S *)&hash_data;
	memset((void *)hash_entry, 0, sizeof(CS_HASH_ENTRY_S));

	qos_hash_entry = (CS_HASH_ENTRY_S *)&qos_hash_data;
	memset((void *)qos_hash_entry, 0, sizeof(CS_HASH_ENTRY_S));

	status = cs_kernel_get_hash_mask_from_cs_cb(cs_cb, &hash_entry->mask);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	status = cs_kernel_get_hash_key_from_cs_cb(cs_cb, &hash_entry->key);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	status = cs_kernel_get_hash_action_from_cs_cb(cs_cb, &hash_entry->result);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	//FIXME: cs_cb->common.vtype ????
	return cs_kernel_core_add_hash(hash_entry, cs_cb->common.vtype,
			cs_cb->common.guid, cs_cb->common.guid_cnt, cs_cb->tmo.interval); //cs_cb->tmo.interval);
} /* cs_kernel_core_add_hash_by_cb */

int __cs_kernel_to_forwarding_result(fe_hash_rslt_s *result)
{
	fe_fwd_result_entry_t *p_rslt = NULL;
	cs_uint8 result_data[sizeof(fe_fwd_result_entry_t)];
	unsigned int l2_da_mac_index, l2_sa_mac_index;
	unsigned int l3_sip_index = 0x0fff, l3_dip_index = 0x0fff;
	unsigned int l2_mac_index, rslt_idx, voqpol_idx;
	cs_boolean is_v6 = 0;
	cs_uint8 *mac_da = NULL, *mac_sa = NULL, status;
	fe_voq_pol_entry_t voq_pol_data;

	p_rslt = (fe_fwd_result_entry_t *)&result_data;
	memset((void *)p_rslt, 0, sizeof(fe_fwd_result_entry_t));

	memset(&voq_pol_data, 0, sizeof(fe_voq_pol_entry_t));

	p_rslt->acl_dsbl = result->action.acl_dsbl;
	p_rslt->parity = result->action.action_parity;

	//FIXME: L2 mac_addr, type, how to know type and da sa ?
	if (result->action.mac_sa_replace_en)
		mac_sa = &result->param.mac_sa[0];

	if (result->action.mac_da_replace_en)
		mac_da = &result->param.mac_da[0];

	if ((mac_sa != NULL) && (mac_da != NULL)) {
		cs_uint8 mac_combo[12];
		memcpy(mac_combo, mac_da, 6);
		memcpy(mac_combo+6, mac_sa, 6);
		status = cs_fe_l2_result_alloc(mac_combo, L2_LOOKUP_TYPE_PAIR, &l2_mac_index);
		if (status != FE_STATUS_OK) {
			(printk("%s: No avaiable L2 result %d\n",__func__,__LINE__));
			no_l2_result++;
			return CS_ERROR;
		}
		l2_result++;
		//printk("%s: l2_mac_index = %d\n",__func__, l2_mac_index);
		p_rslt->l2.l2_index = l2_mac_index;
	} else if (mac_sa != NULL) {
		status = cs_fe_l2_result_alloc(mac_sa, L2_LOOKUP_TYPE_SA, &l2_sa_mac_index);
		if (status != FE_STATUS_OK) {
			(printk("%s: No avaiable L2 result %d\n",__func__,__LINE__));
			no_l2_result++;
			return CS_ERROR;
		}
		l2_result++;
		//printk("%s: l2_sa_mac_index = %d\n",__func__, l2_sa_mac_index);
		p_rslt->l2.l2_index = l2_sa_mac_index;
	} else if (mac_da != NULL) {
		status = cs_fe_l2_result_alloc(mac_da, L2_LOOKUP_TYPE_DA, &l2_da_mac_index);
		if (status != FE_STATUS_OK) {
			(printk("%s: No avaiable L2 result %d\n",__func__,__LINE__));
			no_l2_result++;
			return CS_ERROR;
		}
		l2_result++;
		//printk("%s: l2_da_mac_index = %d\n",__func__, l2_da_mac_index);
		p_rslt->l2.l2_index = l2_da_mac_index;
	}

	if (result->action.ip_sa_replace_en) {
		status = cs_fe_l3_result_alloc(result->param.sip_addr, is_v6, &l3_sip_index);
		//printk("%s:--> l3_sip_index = %d\n",__func__, l3_sip_index);
		if (status != FE_STATUS_OK) {
			(printk("%s: No avaiable L3 result %d\n",__func__,__LINE__));
			//return CS_ERROR;
			no_l3_result++;
			goto l2_result_alloc_fail;
		}
		l3_result++;
	}
	if (result->action.ip_da_replace_en) {
		status = cs_fe_l3_result_alloc(result->param.dip_addr, is_v6, &l3_dip_index);
		//printk("%s: l3_dip_index = %d\n",__func__, l3_dip_index);
		if (status != FE_STATUS_OK) {
			(printk("%s: No avaiable L3 result %d\n",__func__,__LINE__));
			//return CS_ERROR;
			no_l3_result++;
			goto l3_sip_result_alloc_fail;
		}
		l3_result++;
	}

	/* if sw_action is needed, make sure ip_da_replace_en and ip_sa_replace_en
	 * are not enabled! then update l3_sip_index and l3_dip_index for it */
	if (result->action.sw_action && !result->action.ip_da_replace_en &&
			!result->action.ip_sa_replace_en) {
		l3_sip_index = (cs_uint16)(result->param.sw_action_id & 0x0fff);
		l3_dip_index = (cs_uint16)((result->param.sw_action_id >> 12) & 0x00ff);
	}

	/* get voqpol table index by voq and pol id */
	printk("%s:%d:d_voq = %d, d_pol = %d\n", __func__, __LINE__, result->param.d_voq,	result->param.d_pol);

	voq_pol_data.voq_base = result->param.d_voq;
	voq_pol_data.pol_base = result->param.d_pol;
	voq_pol_data.cpu_pid = result->param.cpu_pid;
	voq_pol_data.ldpid = result->param.ldpid;
	voq_pol_data.pppoe_session_id = result->param.pppoe_session_id;
	voq_pol_data.cos_nop = result->action.cos_nop;
	voq_pol_data.parity = result->param.voq_policer_parity;

	status = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voq_pol_data, 
			&voqpol_idx);
	if (status != CS_OK) {
		// FIXME.. any error here?
		printk("%s::unable to obtain/allocate a voqpol idx ", __func__);
		printk("related to forwarding result entry\n");
		voq_result++;
	}

	//rdmsg ("%s::voqpol_idx = %d\n",__func__, voqpol_idx);
	p_rslt->dest.voq_pol_table_index = voqpol_idx;

	/* L2 */
	p_rslt->l2.mac_sa_replace_en = result->action.mac_sa_replace_en;
	p_rslt->l2.mac_da_replace_en = result->action.mac_da_replace_en;
	p_rslt->l2.mcgid = result->param.mcgid;
	p_rslt->l2.mcgid_valid = result->action.mcgid_valid;
	p_rslt->l2.flow_vlan_op_en = result->action.flow_vlan_op_en;
	p_rslt->l2.pppoe_encap_en = result->action.pppoe_encap_en;
	p_rslt->l2.pppoe_decap_en = result->action.pppoe_decap_en;

	/* L3 */
	p_rslt->l3.ip_sa_replace_en = result->action.ip_sa_replace_en;
	p_rslt->l3.ip_da_replace_en = result->action.ip_da_replace_en;
	p_rslt->l3.ip_sa_index = l3_sip_index;
	p_rslt->l3.ip_da_index = l3_dip_index;
	p_rslt->l3.decr_ttl_hoplimit = result->action.decr_ttl_hoplimit;

	/* L4 */
	p_rslt->l4.sp_replace_en = result->action.sp_replace_en;
	p_rslt->l4.dp_replace_en = result->action.dp_replace_en;
	if (result->action.sp_replace_en) {
		p_rslt->l4.sp = result->param.sport;
	}
	if (result->action.dp_replace_en) {
		p_rslt->l4.dp = result->param.dport;
	}

	/* Dest */
	//p_rslt->dest.voq_policy = result->param.d_voq;//FWD_VOQ_POLICY
	//p_rslt->dest.pol_policy = result->param.d_pol;//FWD_POL_POLICY
	p_rslt->dest.voq_policy = 0;//FWD_VOQ_POLICY, debug, need move this code
	p_rslt->dest.pol_policy = result->param.d_pol;//FWD_POL_POLICY

	/* Act */
	p_rslt->act.fwd_type_valid = result->action.fwd_type_valid;
	p_rslt->act.fwd_type = result->param.fwd_type;
	p_rslt->act.drop = result->action.drop;

#if 1
	/* for flow vlan */
	if ((result->param.first_vlan_cmd != 0) || 
			(result->param.second_vlan_cmd != 0)) {
		unsigned int vlan_entry_index;
		fe_flow_vlan_entry_t entry;
		entry.first_vid = result->param.first_vid;
		entry.first_vlan_cmd = result->param.first_vlan_cmd;

		entry.first_tpid_enc = result->param.first_tpid_enc;
		entry.second_vlan_cmd = result->param.second_vlan_cmd;
		entry.second_vid = result->param.second_vid;
		entry.second_tpid_enc = result->param.second_tpid_enc;
		entry.parity = result->param.vlan_parity;

		status = cs_fe_table_add_entry(FE_TABLE_FVLAN, &entry, 
				&vlan_entry_index);
		printk("%s:%d:alloc flow vlan index %x\n", __func__, __LINE__, vlan_entry_index);
		if (status != FE_STATUS_OK) {
			DBG(printk("%s::Alloc FVLAN Table Error!\n", __func__));
			//return CS_ERROR;
			no_vlan_result++;
			goto voqpol_result_alloc_fail;
		}
		vlan_result++;
		p_rslt->l2.flow_vlan_index = vlan_entry_index;
		p_rslt->l2.flow_vlan_op_en = 1;
	}
#endif

	status = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, p_rslt, &rslt_idx);
	if (status != FE_STATUS_OK) {
		printk("%s::alloc FWDRSLT error!\n", __func__);
	}
	fwdrslt_result++;
	DBG(printk("%s:%d:8K main forwarding rslt_idx = %d\n", __func__, __LINE__, 
			rslt_idx));

	return rslt_idx;
	
		
fvlan_rslt_alloc_fail:
	//FIXME: cs_fe_rslt_fvlantbl.c not finish
	//cs_fe_fvlan_rslt_dealloc()
	
voqpol_result_alloc_fail:
	//cs_fe_del_voqpol(&voqpol_idx);
	
l3_dip_result_alloc_fail:
	//cs_fe_l3_result_dealloc(&l3_dip_index);
		
l3_sip_result_alloc_fail:	
	//cs_fe_l3_result_dealloc(&l3_sip_index);
	
l2_result_alloc_fail:	
	//cs_fe_l2_result_dealloc(p_rslt->l2.l2_index);	
	no_avaiable_fwdrslt_entry++;

	return CS_ERROR;
}

cs_uint16 __cs_uu_flow_to_forwarding_result(fe_hash_rslt_s *result)
{
	fe_fwd_result_entry_t *p_rslt = NULL;
	cs_uint8 result_data[sizeof(fe_fwd_result_entry_t)];
	unsigned int rslt_idx, voqpol_idx;
	cs_uint8 status;
	fe_voq_pol_entry_t voq_pol_data;
	unsigned int vlan_entry_index;
	fe_flow_vlan_entry_t entry;

	p_rslt = (fe_fwd_result_entry_t *)&result_data;
	memset((void *)p_rslt, 0, sizeof(fe_fwd_result_entry_t));

	memset(&voq_pol_data, 0, sizeof(fe_voq_pol_entry_t));

	p_rslt->acl_dsbl = result->action.acl_dsbl;
	p_rslt->parity = result->action.action_parity;

	/* get voqpol table index by voq and pol id */
	printk("%s::d_voq = %d, d_pol = %d\n", __func__, result->param.d_voq,
			result->param.d_pol);
	//FIXME: voq for MCAL (i.e. root/transit, ROOT_PORT_VOQ_BASE)
	voq_pol_data.voq_base = result->param.d_voq;
	voq_pol_data.pol_base = result->param.d_pol;
	voq_pol_data.cpu_pid = result->param.cpu_pid;
	voq_pol_data.ldpid = result->param.ldpid;
	voq_pol_data.pppoe_session_id = result->param.pppoe_session_id;
	voq_pol_data.cos_nop = result->action.cos_nop;
	voq_pol_data.parity = result->param.voq_policer_parity;

	status = cs_fe_table_add_entry(FE_TABLE_VOQ_POLICER, &voq_pol_data, 
			&voqpol_idx);
	if (status != CS_OK) {
		// FIXME.. any error here?
		printk("%s::unable to obtain/allocate a voqpol idx ", __func__);
		printk("related to forwarding result entry\n");
	}

	printk("%s:%d:voqpol_idx = %d\n",__func__, __LINE__, voqpol_idx);
	p_rslt->dest.voq_pol_table_index = voqpol_idx;

	/* L2 */
	p_rslt->l2.mcgid = result->param.mcgid;
	p_rslt->l2.mcgid_valid = result->action.mcgid_valid;
	p_rslt->l2.flow_vlan_op_en = result->action.flow_vlan_op_en;

	/* Dest */
	p_rslt->dest.voq_policy = 0;//FWD_VOQ_POLICY, debug, need move this code
	p_rslt->dest.pol_policy = result->param.d_pol;//FWD_POL_POLICY

	/* Act */
	p_rslt->act.fwd_type_valid = result->action.fwd_type_valid;
	//FIXME: FWDTYPE_UU for unknown unicast
	p_rslt->act.fwd_type = result->param.fwd_type;
	p_rslt->act.drop = result->action.drop;

	/* for flow vlan */
	entry.first_vid = result->param.first_vid;
	entry.first_vlan_cmd = result->param.first_vlan_cmd;

	entry.first_tpid_enc = 0;
	entry.second_vlan_cmd = 0;
	entry.second_vid = 0;
	entry.second_tpid_enc = 0;

	status = cs_fe_table_add_entry(FE_TABLE_FVLAN, &entry, &vlan_entry_index);
	printk("%s::alloc flow vlan index %x\n", __func__, vlan_entry_index);
	if (status != FE_STATUS_OK) {
		printk("%s::Alloc FVLAN Table Error!\n", __func__);
		return FE_ERR_ENTRY_NOT_FOUND;
	}
	p_rslt->l2.flow_vlan_index = vlan_entry_index;
	p_rslt->l2.flow_vlan_op_en = 1;

	status = cs_fe_table_add_entry(FE_TABLE_FWDRSLT, p_rslt, &rslt_idx);
	if (status != FE_STATUS_OK) {
		printk("%s::alloc FWDRSLT error!\n", __func__);
	}
	DBG(printk("%s:%d:8K main forwarding rslt_idx = %d\n", __func__, __LINE__, rslt_idx));

	return rslt_idx;
}

cs_status __cs_vtable_to_vlan_table(fe_hash_rslt_s *result)
{
	// FIXME!! don't know what this function does.. 
#if 0
	cs_uint8 vlan_data[sizeof(fe_vlan_entry_t)];
	int status;
	unsigned int vlan_idx;

	result = (fe_vlan_entry_t *)&vlan_data;
	memset((void *)result, 0, sizeof(fe_vlan_entry_t));

	result->vlan_mcgid = CS_VTBL_ID_UU_FLOW;

	status = cs_fe_table_add_entry(FE_TABLE_VLAN, result, &vlan_idx);
	if (status != FE_STATUS_OK) {
		printk("%s:unable to add a VLAN entry\n", __func__);
	}

	printk("%s: VLAN membership table %d\n", __func__, vlan_idx);

#endif
	return CS_OK;
}

cs_status __cs_kernel_to_checkmem(CS_HASH_ENTRY_S *p_hash, 
		fe_hash_check_entry_t *pchkmem)
{
	cs_status status = CS_ERROR;
	cs_uint8 *mac_da = NULL, *mac_sa = NULL;
	unsigned int l2_mac_index, l2_da_mac_index, l2_sa_mac_index;
	unsigned int l3_sip_index, l3_dip_index;
	cs_uint8 mac_combo[12];
	cs_boolean is_v6 = 0;

	if ((NULL == p_hash) || (NULL == pchkmem))	  return CS_ERROR;
	
	if (p_hash->mask & CS_HM_MAC_DA_MASK) {
		pchkmem->check_mac_da_en = 1;
		mac_da = p_hash->key.mac_da;
		//rdmsg ("%s: check_mac_da_en  %x:%x:%x:%x:%x:%x\n", __FILE__, mac_da[0], mac_da[1], mac_da[2], mac_da[3], mac_da[4], mac_da[5]);
	}

	if (p_hash->mask & CS_HM_MAC_SA_MASK) {
		pchkmem->check_mac_sa_en = 1;
		mac_sa = p_hash->key.mac_sa;
		//rdmsg ("%s: check_mac_sa_en  %x:%x:%x:%x:%x:%x\n", __FILE__, mac_sa[0], mac_sa[1], mac_sa[2], mac_sa[3], mac_sa[4], mac_sa[5]);
	}
	
	if ((NULL != mac_sa) && (NULL != mac_da)) {
		memcpy(mac_combo, mac_da, 6);
		memcpy(mac_combo+6, mac_sa, 6);
		status = cs_fe_l2_result_alloc(mac_combo, L2_LOOKUP_TYPE_PAIR, 
				&l2_mac_index);
		if (status != FE_STATUS_OK) {
			//return (-1);
		}
		//rdmsg ("%s: chkmem l2_mac_index = %d\n",__func__, l2_mac_index);
		pchkmem->check_l2_check_idx = l2_mac_index;
	} else if (mac_sa != NULL) {
		status = cs_fe_l2_result_alloc(mac_sa, L2_LOOKUP_TYPE_SA, 
			&l2_sa_mac_index);
		if (status != FE_STATUS_OK) {
			//return (-1);
		}
		//rdmsg ("%s: chkmem l2_sa_mac_index = %d\n",__func__, l2_sa_mac_index);
		pchkmem->check_l2_check_idx = l2_sa_mac_index;
	} else if (mac_da != NULL) {
		status = cs_fe_l2_result_alloc(mac_da, L2_LOOKUP_TYPE_DA, 
			&l2_da_mac_index);
		if (status != FE_STATUS_OK) {
			//return (-1);
		}
		//rdmsg ("%s: chkmem l2_da_mac_index = %d\n",__func__, l2_da_mac_index);
		pchkmem->check_l2_check_idx = l2_da_mac_index;
	}

	if (p_hash->mask & CS_HM_IP_SA_MASK) {
		pchkmem->check_ip_sa_en = 1;
		is_v6 = p_hash->key.ip_version;
		//rdmsg ("%s: check_ip_sa_en  0x%x\n", __FILE__, p_hash->key.sa);
		status = cs_fe_l3_result_alloc(p_hash->key.sa, is_v6, &l3_sip_index);
		//rdmsg ("%s:--> chkmem l3_sip_index = %d\n",__func__, l3_sip_index);
		if (status != FE_STATUS_OK) {
			//return (-1);
		}
		pchkmem->check_ip_sa_check_idx = l3_sip_index;
		pchkmem->check_l2_check_idx = l2_da_mac_index;
	}

	if (p_hash->mask & CS_HM_IP_DA_MASK) {
		pchkmem->check_ip_da_en = 1;
		is_v6 = p_hash->key.ip_version;
		//rdmsg ("%s: check_ip_da_en  0x%x\n", __FILE__, p_hash->key.da);
		status = cs_fe_l3_result_alloc(p_hash->key.da, is_v6, &l3_dip_index);
		//rdmsg ("%s: chkmem l3_dip_index = %d\n",__func__, l3_dip_index);
		if (status != FE_STATUS_OK) {
			//return (-1);
		}
 		pchkmem->check_l4_sp_to_be_chk = p_hash->key.l4_sp;
	}

	if (p_hash->mask & CS_HM_L4_SP_MASK) {
		pchkmem->check_l4_sp_en = 1;
	}

	if (p_hash->mask & CS_HM_L4_DP_MASK) {
		pchkmem->check_l4_dp_en = 1;
 		pchkmem->check_l4_dp_to_be_chk = p_hash->key.l4_dp;
	}

	return status;
}/* __cs_kernel_to_checkmem */

cs_boolean __cs_kernel_check_qos_enbl(CS_HASH_ENTRY_S *p_hash)
{
	if (p_hash->mask & CS_HM_QOS_TUPLE_MASK) return TRUE;
	else return FALSE;
} /* __cs_kernel_check_qos_enbl */

cs_uint16 __cs_kernel_to_qos_result(fe_hash_rslt_s *result)
{
	fe_qos_result_entry_t *p_qos_rslt = NULL;
	cs_uint8 qos_result_data[sizeof(fe_qos_result_entry_t)];
	fe_hash_param_qos_s *p_qos_param;
	unsigned int qos_rslt_idx;
	int status;

	if (result == NULL) return 0xff;
	p_qos_param = &(result->qos_param);

	p_qos_rslt = (fe_qos_result_entry_t *)&qos_result_data;
	memset((void *)p_qos_rslt, 0, sizeof(fe_qos_result_entry_t));

	p_qos_rslt->wred_cos = p_qos_param->wred_cos;
	p_qos_rslt->voq_cos = p_qos_param->voq_cos;
	p_qos_rslt->pol_cos = p_qos_param->pol_cos;
	p_qos_rslt->premark = p_qos_param->premark;
	p_qos_rslt->change_dscp_en = p_qos_param->change_dscp_en;
	p_qos_rslt->dscp = p_qos_param->dscp;
	p_qos_rslt->dscp_markdown_en = p_qos_param->dscp_markdown_en;
	p_qos_rslt->marked_down_dscp = p_qos_param->marked_down_dscp;
	p_qos_rslt->ecn_en = p_qos_param->ecn_en;
	p_qos_rslt->top_802_1p = p_qos_param->top_802_1p;
	p_qos_rslt->marked_down_top_802_1p = p_qos_param->marked_down_top_802_1p;
	p_qos_rslt->top_8021p_markdown_en = p_qos_param->top_8021p_markdown_en;
	p_qos_rslt->top_dei = p_qos_param->top_dei;
	p_qos_rslt->marked_down_top_dei = p_qos_param->marked_down_top_dei;
	p_qos_rslt->inner_802_1p = p_qos_param->inner_802_1p;
	p_qos_rslt->marked_down_inner_802_1p =
		p_qos_param->marked_down_inner_802_1p;
	p_qos_rslt->inner_8021p_markdown_en = p_qos_param->inner_8021p_markdown_en;
	p_qos_rslt->inner_dei = p_qos_param->inner_dei;
	p_qos_rslt->marked_down_inner_dei = p_qos_param->marked_down_inner_dei;
	p_qos_rslt->change_8021p_1_en = p_qos_param->change_8021p_1_en;
	p_qos_rslt->change_dei_1_en = p_qos_param->change_dei_1_en;
	p_qos_rslt->change_8021p_2_en = p_qos_param->change_8021p_2_en;
	p_qos_rslt->change_dei_2_en = p_qos_param->change_dei_2_en;
	p_qos_rslt->parity = p_qos_param->qos_parity;

	status = cs_fe_table_add_entry(FE_TABLE_QOSRSLT, p_qos_rslt, &qos_rslt_idx);
	if (status != FE_STATUS_OK) {
		printk("%s:adding QOSRSLT entry failed!\n", __func__);
	}
	printk("%s: 128 QoS qos_rslt_idx = %d\n", __func__, qos_rslt_idx);

	return qos_rslt_idx;
} /* __cs_kernel_to_qos_result */

/*
 * This function will add QoS hash mask with QoS fields only!
 */
cs_uint8 __cs_kernel_to_hash_mask_qos_only(CS_HASH_ENTRY_S *hash_entry,
		fe_hash_mask_entry_t *mask_rule)
{
	unsigned int idx;
	int status;

	memset((void*)mask_rule, 0XFF, sizeof(fe_hash_mask_entry_t));
	mask_rule->keygen_poly_sel = 0;	// ccitt crc-16
	mask_rule->ip_sa_mask = 0;
	mask_rule->ip_da_mask = 0;

	printk("%s::mask %llx\n", __func__, hash_entry->mask);

	if (hash_entry->mask & CS_HM_8021P_1_MASK)
		mask_rule->_8021p_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_1_MASK)
		mask_rule->dei_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_8021P_2_MASK)
		mask_rule->_8021p_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_2_MASK)
		mask_rule->dei_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DSCP_MASK) {
		mask_rule->dscp_mask = CS_HASH_MASK_ENABLE;
		mask_rule->ip_vld_mask = CS_HASH_MASK_ENABLE;
	}
	if (hash_entry->mask & CS_HM_KEYGEN_POLY_SEL)
		mask_rule->keygen_poly_sel = CS_HASH_MASK_ENABLE;

	/* FIXME! does this function check for existing matched mask rule? */
	status = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, mask_rule, &idx);
	if (status != FE_STATUS_OK) {
		printk("%s:adding HASHMASK entry failed\n", __func__);
	}
	printk("%s:;hask mask index %x\n", __func__, idx);

	//rdmsg ("---> mask_index = %d\n", idx);
	//idx = mask_rule->index = 1;//FIXME: debug cs_fe_hash_mask_ram_alloc()

	hash_entry->key.mask_ptr_0_7 = idx;
	//hash_entry->key.mask_ptr_0_7 = 1;//FIXME: need update this data

	return idx;
}

/* this function is a super set of __cs_kernel_to_hash_mask +
 * QoS related field */
cs_uint8 __cs_kernel_to_qos_hash_mask(CS_HASH_ENTRY_S *hash_entry,
		fe_hash_mask_entry_t *mask_rule)
{
	unsigned int idx=1;
	int status;

	memset((void *)mask_rule, 0xFF, sizeof(fe_hash_mask_entry_t));

	mask_rule->keygen_poly_sel = 0;//ccitt crc-16
	mask_rule->ip_sa_mask = 0x0;
	mask_rule->ip_da_mask = 0x0;

	if (hash_entry->mask & CS_HM_MAC_DA_MASK)
		mask_rule->mac_da_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_MAC_SA_MASK)
		mask_rule->mac_sa_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_ETHERTYPE_MASK)
		mask_rule->ethertype_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_8021P_1_MASK)
		mask_rule->_8021p_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_1_MASK)
		mask_rule->dei_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_VID_1_MASK)
		mask_rule->vid_1_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_8021P_2_MASK)
		mask_rule->_8021p_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DEI_2_MASK)
		mask_rule->dei_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_VID_2_MASK)
		mask_rule->vid_2_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_DA_MASK)
		mask_rule->ip_da_mask = 0x080;	//FIXME:

	if (hash_entry->mask & CS_HM_IP_SA_MASK) {
		//FIXME: according to IP type to fill mask field
		mask_rule->ip_sa_mask = 0x080;//FIXME:IPv4 0x080
	}

	if (hash_entry->mask & CS_HM_IP_PROT_MASK)
		mask_rule->ip_prot_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DSCP_MASK)
		mask_rule->dscp_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_ECN_MASK)
		mask_rule->ecn_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_FRAGMENT_MASK)
		mask_rule->ip_fragment_mask = CS_HASH_MASK_ENABLE;

	// FIXME! what is this for?
	if (hash_entry->mask & CS_HM_KEYGEN_POLY_SEL)
		mask_rule->keygen_poly_sel = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_FLOW_LBL_MASK)
		mask_rule->ipv6_flow_lbl_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_VER_MASK)
		mask_rule->ip_ver_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IP_VLD_MASK)
		mask_rule->ip_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_SP_MASK) {
		// FIXME!!... port range implementation
		// Fix you now!
		// FIXME again,CH
		// 00 = Not ranged (exact matched).
		// 01 = Destination port is ranged but source port is exact matched.
		// 10 = Destination port is exact matched but source port is ranged.
		// 11 = Both destination and source are ranged.
		mask_rule->l4_ports_rngd = CS_HASH_MASK_ENABLE;
		mask_rule->l4_sp_mask = CS_HASH_MASK_ENABLE;
	}

	if (hash_entry->mask & CS_HM_L4_DP_MASK) {
		// FIXME!!... port range implementation
		// Fix you now!
		// FIXME again,CH
		// 00 = Not ranged (exact matched).
		// 01 = Destination port is ranged but source port is exact matched.
		// 10 = Destination port is exact matched but source port is ranged.
		// 11 = Both destination and source are ranged.
		mask_rule->l4_ports_rngd = CS_HASH_MASK_ENABLE;
		mask_rule->l4_dp_mask = CS_HASH_MASK_ENABLE;
	}

	if (hash_entry->mask & CS_HM_TCP_CTRL_MASK)
		mask_rule->tcp_ctrl_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_TCP_ECN_MASK)
		mask_rule->tcp_ecn_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_VLD_MASK)
		mask_rule->l4_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_LSPID_MASK)
		mask_rule->lspid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_FWDTYPE_MASK)
		mask_rule->fwdtype_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPPOE_SESSION_ID_VLD_MASK)
		mask_rule->pppoe_session_id_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPPOE_SESSION_ID_MASK)
		mask_rule->pppoe_session_id_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_RECIRC_IDX_MASK)
		mask_rule->recirc_idx_mask = CS_HASH_MASK_ENABLE;

	// FIXME: mcgid but not multicast, using vtable
	//if (hash_entry->mask & CS_HM_MCGID_MASK & (!CS_HM_MC_DA_MASK)) {
	if (hash_entry->mask & CS_HM_MCGID_MASK) {
		mask_rule->mcgid_mask = 0x10f;//enable bits[7:4]
		//rdmsg ("%s:: vtable mask_rule->mcgid_mask = 0x%x\n", __func__,
		//		mask_rule->mcgid_mask);
	}
	//rdmsg ("%s::==> mask_rule->mcgid_mask = 0x%x\n", __func__, mask_rule->mcgid_mask);

	// FIXME: multicast
	if (hash_entry->mask & CS_HM_MC_DA_MASK) {
		mask_rule->mcgid_mask = 0x1f0;
		//rdmsg ("%s:: multicast mask_rule->mcgid_mask = 0x%x\n", __func__, mask_rule->mcgid_mask);
	}

	// FIXME: broadcast
	if (hash_entry->mask & CS_HM_BC_DA_MASK)
		mask_rule->bc_da_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DA_AN_MAC_SEL_MASK)
		mask_rule->da_an_mac_sel_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_DA_AN_MAC_HIT_MASK)
		mask_rule->da_an_mac_hit_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_ORIG_LSPID_MASK)
		mask_rule->orig_lspid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_MASK)
		mask_rule->l7_field_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_VLD_MASK)
		mask_rule->l7_field_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_HDR_A_FLAGS_CRCERR_MASK)
		mask_rule->hdr_a_flags_crcerr_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L3_CHKSUM_ERR_MASK)
		mask_rule->l3_chksum_err_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L4_CHKSUM_ERR_MASK)
		mask_rule->l4_chksum_err_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_NOT_HDR_A_FLAGS_STSVLD_MASK)
		mask_rule->not_hdr_a_flags_stsvld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_HASH_FID_MASK)
		mask_rule->hash_fid_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_L7_FIELD_SEL) {
		//mask_rule->l7_field_sel = CS_HASH_MASK_ENABLE;//2 bits
	}

	if (hash_entry->mask & CS_HM_SA_BNG_MAC_SEL_MASK)
		mask_rule->sa_bng_mac_sel_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SA_BNG_MAC_HIT_MASK)
		mask_rule->sa_bng_mac_hit_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SPI_VLD_MASK)
		mask_rule->spi_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_SPI_MASK)
		mask_rule->spi_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_NDP_MASK)
		mask_rule->ipv6_ndp_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_HBH_MASK)
		mask_rule->ipv6_hbh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_RH_MASK)
		mask_rule->ipv6_rh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_IPV6_DOH_MASK)
		mask_rule->ipv6_doh_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPP_PROTOCOL_VLD_MASK)
		mask_rule->ppp_protocol_vld_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PPP_PROTOCOL_MASK)
		mask_rule->ppp_protocol_mask = CS_HASH_MASK_ENABLE;

	if (hash_entry->mask & CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK) {
		// FIXME!!
		//mask_rule->fe_hm_pktlen_rng_match_vector_mask = CS_HASH_MASK_ENABLE;//4 bits
	}

	/* FIXME! does this function check for existing matched mask rule? */
	status = cs_fe_table_add_entry(FE_TABLE_HASH_MASK, mask_rule, &idx);
	if (status != FE_STATUS_OK) {
		printk("%s:adding HASHMASK entry failed\n", __func__);
	}
	//cs_fe_hash_mask_ram_alloc(mask_rule, &idx);
	//rdmsg ("---> mask_index = %d\n", idx);
	//idx = mask_rule->index = 1;//FIXME: debug cs_fe_hash_mask_ram_alloc()

	hash_entry->key.mask_ptr_0_7 = idx;
	//hash_entry->key.mask_ptr_0_7 = 1;//FIXME: need update this data

	return idx;
} /* __cs_kernel_to_qos_hash_mask */

cs_status cs_kernel_update_hash_key_from_qos(fe_sw_hash_t *p_swhash)
{
	if (p_swhash == NULL) return CS_ERROR;

	p_swhash->_8021p_1 = 0;
	p_swhash->dei_1 = 0;
	p_swhash->_8021p_2 = 0;
	p_swhash->dei_2 = 0;
	p_swhash->dscp = 0;
	p_swhash->ecn = 0;

	return CS_OK;
} /* cs_kernel_update_hash_key_from_qos */

cs_status cs_kernel_core_del_hash_by_guid(cs_uint64 guid)
{
	struct cs_core_hash *uthash_index = NULL;
	cs_uint8 i;
	cs_uint16 module_id;
	cs_kernel_cb *cb_func;
	/* From GUID --> Hash map, find hash index , notify GUID to another module */
	core_get_users(guid, &uthash_index);
	if (uthash_index==NULL) {
		return CS_OK;
	}

	/* call back to another module notify GUID be deleted */
	for (i = 0; i < uthash_index->guid_cnt; i++) {
		guid = uthash_index->guid[i];
		module_id = (cs_uint16)(guid >> 48);
		cb_func = cs_kernel_get_cb_by_tag_id(module_id);
		if (NULL != cb_func)
			(cb_func)(guid, CS_HASH_CB_DELETE_BY_PEER);
	}
	/* delete hash entry */
	//cs_hash_invalid_by_idx(uthash_index->hash_index);
	cs_fe_hash_del_hash(uthash_index->hash_index);

	/* delete Uthash map */
	core_delete_user(uthash_index);

	return CS_OK;
}

cs_status cs_kernel_get_pktlen_rng_match_vector(cs_uint16 high,
		cs_uint16 low, cs_uint8 *p_vector)
{
	unsigned int pktlen_rng_idx = 4;
	fe_pktlen_rngs_entry_t pktlen_entry;
	cs_status status;

	pktlen_entry.high = high;
	pktlen_entry.low = low;
	pktlen_entry.valid = 1;
	status = cs_fe_table_add_entry(FE_TABLE_PKTLEN_RANGE, &pktlen_entry, 
			&pktlen_rng_idx);
	if (status != FE_STATUS_OK) {
		printk("%s::alloc PKTLEN error!\n", __func__);
	}
	/* shouldn't happen */
	if (pktlen_rng_idx > 3) return CS_ERROR;
	*p_vector = (0x01 << pktlen_rng_idx);

	return CS_OK;
} /* cs_kernel_get_pktlen_rng_match_vector */

/* get the hash key info from control block */
cs_status cs_kernel_get_hash_key_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
		fe_sw_hash_t *p_key)
{
	int i;

	if (p_key == NULL) return CS_ERROR;

	/* When field more than 8 bits, Need to take care bytes order */
	if (cs_cb->input_mask & CS_HM_MAC_DA_MASK) {
		for (i = 0; i < 6; i++)
			p_key->mac_da[i] = cs_cb->input.raw.da[5 - i];
	}

	 if (cs_cb->input_mask & CS_HM_MAC_SA_MASK) {
		for (i = 0; i < 6; i++)
			p_key->mac_sa[i] = cs_cb->input.raw.sa[5 - i];
	}

	if (cs_cb->input_mask & CS_HM_ETHERTYPE_MASK)
		p_key->eth_type = cs_cb->input.raw.eth_protocol;

	//if (cs_cb->input_mask & CS_HASH_MASK_LLC)
	//	p_key->eth_type = cs_cb->input.raw.llc_type_enc;

	// FIXME: need to take care of byte order for all of the fields
	if (cs_cb->input_mask & CS_HM_VID_1_MASK)
		p_key->vid_1 = cs_cb->input.raw.vlan_id & VLAN_VID_MASK;
#if 0
	if (cs_cb->input_mask & CS_HM_8021P_1_MASK)
		p_key->_8021p_1 = (cs_cb->input.raw.vlan_id >> 13) & 0x07;

	if (cs_cb->input_mask & CS_HM_DEI_1_MASK)
		p_key->dei_1 = (cs_cb->input.raw.vlan_id >> 12) & 0x01;
#endif
	// FIXME.. no definition for double tag VLAN
#if 0
	if (cs_cb->input_mask & CS_HM_8021P_2_MASK)
		p_key->_8021p_2 = 0;

	if (cs_cb->input_mask & CS_HM_DEI_2_MASK)
		p_key->dei_2 = 0;

	if (cs_cb->input_mask & CS_HM_VID_2_MASK)
		p_key->vid_2 = 0;
#endif

	//rdmsg ("%s::key vid %x, 8021p_1 %x, dei %x\n",
	//	__func__, p_key->vid_1, p_key->_8021p_1, p_key->dei_1);

	if (cs_cb->input_mask & CS_HM_PPPOE_SESSION_ID_MASK) {
		p_key->pppoe_session_id_valid = 1;
		p_key->pppoe_session_id = ntohs(cs_cb->input.raw.pppoe_frame);
	}

	if (cs_cb->input_mask & CS_HM_IP_VER_MASK)
		p_key->ip_version = cs_cb->input.l3_nh.iph.ver;

	if (cs_cb->input_mask & CS_HM_IP_DA_MASK) {
		if(cs_cb->input_mask & CS_HM_IPV6_MASK){
			p_key->da[0] = ntohl(cs_cb->input.l3_nh.ipv6h.dip[0]);
			p_key->da[1] = ntohl(cs_cb->input.l3_nh.ipv6h.dip[1]);
			p_key->da[2] = ntohl(cs_cb->input.l3_nh.ipv6h.dip[2]);
			p_key->da[3] = ntohl(cs_cb->input.l3_nh.ipv6h.dip[3]);
			p_key->ip_version = 1;

		}
		else{
			p_key->da[0] = ntohl(cs_cb->input.l3_nh.iph.dip);
		}
		//printk("Key: change bytes order: DIP= 0x%X, orig: dip = 0x%X\n",
			//p_key->da[0], cs_cb->input.l3_nh.iph.dip);
	}
	if (cs_cb->input_mask & CS_HM_IP_SA_MASK) {
		if(cs_cb->input_mask & CS_HM_IPV6_MASK){
			p_key->sa[0] = ntohl(cs_cb->input.l3_nh.ipv6h.sip[0]);
			p_key->sa[1] = ntohl(cs_cb->input.l3_nh.ipv6h.sip[1]);
			p_key->sa[2] = ntohl(cs_cb->input.l3_nh.ipv6h.sip[2]);
			p_key->sa[3] = ntohl(cs_cb->input.l3_nh.ipv6h.sip[3]);
			p_key->ip_version = 1;
		}
		else
			p_key->sa[0] = ntohl(cs_cb->input.l3_nh.iph.sip);

		//printk("Key: change bytes order::SIP 0x%X,orig: sip = 0x%X\n",
			//p_key->sa[0], cs_cb->input.l3_nh.iph.sip);
	}

	if(cs_cb->input_mask & CS_HM_IP_VLD_MASK)
		p_key->ip_valid = 1;

	if(cs_cb->input_mask & CS_HM_L3_CHKSUM_ERR_MASK)
		p_key->l3_csum_err = 0;

	// FIXME! not hash mask has been defined for IPv6
	//if (cs_cb->input_mask & CS_HASH_MASK_IPV6_SIP) {
	//	memcpy(p_key->sa, &cs_cb->input.l3_nh.ipv6h.sip, 16);
	//}
	//if (cs_cb->input_mask & CS_HASH_MASK_IPV6_DIP) {
	//	memcpy(p_key->da, &cs_cb->input.l3_nh.ipv6h.dip, 16);
	//}

	if (cs_cb->input_mask & CS_HM_IP_FRAGMENT_MASK)
		p_key->ip_frag = cs_cb->input.l3_nh.iph.frag;

	if (cs_cb->input_mask & CS_HM_IP_PROT_MASK)
		p_key->ip_prot = cs_cb->input.l3_nh.iph.protocol;

	if (cs_cb->input_mask & CS_HM_DSCP_MASK)
		p_key->dscp = (cs_cb->input.l3_nh.iph.tos >> 2) & 0x3f;

	if (cs_cb->input_mask & CS_HM_ECN_MASK)
		p_key->ecn = (cs_cb->input.l3_nh.iph.tos) & 0x03;

	if (cs_cb->input_mask & CS_HM_L4_DP_MASK){
		p_key->l4_dp = ntohs(cs_cb->input.l4_h.th.dport);
	}

	if (cs_cb->input_mask & CS_HM_L4_SP_MASK){
		p_key->l4_sp = ntohs(cs_cb->input.l4_h.th.sport);
	}

	/* TCP Contrl flag */
	if(cs_cb->input_mask & CS_HM_TCP_CTRL_MASK){
		if(cs_cb->input_tcp_flag_mask & CS_HASH_MASK_TCP_URG_FLAG)
			p_key->tcp_ctrl_flags |= cs_cb->input.l4_h.th.urg << 5;
		if(cs_cb->input_tcp_flag_mask & CS_HASH_MASK_TCP_PSH_FLAG)
			p_key->tcp_ctrl_flags |= cs_cb->input.l4_h.th.psh << 3;
		if(cs_cb->input_tcp_flag_mask & CS_HASH_MASK_TCP_RST_FLAG)
			p_key->tcp_ctrl_flags |= cs_cb->input.l4_h.th.rst << 2;
		if(cs_cb->input_tcp_flag_mask & CS_HASH_MASK_TCP_SYN_FLAG)
			p_key->tcp_ctrl_flags |= cs_cb->input.l4_h.th.syn << 1;
		if(cs_cb->input_tcp_flag_mask & CS_HASH_MASK_TCP_FIN_FLAG)
			p_key->tcp_ctrl_flags |= cs_cb->input.l4_h.th.fin;
	}

	if (cs_cb->input_mask & CS_HM_L4_VLD_MASK)
		p_key->l4_valid = 1;

	if(cs_cb->input_mask & CS_HM_L4_CHKSUM_ERR_MASK)
		p_key->l4_csum_err = 0;

	if (cs_cb->input_mask & CS_HM_MCIDX_MASK)
		  p_key->mc_idx = cs_cb->key_misc.mcidx;

	if (cs_cb->input_mask & CS_HM_MCIDX_MASK) {
		  p_key->mc_idx = cs_cb->key_misc.mcidx;
		  DBG(printk("%s:: p_key->mc_idx = 0x%X\n",__func__, p_key->mc_idx));
	}

	if (cs_cb->input_mask & CS_HM_MC_DA_MASK)
		  p_key->mc_da = cs_cb->key_misc.mc_da;

	if (cs_cb->input_mask & CS_HM_BC_DA_MASK)
		  p_key->bc_da = cs_cb->key_misc.bc_da;

	if (cs_cb->input_mask & CS_HM_LSPID_MASK)
		  p_key->lspid = cs_cb->key_misc.lspid;

	if ((cs_cb->input_mask & CS_HM_SPI_VLD_MASK) &&
			(cs_cb->input_mask & CS_HM_SPI_MASK)) {
		p_key->spi_vld = 1;
		p_key->spi_idx = ntohl(cs_cb->input.l4_h.ah_esp.spi);
	}

	if (cs_cb->input_mask & CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK) {
		cs_status status;

		status = cs_kernel_get_pktlen_rng_match_vector(cs_cb->key_misc.pkt_len_high,
			cs_cb->key_misc.pkt_len_low, &p_key->pktlen_rng_match_vector);
		if (status != CS_OK) return status;
	}

	// FIXME! debug message
	//rdmsg ("%s:: cs_cb->input_mask = 0x%llX\n",__func__, cs_cb->input_mask);

	if (cs_cb->input_mask & CS_HM_MCGID_MASK) {
		//p_key->mcgid = cs_cb->key_misc.mcgid;
		p_key->mcgid = ntohs(cs_cb->key_misc.mcgid);
		//rdmsg ("key_misc.mcgid = 0x%04X, ntohs(cs_cb->key_misc.mcgid) = 0x%04X\n",
		//		cs_cb->key_misc.mcgid, ntohs(cs_cb->key_misc.mcgid));
	}

	if (cs_cb->input_mask & CS_HM_ORIG_LSPID_MASK) {
		p_key->orig_lspid = cs_cb->key_misc.orig_lspid;
		//rdmsg ("p_key->orig_lspid = 0x%02X\n", p_key->orig_lspid);
	}

#if defined (CONFIG_QOS_TUPLE7) || defined (CONFIG_QOS_TUPLE6)
	if (cs_cb->input_mask & CS_HM_8021P_1_MASK)
		p_key->_8021p_1 = (cs_cb->input.raw.vlan_id >> 13) & 0x07;

	if (cs_cb->input_mask & CS_HM_DEI_1_MASK)
		p_key->dei_1 = (cs_cb->input.raw.vlan_id >> 12) & 0x01;

	if (cs_cb->input_mask & CS_HM_DSCP_MASK)
		p_key->dscp = (cs_cb->input.l3_nh.iph.tos >>2);
	printk("\t%s::Set hash key for QoS. 8021p %x, dei %x, dscp %x\n",
		__func__, p_key->_8021p_1, p_key->dei_1, p_key->dscp);
#endif


	//FIXME: temp add for debug, CH
	//p_key->mask_ptr_0_7 = 1;

	return CS_OK;
} /* cs_kernel_get_hash_key_from_cs_cb */

cs_status cs_kernel_get_hash_mask_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
		cs_uint64 *p_mask)
{
	if (p_mask == NULL) return CS_ERROR;

	(*p_mask) = cs_cb->input_mask;

	return CS_OK;
}

cs_status cs_kernel_get_hash_action_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
		fe_hash_rslt_s *result)
{
	cs_uint8 i;

	if (result == NULL) return CS_ERROR;

	//rdmsg ("%s:cs_cb->output_mask = 0x%llX\n", __func__, cs_cb->output_mask);
	if (cs_cb->output_mask & CS_HM_MAC_DA_MASK) {
		for (i = 0; i < 6; i++)
			result->param.mac_da[i] = cs_cb->output.raw.da[5 - i];

		result->action.mac_da_replace_en = CS_RESULT_ACTION_ENABLE;

		// FIXME! debug message
		//rdmsg ("MAC DA: ");
		//rdmsg for (i = 0; i < 6; i++)
			//rdmsg ("%2X ",result->param.mac_da[i]);
		//rdmsg ("\n");
	}

	if (cs_cb->output_mask & CS_HM_MAC_SA_MASK) {
		for (i = 0; i < 6; i++)
			result->param.mac_sa[i] = cs_cb->output.raw.sa[5-i];

		result->action.mac_sa_replace_en = CS_RESULT_ACTION_ENABLE;

		// FIXME! debug message
		//rdmsg ("MAC SA: ");
		//rdmsg for (i = 0; i < 6; i++)
			//rdmsg ("%2X ",result->param.mac_sa[i]);
		//rdmsg ("\n");
	}
#if 0

	if (cs_cb->output_mask & CS_HM_8021P_1_MASK) {
		result->param.first_vid = ntohs(cs_cb->output.raw.vlan_id & 0x7000);
		result->action.flow_vlan_op_en = CS_RESULT_ACTION_ENABLE;
	}
	if (cs_cb->output_mask & CS_HM_DEI_1_MASK) {
		result->param.first_vid = ntohs(cs_cb->output.raw.vlan_id & 0x8000);
		result->action.flow_vlan_op_en = CS_RESULT_ACTION_ENABLE;
	}
#endif
	if (cs_cb->output_mask & CS_HM_VID_1_MASK) {
		result->param.first_vid = ntohs(cs_cb->output.raw.vlan_id & 0x0FFF);
		//FIXME: action need from kernel layer
		//result->action.flow_vlan_op_en = CS_RESULT_ACTION_ENABLE;
		//if (cs_cb->action.l2.flow_vlan_op_en == CS_PPPOE_OP_INSERT) {
		//	result->action.flow_vlan_op_en = cs_cb->action.l2.flow_vlan_op_en;
		//} else if (cs_cb->action.l2.flow_vlan_op_en == CS_PPPOE_OP_REMOVE) {
		//	result->action.flow_vlan_op_en = cs_cb->action.l2.flow_vlan_op_en;
		//}
	}

	if (cs_cb->output_mask & CS_HM_PPPOE_SESSION_ID_MASK) {
		result->param.pppoe_session_id = cs_cb->output.raw.pppoe_frame;
		if (cs_cb->action.l2.pppoe_op_en == CS_PPPOE_OP_INSERT) {
			DBG(printk("/////////////////////////////////Force Insert PPPoE head!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"));
			result->param.pppoe_session_id = ntohs(cs_cb->output.raw.pppoe_frame);
			result->action.pppoe_encap_en = cs_cb->action.l2.pppoe_op_en;
			result->action.pppoe_encap_en = 1;
			result->action.pppoe_decap_en = 0;

		} else if (cs_cb->action.l2.pppoe_op_en == CS_PPPOE_OP_REMOVE) {
			DBG(printk("/////////////////////////////////Force Remove PPPoE head!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"));
			cs_cb->output.raw.pppoe_frame = cs_cb->input.raw.pppoe_frame;
			result->param.pppoe_session_id = cs_cb->output.raw.pppoe_frame;
			result->action.pppoe_encap_en = 0;
			result->action.pppoe_decap_en = 1;
		}
	}
#if 0
	/* FIXME, remove this when pppoe adaptor put necessary information */
	if(cs_cb->output.l3_nh.iph.dip == 0x0a03a8c0){
		printk("Force remove PPPoE head!\n");
		cs_cb->output.raw.pppoe_frame = cs_cb->input.raw.pppoe_frame;
		result->param.pppoe_session_id = cs_cb->output.raw.pppoe_frame;
		result->action.pppoe_encap_en = 0;
		result->action.pppoe_decap_en = 1;
	}
	if(cs_cb->input.l3_nh.iph.dip == 0xae3da8c0){
		printk("Force Insert PPPoE head!\n");
		result->param.pppoe_session_id = ntohs(cs_cb->output.raw.pppoe_frame);
		result->action.pppoe_encap_en = cs_cb->action.l2.pppoe_op_en;
		result->action.pppoe_encap_en = 1;
		result->action.pppoe_decap_en = 0;
	}
#endif


	if (cs_cb->output_mask & CS_HM_IP_SA_MASK) {
		//rdmsg ("Oringal: SIP 0x%X\n",cs_cb->output.l3_nh.iph.sip);
		result->param.sip_addr[0] = ntohl(cs_cb->output.l3_nh.iph.sip);
		result->action.ip_sa_replace_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("Change SIP to: sip_addr = 0x%x\n", result->param.sip_addr[0]);
	}
	if (cs_cb->output_mask & CS_HM_IP_DA_MASK) {
		//rdmsg ("Oringal: DIP 0x%X\n",cs_cb->output.l3_nh.iph.dip);
		result->param.dip_addr[0] = ntohl(cs_cb->output.l3_nh.iph.dip);
		result->action.ip_da_replace_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("Change SIP to: dip_addr = 0x%x\n", result->param.dip_addr[0]);
	}
	//if (cs_cb->output_mask & CS_HASH_MASK_IPV6_SIP) {
	//	memcpy(result->param.sip_addr, &cs_cb->output.l3_nh.ipv6h.sip, 32);
	//	result->action.ip_sa_replace_en = CS_RESULT_ACTION_ENABLE;
	//}
	//if (cs_cb->output_mask & CS_HASH_MASK_IPV6_DIP) {
	//	 memcpy(result->param.dip_addr, cs_cb->output.l3_nh.ipv6h.dip, 32);
	//	 result->action.ip_da_replace_en = CS_RESULT_ACTION_ENABLE;
	//}
	if (cs_cb->output_mask & CS_HM_L4_SP_MASK) {
		result->param.sport = ntohs(cs_cb->input.l4_h.th.sport);
		result->action.sp_replace_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("CS_HM_L4_SP_MASK: result->param.mac_da = 0x%x\n", result->param.sport);
	}
	if (cs_cb->output_mask & CS_HM_L4_DP_MASK) {
		result->param.dport = ntohs(cs_cb->output.l4_h.th.dport);
		result->action.dp_replace_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("CS_HM_L4_DP_MASK: result->param.mac_da = 0x%x\n", result->param.sport);
	}
	//rdmsg ("%s:: cs_cb->key_misc.mcgid = 0x%X,cs_cb->action.misc.mcgid =0x%X \n",
	//	__func__, cs_cb->key_misc.mcgid, cs_cb->action.misc.mcgid);
	if (cs_cb->output_mask & CS_HM_MCGID_MASK) {
		//rdmsg ("00: cs_cb->action.misc.mcgid = 0x%X \n",cs_cb->action.misc.mcgid);
		result->param.mcgid = cs_cb->action.misc.mcgid;
		result->action.mcgid_valid = CS_RESULT_ACTION_ENABLE;
	}

#if 1	// WEN
	/* flow VLAN */
	if ((cs_cb->output_mask & (CS_HM_8021P_1_MASK|CS_HM_DEI_1_MASK|CS_HM_VID_1_MASK)) ||
		(cs_cb->input_mask & (CS_HM_8021P_1_MASK|CS_HM_DEI_1_MASK|CS_HM_VID_1_MASK)))	{
		result->param.first_vid = cs_cb->output.raw.vlan_id & 0x0fff;
		/* we need a better logic to determine vlan operations
		 * for priority and double tagged
		 */
		switch (cs_cb->action.l2.flow_vlan_op_en) {
			case CS_VLAN_OP_INSERT:
				result->param.first_vlan_cmd = 6;
				//result->param.first_vlan_cmd = 10;
				break;
			case CS_VLAN_OP_REMOVE:		// ??
				result->param.first_vlan_cmd = 15;
				break;
			case CS_VLAN_OP_REPLACE:
				result->param.first_vlan_cmd = 1;
				break;
			default:
				result->param.first_vlan_cmd = 0;
				break;
		}
		result->action.flow_vlan_op_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("\t***%s::cs_cb vlan operation %d, cmd %x, egress vid %x, input mask %x, output mask %x\n",
		//	__func__, cs_cb->action.l2.flow_vlan_op_en, result->param.first_vlan_cmd,
		//	(int)result->param.first_vid, (int)cs_cb->input_mask, (int)cs_cb->output_mask);
	}

#if 0
		switch (cs_cb->action.l2.flow_vlan_op_en) {
			case CS_VLAN_OP_REMOVE:
				result->param.first_vlan_cmd = 15;
			default:
				break;
		}
		result->action.flow_vlan_op_en = CS_RESULT_ACTION_ENABLE;
		//rdmsg ("\t***%s::cs_cb input mask %x, vlan op %d\n",
			__func__, cs_cb->input_mask, cs_cb->action.l2.flow_vlan_op_en);
#endif
#endif

	if (cs_cb->key_misc.fwd_type != CS_FWD_NORMAL) {
		result->param.fwd_type = cs_cb->key_misc.fwd_type;
		result->action.fwd_type_valid = CS_RESULT_ACTION_ENABLE;
	}
	if (cs_cb->action.misc.drop != CS_NOT_DROP) {
		result->action.drop = CS_RESULT_ACTION_ENABLE;
	}
	if (cs_cb->action.acl_dsbl != CS_ACL_DISABLE) {
		result->action.acl_dsbl = CS_RESULT_ACTION_ENABLE;
	}
	if (cs_cb->common.dec_ttl == CS_DEC_TTL_ENABLE) {
		result->action.decr_ttl_hoplimit = CS_RESULT_ACTION_ENABLE;
		//printk("result->action.decr_ttl_hoplimit Enable\n");
	}
	if (cs_cb->output_mask & CS_HM_MCIDX_MASK) {
		printk("No this field in Main Forawarding Results, in NI.MCAL\n");
		//result->param.mcgid = cs_cb->action.misc.mcgid;
		//result->action.mcgid_valid = CS_RESULT_ACTION_ENABLE;
	}
	if (cs_cb->action.voq_pol.d_voq_id != CS_DEFAULT_VOQ) {
		result->param.d_voq = cs_cb->action.voq_pol.d_voq_id;
		//rdmsg ("%s: result->param.d_voq = %d\n",__func__, result->param.d_voq);
	}
	if (cs_cb->action.voq_pol.d_pol_id != CS_VOQ_POL_DEFAULT) {
		//rdmsg ("result->param.d_pol = %d\n",result->param.d_pol);
		result->param.d_pol = cs_cb->action.voq_pol.d_pol_id;
	}
	if (cs_cb->action.voq_pol.cpu_pid != CS_DEFAULT_VOQ) {
		result->param.cpu_pid = cs_cb->action.voq_pol.cpu_pid;
	}
	if (cs_cb->action.voq_pol.ldpid != CS_VOQ_POL_DEFAULT) {
		result->param.ldpid = cs_cb->action.voq_pol.ldpid;
	}
	if (cs_cb->action.voq_pol.pppoe_session_id != CS_DEFAULT_VOQ) {
//		result->param.pppoe_session_id = cs_cb->action.voq_pol.pppoe_session_id;
	}
	if (cs_cb->action.voq_pol.cos_nop != CS_VOQ_POL_DEFAULT) {
		result->action.cos_nop = cs_cb->action.voq_pol.cos_nop;
	}
	if (cs_cb->action.voq_pol.voq_policer_parity != CS_VOQ_POL_DEFAULT) {
		result->param.voq_policer_parity = cs_cb->action.voq_pol.voq_policer_parity;
	}
	//FIXME: do we need cs_cb->common.mask_ptr ? this field
	if (cs_cb->common.mask_ptr != CS_MASK_PTR_DEFAULT) {
		result->index.mask_ptr_index = cs_cb->common.mask_ptr;
	}
#ifdef CS_UU_TEST
	if (cs_cb->common.uu_flow_enable == CS_RESULT_ACTION_ENABLE)
		result->param.uu_flow_enable = CS_RESULT_ACTION_ENABLE;
#endif
#if 0		// SKIP for now.. no QoS mapping support
	#if defined (CONFIG_QOS_TUPLE7) || defined (CONFIG_QOS_TUPLE6)
		// qos_hash_test
	{
		cs_uint8 i_pri =((cs_cb->input.raw.vlan_id >> 13) & 0x07);
		cs_uint8 i_dscp = (cs_cb->input.l3_nh.iph.tos>>2);
		cs_uint8 e_pri, e_dscp;

		cs_status status = cs_fe_get_qos_mapping_result(i_pri, i_dscp, &e_pri, &e_dscp);
		if (e_pri & QOS_ENTRY_VALID_MASK) {
			result->qos_param.top_802_1p = e_pri & _8021P_MASK;
			//result->qos_param.inner_802_1p = e_pri & _8021P_MASK;
			result->qos_param.change_8021p_1_en = CS_RESULT_ACTION_ENABLE;
		}
		if (e_dscp & QOS_ENTRY_VALID_MASK) {
			result->qos_param.dscp = e_dscp & DSCP_MASK;
			result->qos_param.change_dscp_en = CS_RESULT_ACTION_ENABLE;
		}
		printk("\t%s::QoS result %p, pri %x, dscp %x\n",
			__func__, result, result->qos_param.inner_802_1p, result->qos_param.dscp);
	}
#endif
#endif
	//rdmsg ("%s::result->index.mask_ptr_index = %d\n",__func__, result->index.mask_ptr_index);
	return CS_OK;

} /* cs_kernel_get_hash_action_from_cs_cb */

cs_status cs_kernel_get_module_callback(cs_uint64 *p_guid_array,
		cs_uint8 guid_cnt, 	cs_uint16 status)
{
	cs_uint64 guid;
	cs_uint16 module_id;
	cs_uint8 i;
	cs_kernel_cb *cb_func;

	for (i = 0; i < guid_cnt; i++) {
		guid = *p_guid_array;
		module_id = (cs_uint16)(guid >> 48);
		cb_func = cs_kernel_get_cb_by_tag_id(module_id);
		if (NULL != cb_func)
			(cb_func)(guid, status);
	}

	return CS_OK;
} /* cs_kernel_get_module_callback */

cs_status cs_kernel_input_set_cb(struct sk_buff *skb)
{
	struct ethhdr *eth;
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	CORE_CFG_T *cfg;
//	NI_DEV_T *gmacdev;
#ifdef CS_NE_TEST_NAT
	struct iphdr *iph;
	//struct tcphdr *th = tcp_hdr(skb);
	struct tcphdr *th;
#endif
	cfg = (CORE_CFG_T *)&core_cfg;
	//gmacdev = cs_kernel_core_find_gmacdev(skb->dev);
	//if (gmacdev == NULL) {
		//printk("Error:%s: Dev is not HW device\n", __func__);
		//return CS_ERROR;
	//}

	cs_cb = (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);

	cs_cb->common.tag = CS_CB_TAG;
	cs_cb->common.sw_only = CS_SWONLY_DONTCARE;
	cs_cb->common.module_mask = 0;
	//cs_cb->common.module_mask = CS_MOD_MASK_TO_VTABLE;
#ifdef MULTIPLE_VTABLE
	cs_cb->input_mask |= CS_HM_MCGID_MASK;
#endif

	cs_cb->key_misc.mcidx = 0;
	//cs_cb->key_misc.lspid = 0;
	cs_cb->key_misc.fwd_type = CS_FWD_NORMAL;//default forward type
	cs_cb->action.misc.drop = CS_NOT_DROP;
	cs_cb->common.input_dev = skb->dev;
	//FIXME: GMAC 1, voq 8~15
	//cs_cb->common.dvoq = 8;
	//FIXME: sw_action_id
	cs_cb->common.sw_action_id = 0;
	cs_cb->action.acl_dsbl = CS_ACL_DISABLE;
	cs_cb->common.dec_ttl = CS_DEC_TTL;

	eth = eth_hdr(skb);
	memcpy(cs_cb->input.raw.da, eth->h_dest, ETH_ALEN);
	memcpy(cs_cb->input.raw.sa, eth->h_source, ETH_ALEN);

#ifdef MULTIPLE_VTABLE
	cs_cb->action.voq_pol.d_voq_id = CS_DEFAULT_ROOT_VOQ;
#else
	cs_cb->action.voq_pol.d_voq_id = CS_DEFAULT_VOQ;
#endif

	cs_cb->action.voq_pol.d_pol_id = CS_VOQ_POL_DEFAULT;
	cs_cb->action.voq_pol.cpu_pid = CS_VOQ_POL_DEFAULT;
	cs_cb->action.voq_pol.ldpid = CS_VOQ_POL_DEFAULT;
	cs_cb->action.voq_pol.pppoe_session_id = CS_VOQ_POL_DEFAULT;
	cs_cb->action.voq_pol.cos_nop = CS_VOQ_POL_DEFAULT;
	cs_cb->action.voq_pol.voq_policer_parity = CS_VOQ_POL_DEFAULT;
	cs_cb->common.mask_ptr = CS_MASK_PTR_DEFAULT;
	// FIXME: First virtual table
	//cs_cb->common.vtype = CS_VTBL_ID_L2_FLOW;
	cs_cb->key_misc.mcgid = 0;
	//FIXME: mcgid,  set  first virtual table
	if (cs_cb->common.vtype == CS_VTBL_ID_L2_RULE) {
		cs_cb->key_misc.mcgid = CS_VTBL_ID_L2_RULE;
	}
	if (cs_cb->common.vtype == CS_VTBL_ID_L2_FLOW) {
		cs_cb->key_misc.mcgid = CS_VTBL_ID_L2_FLOW;
	}
	if (cs_cb->common.vtype == CS_VTBL_ID_L3_RULE) {
		cs_cb->key_misc.mcgid = CS_VTBL_ID_L2_RULE;
	}
	if (cs_cb->common.vtype == CS_VTBL_ID_L3_FLOW) {
		cs_cb->key_misc.mcgid = CS_VTBL_ID_L2_FLOW;
	}

	DBG(printk("cs_cb->common.vtype = 0x%X, cs_cb->key_misc.mcgid = 0x%03X\n",
			cs_cb->common.vtype, cs_cb->key_misc.mcgid));

	return CS_OK;
} /* cs_kernel_input_set_cb */



NI_DEV_T *cs_kernel_core_find_gmacdev(struct net_device *device)
{
	int i;

	for (i=0; i < GE_PORT_NUM; i++) {
		if (core_cfg.gmacDev[i].dev == device)
			return &(core_cfg.gmacDev[i]);
	}
	return NULL;
} /* cs_kernel_core_find_gmacdev */

/* adding hash from control block info stored in the skb */
cs_uint32 __cs_kernel_core_add_hash_by_skb(struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	CS_KERNEL_ACCEL_CB_T tmp_cb;
	cs_uint8 status;

	cs_cb = (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);

	if (cs_cb->common.tag != CS_CB_TAG) {
		DBG(printk("----> Not Come from GE\n"));
		return 0;
	}

	//FIXME: mac address need from kernel
	if(memcmp(cs_cb->input.raw.da,skb->data, ETH_ALEN)!=0) {
		cs_cb->output_mask |= CS_HM_MAC_DA_MASK;
		memcpy(cs_cb->output.raw.da, skb->data, ETH_ALEN);
	}
	if(memcmp(cs_cb->input.raw.sa,skb->data + ETH_ALEN, ETH_ALEN)!=0) {
		cs_cb->output_mask |= CS_HM_MAC_SA_MASK;
		memcpy(cs_cb->output.raw.sa, skb->data + ETH_ALEN , ETH_ALEN);
	}

	/* Skip if software only */
	if (cs_cb->common.sw_only == CS_SWONLY_DONTCARE ||
		cs_cb->common.sw_only == CS_SWONLY_STATE) {
		DBG(printk("Skip SW only\n"));
		return 0;
	}

#ifdef NEW_L2_DESIGN
	// FIXME, need re-consider PPPoE flow .
	if(!(cs_cb->common.module_mask & (CS_MOD_MASK_NAT|CS_MOD_MASK_BRIDGE 
		| CS_MOD_MASK_IPV4_MULTICAST | CS_MOD_MASK_IPV6_MULTICAST)))
		return 0;

	/* 1. Create MAC SA no matter L2 or L3 */
	memset(&tmp_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	tmp_cb.input_mask |= CS_HM_MAC_SA_MASK;
	tmp_cb.input_mask |= CS_HM_LSPID_MASK;
	memcpy(&tmp_cb.input.raw.sa , cs_cb->input.raw.sa, ETH_ALEN);

	tmp_cb.common.sw_only = CS_SWONLY_HW_ACCEL;
	tmp_cb.common.vtype = cs_cb->common.vtype;
	/* Output , parameter and action */
	/* FIXME:
	 * Only support two ports bridging currently. IF you want to bridge three interfaces together,
	 * we must know which port to flood to(ROOT_PORT_VOQ_BASE & CS_FWD_UU).
	 */
	if(cs_cb->common.module_mask & CS_MOD_MASK_BRIDGE) {		/* Forward to another port if bridge case */
		tmp_cb.action.voq_pol.d_voq_id = cs_cb->common.pspid * 8; /* ROOT Q, voq: 40 */
	}
	else {
		tmp_cb.action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE;	/* Send to CPU if not L2 bridge case */
	}
	//printk("cs_cb->common.module_mask = 0x%llX\n", cs_cb->common.module_mask);
	tmp_cb.key_misc.lspid = cs_cb->key_misc.orig_lspid;
	tmp_cb.key_misc.fwd_type = CS_FWD_NORMAL;
	
	if(cs_cb->tmo.interval > 0)
		tmp_cb.tmo.interval = cs_cb->tmo.interval;
	else
		tmp_cb.tmo.interval = 300;

	status = __cs_kernel_core_add_hash_by_cb(&tmp_cb);
	if (status == !CS_HASH_ADD_SUCCESSFUL) {
		printk("%s:: CS_HASH_CB_FAILED_RESOURCE\n");
		return 0;
	}

	if(cs_cb->common.module_mask & CS_MOD_MASK_BRIDGE){
		if(!cs_cb->output.raw.l2_flood){		/* Create tuple1 if not flooding(know DA)  */
			/* 2. Create T0(SA) and T1(DA) it's not flooding */
			memset(&tmp_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
			tmp_cb.input_mask |= CS_HM_MAC_DA_MASK;
			memcpy(&tmp_cb.input.raw.da , cs_cb->input.raw.da, ETH_ALEN);

			tmp_cb.common.sw_only = CS_SWONLY_HW_ACCEL;
			tmp_cb.common.vtype = cs_cb->common.vtype;
			/* Output , parameter and action */
			tmp_cb.action.voq_pol.d_voq_id = cs_cb->common.pspid * 8; /* Output voq */
			tmp_cb.key_misc.fwd_type = CS_FWD_NORMAL;
			if(cs_cb->tmo.interval > 0)
				tmp_cb.tmo.interval = cs_cb->tmo.interval;
			else
				tmp_cb.tmo.interval = 300;
			__cs_kernel_core_add_hash_by_cb(&tmp_cb);
			DBG(printk("create known DA\n"));
		}
	}
		else if(cs_cb->common.module_mask & (CS_MOD_MASK_NAT | CS_MOD_MASK_IPV6_ROUTING 
			| CS_MOD_MASK_IPV4_MULTICAST | CS_MOD_MASK_IPV6_MULTICAST)) {
		/* 3. Create T0(SA) and T2(Layer3 rule) */
		return __cs_kernel_core_add_hash_by_cb(cs_cb);		/* Create Tuple2 for L3 rule */
	}
	return 0;
	
#else	
	return __cs_kernel_core_add_hash_by_cb(cs_cb);
#endif
}

/* NEED TO VERIFY THE FOLLOWING IS THE SAME AS ADD_HASH_BY CB */
#if 0
cs_uint32 cs_kernel_core_add_hash_by_skb(struct sk_buff *skb)
{
	CS_HASH_ENTRY_S *hash_entry;
	cs_uint8 hash_data[sizeof(CS_HASH_ENTRY_S)];
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_status status;
	cs_uint64 guid = 0;
	cs_uint16 module_id;
	cs_uint8 i, ipsec_existed = 0, mc_existed = 0;

	struct iphdr *iph;
	struct tcphdr *th = tcp_hdr(skb);
	iph = ip_hdr(skb);

	cs_cb = (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);

	/* check if it is sw_only */
	if (cs_cb->common.sw_only == CS_SWONLY_DONTCARE ||
			cs_cb->common.sw_only == CS_SWONLY_STATE) {
		//rdmsg ("%s:: SW only ??????????????????????????? \n",__func__);
		return 0;
	}

	//cs_cb->input_mask |= CS_HM_IP_DA_MASK|CS_HM_IP_SA_MASK|
	//		CS_HM_IP_PROT_MASK|CS_HM_L4_DP_MASK|CS_HM_L4_SP_MASK;

	//cs_cb->input_mask = CS_HM_IP_DA_MASK | CS_HM_L4_DP_MASK | CS_HM_L4_SP_MASK;
	//rdmsg ("%s:: ====> Force cs_cb->input_mask = 0x%X\n",__func__, cs_cb->input_mask);

	//cs_cb->output_mask |= CS_HM_MAC_DA_MASK | CS_HM_MAC_SA_MASK;

	//rdmsg ("%s::IN cs_cb->output_mask = 0x%X, cs_cb->tag = 0x%X\n",__func__,
	//	cs_cb->output_mask, cs_cb->common.tag);

#ifdef CS_NE_TEST_NAT
	cs_cb->input_mask |= CS_HM_IP_DA_MASK | CS_HM_IP_SA_MASK |
			CS_HM_IP_PROT_MASK|CS_HM_L4_DP_MASK|CS_HM_L4_SP_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	cs_cb->common.vtype = CS_VTBL_ID_L3_FLOW;
	cs_cb->common.output_dev = (cs_uint32)skb->dev;
	cs_cb->output.l3_nh.iph.sip = iph->saddr;
	cs_cb->output.l3_nh.iph.dip = iph->daddr;
	cs_cb->output.l4_h.th.sport = th->dest;;
	cs_cb->output.l4_h.th.dport = th->source;
	cs_cb->action.l2.mac_sa_replace_en = 1;
	cs_cb->action.l2.mac_da_replace_en = 1;
	cs_cb->action.l3.ip_sa_replace_en = 1;
	cs_cb->action.l3.ip_da_replace_en = 1;
	cs_cb->action.l4.sp_replace_en = 1;
	cs_cb->action.l4.dp_replace_en = 1;
#endif /* CS_NE_TEST_NAT */

	//rdmsg ("%s:cs_cb->action.voq_pol.d_voq_id = %d",
	//			__func__,cs_cb->action.voq_pol.d_voq_id);
	if (cs_cb->action.voq_pol.d_voq_id == CS_DEFAULT_VOQ) {
		if (cs_cb->common.pspid == 0) {
			cs_cb->action.voq_pol.d_voq_id = 0;//GE-0
		}
		if (cs_cb->common.pspid == 1) {
			cs_cb->action.voq_pol.d_voq_id = 8;//GE-1
		}
		if (cs_cb->common.pspid == 2) {
			cs_cb->action.voq_pol.d_voq_id = 16;//GE-2
		}
	}
	//rdmsg ("%s::IN cs_cb->output_mask = 0x%X, voq_id = %d\n",__func__,
	//	cs_cb->output_mask, cs_cb->action.voq_pol.d_voq_id);
	if (cs_cb->common.tag != CS_CB_TAG) {
		DBG(printk("---> Not Come from GE\n"));
		return 0;
	}
	//FIXmE: mac address need from kernel
	memcpy(cs_cb->output.raw.da, skb->data, 6);
	memcpy(cs_cb->output.raw.sa, skb->data + 6 , 6);


	//FIXME: IPSEC and MC priority ?
	for (i = 0; i < cs_cb->common.guid_cnt; i++) {
		guid = cs_cb->common.guid[i];
		module_id = (cs_uint16)(guid >> 48);
		if (module_id == CS_KERNEL_IPSEC_ID_TAG) {
			ipsec_existed = 1;
			break;
		}
		if (module_id == CS_KERNEL_MC_ID_TAG) {
			mc_existed = 1;
			break;
		}
	}

	/* IPsec create hash entry */
	if (cs_cb->common.module_mask & CS_MOD_MASK_IPSEC || ipsec_existed) {
		//cs_hw_accel_ipsec_create_hash(skb, guid);
		return 0;
	}

	/* Multicast create hash entry */
	if ((cs_cb->common.module_mask & CS_MOD_MASK_IPV4_MULTICAST) ||
			(cs_cb->common.module_mask & CS_MOD_MASK_IPV6_MULTICAST) ||
			 mc_existed) {
		//FIXME:
		//cs_hw_accel_mc_create_hash(skb, guid);
	}


	hash_entry = (CS_HASH_ENTRY_S *)&hash_data;
	memset((void *)hash_entry, 0, sizeof(CS_HASH_ENTRY_S));

	status = __cs_kernel_get_hash_mask_from_cs_cb(cs_cb, &hash_entry->mask);
	if (CS_OK != status) return CS_HASH_ADD_ERR_SKB_CB;
	status = cs_kernel_get_hash_key_from_cs_cb(cs_cb, &hash_entry->key);
	if (CS_OK != status) return CS_HASH_ADD_ERR_SKB_CB;
	status = cs_kernel_get_hash_action_from_cs_cb(cs_cb, &hash_entry->result);
	if (CS_OK != status) return CS_HASH_ADD_ERR_SKB_CB;
	//FIXME: need management tuple, from vtable,cs_vtable_add_sdb_tuple(1, 2, mask_ptr, vtable);
	//cs_vtable_add_sdb_tuple(1, 2, mask_ptr, l3_flow_vtable);


	return cs_kernel_core_add_hash(hash_entry, cs_cb->common.vtype,
			cs_cb->common.guid, cs_cb->common.guid_cnt, cs_cb->tmo.interval);

} /* cs_kernel_core_add_hash_by_skb */
#endif
