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


int parse_L3(int argc, unsigned char *argv[]);
static int parse_SIP(int argc, unsigned char *argv[]);
static int parse_DIP(int argc, unsigned char *argv[]);
static int parse_TOS(int argc, unsigned char *argv[]);
static int parse_ID(int argc, unsigned char *argv[]);
static int parse_TTL(int argc, unsigned char *argv[]);
static int parse_Fragment(int argc, unsigned char *argv[]);
static int parse_protocol(int argc, unsigned char *argv[]);
static int parse_option(int argc, unsigned char *argv[]);
static int parse_chksum(int argc, unsigned char *argv[]);
static int parse_Traffic(int argc, unsigned char *argv[]);
static int parse_Flow(int argc, unsigned char *argv[]);
static int parse_HopLimit(int argc, unsigned char *argv[]);
static int parse_NextHeader(int argc, unsigned char *argv[]);
static int parse_ExtHdrNum(int argc, unsigned char *argv[]);
static int parse_ExtHeader(int argc, unsigned char *argv[]);
static int parse_ExtHdrLen(int argc, unsigned char *argv[]);

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

KEYWORD_T keyword_L3_tbl[]=
{
	/* Name     		handler 	*/
	{"SIP",				(void *)parse_SIP},
    {"DIP",				(void *)parse_DIP},
	{"TOS",				(void *)parse_TOS},
	{"ID",				(void *)parse_ID},
	{"TTL",				(void *)parse_TTL},
	{"Fragment",		(void *)parse_Fragment},
	{"protocol",		(void *)parse_protocol},
	{"option",			(void *)parse_option},
	{"chksum",			(void *)parse_chksum},
	{"Traffic",			(void *)parse_Traffic},
	{"Flow",			(void *)parse_Flow},
	{"HopLimit",		(void *)parse_HopLimit},
	{"NextHeader",		(void *)parse_NextHeader},
	{"ExtHdrNum",		(void *)parse_ExtHdrNum},
	{"ExtHeader",		(void *)parse_ExtHeader},
	{"ExtHdrLen",		(void *)parse_ExtHdrLen},
    {"L3",				(void *)parse_L3},		// for parsing "L3: end"
    {NULL,				NULL}
};

/*----------------------------------------------------------------------
* parse_L3
*----------------------------------------------------------------------*/
int parse_L3(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;
	int type;
	
	if (argc != 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	info->curr_table = (KEYWORD_T *)&keyword_L3_tbl[0];
	
	if (stricmp(argv[1], "IPV4") == 0)
		type = PKT_L3_IPV4;
	else if (stricmp(argv[1], "IPV6") == 0)
		type = PKT_L3_IPV6;
	else
		return -1;	
	
	memset((char *)&info->L3, 0, sizeof(info->L3));
	info->pkt.have_L3 = 1;
	info->L3.type = type;
	
	// Set default L3 value
	if (info->L3.type == PKT_L3_IPV4)
	{
		info->L3.ttl = 4;
		info->L3.chksum = 1;
		info->L3.type = PKT_L3_IPV4;
	}
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_SIP
*----------------------------------------------------------------------*/
static int parse_SIP(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;
	int					i, bytes;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.sip_random = 1;
		return 0;
	}
	
	if (info->L3.type == PKT_L3_IPV6)
		bytes = 16;
	else
		bytes = 4;
		
	if (argc != (bytes + 1))
		return -1;
		
	for (i=0; i<bytes; i++)
		info->L3.sip[i] = (unsigned char)string2value(argv[i+1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_DIP
*----------------------------------------------------------------------*/
static int parse_DIP(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;
	int					i, bytes;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.dip_random = 1;
		return 0;
	}
	
	if (info->L3.type == PKT_L3_IPV6)
		bytes = 16;
	else
		bytes = 4;
		
	if (argc != (bytes + 1))
		return -1;
		
	for (i=0; i<bytes; i++)
		info->L3.dip[i] = (unsigned char)string2value(argv[i+1]);

	return 0;
}

/*----------------------------------------------------------------------
* parse_TOS
*----------------------------------------------------------------------*/
static int parse_TOS(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.tos_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
		
	info->L3.tos = (unsigned char)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_ID
*----------------------------------------------------------------------*/
static int parse_ID(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.id_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
		
	info->L3.id = (unsigned short)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_TTL
*----------------------------------------------------------------------*/
static int parse_TTL(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.ttl_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
		
	info->L3.ttl = (unsigned char)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_Fragment
*----------------------------------------------------------------------*/
static int parse_Fragment(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc != 2)
		return -1;
		
	info->L3.fragment = (unsigned short)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_protocol
*----------------------------------------------------------------------*/
static int parse_protocol(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.protocol_random = 1;
		info->L3.have_protocol = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
		
	info->L3.protocol = (unsigned char)string2value(argv[1]);
	info->L3.have_protocol = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_option
*----------------------------------------------------------------------*/
static int parse_option(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;
	int					i;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.option_random = 1;
		return 0;
	}
	
	argc--;
	if ((argc % 4) !=0)
	{
		printf("The numbers of options must be 4*N\n");
		return -1;
	}
			
	for (i=0; i<argc; i++)
		info->L3.options[i] = (unsigned char)string2value(argv[i+1]);
		
	info->L3.option_len = argc;
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_chksum
*----------------------------------------------------------------------*/
static int parse_chksum(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	 *info = pInfo;

	if (argc != 2)
		return -1;
			
	info->L3.chksum = (unsigned char)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_Traffic
*----------------------------------------------------------------------*/
static int parse_Traffic(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.traffic_class_random = 1;
		return 0;
	}
	if (argc != 2)
		return -1;
		
	info->L3.traffic_class = (unsigned int)string2value(argv[1]);
	if (info->L3.traffic_class > 255)
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_Flow
*----------------------------------------------------------------------*/
static int parse_Flow(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.flow_lable_random = 1;
		return 0;
	}
	if (argc != 2)
		return -1;
		
	info->L3.flow_lable = (unsigned long)string2value(argv[1]);
	if (info->L3.flow_lable > 0xfffff)
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_HopLimit
*----------------------------------------------------------------------*/
static int parse_HopLimit(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L3.hop_limit_random = 1;
		return 0;
	}
	if (argc != 2)
		return -1;
		
	info->L3.hop_limit = (unsigned int)string2value(argv[1]);
	if (info->L3.hop_limit > 255)
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_NextHeader
*----------------------------------------------------------------------*/
static int parse_NextHeader(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	unsigned int		data;

	if (argc != 2)
		return -1;
		
	data = (unsigned int)string2value(argv[1]);
	if (data > 255)
		return -1;
	
	if (!info->L3.ext_header_begin)
		info->L3.next_header = data;
	else if (info->L3.ext_header_idx <= MAX_EXT_HEADER_NUM)
		info->L3.ext_header_proto[info->L3.ext_header_idx-1] = data;
	else
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_ExtHdrNum
*----------------------------------------------------------------------*/
static int parse_ExtHdrNum(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc != 2)
		return -1;
		
	info->L3.ext_header_num = (unsigned int)string2value(argv[1]);
	if (info->L3.ext_header_num > MAX_EXT_HEADER_NUM)
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_ExtHeader
*----------------------------------------------------------------------*/
static int parse_ExtHeader(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (stricmp(argv[1], "end") == 0)
	{	
		if (info->L3.ext_header_begin == 0)
		{
			printf("Syntax error for extension header!\n");
			return -1;
		}
		info->L3.ext_header_begin = 0;
		return 0;
	}
	
	info->L3.ext_header_begin = 1;
	
	if (info->L3.ext_header_idx > MAX_EXT_HEADER_NUM)
	{
		return -1;
	}
	
	info->L3.ext_header_idx++;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_ExtHdrLen
*----------------------------------------------------------------------*/
static int parse_ExtHdrLen(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	unsigned int		data;

	if (argc != 2 || !info->L3.ext_header_begin)
		return -1;
		
	data = (unsigned int)string2value(argv[1]);
	if (data > 255)
		return -1;
		
	if(info->L3.ext_header_idx <= MAX_EXT_HEADER_NUM)
		info->L3.ext_header_len[info->L3.ext_header_idx-1] = data;
	else
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* build_L3_packet
*----------------------------------------------------------------------*/
unsigned char *build_L3_packet(DESCRIPTION_INFO_T *info, unsigned char *buf, unsigned int *len)
{
	unsigned char		*cp;
	unsigned int		payload_length;
	unsigned char 		ver_type;
	unsigned char		i;
	
	payload_length = *len;
	info->pkt.iphdr = cp = buf;
	info->L3.header_len = 20;
	
	if (info->L3.type == PKT_L3_IPV4)
	{
		if (info->L3.option_random)
		{
			info->L3.option_len = get_random_number(0, IP_MAX_OPTIONS/4) * 4;
			for (i=0; i<info->L3.option_len; i++)
				info->L3.options[i] = get_random_number(0, 0xff);
		}
		if (info->L3.tos_random)		info->L3.tos = get_random_number(0, 0xff);
		if (info->L3.ttl_random)		info->L3.ttl = get_random_number(0, 0xff);
		if (info->L3.id_random)			info->L3.id = get_random_number(0, 0xff);
		if (info->L3.protocol_random)	info->L3.protocol = get_random_number(0, 0xffff);
		if (info->L3.sip_random)
		{
			for (i=0; i<4; i++)
				info->L3.sip[i] = get_random_number(0, 0xff);
		}
		if (info->L3.dip_random)
		{
			for (i=0; i<4; i++)
				info->L3.dip[i] = get_random_number(0, 0xff);
		}
		
		info->L3.header_len += info->L3.option_len;
		ver_type = (unsigned char)(((info->L3.header_len >> 2) & 0x0f)) | (unsigned char)(((PKT_L3_IPV4 << 4) & 0xf0));
		PUSH_BYTE(cp, ver_type);
		PUSH_BYTE(cp, info->L3.tos);
	
		PUSH_WORD(cp, payload_length);
		payload_length -= (info->L3.header_len);

		PUSH_WORD(cp, info->L3.id);
		PUSH_WORD(cp, info->L3.fragment);
		PUSH_BYTE(cp, info->L3.ttl);
		info->L3.protocol_ptr = cp;
		PUSH_BYTE(cp, info->L3.protocol);
		
		info->L3.chksum_ptr = cp;
		PUSH_WORD(cp, 0);	// check sum, init value
	
		for (i=0; i<4; i++)
			PUSH_BYTE(cp, info->L3.sip[i]);
		for (i=0; i<4; i++)
			PUSH_BYTE(cp, info->L3.dip[i]);
		for (i=0; i<info->L3.option_len; i++)
			PUSH_BYTE(cp, info->L3.options[i]);
			
	} 
	else	// IPV6
	{
		unsigned long dword_data;
		
		if (info->L3.traffic_class_random)	info->L3.traffic_class = get_random_number(0, 0xff);
		if (info->L3.flow_lable_random)		info->L3.flow_lable = get_random_number(0, 0xfff);
		if (info->L3.hop_limit_random)		info->L3.hop_limit = get_random_number(0, 0xff);
		if (info->L3.sip_random)
		{
			for (i=0; i<16; i++)
				info->L3.sip[i] = get_random_number(0, 0xff);
		}
		if (info->L3.dip_random)
		{
			for (i=0; i<16; i++)
				info->L3.dip[i] = get_random_number(0, 0xff);
		}
		dword_data = (6 << 28) | (info->L3.traffic_class << 20) | (info->L3.flow_lable);
		PUSH_DWORD(cp, dword_data);
		payload_length -= 40;	// size of main header
		PUSH_WORD(cp, payload_length);
		PUSH_BYTE(cp, info->L3.next_header);
		PUSH_BYTE(cp, info->L3.hop_limit);
		for (i=0; i<16; i++)
			PUSH_BYTE(cp, info->L3.sip[i]);
		for (i=0; i<16; i++)
			PUSH_BYTE(cp, info->L3.dip[i]);
			
		for (i=0; i<info->L3.ext_header_num; i++)
		{
			int bytes;
			PUSH_BYTE(cp, info->L3.ext_header_proto[i]);
			PUSH_BYTE(cp, info->L3.ext_header_len[i]);
			bytes = ((info->L3.ext_header_len[i] + 1) * 8) - 4;
			PUSH_BYTE(cp, 1);
			PUSH_BYTE(cp, bytes);
			payload_length -= (bytes + 4);
			while(bytes--)
			{
				PUSH_BYTE(cp, 0);
			}
		}
	}

	*len = payload_length;
	return cp;
}


/*----------------------------------------------------------------------
* build_L3_chksum
*----------------------------------------------------------------------*/
void build_L3_chksum(DESCRIPTION_INFO_T *info)
{
	unsigned short		chksum;
	// chksum = ip_sum_calc((u_int16_t)(info->L3.header_len + info->L3.option_len)
	//					, iphdr);
	
	if (info->L3.chksum_ptr && info->L3.type == PKT_L3_IPV4)
	{
		chksum = ip_csum((u16 *)info->pkt.iphdr, 
					(u16)(info->L3.header_len),
					0);
		PUSH_WORD(info->L3.chksum_ptr, chksum);
	}
}

