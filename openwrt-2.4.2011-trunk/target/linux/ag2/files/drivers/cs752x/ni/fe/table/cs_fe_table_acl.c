/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_fe_table_acl.c
 *
 * $Id: cs_fe_table_acl.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * It contains the implementation for HW FE ACL Table Resource Management.
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include "cs_fe.h"
#include "cs_fe_table_int.h"
#include "cs_fe_hw_table_api.h"
#include "cs_fe_table_generic.h"
#include "cs752x_ioctl.h"

static cs_fe_table_t cs_fe_acl_table_type;

#define FE_ACL_TABLE_PTR	(cs_fe_acl_table_type.content_table)
#define FE_ACL_LOCK		&(cs_fe_acl_table_type.lock)

static int fe_acl_alloc_entry(unsigned int *rslt_idx, unsigned int start_offset)
{
	return fe_table_alloc_entry(&cs_fe_acl_table_type, rslt_idx,
			start_offset);
} /* fe_acl_alloc_entry */

static int convert_sw_acl_rule_to_data_register(fe_acl_rule_entry_t *entry,
		__u32 *p_data_array, unsigned int size)
{
	__u32 value[4] = {0, 0, 0, 0};
	memset(p_data_array, 0x0, size * 4);

	if (entry->rule_valid == 1) {
		((__u8*)value)[0] = entry->l2.l2_mac_da[0];
		((__u8*)value)[1] = entry->l2.l2_mac_da[1];
		((__u8*)value)[2] = entry->l2.l2_mac_da[2];
		((__u8*)value)[3] = entry->l2.l2_mac_da[3];
		((__u8*)value)[4] = entry->l2.l2_mac_da[4];
		((__u8*)value)[5] = entry->l2.l2_mac_da[5];
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L2_MAC_DA, value, p_data_array, size);

		((__u8*)value)[0] = entry->l2.l2_mac_sa[0];
		((__u8*)value)[1] = entry->l2.l2_mac_sa[1];
		((__u8*)value)[2] = entry->l2.l2_mac_sa[2];
		((__u8*)value)[3] = entry->l2.l2_mac_sa[3];
		((__u8*)value)[4] = entry->l2.l2_mac_sa[4];
		((__u8*)value)[5] = entry->l2.l2_mac_sa[5];
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L2_MAC_SA, value, p_data_array, size);

		value[0] = (__u32)entry->l2.eth_type;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ETHERTYPE, value, p_data_array, size);

		value[0] = (__u32)entry->l2.len_encoded;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LEN_ENCODED, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.tpid_1_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_1_VLD, value, p_data_array, size);

		value[0] = (__u32)entry->l2.tpid_enc_1;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_ENC_1, value, p_data_array, size);

		value[0] = (__u32)entry->l2.vid_1;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_VID_1, value, p_data_array, size);

		value[0] = (__u32)entry->l2._8021p_1;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_8021P_1, value, p_data_array, size);

		value[0] = (__u32)entry->l2.dei_1;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DEI_1, value, p_data_array, size);

		value[0] = (__u32)entry->l2.tpid_2_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_2_VLD, value, p_data_array, size);

		value[0] = (__u32)entry->l2.tpid_enc_2;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_ENC_2, value, p_data_array, size);

		value[0] = (__u32)entry->l2.vid_2;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_VID_2, value, p_data_array, size);

		value[0] = (__u32)entry->l2._8021p_2;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_8021P_2, value, p_data_array, size);

		value[0] = (__u32)entry->l2.dei_2;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DEI_2, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_VLD, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_VER, value, p_data_array, size);

		value[0] = (__u32)entry->l3.da[0];
		value[1] = (__u32)entry->l3.da[1];
		value[2] = (__u32)entry->l3.da[2];
		value[3] = (__u32)entry->l3.da[3];
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_DA, value, p_data_array, size);

		value[0] = (__u32)entry->l3.sa[0];
		value[1] = (__u32)entry->l3.sa[1];
		value[2] = (__u32)entry->l3.sa[2];
		value[3] = (__u32)entry->l3.sa[3];
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_SA, value, p_data_array, size);

		value[0] = (__u32)entry->l3.dscp;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DSCP, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ecn;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ECN, value, p_data_array, size);

		value[0] = (__u32)entry->l3.proto;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_PROT, value, p_data_array, size);

		value[0] = (__u32)entry->l3.fragment;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_FRAGMENT, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.fragment;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_OPTIONS, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ipv6_flow_label;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_FLOW_LBL, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ttl_hoplimit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TTL_HOPLMT, value, p_data_array, size);

		value[0] = (__u32)entry->l4.l4_valid;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_VLD, value, p_data_array, size);

		value[0] = (__u32)entry->l4.dp_lo;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_DPLO, value, p_data_array, size);

		value[0] = (__u32)entry->l4.dp_hi;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_DPHI, value, p_data_array, size);

		value[0] = (__u32)entry->l4.sp_lo;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_SPLO, value, p_data_array, size);

		value[0] = (__u32)entry->l4.sp_hi;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_SPHI, value, p_data_array, size);

		value[0] = (__u32)entry->misc.lspid;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LSPID, value, p_data_array, size);

		value[0] = (__u32)entry->misc.orig_lspid;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ORIG_LSPID, value, p_data_array, size);

		value[0] = (__u32)entry->misc.fwd_type;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FWDTYPE, value, p_data_array, size);

		value[0] = (__u32)entry->misc.spl_pkt_vec;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPL_PKT_VEC, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.class_hit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_CLASS_HIT, value, p_data_array, size);

		value[0] = (__u32)entry->misc.class_svidx;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_CLASS_SVIDX, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.lpm_hit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LPM_HIT, value, p_data_array, size);

		value[0] = (__u32)entry->misc.lpm_hit_idx;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LPM_HIT_IDX, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.hash_hit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_HASH_HIT, value, p_data_array, size);

		value[0] = (__u32)entry->misc.hash_hit_idx;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_HASH_HIT_IDX, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.da_an_mac_hit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DA_AN_MAC_HIT, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.da_an_mac_hit_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DA_AN_MAC_HIT_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->misc.l7_field;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L7_FIELD, value, p_data_array, size);

		value[0] = (__u32)entry->l2.l2_mac_da_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L2_MAC_DA_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.l2_mac_sa_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L2_MAC_SA_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.ethertype_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ETHERTYPE_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.len_encoded_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LEN_ENCODED_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.tpid_1_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_1_VLD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.tpid_enc_1_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_ENC_1_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.vid_1_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_VID_1_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l2._8021p_1_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_8021P_1_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.dei_1_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DEI_1_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l2.tpid_2_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_2_VLD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.tpid_enc_2_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TPID_ENC_2_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.vid_2_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_VID_2_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l2._8021p_2_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_8021P_2_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.dei_2_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DEI_2_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_VLD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ip_ver_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_VER_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ip_da_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_DA_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_sa_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_SA_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.dscp_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DSCP_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ecn_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ECN_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_proto_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_PROT_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ip_fragment_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_FRAGMENT_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ip_options_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IP_OPTIONS_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ipv6_flow_label_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_FLOW_LBL_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l3.ttl_hoplimit_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_TTL_HOPLMT_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l4.l4_valid_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_VLD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l4.l4_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L4_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->misc.lspid_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LSPID_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->misc.orig_lspid_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_ORIG_LSPID_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.fwd_type_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FWDTYPE_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.spl_pkt_vec_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPL_PKT_VEC_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.class_hist_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_CLASS_HIT_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.class_svidx_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_CLASS_SVIDX_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.lpm_hit_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LPM_HIT_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.lpm_hit_idx_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_LPM_HIT_IDX_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.hash_hit_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_HASH_HIT_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.hash_hit_idx_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_HASH_HIT_IDX_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.l7_field_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_L7_FIELD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.da_an_mac_sel;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DA_AN_MAC_SEL, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.da_an_mac_sel_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_DA_AN_MAC_SEL_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l2.sa_bng_mac_sel;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SA_BNG_MAC_SEL, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.sa_bng_mac_sel_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SA_BNG_MAC_SEL_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l2.sa_bng_mac_hit;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SA_BNG_MAC_HIT, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.sa_bng_mac_hit_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SA_BNG_MAC_HIT_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->misc.flags_vec;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FLAGS_VEC, value, p_data_array, size);

		value[0] = (__u32)entry->misc.flags_vec_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FLAGS_VEC_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.flags_vec_or;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FLAGS_VEC_OR, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.spl_pkt_vec_or;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPL_PKT_VEC_OR, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.recirc_idx;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_RECIRC_IDX, value, p_data_array, size);

		value[0] = (__u32)entry->misc.recirc_idx_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_RECIRC_IDX_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.ne_vec;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_NE_VEC, value, p_data_array, size);

		value[0] = (__u32)entry->misc.mc_idx;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_MCIDX, value, p_data_array, size);

		value[0] = (__u32)entry->misc.mc_idx_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_MCIDX_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->misc.sdb_drop;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SDB_DROP, value, p_data_array, size);

		value[0] = (__u32)entry->misc.sdb_drop_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SDB_DROP_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.fwd_drop;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FWD_DROP, value, p_data_array, size);

		value[0] = (__u32)entry->misc.fwd_drop_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_FWD_DROP_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.pppoe_session_id_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPPOE_SESSION_ID_VLD, value,
				p_data_array, size);

		value[0] = (__u32)entry->l2.pppoe_session_id_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPPOE_SESSION_ID_VLD_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l2.pppoe_session_id;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPPOE_SESSION_ID, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.pppoe_session_id_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPPOE_SESSION_ID_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l3.spi_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPI_VLD, value, p_data_array, size);

		value[0] = (__u32)entry->l3.spi_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPI_VLD_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.spi;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPI, value, p_data_array, size);

		value[0] = (__u32)entry->l3.spi_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_SPI_MASK, value, p_data_array, size);

		value[0] = (__u32)entry->l2.ppp_protocol_vld;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPP_PROTOCOL_VLD, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.ppp_protocol_vld_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPP_PROTOCOL_VLD_MASK, value,
				p_data_array, size);

		value[0] = (__u32)entry->l2.ppp_protocol;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPP_PROTOCOL, value, p_data_array,
				size);

		value[0] = (__u32)entry->l2.ppp_protocol_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PPP_PROTOCOL_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ipv6_ndp;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_NDP, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ipv6_ndp_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_NDP_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ipv6_hbh;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_HBH, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ipv6_hbh_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_HBH_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ipv6_rh;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_RH, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ipv6_rh_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_RH_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->l3.ipv6_doh;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_DOH, value, p_data_array, size);

		value[0] = (__u32)entry->l3.ipv6_doh_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_IPV6_DOH_MASK, value, p_data_array,
				size);

		value[0] = (__u32)entry->misc.pktlen_rng_vec;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PKTLEN_RNG_MATCH_VECTOR, value,
				p_data_array, size);

		value[0] = (__u32)entry->misc.pktlen_rng_vec_mask;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_PKTLEN_RNG_MATCH_VECTOR_MASK, value,
				p_data_array,
				size);

		value[0] = (__u32)entry->misc.rsvd_879_878;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_RSVD_879_878, value, p_data_array,
				size);

		value[0] = (__u32)entry->parity;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_MEM_PARITY, value, p_data_array, size);

		value[0] = 1;
		cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_RULE,
				ACL_RULE_VLD, value, p_data_array, size);
	}

	return 0;
} /* convert_sw_acl_rule_to_data_register */

static int convert_sw_acl_action_to_data_register(fe_acl_action_entry_t *entry,
		__u32 *p_data_array, unsigned int size)
{
	__u32 value;
	memset(p_data_array, 0x0, size * 4);

	value = (__u32)entry->misc.voq_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_VOQ_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.voq_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_VOQ_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.voq;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_VOQ, &value, p_data_array, size);

	value = (__u32)entry->misc.ldpid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_LDPID, &value, p_data_array, size);

	value = (__u32)entry->misc.cpucopy;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CPUCOPY, &value, p_data_array, size);

	value = (__u32)entry->misc.cpucopy_voq;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CPUCOPY_VOQ, &value, p_data_array, size);

	value = (__u32)entry->misc.cpucopy_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CPUCOPY_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.mirror_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MIRROR_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.mirror_id;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MIRROR_ID, &value, p_data_array, size);

	value = (__u32)entry->misc.mirror_id_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MIRROR_ID_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.wred_cos_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_WRED_COS_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.wred_cos;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_WRED_COS, &value, p_data_array, size);

	value = (__u32)entry->misc.wred_cos_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_WRED_COS_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.pre_mark_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_PRE_MARK_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.pre_mark;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_PRE_MARK, &value, p_data_array, size);

	value = (__u32)entry->misc.pre_mark_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_PRE_MARK_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.policer_id_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_POLICER_ID_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.policer_id;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_POLICER_ID, &value, p_data_array, size);

	value = (__u32)entry->misc.policer_id_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_POLICER_ID_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.drop_permit_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DROP_PERMIT_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.drop;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DROP, &value, p_data_array, size);

	value = (__u32)entry->misc.permit;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_PERMIT, &value, p_data_array, size);

	value = (__u32)entry->misc.drop_permit_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DROP_PERMIT_PRI, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_1_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_1_VLD, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_1;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_1, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_1_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_1_PRI, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_1_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_1_VLD, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_1;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_1, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_1_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_1_PRI, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_2_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_2_VLD, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_2;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_2, &value, p_data_array, size);

	value = (__u32)entry->misc.keep_ts_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_KEEP_TS_VLD, &value, p_data_array, size);

	value = (__u32)entry->l2._8021p_2_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_8021P_2_PRI, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_2_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_2_VLD, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_2;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_2, &value, p_data_array, size);

	value = (__u32)entry->l2.dei_2_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DEI_2_PRI, &value, p_data_array, size);

	value = (__u32)entry->l3.dscp_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DSCP_VLD, &value, p_data_array, size);

	value = (__u32)entry->l3.dscp;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DSCP, &value, p_data_array, size);

	value = (__u32)entry->l3.dscp_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DSCP_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.fwdtype_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FWDTYPE_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.fwdtype;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FWDTYPE, &value, p_data_array, size);

	value = (__u32)entry->misc.fwdtype_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FWDTYPE_PRI, &value, p_data_array, size);

	value = (__u32)entry->misc.mcgid_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MCGID_VLD, &value, p_data_array, size);

	value = (__u32)entry->misc.mcgid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MCGID, &value, p_data_array, size);

	value = (__u32)entry->misc.mcdid_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MCGID_PRI, &value, p_data_array, size);

	value = (__u32)entry->l2.first_vlan_cmd_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FIRST_VLAN_CMD_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l2.first_vlan_cmd;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FIRST_VLAN_CMD, &value, p_data_array, size);

	value = (__u32)entry->l2.first_vid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FIRST_VID, &value, p_data_array, size);

	value = (__u32)entry->l2.first_tpid_enc;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FIRST_TPID_ENC, &value, p_data_array, size);

	value = (__u32)entry->l2.first_vlan_cmd_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_FIRST_VLAN_CMD_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->l2.second_vlan_cmd_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_SECOND_VLAN_CMD_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l2.second_vlan_cmd;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_SECOND_VLAN_CMD, &value, p_data_array, size);

	value = (__u32)entry->l2.second_vid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_SECOND_VID, &value, p_data_array, size);

	value = (__u32)entry->l2.second_tpid_enc;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_SECOND_TPID_ENC, &value, p_data_array, size);

	value = (__u32)entry->l2.second_vlan_cmd_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_SECOND_VLAN_CMD_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->misc.keep_ts;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_KEEP_TS, &value, p_data_array, size);

	value = (__u32)entry->misc.keep_ts_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_KEEP_TS_PRI, &value, p_data_array, size);

	value = (__u32)entry->l3.ip_sa_replace_en_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_SA_REPLACE_EN_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_sa_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_SA_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_sa_replace_en_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_SA_REPLACE_EN_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_da_replace_en_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_DA_REPLACE_EN_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_da_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_DA_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_da_replace_en_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_DA_REPLACE_EN_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->l4.sp_replace_en_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_SP_REPLACE_EN_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l4.sp_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_SP_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l4.sp;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_SP, &value, p_data_array, size);

	value = (__u32)entry->l4.sp_replace_en_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_SP_REPLACE_EN_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->l4.dp_replace_en_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_DP_REPLACE_EN_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l4.dp_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_DP_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l4.dp;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_DP, &value, p_data_array, size);

	value = (__u32)entry->l4.dp_replace_en_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L4_DP_REPLACE_EN_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->l3.ip_sa_idx;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_SA_IDX, &value, p_data_array, size);

	value = (__u32)entry->l3.ip_da_idx;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_IP_DA_IDX, &value, p_data_array, size);

	value = (__u32)entry->l2.mac_da_sa_replace_en_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MAC_DA_SA_REPLACE_EN_VLD, &value,
			p_data_array, size);

	value = (__u32)entry->l2.mac_da_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MAC_DA_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l2.mac_sa_replace_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MAC_SA_REPLACE_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l2.mac_da_sa_replace_en_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MAC_DA_SA_REPLACE_EN_PRI, &value,
			p_data_array, size);

	value = (__u32)entry->l2.l2_idx;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_L2_IDX, &value, p_data_array, size);

	value = (__u32)entry->l3.change_dscp_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CHANGE_DSCP_EN, &value, p_data_array, size);

	value = (__u32)entry->l3.decr_ttl_hoplimit_vld;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DECR_TTL_HOPLIMIT_VLD, &value, p_data_array,
			size);

	value = (__u32)entry->l3.decr_ttl_hoplimit;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DECR_TTL_HOPLIMIT, &value, p_data_array,
			size);

	value = (__u32)entry->l3.decr_ttl_hoplimit_pri;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_DECR_TTL_HOPLIMIT_PRI, &value, p_data_array,
			size);

	value = (__u32)entry->misc.voq_cpupid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_VOQ_CPUPID, &value, p_data_array, size);

	value = (__u32)entry->misc.cpucopy_cpupid;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CPUCOPY_CPUPID, &value, p_data_array, size);

	value = (__u32)entry->l2.change_8021p_1_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CHANGE_8021P_1_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l2.change_dei_1_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CHANGE_DEI_1_EN, &value, p_data_array, size);

	value = (__u32)entry->l2.change_8021p_2_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CHANGE_8021P_2_EN, &value, p_data_array,
			size);

	value = (__u32)entry->l2.change_dei_2_en;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_CHANGE_DEI_2_EN, &value, p_data_array, size);

	value = (__u32)entry->parity;
	cs_fe_hw_table_set_field_value_to_sw_data(FE_TABLE_ACL_ACTION,
			ACL_ACTION_MEM_PARITY, &value, p_data_array, size);

	return 0;
} /* convert_sw_acl_action_to_data_register */

static int fe_acl_set_entry(unsigned int idx, void *entry)
{
	fe_acl_entry_t *acl_entry = (fe_acl_entry_t *)entry;
	cs_table_entry_t *p_sw_entry;
	__u32 data_array[30];
	unsigned int table_size;
	int ret;
	unsigned long flags;

	if (entry == NULL)
		return FE_TABLE_ENULLPTR;

	if (idx >= cs_fe_acl_table_type.max_entry)
		return FE_TABLE_EOUTRANGE;

	if (FE_ACL_TABLE_PTR == NULL)
		return FE_TABLE_ETBLNOTEXIST;

	p_sw_entry = cs_table_get_entry(FE_ACL_TABLE_PTR, idx);
	if (p_sw_entry == NULL)
		return FE_TABLE_ENULLPTR;
	if ((p_sw_entry->local_data & FE_TABLE_ENTRY_USED) == 0)
		return FE_TABLE_EENTRYNOTRSVD;

	/* Deal with ACL Rule table first */
	ret = cs_fe_hw_table_get_entry_value(FE_TABLE_ACL_RULE, idx, data_array,
			&table_size);
	if (ret != 0)
		return ret;

	/* Generate the data register value based on the given entry */
	ret = convert_sw_acl_rule_to_data_register(&acl_entry->rule, data_array,
			table_size);
	if (ret != 0)
		return ret;

	spin_lock_irqsave(FE_ACL_LOCK, flags);
	/* set it to HW indirect access table */
	ret = cs_fe_hw_table_set_entry_value(FE_TABLE_ACL_RULE, idx, table_size,
			data_array);
	spin_unlock_irqrestore(FE_ACL_LOCK, flags);
	if (ret != 0)
		return ret;

	/* Deal with ACL Action table */
	ret = cs_fe_hw_table_get_entry_value(FE_TABLE_ACL_ACTION, idx,
			data_array, &table_size);
	if (ret != 0)
		return ret;

	/* Generate the data register value based on the given entry */
	ret = convert_sw_acl_action_to_data_register(&acl_entry->action,
			data_array, table_size);
	if (ret != 0)
		return ret;

	/* set it to HW indirect access table */
	spin_lock_irqsave(FE_ACL_LOCK, flags);
	ret = cs_fe_hw_table_set_entry_value(FE_TABLE_ACL_ACTION, idx,
			table_size, data_array);
	spin_unlock_irqrestore(FE_ACL_LOCK, flags);
	if (ret != 0)
		return ret;

	/* store the ACL entry in SW table */
	spin_lock_irqsave(FE_ACL_LOCK, flags);
	if (p_sw_entry->data == NULL)
		p_sw_entry->data = fe_table_malloc_table_entry(
				&cs_fe_acl_table_type);
	if (p_sw_entry->data == NULL) {
		spin_unlock_irqrestore(FE_ACL_LOCK, flags);
		return -ENOMEM;
	}
	memcpy(((fe_table_entry_t*)p_sw_entry->data)->p_entry, entry,
			sizeof(fe_acl_entry_t));
	spin_unlock_irqrestore(FE_ACL_LOCK, flags);

	return FE_TABLE_OK;
} /* fe_acl_set_entry */

static int fe_acl_inc_entry_refcnt(unsigned int idx)
{
	return fe_table_inc_entry_refcnt(&cs_fe_acl_table_type, idx);
} /* fe_acl_inc_entry_refcnt */

static int fe_acl_dec_entry_refcnt(unsigned int idx)
{
	return fe_table_dec_entry_refcnt(&cs_fe_acl_table_type, idx);
} /* fe_acl_dec_entry_refcnt */

static int fe_acl_get_entry_refcnt(unsigned int idx, unsigned int *p_cnt)
{
	return fe_table_get_entry_refcnt(&cs_fe_acl_table_type, idx, p_cnt);
} /* fe_acl_get_entry_refcnt */

static int fe_acl_add_entry(void *entry, unsigned int *rslt_idx)
{
	return fe_table_add_entry(&cs_fe_acl_table_type, entry, rslt_idx);
} /* fe_acl_add_entry */

static int fe_acl_find_entry(void *entry, unsigned int *rslt_idx,
		unsigned int start_offset)
{
	return fe_table_find_entry(&cs_fe_acl_table_type, entry, rslt_idx,
			start_offset);
} /* fe_acl_find_entry */

/* cannot use fe_table_del_entry_by_idx, because ACL requires
 * cleaning 2 HW tables at once.  ACL_RULE and ACL_ACTION */
static int fe_acl_del_entry_by_idx(unsigned int idx, bool f_force)
{
	cs_table_entry_t *p_sw_entry;
	fe_table_entry_t *p_fe_entry;
	bool f_clean_entry = false;
	unsigned long flags;
	int ret;

	if (idx >= cs_fe_acl_table_type.max_entry)
		return FE_TABLE_EOUTRANGE;

	if (FE_ACL_TABLE_PTR == NULL)
		return FE_TABLE_ETBLNOTEXIST;

	p_sw_entry = cs_table_get_entry(FE_ACL_TABLE_PTR, idx);
	if (p_sw_entry == NULL)
		return FE_TABLE_ENULLPTR;

	/* We don't decrement the reference count if the entry is not used */
	if ((p_sw_entry->local_data & FE_TABLE_ENTRY_USED) == 0)
		return FE_TABLE_EENTRYNOTRSVD;

	p_fe_entry = (fe_table_entry_t*)p_sw_entry->data;
	if (p_fe_entry == NULL)
		return FE_TABLE_ENULLPTR;

	spin_lock_irqsave(FE_ACL_LOCK, flags);
	if (f_force == true) atomic_set(&p_fe_entry->users, 0);

	/* if it is already 0, just return it */
	if (atomic_read(&p_fe_entry->users) == 0) f_clean_entry = true;

	if ((f_clean_entry == false) &&
			(atomic_dec_and_test(&p_fe_entry->users)))
		f_clean_entry = true;

	if (f_clean_entry == true) {
		kfree(p_fe_entry->p_entry);
		kfree(p_sw_entry->data);
		cs_fe_acl_table_type.used_entry--;
		ret = cs_fe_hw_table_clear_entry(FE_TABLE_ACL_RULE, idx);
		if (ret != 0) {
			spin_unlock_irqrestore(FE_ACL_LOCK, flags);
			return ret;
		}
		ret = cs_fe_hw_table_clear_entry(FE_TABLE_ACL_ACTION, idx);
		if (ret != 0) {
			spin_unlock_irqrestore(FE_ACL_LOCK, flags);
			return ret;
		}
	}

	spin_unlock_irqrestore(FE_ACL_LOCK, flags);

	return FE_TABLE_OK;
} /* fe_acl_del_entry_by_idx */

static int fe_acl_del_entry(void *entry, bool f_force)
{
	return fe_table_del_entry(&cs_fe_acl_table_type, entry, f_force);
} /* fe_acl_del_entry */

static int fe_acl_get_entry(unsigned int idx, void *entry)
{
	return fe_table_get_entry(&cs_fe_acl_table_type, idx, entry);
} /* fe_acl_get_entry */

static int fe_acl_flush_table(void)
{
	return fe_table_flush_table(&cs_fe_acl_table_type);
} /* fe_acl_flush_table */

static void _fe_acl_print_entry(unsigned int idx)
{
	/* FIXME!! implement */
} /* _fe_acl_print_entry */

static void fe_acl_print_entry(unsigned int idx)
{
	if (idx >= FE_ACL_ENTRY_MAX) {
		printk("%s::Range not acceptable!\n", __func__);
		return;
	}

	printk("\n\n ------------------- ACL Table --------------------\n");
	printk("|------------------------------------------------------\n");

	_fe_acl_print_entry(idx);
} /* fe_acl_print_entry */

static void fe_acl_print_range(unsigned int start_idx, unsigned int end_idx)
{
	unsigned int i;

	if ((start_idx > end_idx) || (end_idx >= FE_ACL_ENTRY_MAX)) {
		printk("%s::Range not acceptable!\n", __func__);
		return;
	}

	printk("\n\n ------------------- ACL Table --------------------\n");
	printk("|------------------------------------------------------\n");

	for (i = start_idx; i <= end_idx; i++) {
		_fe_acl_print_entry(i);
		cond_resched();
	}

	printk("|------------------------------------------------------\n");
} /* fe_acl_print_range */

static void fe_acl_print_table(void)
{
	unsigned int i;

	printk("\n\n ------------------- ACL Table --------------------\n");
	printk("|------------------------------------------------------\n");

	for (i = 0; i < cs_fe_acl_table_type.max_entry; i++) {
		_fe_acl_print_entry(i);
		cond_resched();
	}

	printk("|------------------------------------------------------\n");
} /* fe_acl_print_table */

static int fe_acl_get_avail_count(void)
{
	return fe_table_get_avail_count(&cs_fe_acl_table_type);
} /* fe_acl_get_avail_count */

static cs_fe_table_t cs_fe_acl_table_type = {
	.type_id = FE_TABLE_ACL_RULE,	/* We combine writing to both ACL rule
					 * and action table into 1, since they
					 * are most likely 1-to-1 mapping */
	.max_entry = FE_ACL_ENTRY_MAX,
	.used_entry = 0,
	.curr_ptr = 0,
	.entry_size = sizeof(fe_acl_entry_t),
	.op = {
		.convert_sw_to_hw_data = NULL,
		.alloc_entry = fe_acl_alloc_entry,
		.set_entry = fe_acl_set_entry,
		.add_entry = fe_acl_add_entry,
		.del_entry_by_idx = fe_acl_del_entry_by_idx,
		.del_entry = fe_acl_del_entry,
		.find_entry = fe_acl_find_entry,
		.get_entry = fe_acl_get_entry,
		.inc_entry_refcnt = fe_acl_inc_entry_refcnt,
		.dec_entry_refcnt = fe_acl_dec_entry_refcnt,
		.get_entry_refcnt = fe_acl_get_entry_refcnt,
		.set_field = NULL,	/* ACL does not support set/get a */
		.get_field = NULL,	/* field, because it is controlling */
					/* 2 HW tables at once. */
		.flush_table = fe_acl_flush_table,
		.get_avail_count = fe_acl_get_avail_count,
		.print_entry = fe_acl_print_entry,
		.print_range = fe_acl_print_range,
		.print_table = fe_acl_print_table,
		/* all the rests are NULL */
		.add_l2_mac = NULL,
		.del_l2_mac = NULL,
		.find_l2_mac = NULL,
		.get_l2_mac = NULL,
		.inc_l2_mac_refcnt = NULL,
		.add_l3_ip = NULL,
		.del_l3_ip = NULL,
		.find_l3_ip = NULL,
		.get_l3_ip = NULL,
		.inc_l3_ip_refcnt = NULL,
	},
	.content_table = NULL,
};

/* do we need to include / implement ioctl? */
int cs_fe_ioctl_acl(struct net_device *dev, void *pdata, void *cmd)
{
	// FIXME!! implement
	return 0;
} /* cs_fe_ioctl_acl */

/* this API will initialize ACL Rule/Action table */
int cs_fe_table_acl_init(void)
{
	int ret;

	spin_lock_init(FE_ACL_LOCK);

	cs_fe_acl_table_type.content_table = cs_table_alloc(
			cs_fe_acl_table_type.max_entry);
	if (cs_fe_acl_table_type.content_table == NULL)
		return -ENOMEM;

	ret = cs_fe_table_register(cs_fe_acl_table_type.type_id,
			&cs_fe_acl_table_type);
	if (ret != FE_TABLE_OK) {
		cs_table_dealloc(cs_fe_acl_table_type.content_table);
		return -1;
	}

	/* ACL does not need to reserve a default entry */

	/* FIXME! any other initialization that needs to take place here? */

	return CS_OK;
} /* cs_fe_table_acl_init */
EXPORT_SYMBOL(cs_fe_table_acl_init);

void cs_fe_table_acl_exit(void)
{
	fe_acl_flush_table();

	if (cs_fe_acl_table_type.content_table == NULL)
		return;

	cs_table_dealloc(cs_fe_acl_table_type.content_table);
	cs_fe_table_unregister(cs_fe_acl_table_type.type_id);
} /* cs_fe_table_acl_exit */
EXPORT_SYMBOL(cs_fe_table_acl_exit);

