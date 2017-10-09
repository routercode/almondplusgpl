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

int parse_L5(int argc, unsigned char *argv[]);
static int parse_v_ptp(int argc, unsigned char *argv[]);
static int parse_v_network(int argc, unsigned char *argv[]);
static int parse_subdomain(int argc, unsigned char *argv[]);
static int parse_messageType(int argc, unsigned char *argv[]);
static int parse_sourceCT(int argc, unsigned char *argv[]);
static int parse_sourceUuid(int argc, unsigned char *argv[]);
static int parse_sourcePortId(int argc, unsigned char *argv[]);
static int parse_sequenceId(int argc, unsigned char *argv[]);
static int parse_control(int argc, unsigned char *argv[]);
static int parse_flags(int argc, unsigned char *argv[]);

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

KEYWORD_T keyword_L5_tbl[]=
{
	/* Name     		handler 	*/
	{"versionPTP",		(void *)parse_v_ptp},
	{"versionNetwork",	(void *)parse_v_network},
	{"subdomain",		(void *)parse_subdomain},
	{"messageType",		(void *)parse_messageType},
	{"sourceCT",		(void *)parse_sourceCT},
	{"sourceUuid",		(void *)parse_sourceUuid},
	{"sourcePortId",	(void *)parse_sourcePortId},
	{"sequenceId",		(void *)parse_sequenceId},
	{"control",			(void *)parse_control},
	{"flags",			(void *)parse_flags},
    {"L5",				(void *)parse_L5},
    {NULL,				NULL}
};


/*----------------------------------------------------------------------
* parse_L5
*----------------------------------------------------------------------*/
int parse_L5(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	
	if (argc != 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	info->curr_table = (KEYWORD_T *)&keyword_L5_tbl[0];
	
	if (stricmp(argv[1], "IEEE-1588") == 0)
		info->L5.type = PKT_L5_IEEE_1588;
		
	memset((char *)&info->L5, 0, sizeof(info->L5));
	info->pkt.have_L5 = 1;

	return 0;
}


/*----------------------------------------------------------------------
* parse_v_ptp
*----------------------------------------------------------------------*/
static int parse_v_ptp(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.v_ptp_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.v_ptp = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_v_network
*----------------------------------------------------------------------*/
static int parse_v_network(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.v_network_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.v_network = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_subdomain
*----------------------------------------------------------------------*/
static int parse_subdomain(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int					i, bytes;

//	if (argc == 2 && IS_RANDOM(argv[1]))
//	{
//		info->L5.???? = 1;
//		return 0;
//	}
	bytes = 16;
			
	if (argc == (bytes + 1) || (argc == 1))
	{
		if ((argc == 1) && (info->L3.type == PKT_L3_IPV4) && (info->L3.dip[0] == 224) && (info->L3.dip[1] == 0) && (info->L3.dip[2] == 1))
		{
			switch 	(info->L3.dip[3])
			{
				case	_DFLT:
				{
					info->L5.subdomain[0] = 95;  
					info->L5.subdomain[1] = 68;  
					info->L5.subdomain[2] = 70;  
					info->L5.subdomain[3] = 76; 
					info->L5.subdomain[4] = 84; 
					break;
				}
				case	_ALT1:
				{
					info->L5.subdomain[0] = 95; 
					info->L5.subdomain[1] = 65; 
					info->L5.subdomain[2] = 76; 
					info->L5.subdomain[3] = 84; 
					info->L5.subdomain[4] = 49; 
					break;
				}
				case	_ALT2:
				{
					info->L5.subdomain[0] = 95; 
					info->L5.subdomain[1] = 65; 
					info->L5.subdomain[2] = 76; 
					info->L5.subdomain[3] = 84; 
					info->L5.subdomain[4] = 50; 
					break;
				}
				case	_ALT3:
				{
					info->L5.subdomain[0] = 95; 
					info->L5.subdomain[1] = 65; 
					info->L5.subdomain[2] = 76; 
					info->L5.subdomain[3] = 84; 
					info->L5.subdomain[4] = 51; 
					break;
				}
				default:
					info->L5.subdomain[0] = 69; 
					info->L5.subdomain[1] = 82; 
					info->L5.subdomain[2] = 82; 
					info->L5.subdomain[3] = 79; 
					info->L5.subdomain[4] = 82; 
					break;
				
			}
			for (i=5; i<bytes; i++)
				info->L5.subdomain[i] = 0;
		}	
		else if ((argc == 1) && (info->L3.type == PKT_L3_IPV4))
		{
			for (i=5; i<bytes; i++)
				info->L5.subdomain[i] = 0;
		}
		else if ((argc == (bytes + 1)) && (info->L3.type == PKT_L3_IPV4))
		{
			for (i=0; i<bytes; i++)
			info->L5.subdomain[i] = (unsigned char)string2value(argv[i+1]);
		}
		else
		{
			printf("IPV6 Packet , please fill subdomain 16 bytes 1:2:3:4:5:6 .... :16 \n");
			for (i=0; i<bytes; i++)
			info->L5.subdomain[i] = (unsigned char)string2value(argv[i+1]);
		}
	}
	else
	{
		return -1;
	}

//	for (i=0; i<bytes; i++)
//		info->L5.subdomain[i] = (unsigned char)string2value(argv[i+1]);
				
	return 0;
}

/*----------------------------------------------------------------------
* parse_messageType
*----------------------------------------------------------------------*/
static int parse_messageType(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.messageType_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.messageType = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_sourceCT
*----------------------------------------------------------------------*/
static int parse_sourceCT(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.sourceCT_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.sourceCT = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_sourceUuid
*----------------------------------------------------------------------*/
static int parse_sourceUuid(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int					i, bytes;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.sourceUuid_random = 1;
		return 0;
	}
	
	bytes = 6;
			
	if (argc != (bytes + 1))
		return -1;
		
	for (i=0; i<bytes; i++)
		info->L5.sourceUuid[i] = (unsigned char)string2value(argv[i+1]);		
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_sourcePortId
*----------------------------------------------------------------------*/
static int parse_sourcePortId(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.sourcePortId_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.sourcePortId = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_sequenceId
*----------------------------------------------------------------------*/
static int parse_sequenceId(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.sequenceId_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.sequenceId = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_control
*----------------------------------------------------------------------*/
static int parse_control(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.control_random = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
			
	info->L5.control = (unsigned long)string2value(argv[1]);
	
	return 0;
}

/*----------------------------------------------------------------------
* parse_flags
*----------------------------------------------------------------------*/
static int parse_flags(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int					i, bytes;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L5.flags_random = 1;
		return 0;
	}
	
	bytes = 2;
			
	if (argc != (bytes + 1))
		return -1;
		
	for (i=0; i<bytes; i++)
		info->L5.flags[i] = (unsigned char)string2value(argv[i+1]);	
			
	return 0;
}

/*----------------------------------------------------------------------
* build_L5_packet
*----------------------------------------------------------------------*/
unsigned char *build_L5_packet(DESCRIPTION_INFO_T *info, unsigned char *buf, unsigned int *len)
{
	unsigned char	*cp;
	unsigned int	payload_length;
	int				i;
	
	payload_length = *len;
	
	info->pkt.L5hdr = cp = buf;
	if (info->L5.v_ptp_random) info->L5.v_ptp = get_random_number(0, 0xffff);
	if (info->L5.v_network_random) info->L5.v_network = get_random_number(0, 0xffff);
	
	PUSH_WORD(cp, info->L5.v_ptp);
//	payload_length -= 4;
		
	PUSH_WORD(cp, info->L5.v_network);
//	payload_length -= 4;
		
	if (info->L5.subdomain_random)
	{
		for (i=0; i<16; i++)
			info->L5.subdomain[i] = get_random_number(0, 0xff);
	}
	for (i=0; i<16; i++)
		PUSH_BYTE(cp, info->L5.subdomain[i]);
//		payload_length -= 16;
		
	if (info->L5.messageType_random) info->L5.messageType = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L5.messageType);
//		payload_length -= 2;
		
	if (info->L5.sourceCT_random) info->L5.sourceCT = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L5.sourceCT);
//		payload_length -= 2;
	
	if (info->L5.sourceUuid_random)
	{
		for (i=0; i<6; i++)
			info->L5.sourceUuid[i] = get_random_number(0, 0xff);
	}
	for (i=0; i<6; i++)
		PUSH_BYTE(cp, info->L5.sourceUuid[i]);
//		payload_length -= 6;

	if (info->L5.sourcePortId_random) info->L5.v_ptp = get_random_number(0, 0xffff);
		PUSH_WORD(cp, info->L5.sourcePortId);
//		payload_length -= 4;
	
	if (info->L5.sequenceId_random) info->L5.v_ptp = get_random_number(0, 0xffff);
		PUSH_WORD(cp, info->L5.sequenceId);
//		payload_length -= 4;
	
	if (info->L5.control_random) info->L5.sourceCT = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L5.control);
		PUSH_BYTE(cp, 0);//reserved_1
//		payload_length -= 4;
		
	if (info->L5.flags_random)
	{
		for (i=0; i<2; i++)
			info->L5.flags[i] = get_random_number(0, 0xff);
	}
	for (i=0; i<2; i++)
		PUSH_BYTE(cp, info->L5.flags[i]);
	
	PUSH_WORD(cp, 0); //reserved[4]
	PUSH_WORD(cp, 0); //reserved[4]	
	payload_length -= 40;
	*len = payload_length;
	buf = cp;
	return buf;
}


