/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : cs_hw_accel_vlan.c
   Date        : 2010-09-24
   Description : Process Cortina GoldenGate VLAN Offload.
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <linux/list.h>		/* list_head structure	*/
#include <linux/jiffies.h>
#include <linux/if_vlan.h>
#include <mach/hw_accel.h>
#include "cs_hw_accel_vlan.h"
#include <cs_hw_accel_cb.h>
#include <cs_hw_accel_core.h>
#include <cs_fe.h>

extern struct cs_kernel_hw_accel_jt hw_jt;

// define for CS_HASH_CB_FAILED_RESOURCE
#define HASH_RESOURCE_TIMEOUT       180*HZ
unsigned long time_to_check_vlan_hash_resource;

struct cs_vid_guid_map {
	cs_uint16           vid;
	cs_uint64           guid;
	struct list_head	list;
};

struct list_head map_head;

char *vdev_name[] = {"GE0", "GE1", "GE2", "CPU", "Crypto", "Encap", 0};

void cs_vlan_del_map_by_guid(cs_uint64 guid)
{
	struct list_head	    *next;
	struct cs_vid_guid_map  *pMap;
    
	/* Traverse all elements of the list */
	list_for_each(next, &map_head)
	{
		pMap = (struct cs_vid_guid_map*) list_entry(next, struct cs_vid_guid_map, list);
		if(guid == pMap->guid)
		{
            // delete list and free resource
            list_del(&(pMap->list));
            kfree(pMap);
            break;
		}
	}

    return;    
}// end cs_vlan_del_map_by_guid()

void cs_vlan_delete_map_all(cs_uint16 vlan_id)
{
	struct list_head	    *next;
	struct cs_vid_guid_map  *pMap;
	cs_uint64               guid;
    
	/* Traverse all elements of the list */
	list_for_each(next, &map_head)
	{
		pMap = (struct cs_vid_guid_map*) list_entry(next, struct cs_vid_guid_map, list);
		if(vlan_id == pMap->vid)
		{
		    guid = pMap->guid;
		    
            // delete list and free resource
            list_del(&(pMap->list));
            kfree(pMap);

	        // delete kernel core hardware hash
	        cs_kernel_core_del_hash_by_guid(guid);
		}
	}

    return;
}//end cs_vlan_delete_map_all()


void k_jt_cs_vlan_skb_recv_hook(struct sk_buff *skb, struct net_device *dev,
	cs_uint16 vlan_tci)
{
	cs_uint16               vid = vlan_tci & VLAN_VID_MASK;
	cs_uint64               guid;
	CS_KERNEL_ACCEL_CB_T    *cs_cb=CS_KERNEL_SKB_CB(skb);
	struct cs_vid_guid_map  *pMap;
	struct vlan_ethhdr      *veth;

	if (!cs_cb) return;

	if (time_before(jiffies, time_to_check_vlan_hash_resource)) return;

	guid = cs_kernel_adapt_get_guid(CS_KERNEL_VLAN_ID_TAG);

	// add into map list
	pMap = (struct cs_vid_guid_map*)kmalloc(sizeof(struct cs_vid_guid_map), 
			GFP_ATOMIC);
	if (!pMap) return;

	pMap->vid = vid;
	pMap->guid = guid;
	list_add(&(pMap->list), &map_head);

	veth = (struct vlan_ethhdr *)skb->data;
	/* Keep original vlan information in skb->cs_cb 
	 * 		CS_KERNEL_ACCEL_T.common.module_mask
	 * 		CS_KERNEL_ACCEL_T.common.guid
	 * 		CS_KERNEL_ACCEL_T.input_mask
	 * 		CS_KERNEL_ACCEL_T.input.raw.vlan_protocol
	 * 		CS_KERNEL_ACCEL_T.input.raw.vlan_id */
	cs_kernel_add_guid(guid, skb);

	cs_cb->common.module_mask |= CS_MOD_MASK_VLAN;
	cs_cb->input_mask |= (CS_HM_8021P_1_MASK | CS_HM_DEI_1_MASK | CS_HM_VID_1_MASK);
	/* will set CS_HM_DEI_1_MASK | CS_HM_VID_1_MASK once we know they are 
	 * changed at the TX. */
	/*
	 * input_mask is used for hash matching for ingress packet, no matter vlan
	 * tag will be changed or not. 8021p and dei could be turned off if qos is
	 * disabled, at core logic.
	 */
	cs_cb->input.raw.protocol = veth->h_vlan_encapsulated_proto;
	cs_cb->input.raw.vlan_id = vlan_tci;

	/* Give default action CS_VLAN_OP_REMOVE */
	cs_cb->action.l2.flow_vlan_op_en = CS_VLAN_OP_REMOVE;
	cs_cb->action.l2.flow_vlan_prio_op_en = CS_VLAN_PRIO_OP_NO_ENABLE;

#if 1	// FIXME debug message
	printk("%s::vlan %d, vlan tci %x\n", __func__, vid, vlan_tci);

	if (dev) {
		cs_uint8 pspid, lspid;
		cs_status status;
		status = cs_fe_an_bng_mac_get_port_by_mac(dev->perm_addr, &pspid);
		if (status != CS_OK) {
			printk("%s::not able to find pspid based on dev %p\n", __func__, dev);
			return;
		}
		status = cs_fe_lpb_get_lspid_by_pspid(pspid, &lspid);
		if (status != CS_OK) {
			printk("%s::not able to find lspid based on pspid %x, lspid %x\n", 
					__func__, pspid, lspid);
			return;
		}
		printk("%s::dev %p, pspid %x, lspid %x\n", __func__, dev, pspid, lspid);
	}
#endif

	/* if not specified, how to enable HW forwarding for eth1->eth2.3? */
	/* VLAN module does not know nor make decision of whether or not this flow 
	 * can be accelerated by HW. */
	//cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;		
    return;
} /* k_jt_cs_vlan_skb_recv_hook */


#define VLAN_PRIO_MASK		0xe000
#define VLAN_DEI_MASK		0x1000
static inline void cs_vlan_set_cs_cb_for_vlan_prio_dei(
		CS_KERNEL_ACCEL_CB_T *cs_cb)
{
	if ((cs_cb->input.raw.vlan_id & VLAN_PRIO_MASK) != 
			(cs_cb->output.raw.vlan_id & VLAN_PRIO_MASK)) {
		cs_cb->action.l2.flow_vlan_prio_op_en = CS_VLAN_PRIO_OP_UPDATE_PRIO;
		if (cs_cb->action.l2.flow_vlan_op_en != CS_VLAN_OP_INSERT)
			cs_cb->input_mask |= (CS_HM_8021P_1_MASK);
		cs_cb->output_mask |= (CS_HM_8021P_1_MASK);
	}

	if ((cs_cb->input.raw.vlan_id & VLAN_DEI_MASK) != 
			(cs_cb->output.raw.vlan_id & VLAN_DEI_MASK)) {
		if (cs_cb->action.l2.flow_vlan_prio_op_en == 
				CS_VLAN_PRIO_OP_UPDATE_PRIO)
			cs_cb->action.l2.flow_vlan_prio_op_en = 
				CS_VLAN_PRIO_OP_UPDATE_PRIO_DEI;
		else
			cs_cb->action.l2.flow_vlan_prio_op_en = 
				CS_VLAN_PRIO_OP_UPDATE_DEI;
		if (cs_cb->action.l2.flow_vlan_op_en != CS_VLAN_OP_INSERT)
			cs_cb->input_mask |= (CS_HM_DEI_1_MASK);
		cs_cb->output_mask |= (CS_HM_DEI_1_MASK);
	}
} /* cs_vlan_set_cs_cb_for_vlan_prio_dei */


void k_jt_cs_vlan_dev_hard_start_xmit_hook(struct sk_buff *skb, cs_uint16 vlan_tci)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb=CS_KERNEL_SKB_CB(skb);

	if (!cs_cb) return;

	if (time_before(jiffies, time_to_check_vlan_hash_resource)) return;

	/* No special tag means this packet comes from CPU */
	if (cs_cb->common.tag == CS_CB_TAG) {
		struct vlan_ethhdr *veth;
		veth = (struct vlan_ethhdr *)skb->data;

		/* Fill all information */
		cs_cb->output.raw.protocol = veth->h_vlan_encapsulated_proto;
		cs_cb->output.raw.vlan_id = vlan_tci;
		cs_cb->output_mask |= (CS_HM_VID_1_MASK | CS_HM_DEI_1_MASK | CS_HM_VID_1_MASK);

		/* Action */
		if (cs_cb->common.module_mask & CS_MOD_MASK_VLAN) {
			/* (common.module_mask & CS_MOD_MASK_VLAN) means this packet comes 
			 * from VLAN port, so check original vlan_id */
			cs_cb->action.l2.flow_vlan_op_en = CS_VLAN_OP_KEEP;
			cs_cb->action.l2.flow_vlan_prio_op_en = CS_VLAN_PRIO_OP_NO_ENABLE;
			if (cs_cb->input.raw.vlan_id != cs_cb->output.raw.vlan_id) {
				if ((cs_cb->input.raw.vlan_id & VLAN_VID_MASK) != 
						(cs_cb->output.raw.vlan_id & VLAN_VID_MASK))
					cs_cb->action.l2.flow_vlan_op_en = CS_VLAN_OP_REPLACE;
				/* set up cs_cb based on the prio and dei/cfi */
				cs_vlan_set_cs_cb_for_vlan_prio_dei(cs_cb);
			}
		} else {
			/* common.module_mask have no CS_MOD_MASK_VLAN means this packet 
			 * comes from non-VLAN port */
			cs_cb->common.module_mask |= (CS_MOD_MASK_VLAN);
			cs_cb->action.l2.flow_vlan_op_en = CS_VLAN_OP_INSERT;
			if (cs_cb->output.raw.vlan_id > VLAN_VID_MASK) {
				/* set up cs_cb based on the prio and dei/cfi */
				cs_vlan_set_cs_cb_for_vlan_prio_dei(cs_cb);
			}
		}
	} /* end if (cs_cb->common.tag == CS_CB_TAG) */

	/* FIXME, debug message.. remove when code is verified */
	printk("\t%s::flow vlan op %d, input_mask %llx, output_mask %llx\n",
		__func__, cs_cb->action.l2.flow_vlan_op_en, cs_cb->input_mask, cs_cb->output_mask);

	return;
} //end k_jt_cs_vlan_dev_hard_start_xmit_hook()

/*
 * dev has it's MAC address, and we will check HW LPB table to get PSPID first
 * then lookup LPB table to get corresponding lspid as return value.
 */
void k_jt_cs_vlan_get_port_by_dev_hook(struct net_device *dev, cs_uint8 *port)
{
	cs_uint8 pspid;
	cs_status status;

	status = cs_fe_an_bng_mac_get_port_by_mac(dev->perm_addr, &pspid);
	if (status != FE_STATUS_OK) {
		*port = 0xff;	// right value?
		return;
	}
	status = cs_fe_lpb_get_lspid_by_pspid(pspid, port);
}

#if 0
void k_jt_cs_vlan_get_port_by_dev_hook(cs_uint8 *p_dev_name, cs_uint8 *p_port)
{
    int                 i;
    cs_uint8            portBit=1;

	printk("\t%s::vlan dev name %s\n", __func__, p_dev_name);
    for(i=0; vdev_name[i]!=NULL; i++)
    {
        if(strcmp(p_dev_name, vdev_name[i]) == 0)
        {
            *p_port = portBit;
			printk("%s::dev_name %s, port %d\n",
				__func__, vdev_name[i], portBit);
            break;
        }//end if()
        portBit <<= 1;
    }//end for()

    return;
}//end k_jt_cs_vlan_get_port_by_dev_hook();
#endif

void k_jt_cs_vlan_add_vlan_table_hook(cs_uint8 port, cs_uint16 vlan_id)
{
    fe_vlan_entry_t vlan_table;
	unsigned int vlan_idx;
	int status;
	bool f_inc_refcnt = false;

	/* According to Venkat, VLAN member table index is actual VID */
	status = cs_fe_table_get_entry(FE_TABLE_VLAN, vlan_id, &vlan_table);
	if (status == FE_TABLE_EENTRYNOTRSVD) {
		status = cs_fe_table_alloc_entry(FE_TABLE_VLAN, &vlan_idx, vlan_id);
		if (status != FE_TABLE_OK) {
			printk("%s:unable to alloc entry from VLAN!\n", __func__);
			return;
		}
		if (vlan_idx != vlan_id) {
			printk("%s:vlan idx do not match\n", __func__);
		}
		f_inc_refcnt = true;
		memset(&vlan_table, 0, sizeof(vlan_table));
		vlan_table.vlan_egress_untagged = 0;
		vlan_table.vlan_fid = 0;
		vlan_table.vlan_first_vlan_cmd = 0;
		vlan_table.vlan_first_vid = vlan_id;
		vlan_table.vlan_first_tpid_enc = 0;
		vlan_table.vlan_second_vlan_cmd = 0;	//??
		vlan_table.vlan_second_vid = 0;
		vlan_table.vlan_second_tpid_enc = 0;
		vlan_table.vlan_mcgid = 0;
	}
	vlan_table.vlan_member |= (1 << port);

    status = cs_fe_table_set_entry(FE_TABLE_VLAN, vlan_id, &vlan_table);
	if (status != FE_STATUS_OK) {
		printk("%s:unable to add a VLAN entry %x\n", __func__, status);
	}

	if (f_inc_refcnt == true) {
		status = cs_fe_table_inc_entry_refcnt(FE_TABLE_VLAN, vlan_id);
		if (status != FE_STATUS_OK) {
			printk("%s:unable to inc VLAN entry refcnt %x\n", __func__, status);
		}
	}
    
    return;
}//end k_jt_cs_vlan_add_vlan_table_hook()

void k_jt_cs_vlan_del_vlan_table_hook(cs_uint8 port, cs_uint16 vlan_id)
{
	fe_vlan_entry_t     vlan_table;
	int status;

	// delete all map before delete dev
	cs_vlan_delete_map_all(vlan_id);

	status = cs_fe_table_get_entry(FE_TABLE_VLAN, vlan_id, &vlan_table);
	if (status != FE_STATUS_OK) {
		printk("%s:unable to get a VLAN entry %x\n", __func__, status);
		return;
	}

	vlan_table.vlan_member &= ~(1 << port);

	if (vlan_table.vlan_member == 0) {
		status = cs_fe_table_del_entry_by_idx(FE_TABLE_VLAN, vlan_id, false);
		if (status != FE_STATUS_OK)
			printk("%s:delete vlan entry failed %x\n", __func__, status);
	} else {
		status = cs_fe_table_set_entry(FE_TABLE_VLAN, vlan_id, &vlan_table);
		if (status != FE_STATUS_OK)
			printk("%s:set vlan entry failed %x\n", __func__, status);
	}
    return;
}//end k_jt_cs_vlan_add_vlan_table_hook()

void cs_kernel_vlan_cb(cs_uint64 guid, int status)
{
	switch(status)
    {
        case CS_HASH_CB_FAILED_SW_REQ:
			break;

        case CS_HASH_CB_FAILED_RESOURCE:
            time_to_check_vlan_hash_resource = jiffies + HASH_RESOURCE_TIMEOUT;
			break;

        case CS_HASH_CB_DELETE_BY_PEER:
            cs_vlan_del_map_by_guid(guid);
			break;

		case CS_HASH_CB_SUCCESSFUL:
		default:
			break;
    }
    
    return;
}//end cs_kernel_vlan_cb()


void cs_vlan_init(void)
{
	time_to_check_vlan_hash_resource = jiffies;
	
	/* init vid guid mapping list */
	INIT_LIST_HEAD(&map_head);

	/* initialize forward result vlan table */
	//cs_fe_fvlan_init();

	/* initialize VLAN table */
	//cs_fe_vlan_init();

	// register call back
	cs_kernel_reg_hash_cb(CS_KERNEL_VLAN_ID_TAG, cs_kernel_vlan_cb);

	hw_jt.cs_vlan_skb_recv_hook = &k_jt_cs_vlan_skb_recv_hook;
	hw_jt.cs_vlan_dev_hard_start_xmit_hook = &k_jt_cs_vlan_dev_hard_start_xmit_hook;
	hw_jt.cs_vlan_get_port_by_dev_hook = &k_jt_cs_vlan_get_port_by_dev_hook;
	hw_jt.cs_vlan_add_vlan_table_hook = &k_jt_cs_vlan_add_vlan_table_hook;
	hw_jt.cs_vlan_del_vlan_table_hook = &k_jt_cs_vlan_del_vlan_table_hook;

	return;
}//end cs_vlan_init()

