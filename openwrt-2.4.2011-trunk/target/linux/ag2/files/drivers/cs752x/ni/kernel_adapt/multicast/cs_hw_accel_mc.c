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
 * cs_hw_accel_mc.c
 *
 * $Id: cs_hw_accel_mc.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This file contains the implementation for CS Forwarding Engine Offload Kernel Module.
 */ 
#include <mach/hw_accel.h>
#include <cs_hw_accel_cb.h>
#include <cs_hw_accel_core.h>
#include <cs_hw_accel_base.h>
#include "cs_hw_accel_mc.h"
#include <linux/ip.h>

#define CS_MC_HASH_IPV4		4
#define CS_MC_HASH_IPV6		6
#define CS_MC_INDEX_TABLE	256
#define CS_MAX_MCGID 		256
static cs_boolean cs_mc_enbl = FALSE;
struct cs_mc_hash *mc_hash = NULL;
//cs_uint8 cs_global_mc_counter = CS_VTBL_ID_MC_FLOW;
cs_uint8 cs_global_mc_counter = 1;
cs_uint8 cs_mc_mcgid[CS_MC_INDEX_TABLE] = {0};

cs_mc_table_s *mcgid_table[CS_MAX_MCGID];

extern struct cs_kernel_hw_accel_jt hw_jt;

cs_status cs_mcgid_init(void) 
{
	int i;
	
	for (i=0; i < CS_MAX_MCGID; i++) {
		mcgid_table[i] = NULL;	
	}
	return CS_OK;
}

cs_status cs_mcgid_dallocate(cs_uint8 mcgid) 
{
	kfree(mcgid_table[mcgid]);
	mcgid_table[mcgid] = NULL;
	printk("%s:: Allocate mcgid table resource fail\n",__func__);
	return CS_ERROR;
}

cs_status cs_mcgid_allocate(cs_uint8 mcgid) 
{
	int i;
	
	for (i=0; i < CS_MAX_MCGID; i++) {
		if (mcgid_table[i] == NULL) {
			mcgid_table[i] = kmalloc(sizeof(cs_mc_table_s), GFP_ATOMIC);
			if (mcgid_table[i] == NULL)
				return CS_ERROR;
			memset((void *)mcgid_table[i], 0, sizeof(cs_mc_table_s));	
			break;
		}
	}
	
	mcgid_table[i]->mcgid = mcgid;
	mcgid_table[i]->mcindex = 0;	
}

int cs_find_mcindex_by_mcgid(cs_uint8 mcgid) 
{
	int i;

	for (i=0;i<CS_MAX_MCGID; i++) {
		if (mcgid_table[i] != NULL) 
			if (mcgid_table[i]->mcgid == mcgid) 
				return mcgid_table[i]->mcindex;
	}
	return CS_ERROR;
}


int get_m_index(void* mc_ptr, int type) 
{
#if 0	
	switch (type) {
		case CS_MC_HASH_IPV4:
			return ((struct mfc_cache *)(mc_ptr).mfc_un.res.cs_m_index);
		case CS_MC_HASH_IPV6:
			return ((struct mfc6_cache *)(mc_ptr).mfc_un.res.cs_m_index);
	}
#endif	
	return 1;
}

int set_m_index(struct mfc_cache *mc_ptr, int idx) 
{
#if 0	
	switch (mc_ptr->type) {
		case CS_MC_HASH_IPV4:
			((struct mfc_cache *)(mc_ptr)->mfc_un.res.cs_m_index) = idx;
		case CS_MC_HASH_IPV6:
			((struct mfc6_cache *)(mc_ptr)->mfc_un.res.cs_m_index) = idx;
	}
#endif	
	return 1;
}

int add_cs_mc_hash(cs_uint64 guid, void *ptr, int type, int vifi) 
{
	struct cs_mc_hash *s;

	s = kmalloc(sizeof(struct cs_mc_hash), GFP_KERNEL);
	if (!s) return 0;
	s->guid = guid;
	s->mfc_ptr = ptr;
	s->type = type;
	s->vifi = vifi;
	HASH_ADD_INT(mc_hash, guid, s);
	return 1;
}

struct cs_mc_hash *find_mc_hash(cs_uint64 guid) 
{
	struct cs_mc_hash *s;

	HASH_FIND_INT( mc_hash, &guid, s);
	return s;
}

void delete_cs_mc_hash(struct cs_mc_hash *s) 
{
	HASH_DEL ( mc_hash, s);
	kfree(s);
}

void delete_cs_mc_hash_by_guid(cs_uint64 guid) 
{
	struct cs_mc_hash *s;

	HASH_FIND_INT( mc_hash, &guid, s);
	if (s != NULL) delete_cs_mc_hash( s);
}

/*
	Jump Table Entries
 */
void cs_mc_shared_delete(cs_uint64 *cs_guid) 
{
	int i;

	for(i=0; i<MAXVIFS; i++) {
		if (cs_guid[i] != CS_KERNEL_INVALID_GUID) {
			//FIXME: add later
			//cs_kernel_adapt_delete_hash(cs_guid[i]);
			// remove guid from the hash table
			cs_guid[i] = CS_KERNEL_INVALID_GUID;
		}
	}
}

void k_jt_cs_mc_ipv4_delete_hook(struct mfc_cache *c) 
{
	cs_mc_shared_delete(c->mfc_un.res.cs_guid);
}

void k_jt_cs_mc_ipv6_delete_hook(struct mfc6_cache *c) 
{
	cs_mc_shared_delete(c->mfc_un.res.cs_guid );
}

void cs_mc_add_hash_after_forwarding(struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct iphdr *iph = ip_hdr(skb);
	
	if (cs_cb == NULL) return;
	if (cs_cb->common.tag != CS_CB_TAG) return;
	
	if (!(cs_cb->common.module_mask & CS_MOD_MASK_IPV4_MULTICAST)) return;
	
	//FIXME: what action do we need?	
	cs_cb->output.l3_nh.iph.sip = iph->saddr;
	cs_cb->output.l3_nh.iph.dip = iph->daddr;
	cs_cb->output.l3_nh.iph.protocol = iph->protocol;
	
	if (iph->protocol == IPPROTO_TCP) {
		printk("%s:: iph->protocol = %d\n",__func__, iph->protocol);	
	}
}

int cs_kernel_mc_core_logic_extension(struct sk_buff * skb)  
{
	//cs_uint64 guid;
	//int index, m_index, mcidx;
	//struct cs_mc_hash *mch;

	//index = 0;
	//guid = cs_kernel_adapt_get_guid( CS_KERNEL_MC_ID_TAG, skb, &index);
	//guid = cs_kernel_adapt_get_guid(CS_KERNEL_MC_ID_TAG);
	//if (cs_kernel_get_guid(CS_KERNEL_MC_ID_TAG, skb, &index) ) {
		// FIXME
		// there are more than one multicast guid, something is wrong, should exist and 
		// let software deals with this
	//}
	//mch = find_mfc_from_guid(guid);
	//if (!mch) {
		// FIXME
		// something is really wrong, cannot find the source. Bail again.
	//}
	// m_index = get_m_index((void *)mch, );
	//if (m_index == (-1) ) {
		//goto build_dup_hash_entry;
	//} else {
		// FIXME
		// compare m_index to have the same hash as
		// what we want. If not, allocate new one.
		// get mcgid from the hash
		//goto dup_hash_entry_exists;
	//}
	// FIXME
	// allocate mcgid
	// build hash
	// build result table
	cs_mc_add_hash_after_forwarding(skb);	
	//set_m_index( mch, /* hash index */ );
	// FIXME
	// insert hash/result table in to hardware
//dup_hash_entry_exists:
	// FIXME
	// get mcgid from m_index
	// cs_allocate_std_mcidx ( mcgid, &mcidx );
	// FIXME
	// mark skb with mcgid and mcidx
	// continue the flow
}

void cs_mc_set_skb_cb_info(struct sk_buff *skb, cs_uint8 vif)
{ 
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct ethhdr *eth = eth_hdr(skb);
	struct iphdr *iph = ip_hdr(skb);
	
	/* Set the hash mask here, but cannot insert / complete action.  Hash 
	 * entries will be created when cs_hw_accel_mc_create_hash() is called 
	 * by core logic at the packet transmission stage. */
	 
	 /* 1st entry for CS_MOD_MASK_IPV4_MULTICAST
	 * mask requirement:
	 * WAN --> LAN
	 * 1. l2 MAC DA
	 * 2. l3 SIP 
	 * 3. l3 DIP
	 * 4. vlan
	 * 5. pppoe
	 * Action: 
	 * 1. L2 source mac, vlan, pppoe ??
	 * 2. destination voq = root queue 
	 * 3. MCGID */
	
	cs_cb->common.module_mask |= CS_MOD_MASK_IPV4_MULTICAST;
	//cs_cb->common.vtype = CS_VTBL_ID_MC_FLOW;//FIXME: for multicast vtable, need management Vtable mcgid
	//cs_cb->common.input_dev = &in_dev;	
	//FIXME: for debug to know why hit miss? CH
	cs_cb->input_mask |= (CS_HM_MAC_DA_MASK | CS_HM_IP_DA_MASK | CS_HM_IP_SA_MASK);
	//cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	memcpy(cs_cb->input.raw.da, eth->h_dest, ETH_ALEN);
	cs_cb->input.l3_nh.iph.sip = iph->saddr;
	cs_cb->input.l3_nh.iph.dip = iph->daddr;
	//cs_cb->input.l3_nh.iph.protocol = iph->protocol;
	//cs_cb->key_misc.mcgid = mcgid;
	cs_cb->key_misc.vif = vif;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	return;
}

cs_uint8 cs_mc_get_mcgid(void) 
{
	cs_uint8 tmpVal;
	
	//FIXME: mutex_lock instead of spin_lock
	//spin_lock(&cs_global_guid_lock);
	tmpVal = cs_global_mc_counter;
	cs_global_mc_counter++;
	
	if (tmpVal >= CS_MAX_MCGID)
	{
		printk("Out of mcgid\n");
		return CS_ERROR;
	}
	//spin_unlock( &cs_global_guid_lock);
	return tmpVal;
}

void cs_mc_shared_forwarding(int *cs_stat, cs_uint64 *cs_guid, 
	struct sk_buff *skb, int vifi, int flag, struct mfc_cache *c, cs_uint16 mask) 
{
	cs_uint64 guid;
	struct net *net = mfc_net(c);
 	cs_uint8 mcgid;
 	cs_uint8 vif;
 	
	// Return if the entry has been marked for software only
	//if (*cs_stat == CS_MC_SW_ONLY) {
	//	printk("%s:: ----> SW ONLY \n",__func__);
	//	return;
	//}

#if 0	
	//FIXME: no guid concept in new NE design,CH
	//guid = cs_kernel_adapt_get_guid(CS_KERNEL_MC_ID_TAG);
	// add guid/mfc_cache into hash table
	if (!add_cs_mc_hash(guid, (void *)c, flag, vifi)) {
		*cs_stat = CS_MC_SW_ONLY;
		cs_mc_shared_delete(cs_guid);
		return;
	}
	//cs_kernel_adapt_set_mc_hash_guid(skb, guid);
	//cs_kernel_adapt_set_mc_hash_mask(skb, mask);
	cs_guid[vifi] = guid;
#endif
	
	//mcgid = cs_mc_get_mcgid();
	//cs_mc_mcgid[mcgid_index] = guid;
	//FIXME: to know how many mc_index 
	vif = c->mfc_un.res.maxvif - c->mfc_un.res.minvif;
	printk("%s:: vif = %d\n",__func__, vif);

	//cs_mcgid_allocate(mcgid, mcindex);
	//mc_index = c->mfc_un.res.cs_m_index;
	
	cs_mc_set_skb_cb_info(skb, vif);
	
	return;
}

void k_jt_cs_mc_ipv4_forwarding_hook(struct sk_buff *skb, struct mfc_cache *c, int vifi) 
{
	cs_mc_shared_forwarding(&(c->mfc_un.res.cs_stat), c->mfc_un.res.cs_guid,
			skb, vifi, CS_MC_HASH_IPV4, c, CS_MOD_MASK_IPV4_MULTICAST);
}

void k_jt_cs_mc_ipv6_forwarding_hook(struct sk_buff *skb, struct mfc6_cache *c, int vifi) 
{
	cs_mc_shared_forwarding(&(c->mfc_un.res.cs_stat), c->mfc_un.res.cs_guid,
			skb, vifi, CS_MC_HASH_IPV6, c, CS_MOD_MASK_IPV6_MULTICAST);
}

#if 0
//void k_jt_cs_mc_ipv4_add_hook(struct mfc_cache *c) 
cs_status k_jt_cs_mc_ipv4_add_hook(struct sk_buff *skb, 
						struct in_device *in_dev, cs_uint32 mc_addr)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct iphdr *iph = ip_hdr(skb);
	
	if (cs_mc_enbl == FALSE) return CS_ERROR;

	if (cs_cb == NULL) return CS_ERROR;
	if (cs_cb->common.tag != CS_CB_TAG) return CS_ERROR;

	if (cs_cb->common.sw_only & (CS_SWONLY_HW_ACCEL | CS_SWONLY_STATE)) {
		cs_cb->common.sw_only = CS_SWONLY_STATE;
		return CS_ERROR;
	}
	
	cs_cb->common.module_mask |= CS_MOD_MASK_IPV4_MULTICAST;
	cs_cb->common.vtype = CS_VTBL_ID_MC_FLOW;//FIXME: create MCTABLE ID
	//cs_cb->common.input_dev = &in_dev;	

	cs_cb->input_mask |= (CS_HM_MAC_DA_MASK | CS_HM_IP_DA_MASK | CS_HM_IP_SA_MASK);
	memcpy(cs_cb->input.raw.da, mc_addr, ETH_ALEN);
	cs_cb->input.l3_nh.iph.sip = iph->saddr;
	cs_cb->input.l3_nh.iph.dip = iph->daddr;
	cs_cb->input.l3_nh.iph.protocol = iph->protocol;

	return CS_OK;
}

//void k_jt_cs_mc_ipv6_add_hook(struct mfc6_cache *c, int vifi)
void k_jt_cs_mc_ipv6_add_hook(struct mfc6_cache *c) 
{
	//k_jt_cs_mc_ipv6_delete_hook(c);
}
#endif //#if 0

void cs_mc_shared_init(cs_uint64 *cs_guid, int *cs_stat) 
{
	int i;

	for(i=0; i<MAXVIFS; i++) 
		cs_guid[i] = CS_KERNEL_INVALID_GUID;
	*cs_stat = CS_MC_OK;
}

void k_jt_cs_mc_ipv4_init(struct mfc_cache *c) 
{
	cs_mc_shared_init(c->mfc_un.res.cs_guid, &(c->mfc_un.res.cs_stat));
}

#if 0
void k_jt_cs_mc_ipv6_init(struct mfc6_cache *c) 
{
	cs_mc_shared_init(c->mfc_un.res.cs_guid, &(c->mfc_un.res.cs_stat));
}
#endif

void cs_kernel_mc_cb(cs_uint64 guid, int status) 
{
	//struct mfc_cache *c4;
	//struct mfc6_cache *c6;
	//struct cs_mc_hash *s;

	switch( status) {
		case CS_HASH_CB_FAILED_SW_REQ:
		case CS_HASH_CB_FAILED_RESOURCE:
		case CS_HASH_CB_DELETE_BY_PEER:
#if 0 //FIXME: verify later			
			// determine the multicast group
			s = find_mc_hash(guid);
			if (s) {
				cs_uint64 *stat, *gArray;
				if (s->type ==  CS_MC_HASH_IPV4) {
					gArray =((struct mfc_cache *)(s->mfc_ptr)->
						mfc_un.res.cs_guid); 
					stat = &((struct mfc_cache *)(s->mfc_ptr)->
						mfc_un.res.cs_stat); 
				} else {
					gArray =((struct mfc6_cache *)(s->mfc_ptr)->
						mfc6_un.res.cs_guid); 
					stat = &((struct mfc6_cache *)(s->mfc_ptr)->
						mfc6_un.res.cs_stat); 
				}

				gArray[s->vifi] = CS_KERNEL_INVALID_GUID;
				if (status != CS_HASH_CB_DELETE_BY_PEER)
					(*stat) = CS_MC_SW;		
			}
			cs_mc_shared_delete(gArray);
#endif /* #if 0 */			
		case CS_HASH_CB_SUCCESSFUL:
		default:
			break;
	}
}

cs_status cs_mc_set_2nd_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 mcindex, 
		cs_uint8 o_lspid, cs_uint8 d_voq)
//		cs_uint8 o_lspid, cs_uint8 d_voq, cs_uint8 vid)
{
	CS_HASH_ENTRY_S *hash_entry;
	cs_uint8 hash_data[sizeof(CS_HASH_ENTRY_S)];
	cs_status status;
	
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = o_lspid; /* GE-# */
	cs_cb->key_misc.mcidx = mcindex; /* 0x0010 */
	//cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = d_voq; /* destinatio voq */
	/* FIXME: timeout ?? */
	cs_cb->tmo.interval = 300;
	
	hash_entry = (CS_HASH_ENTRY_S *)&hash_data;
	memset((void *)hash_entry, 0, sizeof(CS_HASH_ENTRY_S));

	status = cs_kernel_get_hash_mask_from_cs_cb(cs_cb, &hash_entry->mask);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	status = cs_kernel_get_hash_key_from_cs_cb(cs_cb, &hash_entry->key);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	status = cs_kernel_get_hash_action_from_cs_cb(cs_cb, &hash_entry->result);
	if (status != CS_OK)
		return CS_HASH_ADD_ERR_SKB_CB;

	return cs_kernel_core_add_hash(hash_entry, CS_VTBL_ID_L2_FLOW,
			cs_cb->common.guid, cs_cb->common.guid_cnt, cs_cb->tmo.interval);
	
	return CS_OK;
}

cs_status cs_hw_accel_mc_create_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint64 guid) 
{
	CS_HASH_ENTRY_S hash_entry;
	cs_boolean f_vlan_remove = FALSE, f_pppoe_remove = FALSE;
	cs_status status;
	unsigned int rc, i;
	cs_uint8 mcindex;
	int mcgid;
	
	if (cs_cb == NULL) return CS_ERROR;
		
	memset(&hash_entry, 0x0, sizeof(CS_HASH_ENTRY_S));
	
	if(cs_cb->common.tag != CS_CB_TAG)
		return CS_ERROR;
		
	if (cs_cb->common.sw_only & CS_SWONLY_STATE) {
		/* this hash was marked with HW_ACCEL earlier, but someone 
		 * modifies the decision.. now we need to get rid of some stuff */
		printk("%s: SW ONLY -----> \n",__func__);
		return CS_ERROR;
	}
	
	/* 1st entry for CS_MOD_MASK_IPV4_MULTICAST
	 * mask requirement:
	 * WAN --> LAN
	 * 1. l2 MAC DA
	 * 2. l3 SIP 
	 * 3. l3 DIP
	 * Action: 
	 * 1. L2 source mac, vlan, pppoe 
	 * 2. destination voq = root queue 
	 * 3. MCGID */
	cs_cb->output_mask |= CS_HM_MCGID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	//cs_cb->common.vtype = CS_VTBL_ID_MC_FLOW;
	/* FIXME: timeout ?? */
	cs_cb->tmo.interval = 300;
	
	//cs_mcgid_allocate(mcgid);
	//FIXME: temp add mcgid, fe/cs_fe_mc.c
	//cs_allocate_mcg_id(&mcgid);
	//FIXME: hard core her for debug
	mcgid = cs_mc_get_mcgid();
	mcindex = cs_cb->key_misc.vif;
	printk("==> mcgid = %d, vif = 0x%X\n", mcgid, cs_cb->key_misc.vif);
	/* we can share the hash mask from cb for the first hash field */
	status = cs_kernel_get_hash_mask_from_cs_cb(cs_cb, &(hash_entry.mask));
		if (status != CS_OK) return status;
			
	status = cs_kernel_get_hash_key_from_cs_cb(cs_cb, &(hash_entry.key));
	
	if (CS_OK != status) return status;
	
	//FIXME: What different between CS_FWD_MC and CS_FWD_NORMAL?
	hash_entry.result.param.fwd_type = CS_FWD_MC;
	//hash_entry.result.param.fwd_type = CS_FWD_NORMAL;
	hash_entry.result.param.d_voq = (ROOT_PORT_VOQ_BASE + 7); //voq: 47,0x2F
	hash_entry.result.action.fwd_type_valid = CS_RESULT_ACTION_ENABLE;
	//FIXME: hard core her for debug
	hash_entry.result.param.mcgid = mcgid;
	hash_entry.result.action.mcgid_valid = CS_RESULT_ACTION_ENABLE;

	/* check if VLAN exists, remove it. */
	if ((cs_cb->action.l2.flow_vlan_op_en == CS_VLAN_OP_INSERT) || 
			(cs_cb->action.l2.flow_vlan_op_en == CS_VLAN_OP_REPLACE)) {
		hash_entry.result.action.flow_vlan_op_en = CS_VLAN_OP_REMOVE;
		f_vlan_remove = TRUE;
	}
#if 0	// FIXME... the following..
		/* check if PPPoE exists, remove it. */
		if (CS_HASH_MASK_PPPOE & hash_entry.mask) {
			hash_entry.result.action.pppoe_decap_en = 1;
			f_pppoe_remove = TRUE;
		}
#endif
	//FIXME: instead of CS_VTBL_ID_L2_FLOW 
	rc = cs_kernel_core_add_hash(&hash_entry, 
				CS_VTBL_ID_L2_FLOW, cs_cb->common.guid, 
				cs_cb->common.guid_cnt, cs_cb->tmo.interval);
	if (rc == CS_HASH_ADD_ERR_OUT_OF_RESOURCE)
		return CS_ERROR;
		
	printk("11 ==> mcgid = %d, vif = 0x%X\n", mcgid, cs_cb->key_misc.vif);
	
	//set_mc_table(cs_cb->key_misc.mcgid, CS_WRITE, cs_cb->key_misc.vif);
	//FIXME: hardcode for 3rd parameter. need from mcindex map table
	set_mc_table(mcgid, CS_WRITE, 0x2); //0b0010
	
	for(i = 0; i < mcindex; i++) {
		//FIXME: hardcode for 2nd parameter.
		cs_mc_set_2nd_hash(cs_cb, 1, GE_PORT2, GE_PORT1_VOQ_BASE + 1);
	}
	
	return CS_OK;
} /* cs_hw_accel_mc_create_hash */

void cs_mc_init(void) 
{
	/* init vid guid mapping list */
	//FIXME: need implement
	cs_kernel_reg_hash_cb(CS_KERNEL_MC_ID_TAG, cs_kernel_mc_cb);
	
	/* register call back function */
	//hw_jt.cs_mc_ipv4_add_hook = &k_jt_cs_mc_ipv4_add_hook;
	hw_jt.cs_mc_ipv4_forwarding_hook = &k_jt_cs_mc_ipv4_forwarding_hook;
	//hw_jt.cs_mc_ipv4_delete_hook = &k_jt_cs_mc_ipv4_delete_hook;
	
	cs_mc_enbl = TRUE;
	cs_mcgid_init();
}
