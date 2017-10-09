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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "fileProcess.h"
#include "public.h"

int parse_L4(int argc, unsigned char *argv[]);
static int parse_Sport(int argc, unsigned char *argv[]);
static int parse_Dport(int argc, unsigned char *argv[]);
static int parse_seq(int argc, unsigned char *argv[]);
static int parse_ack(int argc, unsigned char *argv[]);
static int parse_urg_flag(int argc, unsigned char *argv[]);
static int parse_ack_flag(int argc, unsigned char *argv[]);
static int parse_push_flag(int argc, unsigned char *argv[]);
static int parse_rst_flag(int argc, unsigned char *argv[]);
static int parse_syn_flag(int argc, unsigned char *argv[]);
static int parse_fin_flag(int argc, unsigned char *argv[]);
static int parse_win(int argc, unsigned char *argv[]);
static int parse_chksum(int argc, unsigned char *argv[]);
static int parse_urgent_pointer(int argc, unsigned char *argv[]);
static int parse_option(int argc, unsigned char *argv[]);
static int parse_call_id(int argc, unsigned char *argv[]);
static int parse_spi(int argc, unsigned char *argv[]);

#define L4_OVERWRITE_L3_PROTOCOL	1

extern DESCRIPTION_INFO_T *pInfo;

/*----------------------------------------------------------------------
* Keyword Definition
*
*	Level-1		Level-2			Level-3/L2	Level-3/L3		Level-3/L4  
*	-------		-------------	----------	----------      ----------  
*	Storlink	PacketType		DA			HeaderLength    Sport
*	begin		PacketLength	SA			SIP             Dport         
*	end			PacketNumber	VLAN		DIP             ...         
*				L2				EtherType	Option                
*				L3				PPPoE		TOS                      
*				L4							...                      
*				Payload
*----------------------------------------------------------------------*/

KEYWORD_T keyword_L4_tbl[]=
{
	/* Name     		handler 	*/
	{"Sport",			(void *)parse_Sport},
	{"Dport",			(void *)parse_Dport},
	{"Sequence",		(void *)parse_seq},
	{"acknowledge",		(void *)parse_ack},
	{"urg_flag",		(void *)parse_urg_flag},
	{"ack_flag",		(void *)parse_ack_flag},
	{"push_flag",		(void *)parse_push_flag},
	{"rst_flag",		(void *)parse_rst_flag},
	{"syn_flag",		(void *)parse_syn_flag},
	{"fin_flag",		(void *)parse_fin_flag},
	{"window",			(void *)parse_win},
	{"chksum",			(void *)parse_chksum},
	{"urgent_ptr",		(void *)parse_urgent_pointer},
	{"option",			(void *)parse_option},
	{"call_id",			(void *)parse_call_id},
	{"spi",				(void *)parse_spi},
    {"L4",				(void *)parse_L4},
    {NULL,				NULL}
};


/*----------------------------------------------------------------------
* parse_L4
*----------------------------------------------------------------------*/
int parse_L4(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int					protocol;
	
	if (argc != 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	info->curr_table = (KEYWORD_T *)&keyword_L4_tbl[0];
	
	if (stricmp(argv[1], "UDP") == 0)
		protocol = IPPROTO_UDP;
	else if (stricmp(argv[1], "TCP") == 0)
		protocol = IPPROTO_TCP;
	else if (stricmp(argv[1], "GRE") == 0)
		protocol = IPPROTO_GRE;
	else if (stricmp(argv[1], "AH") == 0)
		protocol = IPPROTO_AH;
	else if (stricmp(argv[1], "ESP") == 0)
		protocol = IPPROTO_ESP;
	else
		return -1;	
		
	if (info->L2.have_vlan_id || info->L2.have_cif || info->L2.have_priority || 
		info->L2.have_inner_vlan_id || info->L2.have_cif_ce || info->L2.have_priority_ce
		|| info->L2.have_outer_vlan_id || info->L2.have_cif_sp || info->L2.have_priority_sp)
	{
		printf("ERROR : VLAN / SP_VLAN / CE_VLAN / CIF /PRIORITY do not exist with L4 simultaneously\n");	
		return -1;	
	}

	memset((char *)&info->L4, 0, sizeof(info->L4));
	info->pkt.have_L4 = 1;
	info->L4.protocol = protocol;
	
	info->Payload.header.have_src_pid = 0;
	info->Payload.header.have_dest_pid = 0;
	info->Payload.header.have_qid = 0;
	info->Payload.header.have_multicast = 0;	

	// TBD: Set default L4 value
	if (protocol == IPPROTO_TCP)
	{
		info->L4.hdr_len = 20;
	}

	return 0;
}

/*----------------------------------------------------------------------
* parse_Sport
*----------------------------------------------------------------------*/
static int parse_Sport(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.sport_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->L4.sport = (unsigned short)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_Dport
*----------------------------------------------------------------------*/
static int parse_Dport(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.dport_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->L4.dport = (unsigned short)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_seq
*----------------------------------------------------------------------*/
static int parse_seq(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.seq_random = 1;
		info->L4.have_seq = 1;
		return 0;
	}
	if (argc == 3 && IS_INCREMENTAL(argv[1]))
	{
		info->L4.have_seq = 1;
		info->L4.seq_incremental = 1;
		info->L4.seq_first = 1;
		info->L4.seq = (unsigned long)string2value(argv[2]);
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L4.seq = (unsigned long)string2value(argv[1]);
	info->L4.have_seq = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_ack
*----------------------------------------------------------------------*/
static int parse_ack(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.ack_random = 1;
		return 0;
	}
	if (argc == 4 && IS_INCREMENTAL(argv[1]))
	{
		info->L4.have_ack = 1;
		info->L4.ack_incremental = 1;
		info->L4.ack_first = 1;
		info->L4.ack = (unsigned long)string2value(argv[2]);
		info->L4.ack_step = (unsigned long)string2value(argv[3]);
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L4.ack = (unsigned long)string2value(argv[1]);
	info->L4.have_ack = 1;
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_urg_flag
*----------------------------------------------------------------------*/
static int parse_urg_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.URG_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_ack_flag
*----------------------------------------------------------------------*/
static int parse_ack_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.ACK_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_push_flag
*----------------------------------------------------------------------*/
static int parse_push_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.PUSH_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_rst_flag
*----------------------------------------------------------------------*/
static int parse_rst_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.RST_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_syn_flag
*----------------------------------------------------------------------*/
static int parse_syn_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.SYN_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_fin_flag
*----------------------------------------------------------------------*/
static int parse_fin_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.FIN_Flag = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_win
*----------------------------------------------------------------------*/
static int parse_win(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.win_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L4.window_size = (unsigned short)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_chksum
*----------------------------------------------------------------------*/
static int parse_chksum(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.chksum = (unsigned short)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_urgent_pointer
*----------------------------------------------------------------------*/
static int parse_urgent_pointer(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L4.urgent_pointer = (unsigned short)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_option
*----------------------------------------------------------------------*/
static int parse_option(int argc, unsigned char *argv[])
{
	int					i;
	DESCRIPTION_INFO_T	*info = pInfo;

	argc--;
	if ((argc % 4) !=0)
	{
		printf("The numbers of options must be 4*N\n");
		return -1;
	}
			
	for (i=0; i<argc; i++)
		info->L4.options[i] = (unsigned char)string2value(argv[i+1]);
		
	info->L4.option_len = argc;
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_call_id
*----------------------------------------------------------------------*/
static int parse_call_id(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.call_id_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L4.call_id = (unsigned short)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_spi
*----------------------------------------------------------------------*/
static int parse_spi(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L4.spi_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L4.spi = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* build_L4_packet
*----------------------------------------------------------------------*/
unsigned char *build_L4_packet(DESCRIPTION_INFO_T *info, unsigned char *buf, unsigned int *len)
{
	unsigned char	*cp;
	unsigned int	payload_length, hdr_len;
	unsigned char	flag;
	unsigned short	gre_flag_ver;
	int				i;
	
	payload_length = *len;
	info->pkt.L4hdr = cp = buf;
	if (info->L4.sport_random) info->L4.sport = get_random_number(0, 0xffff);
	if (info->L4.dport_random) info->L4.dport = get_random_number(0, 0xffff);
	if (info->L4.seq_random)  info->L4.seq = get_random_number(0, 0xffffffff);
	if (info->L4.seq_incremental)
	{
		if (!info->L4.seq_first)
			info->L4.seq += info->prev_payload_size;
		info->L4.seq_first = 0;
	}
	if (info->L4.ack_random) info->L4.ack = get_random_number(0, 0xffffffff);
	if (info->L4.ack_incremental)
	{
		if (!info->L4.ack_first)
			info->L4.ack += info->L4.ack_step;
		info->L4.ack_first = 0;
	}
			
	if (info->L4.win_random)   info->L4.window_size = get_random_number(0, 0xffff);
	if (info->L4.call_id_random)   info->L4.call_id = get_random_number(0, 0xffff);
	if (info->L4.spi_random)   info->L4.spi = get_random_number(0, 0xffffffff);
	
	switch (info->L4.protocol)
	{
		case IPPROTO_UDP:
			PUSH_WORD(cp, info->L4.sport);
			PUSH_WORD(cp, info->L4.dport);
			PUSH_WORD(cp, payload_length);
			info->L4.chksum_ptr = cp;
			info->L4.payload_size = payload_length;
			PUSH_WORD(cp, 0);
			payload_length -= 8;
#ifdef L4_OVERWRITE_L3_PROTOCOL			
			if (info->L3.protocol_ptr && !info->L3.have_protocol)
				PUSH_BYTE(info->L3.protocol_ptr, info->L4.protocol)
#endif				
			break;
		case IPPROTO_TCP:
			PUSH_WORD(cp, info->L4.sport);
			PUSH_WORD(cp, info->L4.dport);
			PUSH_DWORD(cp, info->L4.seq);
			PUSH_DWORD(cp, info->L4.ack);
			hdr_len = 20 + info->L4.option_len;
			PUSH_BYTE(cp, hdr_len<<2);
			flag = ((info->L4.URG_Flag << 5) & 0xff) |
				   ((info->L4.ACK_Flag << 4) & 0xff) |
				   ((info->L4.PUSH_Flag << 3) & 0xff) |
				   ((info->L4.RST_Flag << 2) & 0xff) |
				   ((info->L4.SYN_Flag << 1) & 0xff) |
				   ( info->L4.FIN_Flag       & 0xff);
			PUSH_BYTE(cp, flag);
			PUSH_WORD(cp, info->L4.window_size);
			info->L4.chksum_ptr = cp;
			PUSH_WORD(cp, 0);
			PUSH_WORD(cp, info->L4.urgent_pointer);
			for (i=0; i<info->L4.option_len; i++)
				PUSH_BYTE(cp, info->L4.options[i]);
			info->L4.payload_size = payload_length;
			payload_length -= hdr_len;
#ifdef L4_OVERWRITE_L3_PROTOCOL			
			if (info->L3.protocol_ptr && !info->L3.have_protocol)
				PUSH_BYTE(info->L3.protocol_ptr, info->L4.protocol)
#endif
			break;
		case IPPROTO_GRE:
			gre_flag_ver = PPTP_GRE_VER | PPTP_GRE_FLAG_K;
			if (info->L4.have_seq)
				gre_flag_ver |= PPTP_GRE_FLAG_S;
			if (info->L4.have_ack)
				gre_flag_ver |= PPTP_GRE_FLAG_A;
			
			PUSH_WORD(cp, gre_flag_ver);
			PUSH_WORD(cp, PPTP_GRE_PROTO);
				
			if (info->L4.have_seq == 0)
			{
				payload_length = 0;
				PUSH_WORD(cp, payload_length);	
				PUSH_WORD(cp, info->L4.call_id);
				if (info->L4.have_ack)
					PUSH_DWORD(cp, info->L4.ack);
			}
			else
			{
				payload_length -= 12;
				if (info->L4.have_ack)
					payload_length -= 4;
				PUSH_WORD(cp, payload_length);	
				PUSH_WORD(cp, info->L4.call_id);
				PUSH_DWORD(cp, info->L4.seq);
				if (info->L4.have_ack)
					PUSH_DWORD(cp, info->L4.ack);
				
				// ppp - ip
				//payload_length -= 2;
				//PUSH_WORD(cp, 0x21);
			}
			info->L4.payload_size = payload_length;
#ifdef L4_OVERWRITE_L3_PROTOCOL			
			if (info->L3.protocol_ptr && !info->L3.have_protocol)
				PUSH_BYTE(info->L3.protocol_ptr, info->L4.protocol)
#endif
			break;
		case IPPROTO_AH:
			payload_length -= 12;
			PUSH_BYTE(cp, 0);				// next header
			PUSH_BYTE(cp, payload_length);	// payload length
			PUSH_WORD(cp, 0);				// reserved
			PUSH_DWORD(cp, info->L4.spi);
			PUSH_DWORD(cp, info->L4.seq);
			info->L4.payload_size = payload_length;
#ifdef L4_OVERWRITE_L3_PROTOCOL			
			if (info->L3.protocol_ptr && !info->L3.have_protocol)
				PUSH_BYTE(info->L3.protocol_ptr, info->L4.protocol)
#endif
			break;
		case IPPROTO_ESP:
			payload_length -= 8;
			PUSH_DWORD(cp, info->L4.spi);
			PUSH_DWORD(cp, info->L4.seq);
			info->L4.payload_size = payload_length;
#ifdef L4_OVERWRITE_L3_PROTOCOL			
			if (info->L3.protocol_ptr && !info->L3.have_protocol)
				PUSH_BYTE(info->L3.protocol_ptr, info->L4.protocol)
#endif
			break;
		default:
			printf("Unknown protocol!\n");
			return buf;
	}
	
	*len = payload_length;
	if (info->pkt.have_L5 == 0)
	{
		buf = cp;
		return buf;
	}
	else
	{
		return cp;
	}
}

/*----------------------------------------------------------------------
* build_L4_chksum
*----------------------------------------------------------------------*/
void build_L4_chksum(DESCRIPTION_INFO_T *info)
{
	unsigned char	*cp;
	unsigned short	chksum;
	u32				psum;
	
	if (info->L4.chksum_ptr)
	{
		if (info->L4.protocol == IPPROTO_UDP)
		{
			cp = info->L4.chksum_ptr;
			psum = pseudo_sum(info->L3.type,
								(u16 *)info->L3.sip, 
    							(u16 *)info->L3.dip,
    							(u16)info->L4.payload_size,
    							IPPROTO_UDP);
			chksum = ip_csum((u16 *)info->pkt.L4hdr, 
							info->L4.payload_size,
    						psum);
    						
    		if (info->Payload.type == PKT_PAYLOAD_CHKSUM)
			{
				unsigned char	*payload = info->pkt.Payload_hdr;
				PUSH_WORD(payload, chksum);
				PUSH_WORD(payload, ~info->Payload.force_chksum);
				chksum = ip_csum((u16 *)info->pkt.L4hdr, 
							info->L4.payload_size,
    						psum);
    			if (chksum == 0 && info->Payload.force_chksum != 0)
    				chksum = 0xffff;
				PUSH_WORD(cp, chksum);
 			}
 			else
 			{
    			if (chksum == 0)
    				chksum = 0xffff;
				PUSH_WORD(cp, chksum);
			}
		}
		else if (info->L4.protocol == IPPROTO_TCP)
		{
			cp = info->L4.chksum_ptr;
			psum = pseudo_sum(info->L3.type,
								(u16 *)info->L3.sip, 
    							(u16 *)info->L3.dip,
    							(u16)info->L4.payload_size,
    							IPPROTO_TCP);
			chksum = ip_csum((u16 *)info->pkt.L4hdr, 
							info->L4.payload_size,
    						psum);
    		if (info->Payload.type == PKT_PAYLOAD_CHKSUM)
			{
				unsigned char	*payload = info->pkt.Payload_hdr;
				PUSH_WORD(payload, chksum);
				PUSH_WORD(payload, ~info->Payload.force_chksum);
				chksum = ip_csum((u16 *)info->pkt.L4hdr, 
							info->L4.payload_size,
    						psum);
				if (chksum == 0 && info->Payload.force_chksum == 0xffff)
    				chksum = 0xffff;
 			}
			PUSH_WORD(cp, chksum);
		}    
	}
}

