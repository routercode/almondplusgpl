/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : cs_hw_accel_pppoe.c
   Date        : 2010-09-24
   Description : Process Cortina GoldenGate PPPoE Offload.
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <linux/list.h>		/* list_head structure	*/
#include <linux/jiffies.h>
#include <linux/if_pppox.h>
#include <mach/hw_accel.h>
#include "cs_hw_accel_pppoe.h"

extern struct cs_kernel_hw_accel_jt hw_jt;

#define PPPOE_HASH_RESOURCE_TIMEOUT       180*HZ
unsigned long time_to_check_pppoe_hash_resource;

struct cs_pppoe_guid_map {
	cs_uint16           pppoe_session_id;
	cs_uint64           guid;
	struct list_head	list;
};

struct list_head pppoe_map_head;


void cs_pppoe_del_map_by_guid(cs_uint64 guid)
{
	struct list_head            *next;
	struct cs_pppoe_guid_map    *pMap;
    
	/* Traverse all elements of the list */
	list_for_each(next, &pppoe_map_head)
	{
		pMap = (struct cs_pppoe_guid_map*) list_entry(next, struct cs_pppoe_guid_map, list);
		if(guid == pMap->guid)
		{
            // delete list and free resource
            list_del(&(pMap->list));
            kfree(pMap);
            break;
		}
	}

    return;    
}// end cs_pppoe_del_map_by_guid()

void k_jt_cs_pppoe_skb_recv_hook(struct sk_buff *skb, cs_uint16 pppoe_session_id)
{
	cs_uint64 guid;
	CS_KERNEL_ACCEL_CB_T *cs_cb=CS_KERNEL_SKB_CB(skb);
	struct cs_pppoe_guid_map *pMap;


	if (!cs_cb) return;

	if (time_before(jiffies, time_to_check_pppoe_hash_resource)) return;

	if(cs_cb->common.tag != CS_CB_TAG) {
//		printk("k_jt_cs_pppoe_skb_recv_hook: !CS_CB_TAG\n");
		return ;
	}

//    printk("--> k_jt_cs_pppoe_skb_recv_hook: pppoe_session_id 0x%4.4x, protocol 0x%4.4x\n", pppoe_session_id, skb->protocol);
	if (pppoe_session_id != 0xFFFF) {
    	guid = cs_kernel_adapt_get_guid(CS_KERNEL_PPPOE_ID_TAG);
    
    	// add into map list
    	pMap = (struct cs_pppoe_guid_map*)kmalloc(sizeof(struct cs_pppoe_guid_map), 
    	        GFP_ATOMIC);
    	if (!pMap) return;
    
    	pMap->pppoe_session_id = pppoe_session_id;
    	pMap->guid = guid;
    	list_add(&(pMap->list), &pppoe_map_head);
	
    	cs_kernel_add_guid(guid, skb);
	}

	/* Keep original pppoe information in skb->cs_cb 
	 * 		CS_KERNEL_ACCEL_T.common.module_mask
	 * 		CS_KERNEL_ACCEL_T.common.guid
	 * 		CS_KERNEL_ACCEL_T.input_mask
	 * 		CS_KERNEL_ACCEL_T.input.raw.pppoe_frame */
	cs_cb->common.module_mask |= CS_MOD_MASK_PPPOE;
	cs_cb->input_mask |= (CS_HM_PPPOE_SESSION_ID_VLD_MASK | 
	    CS_HM_PPPOE_SESSION_ID_MASK);

	/* Default remove pppoe header */
	cs_cb->action.l2.pppoe_op_en = CS_PPPOE_OP_REMOVE;
	cs_cb->output_mask |= CS_HM_PPPOE_SESSION_ID_MASK;

	cs_cb->input.raw.pppoe_frame = pppoe_session_id;
	cs_cb->output.raw.pppoe_frame = pppoe_session_id;
	
    return;
} /* k_jt_cs_pppoe_skb_recv_hook */

void k_jt_cs_pppoe_skb_xmit_hook(struct sk_buff *skb, cs_uint16 pppoe_session_id)
{
	cs_uint64 guid;
	CS_KERNEL_ACCEL_CB_T *cs_cb=CS_KERNEL_SKB_CB(skb);
	struct cs_pppoe_guid_map *pMap;


	if (!cs_cb) return;

	if (time_before(jiffies, time_to_check_pppoe_hash_resource)) return;
	    
	if(cs_cb->common.tag != CS_CB_TAG) {
//		printk("k_jt_cs_pppoe_skb_xmit_hook: !CS_CB_TAG\n");
		return ;
	}

//	printk("<-- k_jt_cs_pppoe_skb_xmit_hook: pppoe_session_id 0x%4.4x, protocol 0x%4.4x\n", pppoe_session_id, skb->protocol);
    if (pppoe_session_id == 0xFFFF) {
    // means packet send to LAN
        if (cs_cb->input.raw.pppoe_frame == 0xFFFF) {
        // LAN to LAN
            cs_cb->common.module_mask &= ~CS_MOD_MASK_PPPOE;
            cs_cb->output_mask |= CS_HM_PPPOE_SESSION_ID_MASK;
            cs_cb->action.l2.pppoe_op_en = CS_PPPOE_OP_REMOVE;
            return ;
        }
    } else {
    // means packet send to WAN
        if (cs_cb->input.raw.pppoe_frame == 0xFFFF) {
		    cs_cb->action.l2.pppoe_op_en = CS_PPPOE_OP_INSERT;
	    	cs_cb->input_mask &= ~(CS_HM_PPPOE_SESSION_ID_VLD_MASK | CS_HM_PPPOE_SESSION_ID_MASK);
//            printk("### k_jt_cs_pppoe_skb_xmit_hook: CS_PPPOE_OP_INSERT, cs_cb->input_mask 0x%llx\n", cs_cb->input_mask);
    	}

        cs_cb->common.module_mask |= CS_MOD_MASK_PPPOE;
    	cs_cb->output_mask |= CS_HM_PPPOE_SESSION_ID_MASK;

    	/* Keep original pppoe information in skb->cs_cb 
    	 * 		CS_KERNEL_ACCEL_T.common.module_mask
    	 * 		CS_KERNEL_ACCEL_T.common.guid
    	 * 		CS_KERNEL_ACCEL_T.output_mask
    	 * 		CS_KERNEL_ACCEL_T.input.raw.pppoe_frame */
    	guid = cs_kernel_adapt_get_guid(CS_KERNEL_PPPOE_ID_TAG);
    
    	// add into map list
    	pMap = (struct cs_pppoe_guid_map*)kmalloc(sizeof(struct cs_pppoe_guid_map), 
    	        GFP_ATOMIC);
    	if (!pMap) return;
    
    	pMap->pppoe_session_id = pppoe_session_id;
    	pMap->guid = guid;
    	list_add(&(pMap->list), &pppoe_map_head);
    	
	    cs_kernel_add_guid(guid, skb);
	    
	    cs_cb->output.raw.pppoe_frame = pppoe_session_id;
    }

    return;
} /* k_jt_cs_pppoe_skb_xmit_hook */

void cs_kernel_pppoe_cb(cs_uint64 guid, int status)
{
	switch(status)
    {
        case CS_HASH_CB_FAILED_SW_REQ:
			break;

        case CS_HASH_CB_FAILED_RESOURCE:
            time_to_check_pppoe_hash_resource = jiffies + PPPOE_HASH_RESOURCE_TIMEOUT;
			break;

        case CS_HASH_CB_DELETE_BY_PEER:
            cs_pppoe_del_map_by_guid(guid);
			break;

		case CS_HASH_CB_SUCCESSFUL:
		default:
			break;
    }
    
    return;
}//end cs_kernel_pppoe_cb()

void cs_pppoe_init(void)
{
	time_to_check_pppoe_hash_resource = jiffies;
	
	/* init pppoe session id - guid mapping list */
	INIT_LIST_HEAD(&pppoe_map_head);

	// register call back
	cs_kernel_reg_hash_cb(CS_KERNEL_PPPOE_ID_TAG, cs_kernel_pppoe_cb);

	hw_jt.cs_pppoe_skb_recv_hook = &k_jt_cs_pppoe_skb_recv_hook;
	hw_jt.cs_pppoe_skb_xmit_hook = &k_jt_cs_pppoe_skb_xmit_hook;

	return;
}//end cs_pppoe_init()



