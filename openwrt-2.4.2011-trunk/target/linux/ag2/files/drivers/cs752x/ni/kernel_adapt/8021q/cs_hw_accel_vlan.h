/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : cs_hw_accel_vlan.h
   Date        : 2010-09-24
   Description : This header file defines the data structures and APIs for CS 
                 VLAN Offload.
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef __CS_HW_ACCEL_VLAN_H__
#define __CS_HW_ACCEL_VLAN_H__

#include <mach/cs_types.h>
#include "cs_hw_accel_util.h"


// Jump Table Entires
void k_jt_cs_vlan_skb_recv_hook(struct sk_buff *skb, struct net_device *dev, cs_uint16 vlan_tci); 
void k_jt_cs_vlan_dev_hard_start_xmit_hook(struct sk_buff *skb, cs_uint16 vlan_tci); 
void k_jt_cs_vlan_get_port_by_dev_hook(struct net_device *dev, cs_uint8 *p_port);
//void k_jt_cs_vlan_get_port_by_dev_hook(cs_uint8 *p_dev_name, cs_uint8 *p_port);
void k_jt_cs_vlan_add_vlan_table_hook(cs_uint8 port, cs_uint16 vlan_id);
void k_jt_cs_vlan_del_vlan_table_hook(cs_uint8 port, cs_uint16 vlan_id);

// Initialization
void cs_vlan_init(void);

#endif /* __CS_HW_ACCEL_VLAN_H__ */
