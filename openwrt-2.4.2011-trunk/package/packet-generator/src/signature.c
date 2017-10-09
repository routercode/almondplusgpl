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

int parse_signature(int argc, unsigned char *argv[]);
static int parse_src_pid(int argc, unsigned char *argv[]);
static int parse_dest_pid(int argc, unsigned char *argv[]);
static int parse_qid(int argc, unsigned char *argv[]);
static int parse_multicast(int argc, unsigned char *argv[]);


extern DESCRIPTION_INFO_T *pInfo;

KEYWORD_T keyword_signature_tbl[]=
{
	/* Name     		handler 	*/
    {"SRC_PID",			(void *)parse_src_pid},
    {"DEST_PID",		(void *)parse_dest_pid},
    {"QID",				(void *)parse_qid},
    {"MD_PID",			(void *)parse_multicast},
 	{"Signature",		(void *)parse_signature}, 
    {NULL,				NULL}
};

/*----------------------------------------------------------------------
* parse_payload
*----------------------------------------------------------------------*/
int parse_signature(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	//int type;
	//unsigned short chksum;
	
	if (argc < 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	if (stricmp(argv[1], "begin") != 0)
		return -1;
	
	info->curr_table = (KEYWORD_T *)&keyword_signature_tbl[0];
	return 0;
}

/*----------------------------------------------------------------------
* parse_src_pid
*----------------------------------------------------------------------*/
static int parse_src_pid(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.src_pid = get_random_number(0, 0x4); //must add 0,1,2,3,4,F
		info->Payload.header.have_src_pid = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;
	info->Payload.header.src_pid = (unsigned char)string2value(argv[1]);
	info->Payload.header.have_src_pid = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_dest_pid
*----------------------------------------------------------------------*/
static int parse_dest_pid(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.dest_pid = get_random_number(0, 0x4); //must add 0,1,2,3,4,F
		info->Payload.header.have_dest_pid = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->Payload.header.dest_pid = (unsigned char)string2hex(argv[1]);
	info->Payload.header.have_dest_pid = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_qid
*----------------------------------------------------------------------*/
static int parse_qid(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.qid = get_random_number(0, 0x3); //Queue  0,1,2,3
		info->Payload.header.have_qid = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->Payload.header.qid = (unsigned char)string2value(argv[1]);
	info->Payload.header.have_qid = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_multicast
*----------------------------------------------------------------------*/
static int parse_multicast(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->Payload.header.multicast = get_random_number(0, 0x200); 
		info->Payload.header.have_multicast = 1;
		return 0;
	}
	
	if (argc != 2)
		return -1;

	info->Payload.header.multicast = (unsigned char)string2value(argv[1]);
	info->Payload.header.have_multicast = 1;
	return 0;
}
