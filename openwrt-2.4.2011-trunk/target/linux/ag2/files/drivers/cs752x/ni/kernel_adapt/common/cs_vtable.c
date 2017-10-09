#include <linux/module.h>
#include <linux/init.h>
#include <mach/cs_types.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <cs_hw_accel_util.h>
#include "cs_fe.h"
#include "cs_hw_accel_nat.h"
#include "cs_hw_accel_bridge.h"
#include <cs_hw_accel_core.h>
#include <cs_vtable.h>
#include <cs_hw_accel_cb.h>
#include <cs_hw_accel_base.h>
#ifdef CONFIG_CS752X_HW_ACCELERATION_IPSEC
#include "cs_hw_accel_ipsec.h"
#endif

extern fe_core_tables_s cs_fe_core_table;

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"
extern u32 cs_adapt_debug;
#define DBG(x)  if(cs_adapt_debug & CS752X_ADAPT_COMMON) x
//#else
//u32 cs_adapt_debug = 0;
//#define DBG(x)  if(cs_adapt_debug > 0) x
#endif /* CONFIG_CS752X_PROC */


#define MAX_NUM_TUPLE 6

/*
 * Vtable funtions 
 * SW index in vtable entry:
 * sw_index = (crc16 & 0xfff0) | (0~5)
 */

fe_hash_vtable_s *sys_vtable[MAX_VTABLE];

cs_status cs_vtable_init(void) 
{
	int i;

	for (i=0; i < MAX_VTABLE; i++) sys_vtable[i] = NULL;
	return CS_OK;
}

cs_uint32 cs_vtable_add_hash_by_skb(struct sk_buff *skb , cs_uint8 v_type) 
{
	// add mcgid etc etc into cb
	
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	
	cs_cb->common.vtype = v_type;
	
	return __cs_kernel_core_add_hash_by_skb(skb);
}

cs_uint32 cs_vtable_add_hash_by_cb( CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 v_type) 
{
	// add mcgid etc etc into cb
	
	cs_cb->common.vtype = v_type;
	// IF cb tells us to go to another vtable, then modify mcgid in 
	// cs_cb + voq for the root queue

	return __cs_kernel_core_add_hash_by_cb(cs_cb);
}

cs_uint8 cs_vtable_to_hash_mask(CS_HASH_ENTRY_S *hash_entry,
        fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type) 
{
	// add mcgid etc etc into cb, 
	if (hash_entry->mask & CS_HM_VTABLE_MASK) { //not belong hash mask filed
		DBG(printk("%s::CS_HM_VTABLE_MASK", __func__));
	}
	
	// Add mask to input
	return __cs_kernel_to_hash_mask(hash_entry, mask_rule);
}

cs_uint8 cs_vtable_to_hash_mask_qos_only(CS_HASH_ENTRY_S *hash_entry,
        fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type)
{
    return __cs_kernel_to_hash_mask_qos_only(hash_entry, mask_rule);
}

cs_uint8 cs_vtable_to_qos_hash_mask(CS_HASH_ENTRY_S *hash_entry, 
		fe_hash_mask_entry_t *mask_rule, cs_uint8 v_type)
{
	/* FIXME!! add MCGIC etc etc into hash_entry mask and stuff */

	return __cs_kernel_to_qos_hash_mask(hash_entry, mask_rule);
} /* cs_vtable_to_qos_hash_mask */

#define VTABLE_FWDRLT_DEFAULT 0
#define VTABLE_FWDRLT_TO_VTABLE 1

int cs_vtable_to_forwarding_result(fe_hash_rslt_s *result, 
		cs_uint8 v_type, cs_uint8 vtable_fwd_act, cs_uint8 nxt_vtable_idx)
{
	// add next table, if needs to be
	//FIXME: vtable_fwd_act? set root voq to next vtable
	return __cs_kernel_to_forwarding_result(result);
} /* cs_vtable_to_forwarding_result */

cs_uint16 cs_uu_flow_to_forwarding_result(fe_hash_rslt_s *result, 
		cs_uint8 v_type, cs_uint8 vtable_fwd_act, cs_uint8 nxt_vtable_idx)
{
	// add next table, if needs to be
	//FIXME: vtable_fwd_act? set root voq to next vtable
	return __cs_uu_flow_to_forwarding_result(result);
} /* cs_uu_flow_to_forwarding_result */

cs_uint16 cs_vtable_to_vlan_table(fe_hash_rslt_s *result, 
		cs_uint8 v_type, cs_uint8 vtable_fwd_act, cs_uint8 nxt_vtable_idx)
{
	return __cs_vtable_to_vlan_table(result);
} /* cs_vtable_to_forwarding_result */

cs_status cs_kernel_to_checkmem(CS_HASH_ENTRY_S *p_hash, 
        fe_hash_check_entry_t *pchkmem)
{
	return __cs_kernel_to_checkmem(p_hash, pchkmem);
}/* cs_kernel_to_checkmem */

cs_boolean cs_vtable_check_qos_enbl(CS_HASH_ENTRY_S *p_hash)
{
	return __cs_kernel_check_qos_enbl(p_hash);
} /* cs_vtable_check_qos_enbl */

cs_uint16 cs_vtable_to_qos_result(fe_hash_rslt_s *result, cs_uint8 v_type)
{
	/* FIXME!! Implement me
	 * Have to make sure qos tuple in SDB are set properly, for this 
	 * QoS result.  (In generic case, QoS tuple should be set as the 
	 * current forwarding tuple + the new QoS constraint.. might need 
	 * to take care of that in other function. 
	 * MCGID of vtable type is not needed for creating the QoS result.  
	 * I still have vtable_type as one of the parameters passed in */
	return __cs_kernel_to_qos_result(result);
} /* cs_vtable_to_qos_result */

#if 0 //for Vtable
/*
 *
 * Add default hash to send packet to CPU. For last vtable, key is mcgid,
 * action to default cpu voq
 *
 */
cs_uint32 cs_vtable_add_default_hash_to_cpu(cs_uint8 vid)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_uint8 cs_cb_data[sizeof(CS_KERNEL_ACCEL_CB_T)];
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)&cs_cb_data;
	
	/* (GE-0), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;//for debug, temp remove
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT0; /* GE-0 */
	cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	cs_cb->key_misc.orig_lspid = GE_PORT0;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT0_VOQ_BASE; /* CPU0, voq: 55 */
	//cs_cb->key_misc.fwd_type = CS_FWD_CPU; 
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-1), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT1; /* GE-1 */
	cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	cs_cb->key_misc.orig_lspid = GE_PORT1;
	//cs_cb->key_misc.orig_lspid = 1;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT1_VOQ_BASE; /* CPU1, voq 63 */
	//cs_cb->key_misc.fwd_type = CS_FWD_CPU;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-2), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.orig_lspid = 2;
	cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	cs_cb->key_misc.orig_lspid = GE_PORT2;
	//cs_cb->key_misc.lspid = GE_PORT2; /* GE-2 */
	
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE; /* CPU2, voq: 64 ~ 71 */
	//cs_cb->key_misc.fwd_type = CS_FWD_CPU; //defined in cs_fe_core_table.h
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	return CS_OK;
}
#endif //#if 0, cs_vtable_add_default_hash_to_cpu

/*
 *
 * Add default hash to send packet to CPU. For last vtable, key is mcgid,
 * action to default cpu voq
 *
 */
 //for one vtable testing
#ifndef CS_UU_TEST 
cs_uint32 cs_vtable_add_default_hash_to_cpu(cs_uint8 vid)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_uint8 cs_cb_data[sizeof(CS_KERNEL_ACCEL_CB_T)];
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)&cs_cb_data;
	
	/* (GE-0), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;//for debug, temp remove
	//cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	cs_cb->key_misc.lspid = GE_PORT0; /* GE-0 */
	//cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	//cs_cb->key_misc.orig_lspid = GE_PORT0;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT0_VOQ_BASE; /* CPU0, voq: 55 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; 
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-1), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;
	//cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	cs_cb->key_misc.lspid = GE_PORT1; /* GE-1 */
	//cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	//cs_cb->key_misc.orig_lspid = GE_PORT1;
	//cs_cb->key_misc.orig_lspid = 1;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT1_VOQ_BASE; /* CPU1, voq 63 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU;
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-2), Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;
	//cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	cs_cb->key_misc.lspid = GE_PORT2; /* GE-2 */
	//cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	//cs_cb->key_misc.orig_lspid = GE_PORT2;
	
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE; /* CPU2, voq: 64 ~ 71 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; //defined in cs_fe_core_table.h
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);

	/* (Crypto core), default go to VOQ 59 */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	cs_cb->key_misc.lspid = ENCRYPTION_PORT;
	//cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	//cs_cb->key_misc.orig_lspid = GE_PORT2;
	
	/* Output , parameter and action */
	/* FIXME! a better function for the hardcored value 59 */
	cs_cb->action.voq_pol.d_voq_id = 59;
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; //defined in cs_fe_core_table.h
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);

	return CS_OK;
}
#else /* CS_UU_TEST , FIXME: for UU test, need combination with above code */
cs_uint32 cs_vtable_add_default_hash_to_cpu(cs_uint8 vid)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_uint8 cs_cb_data[sizeof(CS_KERNEL_ACCEL_CB_T)];
	int i;
	//FIXME: temp add , need get from NI driver or AN_BNG table
	cs_uint8 eth_mac[GE_PORT_NUM][6]= {{0x00,0x00,0x00,0x00,0x0,0x30},
			 {0x00,0x00,0x00,0x00,0x00,0x38},
			 {0x00,0x00,0x00,0x00,0x00,0x40}};
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)&cs_cb_data;

#ifdef NEW_L2_DESIGN
	DBG(printk("L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2>>>>>\n"));
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	/* Preallocate tuple0, LSPID + MAC SA */
	cs_cb->input_mask |= CS_HM_MAC_SA_MASK;
	cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->key_misc.lspid = 0;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE; /* ROOT Q, voq: 40 */
	cs_cb->action.voq_pol.d_voq_id = GE_PORT2_VOQ_BASE;
	cs_cb->key_misc.fwd_type = CS_FWD_NORMAL; 
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	DBG(printk("L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2L2<<<<<\n"));
#endif	
	
	/* (GE-0), Unicast Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT0; /* GE-0 */
	for (i = 0; i < 6; i++) {
		cs_cb->input.raw.da[i] = eth_mac[GE_PORT0][i];
	}
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT0_VOQ_BASE; /* CPU0, voq: 48 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; 
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-1), Unicast Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT1; /* GE-1 */
	for (i = 0; i < 6; i++) {
		cs_cb->input.raw.da[i] = eth_mac[GE_PORT1][i];
	}
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT1_VOQ_BASE; /* CPU1, voq 56 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU;
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* (GE-2), Unicast Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_HASH_ENTRY_S));	
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	cs_cb->key_misc.lspid = GE_PORT2; /* GE-2 */
	for (i = 0; i < 6; i++) {
		cs_cb->input.raw.da[i] = eth_mac[GE_PORT2][i];
	}
	cs_cb->common.vtype = sys_vtable[vid]->type;
	
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE; /* CPU2, voq: 64 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; //defined in cs_fe_core_table.h
	
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	
	/* (GE-0),(GE-1),(GE-2) Brocadcast Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT0; /* GE-0 */
	for (i = 0; i < 6; i++) {
		cs_cb->input.raw.da[i] = 0xFF;
	}
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT3_VOQ_BASE; /* CPU3, voq: 72 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; 
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);

#if 0	
	/* (GE-0),(GE-1),(GE-2) multicast Default go to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_CPU;
	cs_cb->input_mask |= CS_HM_MAC_DA_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT0; /* GE-0 */
	cs_cb->input.raw.da[0] = 0x01;
	cs_cb->input.raw.da[1] = 0x00;
	cs_cb->input.raw.da[1] = 0x5e;
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT4_VOQ_BASE; /* CPU0, voq: 80 */
	cs_cb->key_misc.fwd_type = CS_FWD_CPU; 
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
#endif	
	return CS_OK;
}
#endif /* CS_UU_TEST */

int cs_find_vtable_idx_by_type (cs_uint8 type) 
{
	int i;

	for (i=0;i<MAX_VTABLE; i++) {
		if (sys_vtable[i] != NULL) 
			if ( sys_vtable[i]->type == type ) return i;
	}
	return -1;
}

cs_status cs_vtable_add_default_hash_to_vtable(cs_uint8 vid, int next_vtype)
{
	// FIX ME... NEED TO ADD
	// hash input |= mcgid & mask
	// result table change to new mcgid & mask
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_uint8 cs_cb_data[sizeof(CS_KERNEL_ACCEL_CB_T)];
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)&cs_cb_data;
	
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->common.module_mask |= CS_MOD_MASK_TO_VTABLE;
	//cs_cb->input_mask |= CS_HM_MCGID_MASK;
	//cs_cb->input_mask |= CS_HM_LSPID_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	
	/* Input , key */
	// FIXME: original mcgid
	cs_cb->key_misc.mcgid = sys_vtable[vid]->mcg_id & 0x0F0;//5 bits
	//FIXME: Hard code for debug, need remove or mofify
	cs_cb->key_misc.orig_lspid = GE_PORT1;
	//cs_cb->key_misc.lspid = 0; /* GE-0 */
	cs_cb->common.vtype = sys_vtable[vid]->type;
	
	//cs_cb->key_misc.orig_lspid = 0;
	
	/* Output , parameter and action */
	cs_cb->output_mask |= CS_HM_MCGID_MASK | CS_HM_VTABLE_MASK;	
	cs_cb->action.misc.mcgid = 
				sys_vtable[cs_find_vtable_idx_by_type(next_vtype)]->mcg_id & 0x0F0;//5 bits
	cs_cb->action.misc.mcgid_vaild = 1;
	cs_cb->action.voq_pol.d_voq_id = ROOT_PORT_VOQ_BASE; /* Root, voq: 40 ~ 47 */
	DBG(printk("%s: cs_cb->action.voq_pol.d_voq_id = %d\n",__func__,cs_cb->action.voq_pol.d_voq_id));
	//cs_cb->key_misc.fwd_type = CS_FWD_NORMAL; 
	__cs_kernel_core_add_hash_by_cb(cs_cb);

	return CS_OK;
}

cs_uint32 cs_vtable_add_uu_hash_to_lspid(cs_uint8 vid)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_uint8 cs_cb_data[sizeof(CS_KERNEL_ACCEL_CB_T)];
	
	cs_cb = (CS_KERNEL_ACCEL_CB_T *)&cs_cb_data;
#if 0	
	/* UU from GE-1 to GE-2 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT1, 1, GE_PORT2_VOQ_BASE + 1, vid);
	/* UU from GE-1 to GE-0 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT1, 3, GE_PORT0_VOQ_BASE + 1, vid);
	/* UU from GE-1 to CPU */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT1, 2, CPU_PORT1_VOQ_BASE, vid);
	
	/* UU from GE-2 to GE-1 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT2, 1, GE_PORT1_VOQ_BASE + 1, vid);
	/* UU from GE-2 to GE-0 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT2, 3, GE_PORT0_VOQ_BASE + 1, vid);
	/* UU from GE-2 to CPU */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT2, 2, CPU_PORT2_VOQ_BASE, vid);
	
	/* UU from GE-0 to GE-1 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT0, 1, GE_PORT1_VOQ_BASE + 1, vid);
	/* UU from GE-0 to GE-2 */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT0, 3, GE_PORT2_VOQ_BASE + 1, vid);
	/* UU from GE-0 to CPU */
	cs_vtable_set_uu_hash(cs_cb, GE_PORT0, 2, CPU_PORT0_VOQ_BASE, vid);
#endif	/* #if 0 */
#if 1	
	/* UU from GE-1 to GE-2 */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = GE_PORT1; /* GE-1 */
	cs_cb->key_misc.mcidx = 1; /* UU_COPIES = 0x0110 */
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = (GE_PORT2_VOQ_BASE + 1); /* GE-2, voq: 17 */
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	// jason
	//__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* UU from GE-1 to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = GE_PORT1; /* GE-1 */
#ifdef NEW_L2_DESIGN
	 cs_cb->key_misc.mcidx = 1; /* UU_COPIES = 0x0010 */
#else
	cs_cb->key_misc.mcidx = 2; /* UU_COPIES = 0x0110 */
#endif
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT1_VOQ_BASE; /* CPU0, voq: 56 */
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	/* UU from GE-2 to GE-1 */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = GE_PORT2; /* GE-2 */
	cs_cb->key_misc.mcidx = 1; /* UU_COPIES = 0x0110 */
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = (GE_PORT1_VOQ_BASE + 1); /* GE-2, voq: 9 */
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
#ifndef NEW_L2_DESIGN
	__cs_kernel_core_add_hash_by_cb(cs_cb);
#endif
	
	/* UU from GE-2 to CPU */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = GE_PORT2; /* GE-2 */
#ifdef NEW_L2_DESIGN
	cs_cb->key_misc.mcidx = 1; /* UU_COPIES = 0x0110 */
#else
	cs_cb->key_misc.mcidx = 2; /* UU_COPIES = 0x0110 */
#endif
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = CPU_PORT2_VOQ_BASE; /* CPU0, voq: 64 */
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
#endif /* #if 1 */
	
	/* Build UU sdb and mask ram */
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	
	//cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	//cs_cb->input_mask |= (CS_HM_MAC_DA_MASK);
	//cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->output_mask |= CS_HM_MCGID_MASK;
	
	cs_cb->common.module_mask |= CS_MOD_MASK_BRIDGE;
	cs_cb->common.vtype = CS_VTBL_ID_L2_FLOW;
	
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;

	/* Input , key */
	//cs_cb->key_misc.lspid = GE_PORT1; /* GE-1 */
	//cs_cb->key_misc.mcidx = 7; /* CS_VTBL_ID_UU_FLOW = 1000_0000 */
	cs_cb->common.vtype = sys_vtable[vid]->type;
	cs_cb->common.uu_flow_enable = CS_RESULT_ACTION_ENABLE;
	//cs_cb->key_misc.lspid = GE_PORT1;
	//cs_cb->key_misc.orig_lspid = GE_PORT1;
	/* Output , parameter and action */
	//cs_cb->action.voq_pol.d_voq_id = ROOT_PORT_VOQ_BASE; /* Root, voq: 40 */
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	return CS_OK;
}

cs_status cs_vtable_set_uu_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 mc_index, 
		cs_uint8 o_lspid, cs_uint8 d_voq, cs_uint8 vid)
{
	memset((void *)cs_cb, 0, sizeof(CS_KERNEL_ACCEL_CB_T));
	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK;
	cs_cb->input_mask |= CS_HM_MCIDX_MASK;
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
	/* Input , key */
	cs_cb->key_misc.orig_lspid = o_lspid; /* GE-# */
	cs_cb->key_misc.mcidx = mc_index; /* UU_COPIES = 0x1110 */
	cs_cb->common.vtype = sys_vtable[vid]->type;
	/* Output , parameter and action */
	cs_cb->action.voq_pol.d_voq_id = d_voq; /* destinatio voq */
	/* Never age out this hash */
	cs_cb->tmo.interval = 0;
	__cs_kernel_core_add_hash_by_cb(cs_cb);
	
	return CS_OK;
}

cs_status cs_deallocate_vtable( cs_uint8 vid ) {
	if (sys_vtable[vid]->class_idx != VTABLE_UINT8_INVALID)
		cs_fe_table_del_entry_by_idx(FE_TABLE_CLASS, 
				sys_vtable[vid]->class_idx, false);
	if (sys_vtable[vid]->sdb_idx != VTABLE_UINT8_INVALID)
		cs_fe_table_del_entry_by_idx(FE_TABLE_SDB, 
				sys_vtable[vid]->sdb_idx, false);
		
	kfree(sys_vtable[vid]);
	sys_vtable[vid] = NULL;
	DBG(printk("%s:: Allocate vritual table resource fail\n",__func__));
	return CS_ERROR;
}

cs_status cs_vtable_allocate_add_sdb(fe_hash_vtable_s *vtable)
{
	int i;
	fe_sdb_entry_t *p_sdb;
	cs_uint32 sdb_data[sizeof(fe_sdb_entry_t)];

	p_sdb = (fe_sdb_entry_t *)&sdb_data;
	memset((void *)p_sdb, 0, sizeof(fe_sdb_entry_t));

	for (i = 0; i < MAX_NUM_TUPLE; i++) {
		p_sdb->sdb_tuple[i].enable = 0;
	}
		
	//flow_index pointer to result index 1;
	//FIXME: when do we need enable uu_flow.flow_index ????
	//p_sdb->uu_flow.flow_index = 1;
	//p_sdb->uu_flow.enable = CS_SDB_ENABLE;

	cs_fe_table_add_entry(FE_TABLE_SDB, p_sdb, (unsigned int *)&(vtable->sdb_idx));
	DBG(printk("%s:: --->  (vtable->sdb_idx) = %d\n",__func__, vtable->sdb_idx));

	return CS_OK;
} /* cs_add_vtable_sdb */

static int fe_class_default_flow_base_cfg(fe_class_entry_t *p_class, 
		cs_uint8 mcg_id, cs_uint8 mcg_id_mask, cs_uint8 sdb_idx, 
		cs_uint8 priority)
{
	p_class->sdb_idx = sdb_idx;
	p_class->rule_priority = priority;
	p_class->entry_valid = 1;
	p_class->l3.ip_sa_mask = 0x000;
	p_class->l3.ip_da_mask = 0x000;
	p_class->parity = 1;
	p_class->port.mcgid = mcg_id;
	return 0;
} /* fe_class_default_flow_base_cfg */

static int fe_class_add_to_vtable(fe_class_entry_t *p_class, cs_uint8 mcg_id, 
		cs_uint8 mcgid_mask, cs_uint8 sdb_idx, cs_uint8 priority)
{
	p_class->sdb_idx = sdb_idx;
	p_class->rule_priority = priority;
	p_class->entry_valid = 1;
	p_class->l3.ip_sa_mask = 0x000;
	p_class->l3.ip_da_mask = 0x000;
	p_class->parity = 1;
	p_class->port.mcgid = mcg_id;
	return 0;
} /* fe_class_add_to_vtable */

cs_status cs_vtable_allocate_add_class(fe_hash_vtable_s *vtable, int priority, int default_act)
{
	fe_class_entry_t *p_class;
	cs_uint32 class_data[sizeof(fe_class_entry_t)];
	int status;
	unsigned int class_idx;

	p_class = (fe_class_entry_t *)&class_data;
RETRY_CLASS:
	memset((void *)p_class, 0xFF, sizeof(fe_class_entry_t));

	status = cs_fe_table_alloc_entry(FE_TABLE_CLASS, &class_idx, 0);
	if (status != 0) {
		printk("%s:%d:failed to allocate class, status = %d\n", __func__, 
				__LINE__, status);
		return CS_ERROR;
	}
	vtable->class_idx = (cs_uint8)class_idx;
	
	if (CS_OK != status) return CS_ERROR;

	DBG(printk("--> ADD CLASSIFIER ,p_class->class_idx = 0x%X, vtable->mcg_id = 0x%03X, MCG_VTABLE_MASK = 0x%03X\n",
		vtable->class_idx, vtable->mcg_id, MCG_VTABLE_MASK));
	
	switch (default_act) {
	case VTABLE_ACT_DEF_TO_CPU:
		fe_class_default_flow_base_cfg(p_class, vtable->mcg_id, MCG_VTABLE_MASK,
				vtable->sdb_idx, priority);
		break;
	case VTABLE_ACT_DEF_TO_VTBL:
		fe_class_add_to_vtable(p_class, vtable->mcg_id, MCG_VTABLE_MASK,
				vtable->sdb_idx, priority);
		break;
	}	

	status = cs_fe_table_set_entry(FE_TABLE_CLASS, vtable->class_idx, p_class);
	if (status != 0) {
		printk("%s:%d:failed to set class, status = %d\n", __func__, 
				__LINE__, status);
		cs_fe_table_del_entry_by_idx(FE_TABLE_CLASS, vtable->class_idx, true);
		goto RETRY_CLASS;
	}

	status = cs_fe_table_inc_entry_refcnt(FE_TABLE_CLASS, vtable->class_idx);
	if (status != 0) {
		printk("%s:%d:failed to inc refcnt class, status = %d\n", __func__, 
				__LINE__, status);
		cs_fe_table_del_entry_by_idx(FE_TABLE_CLASS, vtable->class_idx, true);
		goto RETRY_CLASS;
	}

	return CS_OK;
}	/* cs_vtable_allocate_add_class */

cs_status cs_alloc_vtable(cs_uint8 *vid, cs_uint8 type, int default_act, 
		int next_vtable_type, int forced, int forced_mcg_id, int priority)
{
	int i;
	DBG(printk("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"));

	for (i=0;i<MAX_VTABLE;i++) {
		if (sys_vtable[i] == NULL) {
			sys_vtable[i] = kmalloc(sizeof(fe_hash_vtable_s), GFP_ATOMIC);
			if (sys_vtable[i] == NULL)
				return CS_ERROR;
			memset((void *)sys_vtable[i], 0, sizeof(fe_hash_vtable_s));
			break;
		}
	}
	if (vid != NULL) 
		*vid = i;
	if (i == MAX_VTABLE) 
		return CS_ERROR; // Allocation of the VTABLE failed
	
	sys_vtable[i]->vid = i;
	sys_vtable[i]->type = type;
	sys_vtable[i]->sdb_idx = VTABLE_UINT8_INVALID;
	sys_vtable[i]->class_idx = VTABLE_UINT8_INVALID;
	
	if (forced) sys_vtable[i]->mcg_id = forced_mcg_id;
	//allocate virtual table sdb resource 
	if (cs_vtable_allocate_add_sdb(sys_vtable[i]) != CS_OK) {
		goto cs_vtable_error_alloc;
	}
	//allocate virtual table classifier resource 
	//if (cs_vtable_allocate_add_class(sys_vtable[i], priority , VTABLE_ACT_DEF_TO_CPU) != CS_OK){
	//	goto cs_vtable_error_alloc;
	//}
	
#if 0
	if (cs_allocate_mcg_vtable_id(&(sys_vtable[i]->mcg_id), forced) != CS_OK) {
		goto cs_vtable_error_alloc;
		//return CS_ERROR;
	}
#endif
	DBG(printk("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n"));
	switch( default_act) {
		case VTABLE_ACT_DEF_TO_CPU:
			if (cs_vtable_allocate_add_class(sys_vtable[i], priority , VTABLE_ACT_DEF_TO_CPU) != CS_OK)
				goto cs_vtable_error_alloc;
			if (cs_vtable_add_default_hash_to_cpu(*vid) != CS_OK) 
				return CS_ERROR;
			break;
		case VTABLE_ACT_DEF_TO_VTBL:
			if (cs_vtable_allocate_add_class(sys_vtable[i], priority , VTABLE_ACT_DEF_TO_VTBL) != CS_OK)
				goto cs_vtable_error_alloc;
			// setup mcgid to match the new table for the result entry
			// put next table's mcgid and mask to cb
			if ((cs_vtable_add_default_hash_to_vtable(*vid, next_vtable_type)) != CS_OK)
				return CS_ERROR;
			break;
	}
#ifdef CS_UU_TEST	
	/* creat default hash for UU of mcidx */
	if (type == CS_VTBL_ID_L2_FLOW) {
		if (cs_vtable_add_uu_hash_to_lspid(*vid) != CS_OK) 
				return CS_ERROR;
	}
#endif	
	
	return CS_OK;
cs_vtable_error_alloc:
	cs_deallocate_vtable( i);
	return CS_ERROR;
	
}


int get_vtable_type_by_idx (cs_uint8 vid) 
{
	if (sys_vtable[ vid ] != 0) {
		return sys_vtable[vid]->type;
	}
	return -1;
}

fe_hash_vtable_s *cs_find_vtable_by_type (cs_uint8 type) 
{
	return (sys_vtable[ cs_find_vtable_idx_by_type(type) ]);
}

cs_status cs_add_vtable_entry(cs_uint16 crc16, cs_uint32 crc32, 
		cs_uint8 mask_ptr, cs_uint16 result_index, cs_uint16 *return_idx, 
		cs_uint8 vid, cs_int16 timeout)
{
	cs_status status = FE_ERR_ENTRY_NOT_FOUND;
	cs_uint16 hash_index = crc16 >> 4;
	fe_vtable_hash_result_s *hash_result;
	unsigned int p_entry_index;
	fe_vtable_hash_entry_s *ventry;
	int i;

	if (sys_vtable[vid]->vtable[hash_index] == NULL) {
		hash_result = (fe_vtable_hash_result_s*)kzalloc(
			sizeof(fe_vtable_hash_result_s), GFP_ATOMIC);
		if (hash_result == NULL)
			return FE_ERR_ALLOC_FAIL;
		sys_vtable[vid]->vtable[hash_index] = hash_result;
	}
	hash_result =  sys_vtable[vid]->vtable[hash_index];

	//spin_lock(&table->lock);//temp remove
	/* check vtable entries first, then try to add to hw table */
#define MAX_TUPLE_NUM	6
	for (i = 0; i < MAX_TUPLE_NUM; i++) {
		ventry = &(hash_result->entries[i]);
		if (ventry->valid == 0) {
			ventry->valid = 1;
			ventry->crc32 = crc32;
			ventry->crc16 = crc16;
			ventry->result_index = result_index;
			ventry->mask_ptr = mask_ptr;
			break;
		} else {
			if ((ventry->crc32 == crc32) && (ventry->crc16 == crc16)) {
				// should we consider duplicated cases?
			}
		}
		if (i==6) {
			/* we do not handle overflow in virtual tables though */
			//spin_unlock(&table->lock);
			return FE_ERR_ENTRY_NOT_FOUND;
		}
	}

	DBG(printk("%s:%d:crc32 = %x, crc16 %x, mask_ptr = %d, rslt_idx = %d\n", 
		__func__, __LINE__, crc32, crc16, mask_ptr, result_index));
	/* now try to allocate an entry in hw table */
	status = cs_fe_hash_add_hash((__u32)crc32, (__u16)crc16,(__u8)mask_ptr, 
			(__u16)result_index, &p_entry_index);
	// what do we want to do with timeout 
	if (status != FE_STATUS_OK) {
		ventry->valid = 0; ventry->crc32 = 0;
		ventry->crc16 = 0; ventry->result_index = 0;
		ventry->mask_ptr = 0;
		/* fail to add to hw hash table */
		//spin_unlock(&table->lock);
		return status;
	}
	ventry->p_entry_index = p_entry_index;

	// FIXME! how come it was the other way around?
	//*return_idx = (hash_index | i);
	*return_idx = p_entry_index;

	//spin_unlock(&table->lock);
	return FE_STATUS_OK;
}

cs_status cs_invalidate_vtable_entry(fe_hash_vtable_s *table, cs_uint16 index)
{
	cs_status status = FE_ERR_ENTRY_NOT_FOUND;
	fe_vtable_hash_result_s *result = table->vtable[index>>4];
	fe_vtable_hash_entry_s  *ventry;
	if (result == NULL)
		return status;
	//spin_lock(&table->lock);//temp remove
	ventry  = &result->entries[index& 0x07];
	if (ventry && ventry->valid) {
		status = cs_fe_hash_del_hash(ventry->p_entry_index);
		if (status == FE_STATUS_OK) {
			ventry->valid = 0;
			ventry->crc32 = 0;
			ventry->crc16 = 0;
			ventry->result_index = 0;
			ventry->mask_ptr = 0;
		}
	}
	//spin_unlock(&table->lock);
	return status;
}

cs_status cs_hash_release_vtable(cs_uint8 vid)
{
	int i, j;
	fe_vtable_hash_result_s *hash_result;
	fe_vtable_hash_entry_s  *hash_entry;
	cs_uint16       hash_index;

	for (i=0; i<FE_VTABLE_SIZE; i++) {
		hash_result = sys_vtable[vid]->vtable[i];
		if (hash_result != NULL) {
			for (j=0; j<6; j++) {
				hash_entry = &hash_result->entries[j];
				if (hash_entry) {
					hash_index = (hash_entry->crc16 & 0xfff0) | j;
					cs_invalidate_vtable_entry(sys_vtable[vid], hash_index);
				}
			}
			kfree(hash_result);
		}
	}
	kfree( sys_vtable[vid] );
	sys_vtable[vid] = NULL;

	return FE_STATUS_OK;
}

static int cs_fe_set_sdb_hash_method(cs_uint8 sdb_idx, cs_uint8 value)
{
	return cs_fe_table_set_field(FE_TABLE_SDB, sdb_idx, FE_SDB_HTPL_STSUPDT_CNTL, 
			(__u32*)&value);
} /* cs_fe_set_sdb_hash_method */

cs_status cs_vtable_add_sdb_tuple(cs_uint8 mask_ptr, fe_hash_vtable_s *vtable)
{
	int i;
	//fe_hash_vtable_s *vtable;
	fe_sdb_entry_t sdb_entry, *p_sdb;
	int status;
	bool f_add_new = false;

	status = cs_fe_table_get_entry(FE_TABLE_SDB, vtable->sdb_idx, &sdb_entry);
	if (status != 0) {
		f_add_new = true;
		memset(&sdb_entry, 0x0, sizeof(sdb_entry));
	}

	p_sdb = &sdb_entry;

	for (i = 0; i < 6; i++) {
		if ((p_sdb->sdb_tuple[i].mask_ptr == mask_ptr) && 
				(p_sdb->sdb_tuple[i].enable == 1)) {
			DBG(printk("%s::Duplicate tuple %d, masK_ptr %d\n", __func__, i, 
					mask_ptr));
			break;
		}
		// find avaiable tuple, FIXME: to handle config priority, 
		// 0: lowest priority
		if (p_sdb->sdb_tuple[i].enable == 0) {
			p_sdb->sdb_tuple[i].enable = 1;
			//FIXME: need get from hash mask ram
			p_sdb->sdb_tuple[i].mask_ptr = mask_ptr;
			p_sdb->sdb_tuple[i].priority = i;
#ifdef NEW_L2_DESIGN
			if(i == 0)
		 		p_sdb->sdb_tuple[i].priority |= 0x8;	/* msb of pirority for logical "and" of tuple */
	 		p_sdb->misc.hash_sts_update_ctrl = 1;		/* Update tuple0 and winning tuple */
#endif				
			break;
		}
	}

	if (f_add_new == true)
		cs_fe_table_add_entry(FE_TABLE_SDB, p_sdb, (unsigned int *)&(vtable->sdb_idx));
	else
		cs_fe_table_set_entry(FE_TABLE_SDB, vtable->sdb_idx, p_sdb);

#ifdef NEW_L2_DESIGN
	cs_fe_set_sdb_hash_method(vtable->sdb_idx, p_sdb->misc.hash_sts_update_ctrl);
#endif

	return CS_OK;
} /* cs_vtable_add_sdb_tuple */

cs_status cs_vtable_add_sdb_uu_flow(cs_uint16 uu_flow_index, fe_hash_vtable_s *vtable)
{
	cs_uint32 value;

	value = uu_flow_index;
	cs_fe_table_set_field(FE_TABLE_SDB, vtable->sdb_idx, 
			FE_SDB_UU_FLOWIDX, (__u32*)&value);

	value = 0;
	cs_fe_table_set_field(FE_TABLE_SDB, vtable->sdb_idx, 
			FE_SDB_VLAN_EGR_MBRSHP_EN, (__u32*)&value);
	cs_fe_table_set_field(FE_TABLE_SDB, vtable->sdb_idx, 
			FE_SDB_VLAN_EGRUNTAG_CHK_EN, (__u32*)&value);

	/* Set Broadcast and Unknown Multicast index as UU flowidx */
	value = uu_flow_index;
	cs_fe_table_set_field(FE_TABLE_SDB, vtable->sdb_idx, 
			FE_SDB_BC_FLOWIDX, (__u32*)&value);
	cs_fe_table_set_field(FE_TABLE_SDB, vtable->sdb_idx, 
			FE_SDB_UM_FLOWIDX, (__u32*)&value);
	return CS_OK;
} /* cs_vtable_add_sdb_uu_flow */

cs_status cs_vtable_add_sdb_qos_tuple(cs_uint8 mask_ptr, fe_hash_vtable_s *vtable)
{
	int i;
	//fe_hash_vtable_s *vtable;
	fe_sdb_entry_t sdb_entry, *p_sdb;
	int status;

	status = cs_fe_table_get_entry(FE_TABLE_SDB, vtable->sdb_idx, &sdb_entry);
	if (status != 0) return status;

	p_sdb = &sdb_entry;

	for (i = 6; i < 8; i++) {
		if (p_sdb->sdb_tuple[i].mask_ptr == mask_ptr) {
			printk("%s::Duplicate tuple %d, mask_ptr %d\n", __func__, i, 
					mask_ptr);
			break;
		}
		// find avaiable tuple, FIXME: to handle config priority, 
		// 0: lowest priority
		if (p_sdb->sdb_tuple[i].enable == 0) {
			p_sdb->sdb_tuple[i].enable = 1;
			//FIXME: need get from hash mask ram
			p_sdb->sdb_tuple[i].mask_ptr = mask_ptr;
			p_sdb->sdb_tuple[i].priority = 10 - i;
			DBG(printk("Enable tuple %d, priority %d \n", i, i));
			break;
		}
	}

	cs_fe_table_set_entry(FE_TABLE_SDB, vtable->sdb_idx, p_sdb);

	return CS_OK;
} /* cs_vtable_add_sdb_qos_tuple */
