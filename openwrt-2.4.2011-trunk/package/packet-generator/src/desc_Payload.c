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

int parse_payload(int argc, unsigned char *argv[]);
static int parse_start(int argc, unsigned char *argv[]);
static int parse_step(int argc, unsigned char *argv[]);
extern int parse_signature(int argc, unsigned char *argv[]);

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

KEYWORD_T keyword_payload_tbl[]=
{
	/* Name     		handler 	*/
    {"Start",			(void *)parse_start},
    {"Step",			(void *)parse_step},
    {"Signature",		(void *)parse_signature},
    {"Payload",			(void *)parse_payload}, // for end
    {NULL,				NULL}
};


/*----------------------------------------------------------------------
* parse_payload
*----------------------------------------------------------------------*/
int parse_payload(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int type;
	unsigned short chksum = 0;
	
	if (argc < 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	if (stricmp(argv[1], "random") == 0)
		type = PKT_PAYLOAD_RANDOM;
	else if (stricmp(argv[1], "Sequential") == 0)
		type = PKT_PAYLOAD_SEQUENTIAL;
	else if (stricmp(argv[1], "chksum") == 0)
	{
		type = PKT_PAYLOAD_CHKSUM;
		if (argc == 3)
			chksum = (unsigned short)string2value(argv[2]);
	}
	else
		return -1;	
		
	memset((char *)&info->Payload, 0, sizeof(info->Payload));
	info->pkt.have_Payload = 1;
	info->Payload.type = type;
	info->Payload.force_chksum = chksum;
	info->curr_table = (KEYWORD_T *)&keyword_payload_tbl[0];
	return 0;
}


/*----------------------------------------------------------------------
* parse_start
*----------------------------------------------------------------------*/
static int parse_start(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.start_data = get_random_number(0, 0xff);;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->Payload.header.start_data = (unsigned char)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* parse_step
*----------------------------------------------------------------------*/
static int parse_step(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.step = get_random_number(0, 0xff);
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->Payload.header.step = (unsigned char)string2value(argv[1]);
	return 0;
}

/*----------------------------------------------------------------------
* build_payload_packet
*----------------------------------------------------------------------*/
unsigned char *build_payload_packet(DESCRIPTION_INFO_T *info, unsigned char *buf,unsigned char *buf_1, unsigned int *len)
{
	unsigned char	*cp,*cp_1 = NULL;
	int				payload_length;
	unsigned char	start_data, step;
	int				i;
	PAYLOAD_HDR_T	*hdr;
	
	info->prev_payload_size = payload_length = *len;
	info->pkt.Payload_hdr = cp = buf;
	if (info->pkt.have_L5 == 0)
		cp_1 = buf_1+PAYLOAD_INDEX_DST_PORT_OFFSET;  //for destination port offset 56
		//cp_1 = buf_1+56;  //for destination port offset 56
	
	if (!info->pkt.have_Payload)
	{
		for (i=0; payload_length && i<payload_length; i++)
			*cp++ = i;
		*len = 0;
		return cp;
	}
	
	switch (info->Payload.type)
	{
		case PKT_PAYLOAD_RANDOM:
			for (i=0; payload_length && i<payload_length; i++)
				*cp++ = get_random_number(0, 0xff);
			break;
		case PKT_PAYLOAD_SEQUENTIAL:
			hdr = (PAYLOAD_HDR_T *)cp;
			//payload_length -= sizeof(info->Payload.header);
			info->Payload.header.tatal_bytes = payload_length;
			//memcpy(cp, (unsigned char *)&info->Payload.header, sizeof(info->Payload.header));
			//cp += sizeof(info->Payload.header);
			start_data = info->Payload.header.start_data;
			step = info->Payload.header.step;
			for (i=0; payload_length && i<payload_length; i++)
			{
				*cp++ = start_data;
				start_data += step;
				//printf("22 cp = 0x%x ,*cp = %02X\n",cp,*cp);
			}
			break;
		case PKT_PAYLOAD_CHKSUM:
			for (i=0; payload_length && i<payload_length; i++)
				*cp++ = 0;
			break;
	}

	if (info->pkt.have_L5 == 1)
	{
		info->Payload.header.have_src_pid = 0;
		info->Payload.header.have_dest_pid = 0;
		info->Payload.header.have_qid = 0;
		info->Payload.header.have_multicast = 0;	
	}
	
	if (info->pkt.have_L5 == 0)
	{
		if (info->Payload.header.have_src_pid == 1)
			PUSH_BYTE(cp_1, info->Payload.header.src_pid);
		if (info->Payload.header.have_dest_pid == 1)
			PUSH_BYTE(cp_1, info->Payload.header.dest_pid);
		if (info->Payload.header.have_qid == 1)
			PUSH_BYTE(cp_1, info->Payload.header.qid);
		if (info->Payload.header.have_multicast == 1)
			PUSH_BYTE(cp_1, info->Payload.header.multicast);
	}
	
	cp_1 = buf_1 + PAYLOAD_CS_MAGIC_OFFSET;
	PUSH_DWORD(cp_1, CORTINA_MAGIC_NUMBER);
	cp_1 = buf_1 + PAYLOAD_SEQUENCE_NUMBER_OFFSET;
	PUSH_DWORD(cp_1, info->SequenceNum);
    info->SequenceNum++;
	
	*len = 0;
	
	return cp;
}

