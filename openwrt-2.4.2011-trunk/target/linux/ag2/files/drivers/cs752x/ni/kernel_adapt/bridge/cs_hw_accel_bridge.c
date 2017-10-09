#include <linux/list.h>		/* list_head structure	*/
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/jiffies.h>

#include <mach/cs_types.h>
#include <mach/hw_accel.h>
#include "cs_hw_accel_core.h"
#include "cs_hw_accel_cb.h"
#include "cs_hw_accel_util.h"
#include "cs_hw_accel_bridge.h"

// Defined for CS_HASH_CB_FAILED_RESOURCE
#define BRIDGE_HASH_RESOURCE_TIMEOUT       180*HZ
unsigned long time_to_check_bridge_hash_resource;

extern struct cs_kernel_hw_accel_jt hw_jt;

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"
extern u32 cs_adapt_debug;
#endif /* CONFIG_CS752X_PROC */

#ifdef CONFIG_CS752X_PROC
#define DBG(x)  if(cs_adapt_debug & CS752X_ADAPT_BRIDGE) x
#else
#define DBG(x)	{}
#endif

struct cs_bridge_guid_map {
	char		name[IFNAMSIZ];
	cs_uint64	guid;
	struct list_head	list;
};
struct list_head	bridge_map_head;

void cs_bridge_del_map_by_guid(cs_uint64 guid)
{
#if 0
	struct list_head        *next;
	struct cs_bridge_guid_map	*pMap;
   
	/* Traverse all elements of the list */
	list_for_each(next, &bridge_map_head)
	{
		pMap = (struct cs_bridge_guid_map*)
			list_entry(next, struct cs_bridge_guid_map, list);
		if(guid == pMap->guid) {
			// delete list and free resource
			list_del(&(pMap->list));
			kfree(pMap);
			break;
		}
	}
#endif
	return;    
}// end cs_bridge_del_map_by_guid()

void k_jt_cs_bridge_hook_before_fwd(struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct ethhdr *eth = eth_hdr(skb);

	if (!cs_cb)
		return;

	if(time_before(jiffies, time_to_check_bridge_hash_resource))
        	return;

	cs_cb->common.module_mask |= CS_MOD_MASK_BRIDGE;
//	cs_cb->common.vtype = CS_VTBL_ID_L2_FLOW;
	cs_cb->common.input_dev = skb->dev;	// input device

#ifdef NEW_L2_DESIGN
	cs_cb->input_mask |= (CS_HM_MAC_SA_MASK);
#else
	cs_cb->input_mask |= (CS_HM_MAC_DA_MASK | CS_HM_MAC_SA_MASK);
	//cs_cb->input_mask |= (CS_HM_MAC_DA_MASK);
#endif

	memcpy(cs_cb->input.raw.sa, eth->h_source, ETH_ALEN);
	memcpy(cs_cb->input.raw.da, eth->h_dest, ETH_ALEN);
	cs_cb->input.raw.eth_protocol = eth->h_proto;
#if 0
	printk("%s::da %x %x %x %x %x %x, proto %x\n",
		__func__,
		cs_cb->input.raw.da[0], cs_cb->input.raw.da[1],
		cs_cb->input.raw.da[2], cs_cb->input.raw.da[3],
		cs_cb->input.raw.da[4],	cs_cb->input.raw.da[5],
		cs_cb->input.raw.eth_protocol);

	printk("%s::sa %x %x %x %x %x %x\n",
		__func__,
		cs_cb->input.raw.sa[0], cs_cb->input.raw.sa[1],
		cs_cb->input.raw.sa[2], cs_cb->input.raw.sa[3],
		cs_cb->input.raw.sa[4], cs_cb->input.raw.sa[5]);
#endif
	DBG(printk("%s::cs_cb->common.module_mask = 0x%X,cs_cb->input_mask=0x%lX\n",__func__, 
		cs_cb->common.module_mask, cs_cb->input_mask));
	/* any more information needed? */
	return;
}//end k_jt_cs_bridge_hook_before_fwd()

void k_jt_cs_bridge_hook_after_fwd(struct sk_buff *skb, cs_uint32 output_dev,
	char *p_dev_name)
{
	CS_KERNEL_ACCEL_CB_T        *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct ethhdr               *eth = eth_hdr(skb);
	cs_uint64                   guid;
	struct cs_bridge_guid_map   *pMap;

	if (!cs_cb)
		return;

	if(time_before(jiffies, time_to_check_bridge_hash_resource))
		return;
	if(cs_cb->common.tag == CS_CB_TAG) {
#if 0
		guid = cs_kernel_adapt_get_guid(CS_KERNEL_BRIDGE_ID_TAG);

		// add into map list
		pMap = (struct cs_bridge_guid_map*)kmalloc(sizeof(
			struct cs_bridge_guid_map), GFP_ATOMIC);

		if(!pMap)
			return;

		strcpy(pMap->name, p_dev_name);
		pMap->guid = guid;
		list_add(&(pMap->list), &bridge_map_head);

		cs_kernel_add_guid(guid, skb);
#endif
		/* output device */
		cs_cb->common.output_dev = output_dev;
		memcpy(cs_cb->output.raw.da, eth->h_dest, ETH_ALEN);
		/* add MAC SA for SA learning, needed by garbage colleciton */
		memcpy(cs_cb->output.raw.sa, eth->h_source, ETH_ALEN);
		cs_cb->action.l2.mac_da_replace_en = TRUE;


#ifdef NEW_L2_DESIGN
		if(cs_cb->output.raw.l2_flood != 1){		/* SW know DA */
			cs_cb->output_mask |= (CS_HM_MAC_DA_MASK);
			cs_cb->input_mask |= (CS_HM_MAC_DA_MASK);
		}
		else{	/* Unknown DA,  */
			cs_cb->output_mask &= ~(CS_HM_MAC_SA_MASK|CS_HM_MAC_DA_MASK);
		}
#else
		cs_cb->output_mask |= (CS_HM_MAC_DA_MASK | CS_HM_MAC_SA_MASK);
		//cs_cb->output_mask |= (CS_HM_MAC_DA_MASK);
#endif		
#if 0
		printk("%s: cs_cb->input_mask = 0x%llX, cs_cb->output_mask=0x%llX\n",__func__, 
			cs_cb->input_mask, cs_cb->output_mask);
		printk("%s::output da %x %x %x %x %x %x, sa %x %x %x %x %x %x\n",
			__func__, cs_cb->output.raw.da[0], cs_cb->output.raw.da[1],
			cs_cb->output.raw.da[2], cs_cb->output.raw.da[3],
			cs_cb->output.raw.da[4], cs_cb->output.raw.da[5],
			cs_cb->output.raw.sa[0], cs_cb->output.raw.sa[1],
			cs_cb->output.raw.sa[2], cs_cb->output.raw.sa[3],
			cs_cb->output.raw.sa[4], cs_cb->output.raw.sa[5]
			);
#endif
		cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;

	    	/* any more informaiton needed? */
    	
	    	cs_cb->tmo.interval = 300; /* 300 seconds */

#if 0 		/*  Test for PPPoE header manipulation */
	printk("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PPPOE Verify ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		cs_cb->output_mask |= CS_HM_PPPOE_SESSION_ID_MASK;
		//cs_cb->output.raw.pppoe_frame = 0x1234;
		//cs_cb->action.l2.pppoe_op_en = CS_PPPOE_OP_INSERT;
#if 1
		cs_cb->input_mask |= CS_HM_PPPOE_SESSION_ID_MASK|CS_HM_PPPOE_SESSION_ID_VLD_MASK;
		cs_cb->input.raw.pppoe_frame = 0x1234;	
		cs_cb->action.l2.pppoe_op_en = CS_PPPOE_OP_REMOVE;
#endif
#endif
	}

	return;
} //end k_jt_cs_bridge_hook_after_fwd()

void k_jt_cs_bridge_del_if_hook(cs_uint8 *p_dev_name)
{
#if 0
	struct list_head	*next, *tmp;
	struct cs_bridge_guid_map	*pMap;
	cs_uint64	guid;

	/* Traverse all elements of the list */
	list_for_each_safe(next, tmp, &bridge_map_head)
	{
		pMap = (struct cs_bridge_guid_map*) list_entry(next,
				struct cs_bridge_guid_map, list);

		if( strcmp(pMap->name, p_dev_name) == 0) {
			guid = pMap->guid;
			// delete list and free resource
            		list_del(&(pMap->list));
            		kfree(pMap);
			// delete kernel core hardware hash
			cs_kernel_core_del_hash_by_guid(guid);
		}
	}
#endif
	return;
}//end k_jt_cs_bridge_del_if_hook()

void cs_kernel_bridge_cb(cs_uint64 guid, int status)
{
	switch(status) {
        case CS_HASH_CB_FAILED_SW_REQ:
		break;

        case CS_HASH_CB_FAILED_RESOURCE:
		time_to_check_bridge_hash_resource = jiffies +
			BRIDGE_HASH_RESOURCE_TIMEOUT;
		break;

        case CS_HASH_CB_DELETE_BY_PEER:
		cs_bridge_del_map_by_guid(guid);
		break;

	case CS_HASH_CB_SUCCESSFUL:
	default:
		break;
    }
    
    return;
}//end cs_kernel_bridge_cb()

void cs_bridge_init(void)
{
	time_to_check_bridge_hash_resource = jiffies;
	
	/* init vid guid mapping list */
	INIT_LIST_HEAD(&bridge_map_head);

	// register call back
	//cs_kernel_reg_hash_cb(CS_KERNEL_BRIDGE_ID_TAG, cs_kernel_bridge_cb);

	hw_jt.cs_bridge_hook_before_fwd = &k_jt_cs_bridge_hook_before_fwd;
	hw_jt.cs_bridge_hook_after_fwd = &k_jt_cs_bridge_hook_after_fwd;
	hw_jt.cs_bridge_del_if_hook = &k_jt_cs_bridge_del_if_hook;
	printk("%s::---------------> \n",__func__);
	return;
}//end cs_vlan_init()
