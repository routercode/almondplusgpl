/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *				CH Hsu <ch.hsu@cortina-systems.com>
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
#ifndef __FILE_PROCESS_H__
#define __FILE_PROCESS_H__

#include "cs_types.h"

#define EOF_KEYWORD				"End of Frame"
#define FRAME_LENGTH_KEYWORD	"Frame Length"

#define MAX_JUMBO_PKT_SIZE			(16 * 1024)
#define FILENAME_SIZE				255
#define OUTPUT_FILENAME_NAME		"packet%04d.txt"
#define MAX_OUTPUT_FILE_NUM			10000

#define htons(x)			(u16) ((((u16)(x) >> 8) & 0xff) | (((u16)(x) << 8) & 0xff00))
#define PUSH_WORD(cp, data)	{*cp++ = (((u16)(data)) >> 8) & 0xff; 	\
							 *cp++ = ((u16)(data)) & 0xff;} 
#define PUSH_DWORD(cp, data) {*cp++ = (u8)(((u32)(data)) >> 24) & 0xff;	\
							  *cp++ = (u8)(((u32)(data)) >> 16) & 0xff;	\
							  *cp++ = (u8)(((u32)(data)) >> 8) & 0xff;	\
							  *cp++ = (u8)((u32)(data)) & 0xff;}
#define PUSH_DWORD_V(cp, data) {*cp++ = (u8)(((u32)(data)) >> 24) & 0x0f;	\
							  *cp++ = (u8)(((u32)(data)) >> 16) & 0xff;	\
							  *cp++ = (u8)(((u32)(data)) >> 8) & 0xff;	\
							  *cp++ = (u8)((u32)(data)) & 0xff;}							  
#define PUSH_BYTE(cp, data)	{*cp++ = ((u8)(data)) & 0xff;}

#define SWAP_WORD(x)	(unsigned short)((((unsigned short)x & 0x00FF) << 8) |	\
										 (((unsigned short)x & 0xFF00) >> 8))
#define SWAP_DWORD(x)	(unsigned long)((((unsigned long)x & 0x000000FF) << 24) |	\
										(((unsigned long)x & 0x0000FF00) << 8) |	\
										(((unsigned long)x & 0x00FF0000) >> 8) |	\
										 (((unsigned long)x & 0xFF000000) >> 24))

#define IS_RANDOM(cp)			(stricmp(cp, "random") == 0 ? 1 : 0)
#define IS_INCREMENTAL(cp)		(stricmp(cp, "INC") == 0 ? 1 : 0)

/*----------------------------------------------------------------------
 * Stuff for writing a PCap file
 ----------------------------------------------------------------------*/
#define	CORTINA_MAGIC_NUMBER	0x43534D47
#define CORTINA_MAGIC		    "Cortina"	// "Cortina Packet Description File"

/*----------------------------------------------------------------------
 * "libpcap" file header.
 ----------------------------------------------------------------------*/
struct pcap_hdr {
	uint32	magic;			/* magic */
	uint16	version_major;	/* major version number */
	uint16	version_minor;	/* minor version number */
	uint32	thiszone;		/* GMT to local correction */
	uint32	sigfigs;		/* accuracy of timestamps */
	uint32	snaplen;		/* max length of captured packets, in octets */
	uint32	network;		/* data link type */
};

/*----------------------------------------------------------------------
 *  "libpcap" record header.
 ----------------------------------------------------------------------*/
struct pcaprec_hdr {
	int32	ts_sec;		/* timestamp seconds */
	uint32	ts_usec;	/* timestamp microseconds */
	uint32	incl_len;	/* number of octets of packet saved in file */
	uint32	orig_len;	/* actual length of packet */
};

/*----------------------------------------------------------------------
 *  keyword table
 ----------------------------------------------------------------------*/
typedef struct _keyword_t {
	char		*name; 						// command name
    int	      	(*handler)(int, char **);	// handler routine
} KEYWORD_T;

/*----------------------------------------------------------------------
 *  Description file data
 ----------------------------------------------------------------------*/
typedef struct {
	int				begin;				// start to parse packet
	int				length; 			// total packet length
	int				number;				// total packet number
	int				parse_level;		// parse level
	int				have_L2;			// has layer 2
	int				have_L3;			// has layer 3
	int				have_L4;			// has layer 4
	int				have_L5;			// has layer 5
	int				have_Payload;		// has payload
	int				have_hash_entry;	// has hash entry
	int				pkt_type;			// packet type
	unsigned char	*iphdr;				// points to IP header
	unsigned char	*L4hdr;				// points to L4 (UDP/TCP) header
	unsigned char	*L5hdr;				// points to L5 IEEE 1588 header
	unsigned char	*Payload_hdr;		// points to Payload header
	char			number_random;		// total packet number
	char			length_random;		// packet length is random 
	char			length_incremental;		// packet length is incremental 
	int				length_inc_start;		// start of incremental packet length
	int				length_inc_step;		// stepping of incremental packet length
	int				swtxq;
	int				txdesc;
	int				txflag;
	int				txmtu;
	int				is_tx_pkt;
	int				length_random_min;	// 2006/6/7 07:49PM
	int				length_random_max;	// 2006/6/7 07:49PM
} PACKET_INFO_T;

typedef struct
{
	unsigned short	priority_sp : 3;
	unsigned short	cif_sp : 1;
	unsigned short	outer_vlan_id : 12;
	unsigned short	priority_ce : 3;
	unsigned short	cif_ce : 1;
	unsigned short	inner_vlan_id : 12;
	unsigned short	priority : 3;
	unsigned short	cif : 1;
	unsigned short	vlan_id : 12;
} PKT_L2_T_VLAN;

#ifdef AG_802_1
typedef struct
{
	unsigned char	md_level : 3;
	unsigned char	version : 5;
} PKT_L2_T_AG;
#endif


#define LLC_DATA_BYTES		3
#define SNAP_DATA_BYTES		3
typedef struct {
	int				type;
	unsigned short	pause_timer;
	unsigned char	pause_timer_random;
	char			have_da;
	char			da_random;
	char			da_incremental;
	int				da_inc_start;
	int				da_inc_step;
	unsigned char	da[6];
	char			have_sa;
	char			sa_random;
	char			sa_incremental;
	char			sa_inc_start;
	int				sa_inc_step;		
	unsigned char	sa[6];
	char			have_priority_sp;
	char			priority_random_sp;
	char			priority_sp_incremental;		// Priority SP is incremental 
	int				priority_sp_inc_start;			// start of incremental Priority SP
	int				priority_sp_inc_step;			// stepping of incremental Priority SP	
	int				p_sp_counter;			
	char			have_cif_sp;
	char			have_outer_vlan_id;
	char			have_priority_ce;
	char			priority_random_ce;
	char			priority_ce_incremental;		// Priority CE is incremental 
	int				priority_ce_inc_start;			// start of incremental Priority CE
	int				priority_ce_inc_step;			// stepping of incremental Priority CE		
	int				p_ce_counter;			
	char			have_cif_ce;
	char			outer_vlan_id_random;		
	char			have_inner_vlan_id;
	char			inner_vlan_id_random;		
	char			have_priority;
	char			priority_random;
	char			priority_incremental;		// Priority is incremental 
	int				priority_inc_start;			// start of incremental Priority 
	int				priority_inc_step;			// stepping of incremental Priority 
	int				p_counter;				
	char			have_cif;
	char			have_vlan_id;
	char			vlan_id_random;		
	PKT_L2_T_VLAN	vlan;
	char			have_ether_type;
#ifdef AG_802_1	
	PKT_L2_T_AG		ag;
	char			have_md_level;
	char			md_level_random;
	char			md_level_incremental;
	int				md_level_inc_start;			
	int				md_level_inc_step;
	int				md_level_counter;			
	char			have_verison;
	char			version_random;
	unsigned char	opcode;
	unsigned char	opcode_random;
	char			have_opcode;
	unsigned char	flag;
	unsigned char	flag_random;
	char			have_flag;
	unsigned char	first_tlv;
	unsigned char	first_tlv_random;
	char			have_first_tlv;
	unsigned long	transaction_id;
	unsigned char	transaction_id_random;
	char			have_transaction_id;
	char			transaction_id_incremental;
	int				transaction_id_inc_start;			
	int				transaction_id_inc_step;
	int				transaction_id_counter;	
#endif		
	char			ether_type_random;
	unsigned short	ether_type;
	char			have_sub_type;
	char			sub_type_random;
	unsigned char	sub_type;
	char			have_pppoe;
	unsigned char	pppoe_ver_type;		// version and type
	unsigned char	pppoe_code;			// code
	unsigned short	pppoe_sid;			// session id
	char			pppoe_random;
	char			ppp_random;
	char			have_ppp;
	unsigned short	ppp_protocol;
	char			have_llc;
	char			llc[LLC_DATA_BYTES];
	char			llc_random;
	char			snap_random;
	char			have_snap;
	char			snap[SNAP_DATA_BYTES];
} PKT_L2_T;



#define IP_MAX_OPTIONS			40
#define MAX_EXT_HEADER_NUM		32
typedef struct {
	int				type;	// IPv4, IPv6
	unsigned char	sip[16];
	unsigned char	dip[16];
	unsigned char	tos;
	unsigned char	ttl;
	unsigned char	protocol;
	unsigned char	header_len;
	unsigned char	ver_type;
	unsigned short 	id;
	unsigned short	fragment;
	unsigned char	options[IP_MAX_OPTIONS];
	unsigned char	option_len;
	unsigned int	traffic_class;
	unsigned long	flow_lable;
	unsigned int	hop_limit;
	unsigned int	next_header;
	unsigned int    ext_header_num;
	unsigned int    ext_header_idx;
	unsigned int    ext_header_begin;
	unsigned int    ext_header_len[MAX_EXT_HEADER_NUM];
	unsigned int    ext_header_proto[MAX_EXT_HEADER_NUM];
	unsigned char	chksum;		// 1: yes, 0: no
	unsigned char	*chksum_ptr;
	unsigned char	*protocol_ptr;
	unsigned char   have_protocol;
	unsigned char	sip_random;
	unsigned char	dip_random;
	unsigned char	tos_random;
	unsigned char	ttl_random;
	unsigned char	id_random;
	unsigned char	option_random;
	unsigned char	protocol_random;
	unsigned char	traffic_class_random;
	unsigned char	flow_lable_random;
	unsigned char	hop_limit_random;
} PKT_L3_T;

#define TCP_MAX_OPTIONS	40
typedef struct {
	int				protocol;
	unsigned short	sport;
	unsigned short	dport;
	unsigned long	seq;
	unsigned long	ack;
	unsigned char	hdr_len;
	unsigned char   th_off;
	unsigned char	URG_Flag;
	unsigned char	ACK_Flag;
	unsigned char	PUSH_Flag;
	unsigned char	RST_Flag;
	unsigned char	SYN_Flag;
	unsigned char	FIN_Flag;
	unsigned char	th_flags;
	unsigned short	window_size;
	unsigned short	chksum;
	unsigned short	urgent_pointer;
	unsigned char	options[TCP_MAX_OPTIONS];
	unsigned char	option_len;
	unsigned char	have_seq;		// GRE
	unsigned char	have_ack;		// GRE
	unsigned short	call_id;		// GRE
	unsigned long	spi;			// AH, ESP
	unsigned char	*payload_ptr;
	unsigned int	payload_size;
	unsigned char	*chksum_ptr;
	unsigned char 	sport_random;
	unsigned char 	dport_random;
	unsigned char 	seq_random;
	unsigned char 	ack_random;
	unsigned char 	win_random;
	unsigned char 	call_id_random;
	unsigned char 	spi_random;
	unsigned char	seq_first;				// 1: if first seq
	unsigned char	ack_first;				// 1: if first ack
	char			seq_incremental;		// data is incremental 
	char			ack_incremental;		// data is incremental 
	unsigned long	ack_step;				// stepping of incremental data
} PKT_L4_T;

typedef struct {
	int				type;		//IEEE 1588 start
	unsigned short	v_ptp;			
	unsigned short	v_network;
	unsigned char	subdomain[16];
	unsigned char	messageType;
	unsigned char	sourceCT;
	unsigned char 	sourceUuid[6];
	unsigned short	sourcePortId;
	unsigned short 	sequenceId;
	unsigned char	control;
	unsigned char	reserved_1;//must be 0
	unsigned char	flags[2];
	unsigned char	reserved[4];  //IEEE 1588 end
	unsigned char 	v_ptp_random;
	unsigned char 	v_network_random;
	unsigned char 	subdomain_random;
	unsigned char 	messageType_random;
	unsigned char 	sourceCT_random;
	unsigned char 	sourceUuid_random;
	unsigned char 	sourcePortId_random;
	unsigned char 	sequenceId_random;
	unsigned char 	control_random;
	unsigned char 	flags_random;
} PKT_L5_T;

typedef struct {
	unsigned char	start_data;
	unsigned char	step;
	unsigned char	src_pid;
	unsigned char	dest_pid;
	unsigned char	qid;
	unsigned char	multicast;
	char			have_src_pid;
	char			have_dest_pid;
	char			have_qid;
	char			have_multicast;
	unsigned long	tatal_bytes;	// not including header
} PAYLOAD_HDR_T;

typedef struct {
	int				type;			// 0: Random, 1: Sequential data
	unsigned short	force_chksum;	// 
	PAYLOAD_HDR_T	header;
} PKT_PAYLOAD_T;

#define PKT_PAYLOAD_RANDOM		0
#define PKT_PAYLOAD_SEQUENTIAL	1
#define PKT_PAYLOAD_CHKSUM		2

#define MAX_ARGUMENTS	        32
#define MAX_CHARS		        255
#define HEX_RECEIVE_FILENAME    "cspacket_rev.hex"
#define PAYLOAD_INDEX_DST_PORT_OFFSET       48
#define PAYLOAD_CS_MAGIC_OFFSET             52
#define PAYLOAD_SEQUENCE_NUMBER_OFFSET      56
#define PAYLOAD_TAG                         "0000000000000000000000\n"

typedef enum
{
	RS_READ_PACKET_CONTENT,
	RS_READ_PACKET_LENGTH_HIGH,
	RS_READ_PACKET_LENGTH_LOW,
	RS_READ_PACKET_SEQUENCE_B1,
	RS_READ_PACKET_SEQUENCE_B2,
	RS_READ_PACKET_SEQUENCE_B3,
	RS_READ_PACKET_SEQUENCE_B4,
	RS_READ_PACKET_TAG,
}READ_PACKET_STATE;

typedef struct {
	int				argc;						// number of arguments
	unsigned char	*argv[MAX_ARGUMENTS];		// points arguments
	unsigned char	buf[MAX_CHARS+1];			// input buffer
	KEYWORD_T		*curr_table;
	KEYWORD_T		*parent_table;
	FILE			*hexrev_fp;				// file pointer
	unsigned char	saveHexRcvFile;
	int				pktnum;
	PACKET_INFO_T	pkt;
	PKT_L2_T		L2;
	PKT_L3_T		L3;
	PKT_L4_T		L4;
	PKT_L5_T		L5;
	PKT_PAYLOAD_T	Payload;
	unsigned long	prev_payload_size;
	unsigned long   SequenceNum;
	unsigned long   SequenceNumBackup;
} DESCRIPTION_INFO_T;

#define PARSER_LEVEL_1		0
#define PARSER_LEVEL_2		1
#define PARSER_LEVEL_3		2

#define PACKET_TYPE_L1		0
#define PACKET_TYPE_L2		1
#define PACKET_TYPE_L3		2
#define PACKET_TYPE_L4		3
#define PACKET_TYPE_L5		4
#define PACKET_TYPE_PAYLOAD	5

#define PKT_L2_ETHERNET_II	0
#define PKT_L2_802_3		1
#define PKT_L2_PAUSE		2


#define PKT_L5_IEEE_1588	7
#define	_DFLT				129
#define	_ALT1				130
#define	_ALT2				131
#define	_ALT3				132

#define VLAN_TPID			0x8100 
#define SP_VLAN_TPID		0x88A8

#define	IPPROTO_IP			0		/* dummy for IP */
#define IPPROTO_HOPOPTS		IPPROTO_IP	/* Hop-by-hop option header. */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_IGMP		2		/* group mgmt protocol */
#define	IPPROTO_GGP			3		/* gateway^2 (deprecated) */
#define	IPPROTO_IPIP		4		/* IP inside IP */
#define	IPPROTO_IPV4		IPPROTO_IPIP	/* IP inside IP */
#define	IPPROTO_TCP			6		/* tcp */
#define	IPPROTO_EGP			8		/* exterior gateway protocol */
#define	IPPROTO_PUP			12		/* pup */
#define	IPPROTO_UDP			17		/* user datagram protocol */
#define	IPPROTO_IDP			22		/* xns idp */
#define	IPPROTO_TP			29 		/* tp-4 w/ class negotiation */
#define IPPROTO_IPV6		41		/* IPv6 in IPv6 */
#define IPPROTO_ROUTING		43		/* Routing header. */
#define IPPROTO_FRAGMENT	44		/* Fragmentation/reassembly header. */
#define IPPROTO_RSVP		46		/* resource reservation */
#define IPPROTO_GRE			47		/* GRE encaps RFC 1701 */
#define	IPPROTO_ESP			50		/* Encap. Security Payload */
#define	IPPROTO_AH			51		/* Authentication header */
#define IPPROTO_ICMPV6		58		/* ICMP for IPv6 */
#define IPPROTO_NONE		59		/* No next header */
#define IPPROTO_DSTOPTS		60		/* Destination options header. */
#define	IPPROTO_EON			80		/* ISO cnlp */
#define IPPROTO_ETHERIP		97		/* Ethernet in IPv4 */
#define	IPPROTO_ENCAP		98		/* encapsulation header */
#define IPPROTO_PIM			103		/* Protocol indep. multicast */
#define IPPROTO_IPCOMP		108		/* IP Payload Comp. Protocol */
#define	IPPROTO_RAW			255		/* raw IP packet */

#define	IPPROTO_MAX			256

#define PPTP_GRE_PROTO  0x880B
#define PPTP_GRE_VER    0x1

#define PPTP_GRE_FLAG_C	0x8000
#define PPTP_GRE_FLAG_R	0x4000
#define PPTP_GRE_FLAG_K	0x2000
#define PPTP_GRE_FLAG_S	0x1000
#define PPTP_GRE_FLAG_A	0x0080

#define TYPE_SEND_PACKET    0
#define TYPE_RECEIVE_PACKET 1

extern KEYWORD_T keyword_2nd_tbl[];

int cs_process_specify_file(char *sndFile, char *rcvFile, char *pHelp);
unsigned char read_a_pkt_hex_file(unsigned long SeqNo, unsigned char *pBuffer, unsigned long *pPktLength);

#endif //__FILE_PROCESS_H__
