/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : cs_hw_accel_pppoe.h
   Date        : 2010-09-24
   Description : This header file defines the data structures and APIs for CS 
                 PPPoE Offload.
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef __CS_HW_ACCEL_PPPOE_H__
#define __CS_HW_ACCEL_PPPOE_H__

#include <mach/cs_types.h>
#include "cs_hw_accel_util.h"

// Jump Table Entires
void k_jt_cs_pppoe_skb_recv_hook(struct sk_buff *skb, cs_uint16 pppoe_session_id);
void k_jt_cs_pppoe_skb_xmit_hook(struct sk_buff *skb, cs_uint16 pppoe_session_id);

void cs_pppoe_init(void);

#endif /* __CS_HW_ACCEL_PPPOE_H__ */
