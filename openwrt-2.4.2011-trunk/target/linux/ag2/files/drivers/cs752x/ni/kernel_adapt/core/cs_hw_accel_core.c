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
 * $Id: cs_hw_accel_core.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * This file contains the implementation for CS Forwarding Engine Offload Kernel Module.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <mach/cs_types.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <cs_vtable.h>
#include <cs_hw_accel_core.h>
#include <cs_hw_accel_base.h>
#include <cs_hw_accel_util.h>
#include "cs_fe.h"
#include "cs_hw_accel_nat.h"
#include "cs_hw_accel_bridge.h"
#include "cs_hw_accel_vlan.h"
#include "cs_hw_accel_qos.h"
#include "cs_hw_accel_pppoe.h"
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
#include "cs_hw_accel_ipsec.h"
#endif
#ifdef CONFIG_IPV6
#include "cs_hw_accel_ipv6.h"
#endif
#include "cs_hw_accel_mc.h"

cs_hash_guid_s cs_hash_map_guid[CS_HASH_MAP_GUID_NUM_POWER];
struct timer_list fe_hash_timer_obj;
struct cs_core_hash *users = NULL;
cs_uint8	core_initialized;
CORE_CFG_T core_cfg;
int cs_hw_accel_major = CS_HW_ACCEL_MAJOR_N;
//cs_uint8 vtable_hash_data[sizeof(fe_hash_vtable_s)];
//cs_uint8 mask_ptr;

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"

extern u32 cs_adapt_debug;
#endif /* CONFIG_CS752X_PROC */

#define DBG(x)  if(cs_adapt_debug & CS752X_ADAPT_CORE) x
	
//FIXME: for test
bool CheckMemEnabled = false;

/* TOPOLOGY
 *
 *          STD topology
 *      
 *       +-----------+            +-------------+
 *       |           |  ACCEPT    |             | UNKNOWN
 * =====>| L2 RULE   | =========> | L2 FLOW     | =======> CPU
 *       |           |            |             |
 *       +-----------+            +-------------+
 *           ||   || REJECT           ||  
 *           ||   --                  ++=================> Egress
 *           || 
 *           \/
 *       +-----------+            +-------------+
 *       |           |  ACCEPT    |             | UNKNOWN
 *       | L3 RULE   | =========> | L3 FLOW     | =======> CPU
 *       |           |            |             |
 *       +-----------+            +-------------+
 *                || REJECT           ||  
 *                --                  ++=================> Egress
 *              
 *             
 */
int l3_flow_vid, l3_rule_vid, l2_flow_vid, l2_rule_vid;

/* This API will initialize FE resource */
void cs_kernel_core_hw_fe_init(void)
{
//	cs_mcg_init();
	cs_vtable_init();
}

/* create the hash entry based on the hash info and virtual table ID. 
 * also maintain the guid <-> hash id relationship */
cs_uint32 cs_kernel_core_add_hash(CS_HASH_ENTRY_S *p_hash, cs_uint8 vt_type,
				cs_uint64 *p_guid_array, cs_uint8 guid_cnt,cs_int16 timeout)
{
	cs_uint32 crc32, qos_crc32;
	cs_uint16 crc16, hash_index, qos_rslt_index = 0, qos_crc16;
	int result_index;
	cs_uint8 status;
	fe_hash_mask_entry_t *mask_rule;
	cs_uint8 mask_ram_data[sizeof(fe_hash_mask_entry_t)];
	cs_uint8 mask_ptr = 0, qos_mask_ptr;
	cs_boolean f_qos_enbl = FALSE;
	fe_hash_check_entry_t chkmem;
#ifdef CS_UU_TEST	
	cs_uint16 uu_result_index;
	if (p_hash->result.param.uu_flow_enable == CS_RESULT_ACTION_DISABLE) {
		result_index = cs_vtable_to_forwarding_result(&p_hash->result, vt_type, 
			CS_VTBL_ID_L2_FLOW,	cs_find_vtable_idx_by_type(vt_type));
		if (result_index == CS_ERROR) {
			DBG(printk("%s: No avaiable forwarding result \n",__func__));
			return CS_HASH_CB_FAILED_RESOURCE;
		}
			
			
	}
#else	
	// FIXME: CS_VTBL_ID_L2_FLOW ??? cant not fix
	result_index = cs_vtable_to_forwarding_result(&p_hash->result, vt_type, 
		CS_VTBL_ID_L2_FLOW,	cs_find_vtable_idx_by_type(vt_type));
		if (result_index == CS_ERROR) {
			DBG(printk("%s: No avaiable forwarding result \n",__func__));
			//FIXME: need free all of allocated resource.
			return CS_HASH_CB_FAILED_RESOURCE;
		}
			
#endif /* CS_UU_TEST */	

//	f_qos_enbl = cs_vtable_check_qos_enbl(p_hash);

//	if (f_qos_enbl == TRUE)
//		qos_rslt_index = cs_vtable_to_qos_result(&p_hash->result, vt_type);

	DBG(printk("%s:%d:vt_type = %d, esult->param.d_voq = %d\n", __func__, __LINE__, vt_type, p_hash->result.param.d_voq));

	DBG(printk("%s::00 mask_ptr %d, cs_find_vtable_idx_by_type(vt_type) = 0x%X\n", 
			__func__, mask_ptr, cs_find_vtable_idx_by_type(vt_type)));

#ifdef CS_UU_TEST	
	if ((vt_type == CS_VTBL_ID_L2_FLOW) && 
			(p_hash->result.param.uu_flow_enable == CS_RESULT_ACTION_ENABLE)) {
			//(uu_flow_enable == CS_RESULT_ACTION_ENABLE)) {
		/* first pass to root voq */
		p_hash->result.param.fwd_type = CS_FWD_UU;
#ifdef NEW_L2_DESIGN
		p_hash->result.param.d_voq = CPU_PORT1_VOQ_BASE;		/* default send to CPU */
#else
		p_hash->result.param.d_voq = ROOT_PORT_VOQ_BASE;
#endif
		p_hash->result.action.fwd_type_valid = CS_RESULT_ACTION_ENABLE;
		p_hash->result.param.mcgid = CS_VTBL_ID_UU_FLOW;
		p_hash->result.action.mcgid_valid = CS_RESULT_ACTION_ENABLE;
		DBG(printk("%s:CS_UU_TEST:: result->param.d_voq = %d\n",__func__, 
			p_hash->result.param.d_voq));
		DBG(printk("CHCHCHCHCHCHCHCHCHCHHCCHHCHCHCHCHCHCHCHCHCHCHCHHCHCHHCHCHCHC\n"));
		uu_result_index = cs_uu_flow_to_forwarding_result(&p_hash->result, vt_type, 
			CS_VTBL_ID_L2_FLOW,	cs_find_vtable_idx_by_type(vt_type));
		DBG(printk("%s::uu_result_index == %d\n",__func__, uu_result_index));
		cs_vtable_add_sdb_uu_flow(uu_result_index, cs_find_vtable_by_type(vt_type));
		//uu_flow_enable = CS_RESULT_ACTION_DISABLE;
		p_hash->result.param.uu_flow_enable = CS_RESULT_ACTION_DISABLE;
		//FIXME: next step use VLAN MEMBERSHIP FE_VLN_MCGID table?
		//cs_vtable_to_vlan_table(&p_hash->result, vt_type, 
		//	CS_VTBL_ID_L2_FLOW,	cs_find_vtable_idx_by_type(vt_type));
		
		//FIXME: don't need add mask ram when UU flow, consider move to cs_vtable_add_uu_hash_to_lspid() or initial
		return CS_HASH_ADD_SUCCESSFUL;
	} 
	
	if (CheckMemEnabled == true) {
	    memset(&chkmem, 0, sizeof(fe_hash_check_entry_t));
	   	if (p_hash->result.param.uu_flow_enable == CS_RESULT_ACTION_DISABLE) {
	    	if (CS_OK == cs_kernel_to_checkmem(p_hash, &chkmem)) {
				cs_fe_table_set_entry(FE_TABLE_HASH_CHECK_MEM, result_index, 
						&chkmem);
		    	printk("%s: cs_fe_checkmem_alloc result_index = %d\n",__func__, result_index);
	    	}
		}
	}
#else /* CS_UU_TEST */
	if (CheckMemEnabled == true) {
	    memset(&chkmem, 0, sizeof(fe_hash_check_entry_t));
	    if (CS_OK == cs_kernel_to_checkmem(p_hash, &chkmem)) {
			cs_fe_table_set_entry(FE_TABLE_HASH_CHECK_MEM, result_index, &chkmem);
		    DBG(printk("%s: cs_fe_checkmem_alloc result_index = %d\n",__func__, result_index));
	    }
	}
#endif /* CS_UU_TEST */	    
	/* add hash mask ram */
	mask_rule = (fe_hash_mask_entry_t *)&mask_ram_data;
	mask_ptr = cs_vtable_to_hash_mask(p_hash, mask_rule, 
		cs_find_vtable_idx_by_type(vt_type));
	
	DBG(printk("%s::11 mask_ptr %d, cs_find_vtable_idx_by_type(vt_type) = 0x%X\n", 
			__func__, mask_ptr, cs_find_vtable_idx_by_type(vt_type)));

	if (f_qos_enbl == TRUE) {
		/* If QoS field is required to change, we need to allocate the 
		 * mask pointer for this QoS tuple */
		qos_mask_ptr = cs_vtable_to_qos_hash_mask(p_hash, mask_rule, 
				cs_find_vtable_idx_by_type(vt_type));
		/* we then calculate crc32 and crc16 for QoS hash key */
		cs_fe_hash_calc_crc(&p_hash->key, &qos_crc32, &qos_crc16, 
				CRC16_CCITT);
		/* now.. this looks a bit ugly, but we need to remove QoS related 
		 * key from p_hash->key */
		cs_kernel_update_hash_key_from_qos(&p_hash->key);
	}
	// choose the right hash mask to calculate the crc
	cs_fe_hash_calc_crc(&p_hash->key, &crc32, &crc16, CRC16_CCITT);
	DBG(printk("%s: crc32= 0x%08X, crc16 = 0x%04X, CRC16_CCITT \n",__func__, crc32, crc16));
	 
	DBG(printk("%s:: mask_ptr = %d, vt_type = 0x%X\n",__func__, mask_ptr, vt_type));

	status = cs_vtable_add_sdb_tuple(mask_ptr, cs_find_vtable_by_type(vt_type));
	//FIXME: cs_vtable_add_sdb_field() not implement, for add UU_flow.
	if (status == CS_ERROR) {
		printk("%s: No avaiable sdb tuple \n",__func__);
		//FIXME: need free all of allocated resource.
		return CS_HASH_CB_FAILED_RESOURCE;
	}
	
	if (f_qos_enbl == TRUE)
		cs_vtable_add_sdb_qos_tuple(qos_mask_ptr, cs_find_vtable_by_type(vt_type));

	status = cs_add_vtable_entry(crc16, crc32, mask_ptr,
			result_index, &hash_index,  cs_find_vtable_idx_by_type(vt_type), 
			timeout);
	DBG(printk("%s: 24K hash_index %d \n",__func__, hash_index));

	if (f_qos_enbl == TRUE) {
		status = cs_add_vtable_entry(qos_crc16, qos_crc32, qos_mask_ptr, 
			qos_rslt_index, &hash_index,  cs_find_vtable_idx_by_type(vt_type), 
			timeout);
		DBG(printk("%s: 24K hash_index %d for qos hash\n",__func__, hash_index));
	}
#ifdef CONFIG_QOS_TUPLE6
	if (cs_vtable_check_qos_enbl(p_hash) == TRUE) {
		/*
		 * Add:
		 * QoS result table entry
		 * QoS hash mask
		 * SDB tuple to QoS hash mask
		 * Hash entries to QoS result
		 */
		printk("%s::qos hash key, ingress 8021p %x, dei %x, dscp %x\n",
			__func__, p_hash->key._8021p_1, p_hash->key.dei_1, p_hash->key.dscp);

		printk("%s::qos hash result %p, egress 8021p %x, op %x, dscp %x, op %x\n",
			__func__, &p_hash->result, p_hash->result.qos_param.top_802_1p,
			p_hash->result.qos_param.change_8021p_1_en, p_hash->result.qos_param.dscp,
			p_hash->result.qos_param.change_dscp_en);

		qos_rslt_index = cs_vtable_to_qos_result(&p_hash->result, vt_type);

		printk("%s::qos result index %x\n", __func__, qos_rslt_index);

		qos_mask_ptr = cs_vtable_to_qos_hash_mask(p_hash, mask_rule,
				cs_find_vtable_idx_by_type(vt_type));
		printk("%s::QoS hash mask ptr %x\n",
			__func__, qos_mask_ptr);

		cs_fe_hash_calc_crc(&p_hash->key, &qos_crc32, &qos_crc16, CRC16_CCITT);
		printk("%s::qos tuple6 crc32 %x, crc16 %x\n",
			__func__, qos_crc32, qos_crc16);

		cs_vtable_add_sdb_qos_tuple(qos_mask_ptr, cs_find_vtable_by_type(vt_type));
		printk("%s::Add sdb tuple\n", __func__);


		cs_add_vtable_qos_entry(qos_crc16, qos_crc32, qos_mask_ptr,
			qos_rslt_index, &hash_index, cs_find_vtable_idx_by_type(vt_type),
			timeout);

		printk("\t***%s::Add hash index %x for QoS, qos result index %x, crc16 %x\n",
			__func__, hash_index, qos_rslt_index, qos_crc16);

	}
#endif
	
#ifdef CONFIG_QOS_TUPLE7
//	if (cs_vtable_check_qos_enbl(p_hash) == TRUE)
	{
		/*
		 * Add:
		 * QoS result table entry
		 * QoS hash mask
		 * SDB tuple to QoS hash mask
		 * Hash entries to QoS result
		 */
		fe_sw_hash_t	qos_hash_key;
#if 0
		printk("%s::qos hash key, ingress 8021p %x, dei %x, dscp %x, mask %llx\n",
			__func__, p_hash->key._8021p_1, p_hash->key.dei_1, p_hash->key.dscp, p_hash->mask);

		printk("%s::qos hash result %p, egress 8021p %x, op %x, dscp %x, op %x\n",
			__func__, &p_hash->result, p_hash->result.qos_param.top_802_1p,
			p_hash->result.qos_param.change_8021p_1_en, p_hash->result.qos_param.dscp,
			p_hash->result.qos_param.change_dscp_en);
		qos_rslt_index = cs_vtable_to_qos_result(&p_hash->result, vt_type);
		printk("%s::qos result index %x\n", __func__, qos_rslt_index);
#endif

		p_hash->mask = (CS_HM_IP_VLD_MASK | CS_HM_DSCP_MASK | CS_HM_KEYGEN_POLY_SEL);
		//p_hash->mask = (CS_HM_8021P_1_MASK | CS_HM_DEI_1_MASK | CS_HM_KEYGEN_POLY_SEL);
		qos_mask_ptr = cs_vtable_to_hash_mask_qos_only(p_hash, mask_rule,
						cs_find_vtable_idx_by_type(vt_type));
		printk("%s::qos hash mask ptr %x\n", __func__, qos_mask_ptr);
#if 0
		{
			memset(&qos_hash_key, 0, sizeof(fe_sw_hash_t));
			qos_hash_key._8021p_1 = p_hash->key._8021p_1;
			qos_hash_key.dei_1 = p_hash->key.dei_1;
			qos_hash_key._8021p_2 = p_hash->key._8021p_2;
			qos_hash_key.dei_2 = p_hash->key.dei_2;
			qos_hash_key.dscp = p_hash->key.dscp;

			qos_hash_key.mask_ptr_0_7 = p_hash->key.mask_ptr_0_7;
//			qos_hash_key.ecn = p_hash->key.ecn;
		}
		cs_fe_hash_calc_crc(&qos_hash_key, &qos_crc32, &qos_crc16, CRC16_CCITT);
		printk("\t***%s::QoS hash key 8021p1 %x, dscp %x, result crc16 %x, crc32 %x, qos_mask_ptr %x, mask_ptr %x\n",
			__func__, qos_hash_key._8021p_1, qos_hash_key.dscp, qos_crc16, qos_crc32, qos_mask_ptr, qos_hash_key.mask_ptr_0_7);
#endif
		cs_vtable_add_sdb_qos_tuple(qos_mask_ptr, cs_find_vtable_by_type(vt_type));
		printk("\t***%s::Add sdb tuple\n", __func__);
#if 0
		cs_add_vtable_qos_entry(qos_crc16, qos_crc32, qos_mask_ptr,
			qos_rslt_index, &hash_index, cs_find_vtable_idx_by_type(vt_type),
			timeout);
#endif
		printk("\t***%s::Add hash index %x for QoS, qos result index %x, crc16 %x\n",
			__func__, hash_index, qos_rslt_index, qos_crc16);
	}
#endif

	// FIXME!! how about hash key generated for QoS tuple.. cs_add_vtable_entry 
	// does not support this case

	if (status == FE_STATUS_OK) {
		hash_index = CS_HASH_CB_SUCCESSFUL;
	} else if (status == FE_ERR_ENTRY_NOT_FOUND || status == FE_ERR_ALLOC_FAIL) {
		hash_index = CS_HASH_CB_FAILED_SW_REQ;
	} else {
		hash_index = CS_HASH_CB_FAILED_SW_REQ;
	} 
	
	//FIXME: guid <-> hash id relationshp, add this later
	//core_add_user(hash_index, vtbl_id, guid_cnt, p_guid_array);

	// Callback to each module that has guid assigned on this hash entry , add this later
	//cs_kernel_get_module_callback(p_guid_array, guid_cnt, hash_index);
	
	//cs_kernel_core_allocate_resource();
	
	return CS_HASH_ADD_SUCCESSFUL;
} /* cs_kernel_core_add_hash */


//#ifdef CS_NE_TEST_BY_IXIA
/* For Ixia test */
#define  CS_IPIV(a,b,c,d)		((a<<24)+(b<<16)+(c<<8)+d)
#define	 CS_TEST_CLIENT_IP 		CS_IPIV(192,168,2,10)	
#define	 CS_TEST_SERVER_IP 		CS_IPIV(192,168,3,20)
#define	 CS_TEST_LAN_IP			CS_IPIV(192,168,2,254)
#define	 CS_TEST_WAN_IP			CS_IPIV(192,168,3,254)
#define	 CS_TEST_MAP_PORT_BASE	2001	//0x7d1
#define	 CS_TEST_SPORT			50 		//0x32
#define	 CS_TEST_DPORT			80		//0x50
#define	 CS_TEST_PROTOCOL		6
cs_uint8 cs_test_lan_target_da[6]={0x00,0x11,0x22,0x33,0x44,0x55};
cs_uint8 cs_test_wan_target_da[6]={0x00,0xaa,0xbb,0xcc,0xdd,0xee};
cs_uint8 cs_test_lan_my_da[6]={0x00,0x11,0x11,0x11,0x11,0x11};
cs_uint8 cs_test_wan_my_da[6]={0x00,0x22,0x22,0x22,0x22,0x22};

cs_uint32 cs_kernel_core_add_hash_cfg(struct sk_buff *skb)
{
	cs_uint64 nat_guid;
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));

	/* LAN(GE-1) --> WAN(GE-2) */
	cs_cb->input_mask |= CS_HM_IP_DA_MASK|CS_HM_IP_SA_MASK|
			CS_HM_IP_PROT_MASK|CS_HM_L4_DP_MASK|CS_HM_L4_SP_MASK;
	
	cs_cb->output_mask |= CS_HM_MAC_DA_MASK | CS_HM_MAC_SA_MASK | CS_HM_IP_DA_MASK |
			CS_HM_IP_SA_MASK | CS_HM_IP_PROT_MASK | CS_HM_L4_DP_MASK | CS_HM_L4_SP_MASK;
			
	cs_cb->common.tag = CS_CB_TAG;						
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	cs_cb->common.vtype = CS_VTBL_ID_L3_FLOW;
	cs_cb->common.module_mask |= CS_MOD_MASK_NAT;
	//cs_cb->common.dvoq = 16; /* GE-2, voq: 16 ~ 23 */
	cs_cb->common.output_dev = skb->dev;
	cs_cb->key_misc.fwd_type = 0; 
	cs_cb->key_misc.mcgid = 0; //FIXME:
	cs_cb->key_misc.lspid = 1; /* GE-1 */
	cs_cb->action.acl_dsbl = 0;
	nat_guid = cs_kernel_adapt_get_guid(CS_KERNEL_NAT_ID_TAG);
	cs_cb->common.guid_cnt = 1;
	cs_cb->common.guid[cs_cb->common.guid_cnt] = nat_guid;
	/* Input , key */
	cs_cb->input.l3_nh.iph.protocol = CS_TEST_PROTOCOL;
	cs_cb->input.l3_nh.iph.sip = CS_TEST_CLIENT_IP;
	cs_cb->input.l3_nh.iph.dip = CS_TEST_SERVER_IP;
	cs_cb->input.l4_h.th.sport = CS_TEST_SPORT;
	cs_cb->input.l4_h.th.dport = CS_TEST_DPORT;
	/* Output , parameter and action */
	cs_cb->action.l2.mac_sa_replace_en = 1;
	cs_cb->action.l2.mac_da_replace_en = 1;
	cs_cb->action.l3.ip_sa_replace_en = 1;
	cs_cb->action.l3.ip_da_replace_en = 1;
	cs_cb->action.l4.sp_replace_en = 1;
	cs_cb->action.l4.dp_replace_en = 1;
	memcpy(cs_cb->output.raw.da, cs_test_wan_target_da, 6);
	memcpy(cs_cb->output.raw.sa, cs_test_wan_my_da, 6);
	cs_cb->output.l3_nh.iph.sip = CS_TEST_WAN_IP;	
	cs_cb->output.l3_nh.iph.dip = CS_TEST_SERVER_IP;
	cs_cb->output.l4_h.th.sport = CS_TEST_MAP_PORT_BASE;
	cs_cb->output.l4_h.th.dport = CS_TEST_DPORT;
	cs_cb->action.voq_pol.d_voq_id = 16; //GE-2 
	cs_cb->action.voq_pol.d_pol_id = 1; 
	cs_cb->action.voq_pol.cpu_pid = 0; 
	cs_cb->action.voq_pol.ldpid = 0; 
	cs_cb->action.voq_pol.pppoe_session_id = 10;//for debug
	cs_cb->action.voq_pol.cos_nop = 0; 
	cs_cb->action.voq_pol.voq_policer_parity = 1; 
	
	cs_vtable_add_hash_by_cb(cs_cb, CS_VTBL_ID_L3_FLOW);
	
	
	/* WAN --> LAN */
	
	return CS_OK;
}	/* cs_kernel_core_add_hash_cfg */

cs_status cs_add_qos_tuple7_entries(int8_t i_pri, int8_t i_dscp,
	int8_t e_pri, int8_t e_dscp)
{
	/*
	 * Add QoS hash and result for tuple 7 here?
	 */

	fe_hash_rslt_s qos_result;
	cs_uint16	result_index, hash_mask_index, qos_crc16, hash_index;
	fe_sw_hash_t	qos_hash_key;
	CS_HASH_ENTRY_S	p_hash;
	fe_hash_mask_entry_t mask_rule;
	cs_uint32	qos_crc32;

#if 0
	p_hash.mask = (CS_HM_8021P_1_MASK	| CS_HM_DEI_1_MASK	|
					CS_HM_8021P_2_MASK	| CS_HM_DEI_2_MASK	|
					CS_HM_DSCP_MASK		| CS_HM_KEYGEN_POLY_SEL);
#endif
	p_hash.mask = (CS_HM_IP_VLD_MASK | CS_HM_DSCP_MASK | CS_HM_KEYGEN_POLY_SEL);
	//p_hash.mask = (CS_HM_8021P_1_MASK | CS_HM_DEI_1_MASK | CS_HM_KEYGEN_POLY_SEL);

	hash_mask_index = cs_vtable_to_hash_mask_qos_only(&p_hash, &mask_rule, 1);

	if (e_pri >= 0) {
		qos_result.qos_param.top_802_1p = e_pri;
		//result->qos_param.inner_802_1p = e_pri & _8021P_MASK;
		qos_result.qos_param.change_8021p_1_en = CS_RESULT_ACTION_ENABLE;
	}
	if (e_dscp >= 0) {
		qos_result.qos_param.dscp = e_dscp;
		qos_result.qos_param.change_dscp_en = CS_RESULT_ACTION_ENABLE;
	}
	printk("\t%s::QoS result %p, pri %x, dscp %x\n",
		__func__, &qos_result, qos_result.qos_param.inner_802_1p, qos_result.qos_param.dscp);

//	if (result->qos_param.change_8021p_1_en ||
//		result->qos_param.change_dscp_en)
		result_index = cs_vtable_to_qos_result(&qos_result, 1);

	memset(&qos_hash_key, 0, sizeof(fe_sw_hash_t));
//	qos_hash_key._8021p_1 = i_pri;
	qos_hash_key.dscp = i_dscp;
	qos_hash_key.ip_valid = 1;
	qos_hash_key.mask_ptr_0_7 = hash_mask_index;

	printk("%s::i_dscp %x, ip_valid on, hash mask index %x\n",
		__func__, i_dscp, hash_mask_index);

	cs_fe_hash_calc_crc(&qos_hash_key, &qos_crc32, &qos_crc16, CRC16_CCITT);
//	cs_vtable_add_sdb_qos_tuple(hash_mask_ptr, 1);

	printk("%s::add qos crc16 %x, crc32 %x, hash_mask_index %x, result_index %x\n",
		__func__, qos_crc16, qos_crc32, hash_mask_index, result_index);
//	cs_add_vtable_qos_entry(qos_crc16, qos_crc32, hash_mask_index,
//		result_index, &hash_index, 1, 0);	// 0 means never timeout

	//cs_hash_add_hash_entry(qos_crc16, qos_crc32, hash_mask_index, result_index,
	//	&hash_index, 0);
	cs_fe_hash_add_hash(qos_crc32, qos_crc16, hash_mask_index, result_index, &hash_index);

	printk("%s::add qos crc16 %x, crc32 %x, hash_mask_index %x, result_index %x, hash_index %x\n",
		__func__, qos_crc16, qos_crc32, hash_mask_index, result_index, hash_index);
	return CS_OK;
}
EXPORT_SYMBOL(cs_add_qos_tuple7_entries);

cs_status cs_kernel_core_init_cfg(void)
{
	CORE_CFG_T *cfg;
	//fe_hash_vtable_s *p_vtable_hash;
	
	if (core_initialized == 1) {
		return CS_ERROR;
	}
		
	core_initialized = 1;
	
	cfg = (CORE_CFG_T *)&core_cfg;
	memset((void *)cfg, 0, sizeof(CORE_CFG_T));
	
	cfg->enabled = 1;
	
	//FIXME: If only exist l2, then l2 need add default to cpu, modify later.
	//cs_alloc_vtable(&(cfg->l2_rule_vtable_id), CS_VTBL_ID_L2_RULE, 
	//		VTABLE_ACT_DEF_TO_VTBL, 0, VTABLE_FORCE_ID, VTABLE_DEFAULT_MCG_ID, 
	//		CS_VTBL_ID_L2_RULE_PRIORITY);
	
	//cs_alloc_vtable(&(cfg->l3_rule_vtable_id), CS_VTBL_ID_L3_RULE, 
	//		VTABLE_ACT_DEF_TO_VTBL, 0, VTABLE_FORCE_ID, VTABLE_DEFAULT_MCG_ID, 
	//		CS_VTBL_ID_L3_RULE_PRIORITY);	
	//FIXME:2nd vtable VTABLE_ACT_DEF_TO_VTBL, VTABLE_L3_FLOW_MCG_ID = 3  ???
#ifdef CS_UU_TEST	
	cs_alloc_vtable(&(cfg->l2_flow_vtable_id), CS_VTBL_ID_L2_FLOW, 
			VTABLE_ACT_DEF_TO_CPU, 0, VTABLE_FORCE_ID, CS_VTBL_ID_L2_FLOW, 
			CS_VTBL_ID_L2_FLOW_PRIORITY);	
#else		
	cs_alloc_vtable(&(cfg->l3_flow_vtable_id), CS_VTBL_ID_L3_FLOW, 
			VTABLE_ACT_DEF_TO_CPU, 0, VTABLE_FORCE_ID, CS_VTBL_ID_L3_FLOW, 
			CS_VTBL_ID_L3_FLOW_PRIORITY);
#endif /* CS_UU_TEST */
	
	//cs_alloc_vtable(&(cfg->l2_flow_vtable_id), CS_VTBL_ID_L2_FLOW, 
	//		VTABLE_ACT_DEF_TO_VTBL, CS_VTBL_ID_L3_FLOW, VTABLE_FORCE_ID, CS_VTBL_ID_L2_FLOW, 
	//		CS_VTBL_ID_L2_FLOW_PRIORITY);				
	return CS_OK;
} /* cs_kernel_core_init_cfg */

const struct file_operations cs_hw_accel_fops = {
	.read = cs_hw_accel_read,
	.open = cs_hw_accel_open,
	.release = cs_hw_accel_release,
};

ssize_t cs_hw_accel_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	DBG(printk("%s: \n",__func__));
	return  0;
}

int cs_hw_accel_open(struct inode *inode, struct file *filp)
{
	DBG(printk("%s: \n",__func__));
	return 0;
}

int cs_hw_accel_release(struct inode *inode, struct file *filp)
{
	DBG(printk("%s: \n",__func__));
	return 0;
}

int __init cs_hw_accel_init(void)
{
	int ret;

	ret = register_chrdev(cs_hw_accel_major, "CS752X_HW_ACCELERATION", &cs_hw_accel_fops);
	if (ret < 0) {
		printk(KERN_ERR "cs_hw_accel: can't get major %d.\n", cs_hw_accel_major);
		return ret;
	}
	
	cs_hw_accel_major = ret;
	cs_kernel_core_hw_fe_init();
	cs_kernel_core_init_cfg();
	//cs_nat_init();
	//cs_bridge_init();
	//cs_vlan_init();
#ifdef CONFIG_NET_SCHED
	//cs_qos_init();
#endif
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
	//cs_hw_accel_ipsec_init();
#endif
	//cs_pppoe_init();

#ifdef CONFIG_IPV6
	//cs_v6_route_init(); 
#endif
    //cs_mc_init();
	return 0;
}

void __exit cs_hw_accel_cleanup(void)
{
	unregister_chrdev(cs_hw_accel_major, "CS752X_HW_ACCELERATION");
}

module_init(cs_hw_accel_init);
module_exit(cs_hw_accel_cleanup);
