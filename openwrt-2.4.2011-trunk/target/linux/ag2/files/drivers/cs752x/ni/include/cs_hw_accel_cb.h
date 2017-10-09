/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                CH Hsu <ch.hsu@cortina-systems.com>
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
#ifndef __CS_KERNEL_ACCEL_CB_H__
#define __CS_KERNEL_ACCEL_CB_H__

#include <mach/cs75xx_fe_core_table.h>

#define CS_KERNEL_SKB_CB(skb) (CS_KERNEL_ACCEL_CB_T *)&(skb->cb[48])

#define CS_DEFAULT_VOQ		255
#define CS_DEFAULT_ROOT_VOQ	47
#define CS_DEC_TTL_ENABLE	1
  
/* module_mask */
#define CS_MOD_MASK_BRIDGE              0x00000001
#define CS_MOD_MASK_PPPOE               0x00000002
#define CS_MOD_MASK_VLAN                0x00000004
#define CS_MOD_MASK_IPV4_ROUTING        0x00000008
#define CS_MOD_MASK_IPV4_PROTOCOL       0x00000010
#define CS_MOD_MASK_NAT                 0x00000020
#define CS_MOD_MASK_IPSEC               0x00000040
#define CS_MOD_MASK_QOS                 0x00000080
#define CS_MOD_MASK_IPV4_NETFILTER      0x00000100
#define CS_MOD_MASK_IPV4_MULTICAST      0x00000200
#define CS_MOD_MASK_IPV6_ROUTING        0x00000400
#define CS_MOD_MASK_IPV6_MULTICAST      0x00000800
#define CS_MOD_MASK_IPV6_NETFILTER      0x00001000
#define CS_MOD_MASK_TO_CPU      	0x00002000
#define CS_MOD_MASK_NAT_PREROUTE	0x00004000
#define CS_MOD_MASK_TO_VTABLE		0x00008000
// L2 rule?
#define CS_MOD_MASK_BRIDGE_NETFILTER	0x00010000
#define CS_MOD_MASK_L2_MULTICAST	0x00020000

/* guid[MODULE_GUID_NUM] */		
enum guid_array_def {
    CS_GUID_BRIDGE,
    CS_GUID_PPPOE,
    CS_GUID_VLAN,
    CS_GUID_IPV4_ROUTING,
    CS_GUID_NAT,
    CS_GUID_IPV4_IPSEC,
    CS_GUID_QOS,
    CS_GUID_IPV4_NETFILTER,
    CS_GUID_MULTICAST_1,
    CS_GUID_MULTICAST_2,
    CS_GUID_MULTICAST_3,
    CS_GUID_MULTICAST_4,
    MODULE_GUID_NUM,
};

/* protocol_attack */
#define CS_PROTOATT_ARP                 0x00000001
#define CS_PROTOATT_ICMPV4              0x00000002
#define CS_PROTOATT_IPV4_FRAGMENT       0x00000004
#define CS_PROTOATT_ICMPV6              0x00000008
#define CS_PROTOATT_TCP_SYN             0x00000010
#define CS_PROTOATT_TCP_FIN             0x00000020

/* sw_only */
#define CS_SWONLY_HW_ACCEL              0x01	/* Build a HW hash entry */
#define CS_SWONLY_DONTCARE              0x02	/* Don't build HW hash entry */
#define CS_SWONLY_STATE		 	0x04	/* Don't build HW hash entry */
#define CS_SWONLY_HW_ACCELL_DROP        0x08	/* What it means is that the hash 
						   needs to be build up but we do 
						   not need to transmit it.  
						   Only the bridge flooding code 
						   will take advantage of this value, 
						   other modules would not for now. */

#define CS_CB_TAG 	0x4C43 /* "CS" tag */
struct common_field {
	cs_uint16	tag;		/* 	CS_CB_TAG */
	cs_uint8 	sw_only;
	cs_uint8	vtype;		/* vtable type */
	cs_uint32	module_mask;	/* NAT, bridge,VLAN */
#if 0
	cs_uint64	guid[MODULE_GUID_NUM];
	cs_uint8	guid_cnt;	/* Each module add guid[MODULE_GUID_NUM]
					   need add this counter */
#endif
	struct net_device *input_dev;	/* input virtual ethernet interface  */
	/* FIXME:struct net_device *dev for readable */
	struct net_device *output_dev;	/* output virtual ethernet interface */
	cs_uint32	sw_action_id;
	cs_uint8	ingress_port_id;
	cs_uint8	egress_port_id;

	cs_uint8	protocol_attack;
     	cs_uint8	mask_ptr;	/* mask ram pointer */
	cs_uint8	dec_ttl;
//	cs_uint8	pspid;		/* physical port id */
	cs_uint8 	uu_flow_enable;
	cs_uint8	state;		/* tcp connection state */
};

struct l2_mac {
	cs_uint8	sa[6];
	cs_uint8	da[6];
	cs_uint16	protocol;		/* vlan: 0x8100, pppoe: 0x8864 */
	cs_uint16	vlan_id; 		/* priority, dei/cfi, vlan id */
	cs_uint16	pppoe_frame;
	cs_uint16	eth_protocol; 	
	cs_uint8	l2_flood;
};

struct ip_hdr {
	cs_uint8	ver;
	cs_uint8	tos;		/* DSCP[7:2] + ECN[1:0]: Type of service */
	cs_uint8	protocol;	/* IP protocol, AH,ESP ... */
	cs_uint8	frag;		/* Not fragmented: 0; Fragmented: 1 */
	cs_uint32	sip;		/* source IP address */
	cs_uint32	dip;		/* destination IP address */
};

struct ipv6_hdr {
	cs_uint8	ver;
	cs_uint8	ds_byte;
	cs_uint8	flow_lbl[3];
	cs_uint16	payload_len;
	cs_uint8	nexthdr;
	cs_uint32	sip[4];
	cs_uint32	dip[4];
};

struct arp_hdr
{
	cs_uint8	ar_sha[6];	/* sender hardware address	*/
	cs_uint8	ar_sip[4];	/* sender IP address		*/
	cs_uint8	ar_tha[6];	/* target hardware address	*/
	cs_uint8	ar_tip[4];	/* target IP address		*/
};

struct tcp_hdr {
	cs_uint16	sport;//FIXME: remove later,CH
	cs_uint16	dport;//FIXME: remove later,CH
	cs_uint16	sport_start;
	cs_uint16	sport_end;
	cs_uint16	dport_start;
	cs_uint16	dport_end;
	cs_uint16	res1:4,
			doff:4,
			fin:1,
			syn:1,
			rst:1,
			psh:1,
			ack:1,
			urg:1,
			ece:1,
			cwr:1;
};

struct udp_hdr {
	cs_uint16	sport;//FIXME: remove later,CH
	cs_uint16	dport;//FIXME: remove later,CH
	cs_uint16	sport_start;
	cs_uint16	sport_end;
	cs_uint16	dport_start;
	cs_uint16	dport_end;
};

struct igmp_hdr
{
	cs_uint32 group_id;
};

struct ip_ah_esp_hdr {
	cs_uint32 spi;
};

/* flow_vlan_op_en */
enum cs_vlan_operation {
	CS_VLAN_OP_NO_ENABLE,
	CS_VLAN_OP_INSERT,
	CS_VLAN_OP_REMOVE,
	CS_VLAN_OP_KEEP,
	CS_VLAN_OP_REPLACE,
};

/* flow_vlan_prio_op_en */
enum cs_vlan_prio_operation {
	CS_VLAN_PRIO_OP_NO_ENABLE,
	CS_VLAN_PRIO_OP_UPDATE_PRIO,
	CS_VLAN_PRIO_OP_UPDATE_DEI,
	CS_VLAN_PRIO_OP_UPDATE_PRIO_DEI,
};

/* pppoe_op_en */
enum cs_pppoe_operation {
	CS_PPPOE_OP_NO_ENABLE,
	CS_PPPOE_OP_INSERT,
	CS_PPPOE_OP_REMOVE,
	CS_PPPOE_OP_KEEP,
	CS_PPPOE_OP_REPLACE,
};

typedef struct cb_result_l2 {
	cs_uint8	mac_sa_replace_en;
	cs_uint8	mac_da_replace_en;
	cs_uint8	flow_vlan_op_en;	
	cs_uint8	flow_vlan_prio_op_en;
	cs_uint8	pppoe_op_en;	
} cb_result_l2_s;

typedef struct cb_result_l3 {
	cs_uint8	ip_sa_replace_en;
	cs_uint8	ip_da_replace_en;
	cs_uint8	decr_ttl_hoplimit;
} cb_result_l3_s;

typedef struct cb_result_l4 {
	cs_uint8	sp_replace_en;
	cs_uint8	dp_replace_en;
} cb_result_l4_s;

typedef struct cb_result_dest {
	cs_uint8	d_voq_id;
	cs_uint8	d_pol_id;
	cs_uint8	cpu_pid;		/* CPU sub port ID. */
	cs_uint8	ldpid;			/* logical destination port that voq maps to */
	cs_uint16	pppoe_session_id;	/* pppoe session ID to be encaped */
	cs_uint8	cos_nop;
	cs_boolean	voq_policer_parity;
} cb_result_voq_pol_s;

typedef struct cb_result_act {
	cs_uint8	drop;
	cs_uint16	mcgid;
	cs_uint8	mcgid_vaild;
} cb_result_misc_s;

typedef struct cb_result_entry {
	cb_result_l2_s	l2;
	cb_result_l3_s	l3;
	cb_result_l4_s	l4;
	cb_result_voq_pol_s	voq_pol;
	cb_result_misc_s misc;
	cs_boolean	acl_dsbl;
} cb_hash_result_s;

typedef struct cb_key_misc {
	cs_uint8	sdbid;
	cs_uint8	lspid;
	cs_uint8	mc_da;
	cs_uint8	bc_da;	
	cs_uint16	mcgid;
	cs_uint16	mcidx;
	cs_uint8	hw_fwd_type;
	cs_uint8	da_an_mac_sel;
	cs_uint8	da_an_mac_hit;
	cs_uint8	sa_bng_mac_sel;
	cs_uint8	sa_bng_mac_hit;
	cs_uint8	orig_lspid;
	cs_uint32	recirc_idx;
	cs_uint32	l7_field;
	cs_uint8	l7_field_valid;
	cs_uint8	mask_ptr_0_7;
	cs_uint8	hdr_a_flags_crcerr;
	cs_uint8	l3_csum_err;
	cs_uint8	l4_csum_err;
	cs_uint8	not_hdr_a_flags_stsvld;
	cs_uint8	hash_fid;
	cs_uint8	spi_vld;
	cs_uint32	spi_idx;
	cs_uint8	ipv6_ndp;
	cs_uint8	ipv6_hbh;
	cs_uint8	ipv6_rh;
	cs_uint8	ipv6_doh;
	cs_uint16	pkt_len_low;
	cs_uint16	pkt_len_high;
	cs_uint8	vif;
} cb_key_misc_s;

struct network_field {
	struct l2_mac 	raw;

	union {
		struct ip_hdr	iph;
		struct ipv6_hdr	ipv6h;
		struct arp_hdr	arph;
	} l3_nh;

	union {
		struct tcp_hdr	th;
		struct udp_hdr	uh;
		struct igmp_hdr	igmph;
		struct ip_ah_esp_hdr ah_esp;
	} l4_h;
};

struct hash_timeout_t {
	cs_uint16 counter;
	cs_int16 interval;
};
//input_mask & output_mask
#define	CS_HM_MAC_DA_MASK 			0x0000000000000001LL
#define CS_HM_MAC_SA_MASK 			0x0000000000000002LL
#define CS_HM_ETHERTYPE_MASK 			0x0000000000000004LL
#define	CS_HM_LLC_TYPE_ENC_MSB_MASK 		0x0000000000000008LL
#define	CS_HM_LLC_TYPE_ENC_LSB_MASK 		0x0000000000000010LL
#define	CS_HM_TPID_ENC_1_MSB_MASK 		0x0000000000000020LL
#define	CS_HM_TPID_ENC_1_LSB_MASK 		0x0000000000000040LL
#define	CS_HM_8021P_1_MASK 			0x0000000000000080LL
#define	CS_HM_DEI_1_MASK 			0x0000000000000100LL
#define	CS_HM_VID_1_MASK 			0x0000000000000200LL
#define	CS_HM_TPID_ENC_2_MSB_MASK 		0x0000000000000400LL
#define	CS_HM_TPID_ENC_2_LSB_MASK 		0x0000000000000800LL
#define	CS_HM_8021P_2_MASK 			0x0000000000001000LL
#define	CS_HM_DEI_2_MASK 			0x0000000000002000LL
#define	CS_HM_VID_2_MASK 			0x0000000000004000LL
#define	CS_HM_IP_DA_MASK 			0x0000000000008000LL
#define	CS_HM_IP_SA_MASK 			0x0000000000010000LL
#define	CS_HM_IP_PROT_MASK 			0x0000000000020000LL
#define	CS_HM_DSCP_MASK 			0x0000000000040000LL
#define	CS_HM_ECN_MASK 				0x0000000000080000LL
#define	CS_HM_IP_FRAGMENT_MASK 			0x0000000000100000LL
#define	CS_HM_KEYGEN_POLY_SEL 			0x0000000000200000LL
#define	CS_HM_IPV6_FLOW_LBL_MASK 		0x0000000000400000LL
#define	CS_HM_IP_VER_MASK 			0x0000000000800000LL
#define	CS_HM_IP_VLD_MASK 			0x0000000001000000LL
#define	CS_HM_L4_PORTS_RNGD 			0x0000000002000000LL
#define	CS_HM_L4_DP_MASK 			0x0000000004000000LL
#define	CS_HM_L4_SP_MASK 			0x0000000008000000LL
#define	CS_HM_TCP_CTRL_MASK 			0x0000000010000000LL
#define	CS_HM_TCP_ECN_MASK 			0x0000000020000000LL
#define	CS_HM_L4_VLD_MASK 			0x0000000040000000LL
#define	CS_HM_LSPID_MASK 			0x0000000080000000LL
#define	CS_HM_FWDTYPE_MASK 			0x0000000100000000LL
#define	CS_HM_PPPOE_SESSION_ID_VLD_MASK 	0x0000000200000000LL
#define	CS_HM_PPPOE_SESSION_ID_MASK 		0x0000000400000000LL
//#define	CS_HM_RSVD_109 			0x8000000000000000LL
#define	CS_HM_RECIRC_IDX_MASK			0x0000000800000000LL
#define	CS_HM_MCIDX_MASK 			0x0000001000000000LL
#define	CS_HM_MC_DA_MASK 			0x0000002000000000LL
#define	CS_HM_BC_DA_MASK 			0x0000004000000000LL
#define	CS_HM_DA_AN_MAC_SEL_MASK 		0x0000008000000000LL
#define	CS_HM_DA_AN_MAC_HIT_MASK 		0x0000010000000000LL
#define	CS_HM_ORIG_LSPID_MASK 			0x0000020000000000LL
#define	CS_HM_L7_FIELD_MASK 			0x0000040000000000LL
#define	CS_HM_L7_FIELD_VLD_MASK 		0x0000080000000000LL
#define	CS_HM_HDR_A_FLAGS_CRCERR_MASK 		0x0000100000000000LL
#define	CS_HM_L3_CHKSUM_ERR_MASK 		0x0000200000000000LL
#define	CS_HM_L4_CHKSUM_ERR_MASK 		0x0000400000000000LL
#define	CS_HM_NOT_HDR_A_FLAGS_STSVLD_MASK 	0x0000800000000000LL
#define	CS_HM_HASH_FID_MASK 			0x0001000000000000LL
#define	CS_HM_L7_FIELD_SEL 			0x0002000000000000LL
#define	CS_HM_SA_BNG_MAC_SEL_MASK 		0x0004000000000000LL
#define	CS_HM_SA_BNG_MAC_HIT_MASK 		0x0008000000000000LL
#define	CS_HM_SPI_VLD_MASK 			0x0010000000000000LL
#define	CS_HM_SPI_MASK 				0x0020000000000000LL
#define	CS_HM_IPV6_NDP_MASK 			0x0040000000000000LL
#define	CS_HM_IPV6_HBH_MASK 			0x0080000000000000LL
#define	CS_HM_IPV6_RH_MASK 			0x0100000000000000LL
#define	CS_HM_IPV6_DOH_MASK 			0x0200000000000000LL
#define	CS_HM_PPP_PROTOCOL_VLD_MASK 		0x0400000000000000LL
#define	CS_HM_PPP_PROTOCOL_MASK 		0x0800000000000000LL
#define	CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK 	0x1000000000000000LL
#define	CS_HM_MCGID_MASK 			0x2000000000000000LL
#define	CS_HM_VTABLE_MASK 			0x4000000000000000LL //for vtable use only
#define CS_HM_IPV6_MASK				0x8000000000000000LL

#define CS_HM_QOS_TUPLE_MASK	(CS_HM_8021P_1_MASK | CS_HM_DEI_1_MASK | \
				CS_HM_8021P_2_MASK  | CS_HM_DEI_2_MASK | \
				CS_HM_DSCP_MASK     | CS_HM_ECN_MASK)

/* for cs_cb->input_tcp_flag_mask */
#define CS_HASH_MASK_TCP_URG_FLAG       0x00008000
#define CS_HASH_MASK_TCP_ACK_FLAG       0x00010000
#define CS_HASH_MASK_TCP_PSH_FLAG       0x00020000
#define CS_HASH_MASK_TCP_RST_FLAG       0x00040000
#define CS_HASH_MASK_TCP_SYN_FLAG       0x00080000
#define CS_HASH_MASK_TCP_FIN_FLAG       0x00100000

typedef struct {
	struct common_field common;
	cs_uint64 input_mask;	
	cs_uint32 input_tcp_flag_mask;	
	struct network_field input;
	cs_uint64 output_mask;		
	struct network_field output;
	cb_key_misc_s key_misc;
	cb_hash_result_s action;
	struct hash_timeout_t tmo;

	HEADER_E_T	*hdrE;
	CPU_HEADER0_T	*cpu_hdr0;
	CPU_HEADER1_T	*cpu_hdr1;
} CS_KERNEL_ACCEL_CB_T;


#define CS_RESULT_ACTION_ENABLE	1

#endif	/* __CS_KERNEL_ACCEL_CB_H__ */
