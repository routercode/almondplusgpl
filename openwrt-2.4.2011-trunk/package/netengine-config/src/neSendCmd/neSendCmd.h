/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : neSendCmd.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility neSendCmd 
                 header file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef _NE_SEND_CMD__H_
#define _NE_SEND_CMD__H_

#ifndef bool
#define bool int
#endif

#include <cs_types.h>
#include <cs_tunnel.h>
#include <linux/cs_ne_ioctl.h>
#include <cs75xx_fe_core_table.h>
#include "ne_defs.h"

/*
 * Classifier Table
 */
typedef struct _CLASSIFIER_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_class_entry_t tbl_Classifier;
} CLASSIFIER_INFO;

/*
 * SDB Table
 */
typedef struct _SDB_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_sdb_entry_t tbl_SDB;
} SDB_INFO;

/*
 * Hash Mask Table
 */
typedef struct _HASH_MASK_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_hash_mask_entry_t tbl_HashMask;
} HASH_MASK_INFO;

/*
 * LPM Table
 */
typedef struct _LPM_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_lpm_entry_t tbl_LPM;
} LPM_INFO;

/*
 * Hash Match Table
 */
typedef struct _HASH_MATCH_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_hash_hash_entry_t tbl_HashMatch;
} HASH_MATCH_INFO;

/*
 * FWDRSLT Table
 */
typedef struct _FWDRSLT_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_fwd_result_entry_t tbl_FWDRSLT;
} FWDRSLT_INFO;

/*
 * QOSRSLT Table
 */
typedef struct _QOSRSLT_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_qos_result_entry_t tbl_QOSRSLT;
} QOSRSLT_INFO;

/*
 * L3 IP Table
 */
typedef struct _L3_IP_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_l3_addr_entry_t tbl_L3IP;
} L3_IP_INFO;

/*
 * L2 MAC Table
 */
typedef struct _L2_MAC_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_l2_addr_pair_entry_t tbl_L2MAC;
} L2_MAC_INFO;

/*
 * VoQ Policer Table
 */
typedef struct _VOQ_POLICER_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_voq_pol_entry_t tbl_VoQPolicer;
} VOQ_POLICER_INFO;

/*
 * LPB Table
 */
typedef struct _LPB_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_lpb_entry_t tbl_LPB;
} LPB_INFO;

/*
 * AN BNG MAC Table
 */
typedef struct _AN_BNG_MAC_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_an_bng_mac_entry_t tbl_ANBNGMAC;
} AN_BNG_MAC_INFO;

/*
 * Port Range Table
 */
typedef struct _PORT_RANGE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_port_range_entry_t tbl_PortRange;
} PORT_RANGE_INFO;

/*
 * VLAN Table
 */
typedef struct _VLAN_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_vlan_entry_t tbl_VLAN;
} VLAN_INFO;

/*
 * ACL Rule Table
 */
typedef struct _ACL_RULE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_acl_rule_entry_t tbl_ACLRule;
} ACL_RULE_INFO;

/*
 * ACL Action Table
 */
typedef struct _ACL_ACTION_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_acl_action_entry_t tbl_ACLAction;
} ACL_ACTION_INFO;

/*
 * ACL Table
 */
typedef struct _ACL_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_acl_entry_t tbl_ACL;
} ACL_INFO;

/*
 * PE VOQ DROP Table
 */
typedef struct _PE_VOQ_DROP_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_pe_voq_drp_entry_t tbl_PEVOQDROP;
} PE_VOQ_DROP_INFO;

/*
 * ETYPE Table
 */
typedef struct _ETYPE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_eth_type_entry_t tbl_ETYPE;
} ETYPE_INFO;

/*
 * LLC HDR Table
 */
typedef struct _LLC_HDR_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_llc_hdr_entry_t tbl_LLCHDR;
} LLC_HDR_INFO;

/*
 * FVLAN Table
 */
typedef struct _FVLAN_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_flow_vlan_entry_t tbl_FVLAN;
} FVLAN_INFO;

/*
 * HASH HASH Table
 */
typedef struct _HASH_HASH_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	cs_uint32 crc32;
	cs_uint16 crc16;
} HASH_HASH_INFO;

/*
 * HASH Check Table
 */
typedef struct _HASH_CHECK_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_hash_check_entry_t tbl_HashCheck;
} HASH_CHECK_INFO;

/*
 * PKTLEN Range Table
 */
typedef struct _PKTLEN_RANGE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_pktlen_rngs_entry_t tbl_PktlenRange;
} PKTLEN_RANGE_INFO;

/*
  *[ingress qos]--add by ethan for ingress qos
 * Ingress QOS Hash Table 
 */
typedef struct _INGRESS_QOS_TABLE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_ingress_qos_table_entry_t tbl_QOS_TABLE;
} INGRESS_QOS_TABLE_INFO;

/*
  *[ingress qos]--add by ethan for ingress qos
 * Ingress QOS SHAPER Table 
 */
typedef struct _INGRESS_QOS_SHAPER_VOQ_TABLE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_ingress_qos_shaper_voq_entry_t tbl_SHAPER_VOQTABLE;
} INGRESS_QOS_SHAPER_VOQ_TABLE_INFO;

/*
  *[ingress qos]--add by ethan for ingress qos
 * Ingress QOS SHAPER Table 
 */
typedef struct _INGRESS_QOS_SHAPER_PORT_TABLE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	fe_ingress_qos_shaper_port_entry_t tbl_SHAPER_PORTTABLE;
} INGRESS_QOS_SHAPER_PORT_TABLE_INFO;

/*
 * Ingress QOS API Table 
 */
typedef struct _INGRESS_QOS_API_TABLE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	cs_qos_ingress_api_entry_t tbl_inqos_api;
} INGRESS_QOS_API_TABLE_INFO;

typedef struct _INGRESS_QOS_SUB_CMD_T_ {
	char *name;
	int id;
} INGRESS_QOS_SUB_CMD_T;

/* VOQ Counter API table */
typedef struct _VOQ_COUNTER_API_TABLE_INFO_ {
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T cmd_Hdr;
	cs_voq_counter_api_entry_t api;
} VOQ_COUNTER_API_TABLE_INFO;

typedef struct {
	char *name;
	int id;
} VOQ_COUNTER_SUB_CMD_T;

typedef struct _NE_FIELD_T_ {
	char *name;
	int (*action) ();
} NE_FIELD_T;

typedef struct _TUNNEL_IPLIP_TABLE__INFO_ {
        NECMD_HDR_T ne_hdr;
        NEFE_CMD_HDR_T cmd_Hdr;
        cs_tunnel_iplip_api_entry_t api;
} TUNNEL_IPLIP_API_TABLE_INFO;

typedef struct {
        char *name;
        int id;
} TUNNEL_IPLIP_SUB_CMD_T;

extern NE_FIELD_T fe_tbl_Classifier[];
extern NE_FIELD_T fe_tbl_SDB[];
extern NE_FIELD_T fe_tbl_Hash_Mask[];
extern NE_FIELD_T fe_tbl_LPM[];
extern NE_FIELD_T fe_tbl_Hash_Match[];
extern NE_FIELD_T fe_tbl_FWDRSLT[];
extern NE_FIELD_T fe_tbl_QOSRSLT[];
extern NE_FIELD_T fe_tbl_L3_IP[];
extern NE_FIELD_T fe_tbl_L2_MAC[];
extern NE_FIELD_T fe_tbl_VoQ_Policer[];
extern NE_FIELD_T fe_tbl_LPB[];
extern NE_FIELD_T fe_tbl_AN_BNG_MAC[];
extern NE_FIELD_T fe_tbl_Port_Range[];
extern NE_FIELD_T fe_tbl_VLAN[];
extern NE_FIELD_T fe_tbl_ACL[];
extern NE_FIELD_T fe_tbl_PE_VOQ_DROP[];
extern NE_FIELD_T fe_tbl_ETYPE[];
extern NE_FIELD_T fe_tbl_LLC_HDR[];
extern NE_FIELD_T fe_tbl_FVLAN[];
extern NE_FIELD_T fe_tbl_HASH_HASH[];
extern NE_FIELD_T fe_tbl_Hash_Check[];
extern NE_FIELD_T fe_tbl_Pktlen_Range[];
/*
 * [begin][ingress qos]--add by ethan for ingress qos
 */
extern NE_FIELD_T fe_tbl_QOS_TABLE[];
extern NE_FIELD_T fe_tbl_QOS_SHAPER_VOQ[];
extern NE_FIELD_T fe_tbl_QOS_SHAPER_PORT[];
/*
 * [end][ingress qos]--add by ethan for ingress qos
 */
extern NE_FIELD_T inqos_qos_api[];
extern NE_FIELD_T voqcntr_api[];
extern NE_FIELD_T tunnel_iplip_api[];
#endif /* _NE_SEND_CMD__H_ */
