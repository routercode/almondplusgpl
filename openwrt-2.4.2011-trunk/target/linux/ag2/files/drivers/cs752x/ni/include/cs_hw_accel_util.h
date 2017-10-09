#ifndef CORTINA_KERNEL_ADAPTION_UTILITIES
#define CORTINA_KERNEL_ADAPTION_UTILITIES 1

#include <linux/skbuff.h>
#include "cs_hw_accel_cb.h"
#include <mach/cs75xx_fe_core_table.h>

/*
	GUID Definition
	===============================================

	BITFIELD
	-----------------------------------------------
*/
#define CS_KERNEL_MC_ID_TAG			0x0001
#define CS_KERNEL_NETFILTER_ID_TAG	0x0002
#define CS_KERNEL_BRIDGE_ID_TAG		0x0004
#define CS_KERNEL_EBTABLE_ID_TAG	0x0008
#define CS_KERNEL_QOS_ID_TAG		0x0010
#define CS_KERNEL_VLAN_ID_TAG		0x0020
#define CS_KERNEL_NAT_ID_TAG		0x0040
#define CS_KERNEL_IPSEC_ID_TAG		0x0080
#define CS_KERNEL_PPPOE_ID_TAG      0x0100
#define CS_KERNEL_ID_TAG_MAX		0x0200
#define CS_KERNEL_MAX_ID_NUM		0x0200 /* equate to the last entry */

#define CS_KERNEL_INVALID_GUID		0xFFFFFFFFFFFFFFFF


/*
	Helper Functions
*/
cs_uint64 cs_kernel_adapt_get_guid(cs_uint16 guid_type);

/*
	Call Back Definition
	===============================================

	Definitions
	-----------------------------------------------
*/
#define CS_REG_SUCCESSFUL				0x0001
#define CS_REG_FAILURE_EXISTS			0xFFFF
#define CS_REG_FAILURE_TAG_OUT_OF_RANGE	0xFFFE

#define CS_HASH_CB_SUCCESSFUL			0x0001
#define CS_HASH_CB_FAILED_SW_REQ		0xFFFF
#define CS_HASH_CB_FAILED_RESOURCE		0xFFFE
#define CS_HASH_CB_DELETE_BY_PEER		0xFFFD

/*
	Helper Functions
*/
cs_int32 cs_kernel_reg_hash_cb(cs_uint16 id_tag, void*);
	/* cb_func(cs_uint64 guid, cs_int32 status) */
cs_int32 cs_kernel_dereg_hash_cb(cs_uint16 id_tag);
void cs_kernel_invoke_cb( cs_uint16 id_tag, cs_uint64 guid, int status);
typedef void cs_kernel_cb( cs_uint64 guid, int status );
cs_kernel_cb *cs_kernel_get_cb_by_tag_id(cs_uint16 id_tag);

/*
	SKB Manipulation Definition
	===============================================
*/
#define CS_HASH_CB_ENTRYS			2
/*
	Helper Functions
*/
cs_status cs_kernel_add_guid( cs_uint64 guid, struct sk_buff *skb);

// index = starting index in case there are multiple guid for the same
// module
cs_uint64 cs_kernel_get_guid( cs_uint16 tag, struct sk_buff *skb, cs_int32 index);

// FIXME mask type is wrong
cs_int32 cs_kernel_set_hash_mask ( cs_uint32 mask, struct sk_buff *skb);

cs_int32 cs_kernel_get_mask ( struct sk_buff *skb);
cs_int32 cs_kernel_get_field (void );
cs_int32 cs_kernel_get_action ( cs_int32 hash_id );
cs_int32 cs_kernel_set_action ( cs_int32 hash_id );

cs_int32 cs_kernel_set_mcg_info_fields ( struct sk_buff *skb, unsigned int mcgid,
	unsigned int mcidx);
int cs_kernel_core_del_hash_by_guid(cs_uint64 guid);

/*
	Hash Manipulation Definition
	===============================================
*/

typedef struct {
	/* L2 Address Results */
	cs_uint8	mac_sa[6];
	cs_uint8	mac_da[6];
	cs_boolean	l2_parity;
	/* Flow VLAN Results */
	cs_uint8	first_vlan_cmd;
	cs_uint16	first_vid;
	cs_uint8	first_tpid_enc;
	cs_uint8	second_vlan_cmd;
	cs_uint16	second_vid;
	cs_uint8	second_tpid_enc;
	cs_boolean	vlan_parity;
	/* L3 Results */
	cs_uint32	sip_addr[4];
	cs_uint32	dip_addr[4];
	cs_boolean	l3_parity;
	/* L4 Results */
	cs_uint16	sport;
	cs_uint16	dport;
	/* Voq & Policer Results */
	cs_uint8	d_voq;			/* destination voq of the packet */
	cs_uint8	d_pol;			/* destinaiton policer id of the packet */
	cs_uint8	cpu_pid;		/* CPU sub port ID. */
	cs_uint8	ldpid;			/* logical destination port that voq maps to */
	cs_uint16	pppoe_session_id;	/* pppoe session ID to be encaped */
	//cs_uint8	cos_nop;//action
	cs_boolean	voq_policer_parity;
	/* Mice */
	cs_uint16	mcgid;
	cs_uint8	mcidx;
	cs_uint8	fwd_type;
	cs_uint32	sw_action_id;
	cs_uint8	uu_flow_enable;
} fe_hash_param_s;

typedef struct {
	/* QoS Results */
	cs_uint8	wred_cos;
	cs_uint8	voq_cos;
	cs_uint8	pol_cos;
	cs_uint8	premark;
	cs_uint8	change_dscp_en;
	cs_uint8	dscp;
	cs_uint8	dscp_markdown_en;
	cs_uint8	marked_down_dscp;
	cs_uint8	ecn_en;
	cs_uint8	top_802_1p;
	cs_uint8	marked_down_top_802_1p;
	cs_uint8	top_8021p_markdown_en;
	cs_uint8	top_dei;
	cs_uint8	marked_down_top_dei;
	cs_uint8	inner_802_1p;
	cs_uint8	marked_down_inner_802_1p;
	cs_uint8	inner_8021p_markdown_en;
	cs_uint8	inner_dei;
	cs_uint8	marked_down_inner_dei;
	cs_uint8	change_8021p_1_en;
	cs_uint8	change_dei_1_en;
	cs_uint8	change_8021p_2_en;
	cs_uint8	change_dei_2_en;
	cs_boolean	qos_parity;
} fe_hash_param_qos_s;

typedef struct {
	/* L2 */
	cs_uint8	mac_sa_replace_en;
	cs_uint8	mac_da_replace_en;
	cs_uint8	flow_vlan_op_en;
	cs_uint8	pppoe_encap_en;	/* Insert PPPoE */
	cs_uint8	pppoe_decap_en; /* Remove PPPoE */
	/* L3 */
	cs_uint8	ip_sa_replace_en;
	cs_uint8	ip_da_replace_en;
	cs_uint8	decr_ttl_hoplimit;
	/* L4 */
	cs_uint8	sp_replace_en;
	cs_uint8	dp_replace_en;
	/* Voq & Policer Results */
	cs_uint8	cos_nop;	
	/* Mice */
	cs_uint8	d_pol_id;	/* Policer ID */
	cs_uint8	d_voq_id; 	/* Voq ID */
	cs_uint8	sw_action; 
	cs_uint8	fwd_type_valid;
	cs_uint8	mcgid_valid;
	cs_uint8	drop;
	cs_boolean	acl_dsbl;
	cs_boolean	action_parity;
} fe_hash_action_s;

typedef struct {
	cs_uint16	l2_index;		/* index to MAC table */
	cs_uint16	l3_index;		/* Pointer to IP table */
	cs_uint16	voq_pol_table_index;
	cs_uint16	flow_vlan_index;	/* index to flow vlan table entry */
	cs_uint8    mask_ptr_index;
} fe_hash_rslt_index_s;

typedef struct {
	fe_hash_param_s param;
	fe_hash_param_qos_s qos_param;
	fe_hash_action_s action;
	fe_hash_rslt_index_s index;
	fe_vlan_entry_t vlan;
} fe_hash_rslt_s;

/* Hash Entry */
typedef struct {
    cs_uint64 mask;
    fe_sw_hash_t key;
    fe_hash_rslt_s result;
} CS_HASH_ENTRY_S;

/*
	Helper Functions
*/
cs_int32 cs_kernel_remove_hash_guid (cs_uint64 guid);
// int cs_kernel_remove_hash_by_lsp (src/dst); Is this needed?

// calculate hash
cs_uint32 cs_kernel_calc_hash(void
	// field mask
	// field value
	// vtable id
);


// OK
// Failed - duplicate, out of resource
// return the hash table id for future reference
// return error with duplication
#define CS_HASH_ADD_SUCCESSFUL			0x0001
#define CS_HASH_ADD_ERR_OUT_OF_RESOURCE	0xFFFFFFFF
#define CS_HASH_ADD_ERR_DUPLICATE		0xFFFFFFFE
#define CS_HASH_ADD_ERR_SKB_CB			0xFFFFFFFD
cs_uint32 cs_kernel_core_add_hash(CS_HASH_ENTRY_S *p_hash, cs_uint8 vtable_id, 
		cs_uint64 *p_guid_array, cs_uint8 guid_cnt, cs_int16 timeout);

cs_uint32 cs_kernel_core_add_hash_by_skb(struct sk_buff *skb);

cs_status cs_kernel_get_hash_mask_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb, 
		cs_uint64 *p_mask);
cs_status cs_kernel_get_hash_key_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb, 
		fe_sw_hash_t *p_key);
cs_status cs_kernel_get_hash_action_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb, 
		fe_hash_rslt_s *p_action);
cs_status cs_kernel_input_set_cb(struct sk_buff *skb);
// Get hash value from the hash table for comparison
cs_uint32 cs_kernel_get_hash_val_by_index(cs_uint32 hash_idx, cs_uint64 *p_hash
	// return the hash info
);

/*
	Peripheral Manipulation Definition
	===============================================
*/

#define CS_MC_MCGID_EXHAUSTED	(-1)
#define CS_MC_MGIDX_EXHAUSTED	(-1)

/*
	Helper Functions
*/

// return the mcgid that just got allocated
cs_int32 cs_kernel_allocate_mcgid(void);
// return the idx that just got added
cs_int32 cs_kernel_add_mcidx(void);
// return the field for checking
cs_int32 cs_kernel_get_mcidx_field(void);
cs_int32 cs_kernel_allocate_sdb (void);
cs_int32 cs_kernel_allocate_sdb_tuple (void);
cs_int32 cs_kernel_allocate_classifier (void);

/*


	Support Definition
	===============================================
*/
/*

	Helper Functions
*/
void cs_kernel_util_init(void);
void cs_kernel_alloc_cb(void);
void cs_kernel_dalloc_cb(void);
void cs_kernel_hash_cb_init(void);

#endif
