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
 * cs_hw_accel_qos_ingress.c
 *
 * $Id: cs_hw_accel_qos_ingress.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This file contains the implementation for CS QoS implementation 
 * for ingress traffic.
 */
#ifdef CONFIG_NET_SCH_INGRESS
#include "cs_hw_accel_qos.h"

extern cs_uint8 cs_ni_get_port_id(struct net_device *dev);

void k_jt_cs_qos_set_pol_cfg_ingress_qdisc(struct Qdisc *qdisc, cs_uint8 enbl, 
		cs_uint8 bypass_yellow, cs_uint8 bypass_red, cs_uint32 rate_bps, 
		cs_uint32 cbs, cs_uint32 pbs)
{
	struct net_device *dev;
	cs_uint8 port_id;

	dev = qdisc_dev(qdisc);
	if (NULL == dev) return;

	port_id = cs_ni_get_port_id(dev);

	cs_qos_set_port_pol_cfg(port_id, enbl, bypass_yellow, bypass_red, 
			rate_bps, cbs, pbs);

	return;
} /* k_jt_cs_qos_set_pol_cfg_ingress_qdisc */

int cs_qos_ingress_init(void)
{
	hw_jt.cs_qos_set_pol_cfg_ingress_qdisc = 
		k_jt_cs_qos_set_pol_cfg_ingress_qdisc;

	return 0;
} /* cs_qos_ingress_init */

#endif /* CONFIG_NET_SCH_INGRESS */
