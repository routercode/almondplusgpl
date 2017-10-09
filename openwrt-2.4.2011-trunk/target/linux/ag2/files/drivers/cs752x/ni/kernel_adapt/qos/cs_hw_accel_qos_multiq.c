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
 * cs_hw_accel_qos_multiq.c
 *
 * $Id: cs_hw_accel_qos_multiq.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This file contains the implementation for CS QoS implementation 
 * based on MultiQ.
 */
#ifdef CONFIG_NET_SCH_MULTIQ
#include "cs_hw_accel_qos.h"
#include "cs_hw_accel_qos_data.h"

extern cs_uint8 cs_ni_get_port_id(struct net_device *dev);

static cs_status cs_qos_handle_dev_cpu_collision(struct sk_buff *skb, 
		struct netdev_queue *dev_queue)
{
	if (unlikely(dev_queue->xmit_lock_owner == smp_processor_id())) {
		/* same cpu holding the lock. It may be a transient configuration 
		 * error, when hard_start_xmit() recurses. We detect it by checking 
		 * xmit owner and drop the packet when deadloop is detected.  
		 * Return CS_OK to try the next skb */
		kfree_skb(skb);
		if (net_ratelimit())
			printk(KERN_WARNING "Dead loop on netdeivce %s, "
					"fix it urgently!\n", dev_queue->dev->name);
		return CS_OK;
	} else return CS_ERROR;
} /* cs_qos_handle_dev_cpu_collision */

/* check if the qdisc is a multiq. (well.. most likely is) if HW 
 * acceleration is turned on.. check if this SKB is marked sw_only. 
 * If skb is qos_sw_only.. don't do anything. if skb can be hw accelerated, 
 * then it will be placed into its corresponding HW queue for transmitting */
int k_jt_cs_qos_check_and_steal_multiq_skb(struct Qdisc *qdisc, 
		struct sk_buff *skb)
{
	struct netdev_queue *txq;
	struct  net_device *dev;
	int ret = NETDEV_TX_BUSY;

	/* if this queue is not supported by HW, just return anything but NOT 
	 * NET_XMIT_SUCCESS. */
	if (!(CS_QOS_HWQ_MAP & qdisc->cs_handle)) return NET_XMIT_MASK;

	__qdisc_update_bstats(qdisc, qdisc_pkt_len(skb));

	dev = qdisc_dev(qdisc);
	txq = netdev_get_tx_queue(dev, skb_get_queue_mapping(skb));

	cs_qos_set_voq_id_to_skb_cs_cb(skb, dev, skb_get_queue_mapping(skb));

	HARD_TX_LOCK(dev, txq, smp_processor_id());
	if (!netif_tx_queue_stopped(txq) && 
			!netif_tx_queue_frozen(txq))
		ret = dev_hard_start_xmit(skb, dev, txq);
	HARD_TX_UNLOCK(dev, txq);

	switch (ret) {
		case NETDEV_TX_OK:
			return NET_XMIT_SUCCESS;
		case NETDEV_TX_LOCKED:
			/* driver try lock failed */
			if (CS_OK == cs_qos_handle_dev_cpu_collision(skb, txq))
				return NET_XMIT_SUCCESS;
			else return NET_XMIT_MASK;
		default:
			/* driver returned NETDEV_TX_BUSY.. we will just let 
			 * the rest of SW QoS to re-queue the skb */
			return NET_XMIT_MASK;
	};

	return NET_XMIT_MASK;
}	/* k_jt_cs_qos_check_and_steal_multiq_skb */

/* burst size is in the range of 16 to 256 */
void k_jt_cs_qos_set_multiq_attribute(struct Qdisc *qdisc, 
		cs_uint16 burst_size, cs_uint32 rate_bps)
{
	struct net_device *dev;
	cs_uint8 port_id;
	cs_status status;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return;

	if ((burst_size != 0) && ((burst_size < 16) || (burst_size > 256)))
		return;

	port_id = cs_ni_get_port_id(dev);

	status = cs_qos_set_port_param(port_id, burst_size, rate_bps);
	if (CS_OK != status) return;

	/* FIXME! do we want to introduce error code? */
} /* k_jt_cs_qos_set_multiq_attribute */

int k_jt_cs_qos_set_multisubq_attribute(struct Qdisc *qdisc, cs_uint8 band_id, 
		cs_uint8 priority, cs_uint32 weight, cs_uint32 rsrv_size, 
		cs_uint32 max_size, cs_uint32 rate_bps, void *p_wred)
{
	struct net_device *dev;
	cs_uint8 port_id;
	cs_status status;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return -EINVAL;

	port_id = cs_ni_get_port_id(dev);

	status = cs_qos_set_voq_param(port_id, 
			CS_QOS_VOQ_ID_FROM_PORT_QUEUE_ID(port_id, band_id), priority, 
			weight, rsrv_size, max_size, rate_bps, (struct tc_wredspec *)p_wred);
	if (CS_OK != status) return -EINVAL;

	return 0;
} /* k_jt_cs_qos_set_multisubq_attribute */

int k_jt_cs_qos_get_multisubq_depth(struct Qdisc *qdisc, cs_uint8 band_id, 
		cs_uint16 *p_min_depth, cs_uint16 *p_max_depth)
{
	struct net_device *dev;
	cs_uint8 port_id;
	cs_status status;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return -EINVAL;

	port_id = cs_ni_get_port_id(dev);

	status = cs_qos_get_voq_depth(port_id, 
			CS_QOS_VOQ_ID_FROM_PORT_QUEUE_ID(port_id, band_id), p_min_depth, 
			p_max_depth);
	if (status != CS_OK) return -EINVAL;

	return 0;
} /* k_jt_cs_qos_get_multisubq_depth */

void k_jt_cs_qos_reset_multiq(struct Qdisc *qdisc)
{
	struct net_device *dev;
	cs_uint8 port_id;
	cs_status status;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return;

	port_id = cs_ni_get_port_id(dev);

	status = cs_qos_reset_port(port_id);
	if (CS_OK != status) return;

	/* FIXME! do we want to introduce error code? */
} /* k_jt_cs_qos_reset_multiq */

void k_jt_cs_qos_reset_multisubq(struct Qdisc *qdisc, cs_uint8 band_id)
{
	struct net_device *dev;
	cs_uint8 port_id;
	cs_status status;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return;

	port_id = cs_ni_get_port_id(dev);

	status = cs_qos_reset_voq(port_id, 
			CS_QOS_VOQ_ID_FROM_PORT_QUEUE_ID(port_id, band_id));
	if (CS_OK != status) return;

	/* FIXME! do we want to introduce error code? */
} /* k_jt_cs_qos_reset_multisubq */

int cs_qos_multiq_init(void)
{
	hw_jt.cs_qos_check_and_steal_multiq_skb = 
		k_jt_cs_qos_check_and_steal_multiq_skb;
	hw_jt.cs_qos_set_multiq_attribute = k_jt_cs_qos_set_multiq_attribute;
	hw_jt.cs_qos_set_multisubq_attribute = k_jt_cs_qos_set_multisubq_attribute;
	hw_jt.cs_qos_get_multisubq_depth = k_jt_cs_qos_get_multisubq_depth;

	return 0;
} /* cs_qos_multiq_init */

void cs_qos_multiq_exit(void)
{
	hw_jt.cs_qos_check_and_steal_multiq_skb = (void *)cs_hw_accel_nop;

	return;
} /* cs_qos_multiq_exit */
#endif /* CONFIG_NET_SCH_MULTIQ */
