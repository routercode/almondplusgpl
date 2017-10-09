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
/*
 * cs_hw_accel_qos_core.c
 *
 * $Id: cs_hw_accel_qos_core.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This file contains the core implementation for CS QoS implementation.
 */
#ifdef CONFIG_NET_SCHED
#include <linux/netdevice.h>
#include <cs_hw_accel_cb.h>
#include <cs_hw_accel_core.h>
#include <cs_hw_accel_util.h>
#include "cs_hw_accel_qos.h"
#include "cs_hw_accel_qos_data.h"
#include "cs_sch.h"
#include "cs_tm.h"

static cs_qos_voq_param_t voq_darray[CS_QOS_PORT_NUM][CS_QOS_VOQ_PER_PORT];
static cs_qos_pol_guid_t pol_guid_map_array[CS_QOS_FLOW_POL_NUM];
static struct list_head guid_list_head;

extern cs_uint8 cs_ni_get_port_id(struct net_device *dev);

/* Internal APIs */
static cs_status cs_qos_check_queue_grouping(cs_uint8 voq_id, 
		cs_qos_voq_type_e type, cs_uint8 priority)
{
	cs_uint8 port_idx, queue_idx, iii, curr_priority;

	port_idx = voq_id >> 3;
	queue_idx = voq_id & 0x07;

	/* if setting VOQ to SP mode, we have to check the following VOQ group 
	 * conditions:
	 * 1) All the previous VOQs must be in SP mode.
	 * 2) The VOQ with highest priority is in the earliest position. meaning 
	 * the priority of the previous VOQ must be larger than the current VOQ. */
	if (type == CS_QOS_VOQ_TYPE_SP) {
		curr_priority = 0xff;
		for (iii = 0; iii < queue_idx; iii++) {
			if (voq_darray[port_idx][iii].type != CS_QOS_VOQ_TYPE_SP)
				return CS_ERROR;
			if (((iii == 0) && 
						(curr_priority < voq_darray[port_idx][iii].priority)) || 
					((iii != 0) && 
					 (curr_priority <= voq_darray[port_idx][iii].priority)))
				return CS_ERROR;
			curr_priority = voq_darray[port_idx][iii].priority;
		}
		/* if reach this point, we need to check whether it's ok to insert/update 
		 * the priority of this SP VOQ here.  Conditions are:
		 * 1) the new priority is smaller than the previous VOQ's priority.
		 * 2) the new priority assigned is larger then the next queue if the next VOQ 
		 * is running in SP mode. */
		if (priority >= curr_priority) return CS_ERROR;

		if (queue_idx < (CS_QOS_VOQ_PER_PORT - 1)) {
			/* don't have to check if we are modifying the last VOQ */
			if (voq_darray[port_idx][queue_idx+1].type == CS_QOS_VOQ_TYPE_SP) {
				/* only perform the check when the next VOQ is also running in 
				 * SP mode. */
				if (priority <= voq_darray[port_idx][queue_idx+1].priority)
					return CS_ERROR;
			}
		}
	} else {	/* if (CS_QOS_VOQ_TYPE_DRR == type) */
		/* all we need to check is that the later VOQs are running in DRR mode. */
		for (iii = (queue_idx + 1); iii < CS_QOS_VOQ_PER_PORT; iii++) {
			if (voq_darray[port_idx][iii].type != CS_QOS_VOQ_TYPE_DRR)
				return CS_ERROR;
		}
	}

	/* if reach here.. everything has been checked ok. */
	return CS_OK;
} /* cs_qos_check_queue_grouping */

/* this internal API is to find the lower rate in the DRR VOQ group that this 
 * VOQ with given voq_id is belonged to. */
static cs_status cs_qos_get_drr_group_rate(cs_uint8 voq_id, 
		cs_uint32 new_rate, cs_uint32 *p_rslt_rate)
{
	cs_uint8 port_idx, queue_idx, iii;

	port_idx = voq_id >> 3;
	queue_idx = voq_id & 0x07;

	*p_rslt_rate = new_rate;

	for (iii = 0; iii < CS_QOS_VOQ_PER_PORT; iii++) {
		if ((voq_darray[port_idx][iii].type == CS_QOS_VOQ_TYPE_DRR) && 
				(iii != queue_idx) && 
				(voq_darray[port_idx][iii].rate != 0) && 
				(voq_darray[port_idx][iii].rate < (*p_rslt_rate)))
			*p_rslt_rate = voq_darray[port_idx][iii].rate;
	}

	return CS_OK;
} /* cs_qos_drr_group_rate */

/* Module APIs */
void cs_qos_set_voq_id_to_skb_cs_cb(struct sk_buff *skb, 
		struct net_device *dev, cs_uint8 queue_id)
{
	cs_uint8 port_id = cs_ni_get_port_id(dev);
	cs_uint8 qid = queue_id;
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);

	if (8 <= qid) qid = 0;
	else cs_cb->action.voq_pol.d_voq_id = 
		CS_QOS_VOQ_ID_FROM_PORT_QUEUE_ID(port_id, qid);
} /* cs_qos_set_voq_id_to_skb_cs_cb */

/* This API is to set up port parameters. burst_size is in bytes. 
 * rate is in bps.  Won't do anything if the value is 0. */
cs_status cs_qos_set_port_param(cs_uint8 port_id, cs_uint16 burst_size, 
		cs_uint32 rate)
{
	if (CS_OK != cs_sch_set_port_burst(port_id, burst_size))
		return CS_ERROR;

	if (CS_OK != cs_sch_set_port_rate(port_id, rate, 0))
		return CS_ERROR;

	return CS_OK;
} /* cs_qos_set_port_param */

/* This API is to set up VoQ parameters, such as whether this VOQ is 
 * running in SP mode or DRR mode. 
 * Acceptable range for priority is 0 to 7.
 * VOQ will be running in SP mode if (priority != 0) || (weight == 0). 
 * If (priority == 0) && (weight != 0), VOQ will be running in DRR more. 
 * Rate is in bps. */
cs_status cs_qos_set_voq_param(cs_uint8 port_id, cs_uint8 voq_id, 
		 cs_uint8 priority, cs_uint32 weight, cs_uint32 rsrv_size, 
		 cs_uint32 max_size, cs_uint32 rate, struct tc_wredspec *p_wred)
{
	cs_uint8 port_idx, queue_idx;

	port_idx = ((voq_id >> 3) & 0x1f);
	queue_idx = (voq_id & 0x07);

	if ((weight == 0) || (priority != 0)) {
		/* turning this VOQ into SP VOQ */
		if (priority > CS_QOS_VOQ_PRIORITY_MAX) return CS_ERROR;

		/* need to make sure all the SP and DRR queues are grouping properly. */
		if (cs_qos_check_queue_grouping(voq_id, CS_QOS_VOQ_TYPE_SP, priority) 
				!= CS_OK)
			return CS_ERROR;

		if (cs_sch_set_queue_sp(port_idx, queue_idx, rate) != CS_OK)
			return CS_ERROR;

		/* update SW values */
		voq_darray[port_idx][queue_idx].type = CS_QOS_VOQ_TYPE_SP;
		voq_darray[port_idx][queue_idx].priority = priority;
		voq_darray[port_idx][queue_idx].weight = 0;	/* well in SP mode, weight 
													   is not needed. */
		voq_darray[port_idx][queue_idx].rate = rate;
	} else {
		cs_uint32 real_rate;

		/* need to make sure all the SP and DRR queues are grouping properly. */
		if (cs_qos_check_queue_grouping(voq_id, CS_QOS_VOQ_TYPE_DRR, 0) != CS_OK)
			return CS_ERROR;

		/* in DRR case, the real rate is the lowest rate of all the DRR VOQ 
		 * in the group. */
		if (cs_qos_get_drr_group_rate(voq_id, rate, &real_rate) != CS_OK)
			return CS_ERROR;

		if (cs_sch_set_queue_drr(port_idx, queue_idx, weight, real_rate) != CS_OK)
			return CS_ERROR;

		/* update SW values */
		voq_darray[port_idx][queue_idx].type = CS_QOS_VOQ_TYPE_DRR;
		voq_darray[port_idx][queue_idx].priority = priority;	/* == 0 */
		voq_darray[port_idx][queue_idx].weight = weight;
		voq_darray[port_idx][queue_idx].rate = rate;
	}

	if (cs_tm_bm_set_voq_profile(voq_id, rsrv_size, max_size, p_wred) != CS_OK)
		return CS_ERROR;

	return CS_OK;
} /* cs_qos_set_voq_param */

/* this API is used to retrieve the current depth value store in the 
 * hardware table */
cs_status cs_qos_get_voq_depth(cs_uint8 port_id, cs_uint8 voq_id, 
		cs_uint16 *p_min_depth, cs_uint16 *p_max_depth)
{
	return cs_tm_bm_get_voq_depth(voq_id, p_min_depth, p_max_depth);
} /* cs_qos_get_voq_depth */

cs_status cs_qos_reset_port(cs_uint8 port_id)
{
	if (cs_sch_reset_port(port_id) != CS_OK) return CS_ERROR;
	return CS_OK;
} /* cs_qos_reset_port */

cs_status cs_qos_reset_voq(cs_uint8 port_id, cs_uint8 voq_id)
{
	if (cs_tm_voq_reset(voq_id) != CS_OK) return CS_ERROR;
	if (cs_sch_reset_queue(port_id, (voq_id & 0x07)) != CS_OK)
		return CS_ERROR;
	return CS_OK;
} /* cs_qos_reset_voq */

cs_status cs_qos_set_port_pol_cfg(cs_uint8 port_id, cs_uint8 enbl, 
		cs_uint8 bypass_yellow, cs_uint8 bypass_red, cs_uint32 rate, 
		cs_uint32 cbs, cs_uint32 pbs)
{
	cs_tm_pol_profile_mem_t pol_profile;
	cs_tm_pol_freq_select_t freq_sel;
	cs_uint16 cir_credit;

	memset(&pol_profile, 0, sizeof(pol_profile));

	if (enbl == 1) {
		if (cs_tm_pol_convert_rate_to_hw_value(rate, FALSE, &freq_sel, 
					&cir_credit) != CS_OK)
			return CS_ERROR;

		pol_profile.policer_type = CS_TM_POL_RFC_2697;
		pol_profile.range = freq_sel >> 1;
		pol_profile.cir_credit = cir_credit;
		pol_profile.cir_max_credit = cbs >> 7;
		pol_profile.pir_max_credit = pbs >> 7;
		pol_profile.bypass_yellow = bypass_yellow;
		pol_profile.bypass_red = bypass_red;
	} else pol_profile.policer_type = CS_TM_POL_DISABLE;

	if (cs_tm_pol_set_profile_mem(CS_TM_POL_SPID_PROFILE_MEM, port_id, 
				&pol_profile) != CS_OK)
		return CS_ERROR;

	return CS_OK;
} /* cs_qos_set_port_pol_cfg */

/* Jump table APIs */
void k_jt_cs_qos_set_skb_sw_only(struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);

	cs_cb->common.sw_only = CS_SWONLY_STATE;
} /* k_jt_cs_qos_set_skb_sw_only */

static cs_status cs_qos_insert_map_guid_to_pol_id(cs_uint8 pol_id, cs_uint64 guid)
{
	cs_qos_pol_guid_t *p_pol_guid_map;
	cs_qos_guid_map_t *p_guid_map;

	p_pol_guid_map = &pol_guid_map_array[pol_id];

	if (p_pol_guid_map->cnt == 0)
		INIT_LIST_HEAD(&(p_pol_guid_map->guid_map_head));

	p_guid_map = kmalloc(sizeof(cs_qos_guid_map_t), GFP_KERNEL);
	if (p_guid_map == NULL) return CS_ERROR;

	p_guid_map->pol_id = pol_id;
	p_guid_map->guid = guid;
	list_add(&(p_guid_map->pol_list), &(p_pol_guid_map->guid_map_head));
	list_add(&(p_guid_map->guid_list), &guid_list_head);
	p_pol_guid_map->cnt++;

	return CS_OK;
} /* cs_qos_insert_map_guid_to_pol_id */

void cs_qos_delete_by_guid(cs_uint64 guid)
{
	struct list_head *next;
	cs_qos_guid_map_t *p_guid_map;

	list_for_each(next, &guid_list_head) {
		p_guid_map = (cs_qos_guid_map_t*)list_entry(next, cs_qos_guid_map_t, 
				guid_list);
		if (p_guid_map->guid == guid) {
			list_del(&p_guid_map->guid_list);
			list_del(&p_guid_map->pol_list);
			return;
		}
	}
	return;
} /* cs_qos_delete_by_guid */

void cs_qos_delete_by_pol_id(cs_uint8 pol_id)
{
	cs_qos_pol_guid_t *p_pol_guid_map;
	cs_qos_guid_map_t *p_guid_map;
	struct list_head *next;

	p_pol_guid_map = &pol_guid_map_array[pol_id];

	if (p_pol_guid_map->cnt == 0) return;

	list_for_each(next, &(p_pol_guid_map->guid_map_head)) {
		p_guid_map = (cs_qos_guid_map_t*)list_entry(next, 
				cs_qos_guid_map_t, pol_list);
		/* FIXME!!.. at this point, we just delete the hash.. maybe 
		 * we can set the hash entry to use another pol_id */
		cs_kernel_core_del_hash_by_guid(p_guid_map->guid);
		list_del(&p_guid_map->guid_list);
		list_del(&p_guid_map->pol_list);
		p_pol_guid_map->cnt--;
	}
	if (p_pol_guid_map->cnt < 0)
		printk("%s::weird. are we deleting more than what we have?\n", 
				__func__);
	p_pol_guid_map->cnt = 0;
	return;
}

#ifdef CONFIG_NET_ACT_POLICE
void k_jt_cs_qos_set_skb_pol_id(struct sk_buff *skb, u8 pol_id)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	cs_uint64 guid;
	cs_status status;

	if (cs_cb->action.voq_pol.d_pol_id != 0) {
		/* check if there is a previously existing pol_id assigned on this 
		 * skb/flow, if so, mark the skb sw-only and reassign the pol_id to 0 
		 * on this skb. */
		cs_cb->common.sw_only = CS_SWONLY_STATE;
		cs_cb->action.voq_pol.d_pol_id = 0;
		/* FIXME!!. the following API is not yet implemented */
		//guid = cs_kernel_get_guid(CS_KERNEL_QOS_ID_TAG, skb, 0);
		guid = 0;
		if (guid != 0) cs_qos_delete_by_guid(guid);
		return;
	}

	/* for every new flow, we have new guid, and we keep pol_id<->gu_id info, 
	 * but if a later decision indicating this packet is SW only, how do we 
	 * clean this entry off the list? */

	guid = cs_kernel_adapt_get_guid(CS_KERNEL_QOS_ID_TAG);
	if (guid == CS_KERNEL_INVALID_GUID) return;

	status = cs_kernel_add_guid(guid, skb);
	if (status != CS_OK) return;

	status = cs_qos_insert_map_guid_to_pol_id(pol_id, guid);
	if (status != CS_OK) goto CLEAN_SKB_CB;

	cs_cb->action.voq_pol.d_pol_id = pol_id;

	return;

CLEAN_SKB_CB:
	// FIXME!! more stuff?
	return;
} /* k_jt_cs_qos_set_skb_pol_id */

void k_jt_cs_qos_enbl_filter_policer(u8 *p_pol_id, u32 cir, u32 cbs, u32 pir, 
		u32 pbs)
{
	cs_tm_pol_profile_mem_t pol_profile;
	cs_status status;
	u8 f_get_new = 1, pol_id;

	memset(&pol_profile, 0, sizeof(pol_profile));

	pol_profile.bypass_yellow = 0;
	pol_profile.bypass_red = 0;
	pol_profile.pir_max_credit = pbs >> 7;	/* it's in a unit of 128 bytes */
	pol_profile.cir_max_credit = cbs >> 7;	/* it's in a unit of 128 bytes */

	/* if pir != 0, set up dual-rate.. if not, set up single-rate */
	if (pir != 0) {
		pol_profile.policer_type = CS_TM_POL_RFC_2698;	// FIXME right one?
		if (cs_tm_pol_convert_rate_to_hw_value(pir, TRUE, &pol_profile.range, 
					&pol_profile.pir_credit) != CS_OK)
			return;
		if (cs_tm_pol_rate_divisor_to_credit(cir, pol_profile.range, 
					&pol_profile.cir_credit) != CS_OK)
			return;
	} else {
		pol_profile.policer_type = CS_TM_POL_RFC_2697;
		if (cs_tm_pol_convert_rate_to_hw_value(cir, FALSE, &pol_profile.range, 
					&pol_profile.cir_credit) != CS_OK)
			return;
		if (pol_profile.pir_max_credit == 0)
			pol_profile.pir_max_credit = pol_profile.cir_max_credit + 1;
	}

	if (*p_pol_id != 0) {
		u32 count;
		if (cs_tm_pol_get_flow_policer_used_count(*p_pol_id, &count) != CS_OK)
			return;

		if (count <= 1) f_get_new = 0;
	}

	status = cs_tm_pol_find_flow_policer(&pol_profile, &pol_id);
	if (status == CS_OK) {
		/* We found a pre-existing flow profile that we can use for this 
		 * new filter policer. */
		if (*p_pol_id != 0) cs_tm_pol_del_flow_policer(*p_pol_id);

		*p_pol_id = pol_id;
		cs_tm_pol_inc_flow_policer_used_count(pol_id);
		return;
	}

	if (f_get_new == 1) {
		status = cs_tm_pol_get_avail_flow_policer(&pol_id);
		if (status != CS_OK) return;
		if (*p_pol_id != 0) cs_tm_pol_del_flow_policer(*p_pol_id);
		*p_pol_id = pol_id;
	}

	/* at this point, we should've found an available flow policer that we 
	 * can modify. It is either free or currently used by this filter policer. */
	status = cs_tm_pol_set_flow_policer(*p_pol_id, &pol_profile);
	if (status != CS_OK) goto fail_delete_policer;

	if (f_get_new == 1) cs_tm_pol_inc_flow_policer_used_count(*p_pol_id);

	return;

fail_delete_policer:
	cs_tm_pol_del_flow_policer(*p_pol_id);
	*p_pol_id = 0;
	return;
} /* k_jt_cs_qos_enbl_filter_policer */

void k_jt_cs_qos_dsbl_filter_policer(u8 pol_id)
{
	cs_qos_delete_by_pol_id(pol_id);
	cs_tm_pol_del_flow_policer(pol_id);
} /* k_jt_cs_qos_dsbl_filter_policre */
#endif /* CONFIG_NET_ACT_POLICE */

int cs_qos_init(void)
{
	cs_uint8 iii, jjj;

	hw_jt.cs_qos_set_skb_sw_only = k_jt_cs_qos_set_skb_sw_only;
#ifdef CONFIG_NET_ACT_POLICE
	hw_jt.cs_qos_set_skb_pol_id = k_jt_cs_qos_set_skb_pol_id;
	hw_jt.cs_qos_enbl_filter_policer = k_jt_cs_qos_enbl_filter_policer;
	hw_jt.cs_qos_dsbl_filter_policer = k_jt_cs_qos_dsbl_filter_policer;
#endif

	/* FIXME! more jump table */

	INIT_LIST_HEAD(&guid_list_head);

	/* FIXME!! do we need to initialize all or some of the HW VOQs here? */
	for (iii = 0; iii < CS_QOS_PORT_NUM; iii++) {
		for (jjj = 0; jjj < CS_QOS_VOQ_PER_PORT; jjj++) {
			voq_darray[iii][jjj].type = CS_QOS_VOQ_TYPE_DRR;
			voq_darray[iii][jjj].weight = CS_QOS_VOQ_DEFAULT_QUANTA;
			voq_darray[iii][jjj].priority = 0;
			voq_darray[iii][jjj].rate = 0;
		}
	}

	memset(pol_guid_map_array, 0, sizeof(cs_qos_pol_guid_t) * CS_QOS_FLOW_POL_NUM);

#if defined(CS_QOS_MULTIQ_SUPPORT) && defined(CONFIG_NET_SCH_MULTIQ)
	cs_qos_multiq_init();
#endif
#ifdef CONFIG_NET_SCH_INGRESS
	cs_qos_ingress_init();
#endif

	return 0;
} /* cs_qos_init */

void cs_qos_exit(void)
{
	hw_jt.cs_qos_set_skb_sw_only = (void *)cs_hw_accel_nop;
#ifdef CS_QOS_MULTIQ_SUPPORT
	cs_qos_multiq_exit();
#endif
} /* cs_qos_exit */
#endif /* CONFIG_NET_SCHED */

