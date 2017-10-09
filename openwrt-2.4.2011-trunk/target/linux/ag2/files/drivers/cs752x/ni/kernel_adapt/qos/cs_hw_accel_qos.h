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
 * cs_hw_accel_qos.h
 *
 * $Id: cs_hw_accel_qos.h,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This header file defines the data structures and APIs for CS QoS.
 */

#ifndef __CS_HW_ACCEL_QOS_H__
#define __CS_HW_ACCEL_QOS_H__
#ifdef CONFIG_NET_SCHED
#include <net/sch_generic.h>
#include <mach/cs_types.h>
#include <mach/hw_accel.h>
#include <net/pkt_sched.h>

#ifndef CS_QOS_MULTIQ_SUPPORT
#define CS_QOS_MULTIQ_SUPPORT	1
#endif

extern struct cs_kernel_hw_accel_jt hw_jt;

int cs_qos_init(void);
void cs_qos_exit(void);
void cs_qos_set_voq_id_to_skb_cs_cb(struct sk_buff *skb, 
		struct net_device *dev, cs_uint8 queue_id);
cs_status cs_qos_set_voq_param(cs_uint8 port_id, cs_uint8 voq_id, 
		 cs_uint8 priority, cs_uint32 weight, cs_uint32 rsrv_size, 
		 cs_uint32 max_size, cs_uint32 rate, struct tc_wredspec *p_wred);
cs_status cs_qos_get_voq_depth(cs_uint8 port_id, cs_uint8 voq_id, 
		cs_uint16 *p_min_depth, cs_uint16 *p_max_depth);
cs_status cs_qos_set_port_param(cs_uint8 port_id, cs_uint16 burst_size, 
		cs_uint32 rate);
cs_status cs_qos_reset_port(cs_uint8 port_id);
cs_status cs_qos_reset_voq(cs_uint8 port_id, cs_uint8 voq_id);
cs_status cs_qos_set_port_pol_cfg(cs_uint8 port_id, cs_uint8 enbl, 
		cs_uint8 bypass_yellow, cs_uint8 bypass_red, cs_uint32 rate_bps, 
		cs_uint32 cbs, cs_uint32 pbs);

void k_jt_cs_qos_set_skb_sw_only(struct sk_buff *skb);
#ifdef CONFIG_NET_ACT_POLICE
void k_jt_cs_qos_set_skb_pol_id(struct sk_buff *skb, u8 pol_id);
void k_jt_cs_qos_enbl_filter_policer(u8 *p_pol_id, u32 cir, u32 cbs, u32 pir, u32 pbs);
void k_jt_cs_qos_dsbl_filter_policer(u8 pol_id);
#endif

#ifdef CONFIG_NET_SCH_MULTIQ
#ifdef CS_QOS_MULTIQ_SUPPORT
int cs_qos_multiq_init(void);
void cs_qos_multiq_exit(void);
int k_jt_cs_qos_check_and_steal_multiq_skb(struct Qdisc *qdisc, struct sk_buff *skb);
void k_jt_cs_qos_set_multiq_attribute(struct Qdisc *qdisc, cs_uint16 burst_size, cs_uint32 rate_bps);
int k_jt_cs_qos_set_multisubq_attribute(struct Qdisc *qdisc, cs_uint8 band_id, cs_uint8 priority, cs_uint32 weight, cs_uint32 rsrv_size, cs_uint32 max_size, cs_uint32 rate_bps, void *p_wred);
int k_jt_cs_qos_get_multisubq_depth(struct Qdisc *qdisc, cs_uint8 band_id, cs_uint16 *p_min_depth, cs_uint16 *p_max_depth);
void k_jt_cs_qos_reset_multiq(struct Qdisc *qdisc);
void k_jt_cs_qos_reset_multisubq(struct Qdisc *qdisc, cs_uint8 band_id);
#endif /* CS_QOS_MULTIQ_SUPPORT */
#endif /* CONFIG_NET_SCH_MULTIQ */

#ifdef CONFIG_NET_SCH_INGRESS
int cs_qos_ingress_init(void);
void k_jt_cs_qos_set_pol_cfg_ingress_qdisc(struct Qdisc *qdisc, cs_uint8 enbl, cs_uint8 bypass_yellow, cs_uint8 bypass_red, cs_uint32 rate_bps, cs_uint32 cbs, cs_uint32 pbs);
#endif /* CONFIG_NET_SCH_INGRESS */

#endif /* CONFIG_NET_SCHED */
#endif /* __CS_HW_ACCEL_QOS_H__ */
