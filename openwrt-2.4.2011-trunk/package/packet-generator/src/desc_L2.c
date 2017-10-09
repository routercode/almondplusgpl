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

#undef EPON
#ifdef EPON
int parse_IPG(int argc, unsigned char *argv[]);
int parse_LLID(int argc, unsigned char *argv[]);
#endif
int parse_L2(int argc, unsigned char *argv[]);
static int parse_DA(int argc, unsigned char *argv[]);
static int parse_SA(int argc, unsigned char *argv[]);
static int parse_priority_sp(int argc, unsigned char *argv[]);
static int parse_cif_sp(int argc, unsigned char *argv[]);
static int parse_OUTER_VLAN(int argc, unsigned char *argv[]);
static int parse_priority_ce(int argc, unsigned char *argv[]);
static int parse_cif_ce(int argc, unsigned char *argv[]);
static int parse_INNER_VLAN(int argc, unsigned char *argv[]);
static int parse_priority(int argc, unsigned char *argv[]);
static int parse_cif(int argc, unsigned char *argv[]);
static int parse_VLAN(int argc, unsigned char *argv[]);
static int parse_EtherType(int argc, unsigned char *argv[]);
#ifdef AG_802_1	
static int parse_802_1ag_md_level(int argc, unsigned char *argv[]);
static int parse_802_1ag_version(int argc, unsigned char *argv[]);
static int parse_802_1ag_opcode(int argc, unsigned char *argv[]);
static int parse_802_1ag_flag(int argc, unsigned char *argv[]);
static int parse_802_1ag_first_tlv(int argc, unsigned char *argv[]);
static int parse_802_1ag_transaction_id(int argc, unsigned char *argv[]);
#endif
static int parse_SubType(int argc, unsigned char *argv[]);
static int parse_PPPoE(int argc, unsigned char *argv[]);
static int parse_PPP(int argc, unsigned char *argv[]);
static int parse_LLC(int argc, unsigned char *argv[]);
static int parse_SNAP(int argc, unsigned char *argv[]);
static int parse_pause_timer(int argc, unsigned char *argv[]);

/*************************************************************
 *         Global Variable
 *************************************************************/
#ifdef EPON
extern char have_ipg,have_llid; 
extern unsigned int llid;
extern unsigned long ipg;
extern char	ipg_random;
extern char ipg_incremental;
extern int	ipg_inc_start;
extern int	ipg_inc_step;
extern int	ipg_counter;	
extern char llid_random;
extern char llid_incremental;
extern int	 llid_inc_start;
extern int	 llid_inc_step;
extern int	 llid_counter;
#endif

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
*				L4				SNAP		...                      
*				Payload			LLC
*----------------------------------------------------------------------*/
KEYWORD_T keyword_L2_tbl[]=
{
	/* Name     		handler 	*/
#ifdef EPON	
	{"IPG",				(void *)parse_IPG},
	{"LLID",			(void *)parse_LLID},
#endif
	{"DA",				(void *)parse_DA},
	{"SA",				(void *)parse_SA},
	{"PRIORITY_SP",		(void *)parse_priority_sp},
    {"CIF_SP",			(void *)parse_cif_sp},
    {"SP_VLAN",			(void *)parse_OUTER_VLAN},    //outer vlan --> SP VLAN
    {"PRIORITY_CE",		(void *)parse_priority_ce},
    {"CIF_CE",			(void *)parse_cif_ce},
    {"CE_VLAN",			(void *)parse_INNER_VLAN},    //inner vlan --> CE VLAN
    {"PRIORITY",		(void *)parse_priority},
    {"CIF",				(void *)parse_cif},
    {"VLAN",			(void *)parse_VLAN},
    {"EtherType",		(void *)parse_EtherType},
#ifdef AG_802_1	    
    {"MD_LEVEL",		(void *)parse_802_1ag_md_level},
    {"VERSION",			(void *)parse_802_1ag_version},
    {"OPCODE",			(void *)parse_802_1ag_opcode},
    {"FLAG",			(void *)parse_802_1ag_flag},
    {"FIRST_TLV",		(void *)parse_802_1ag_first_tlv},
    {"TRANSACTION",		(void *)parse_802_1ag_transaction_id},
#endif    
    {"SubType",			(void *)parse_SubType},
    {"PPPoE",			(void *)parse_PPPoE},
    {"PPP",				(void *)parse_PPP},
    {"LLC",				(void *)parse_LLC},
    {"SNAP",			(void *)parse_SNAP},
	{"timer",			(void *)parse_pause_timer},
    {"L2",				(void *)parse_L2},			// for parsing "L2: end"
    {NULL,				NULL}
};

static unsigned char pause_frame[]={0x01, 0x80, 0xc2, 0x00, 0x00, 0x01};

/*----------------------------------------------------------------------
* parse_L2
*	L2: Ethernet-II | 802_3 | end
*----------------------------------------------------------------------*/
int parse_L2(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (stricmp(argv[1], "end") == 0)
	{
		info->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
		return 0;
	}
	
	info->curr_table= (KEYWORD_T *)&keyword_L2_tbl[0];
		
	if (stricmp(argv[1], "Ethernet-II") == 0)
		info->L2.type = PKT_L2_ETHERNET_II;
	else if (stricmp(argv[1], "802_3") == 0)
		info->L2.type = PKT_L2_802_3;
	else if (stricmp(argv[1], "Pause") == 0)
		info->L2.type = PKT_L2_PAUSE;
	else
		return -1;	
	
	info->pkt.have_L2 = 1;
	return 0;
}

#ifdef EPON
/*----------------------------------------------------------------------
* parse_IPG
	IPG [0x0000000c] or [random] or [inc start step] (inc 0 1 )
*----------------------------------------------------------------------*/
int parse_IPG(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			ipg_random = 1;
			have_ipg = 1;
			ipg_incremental = 0;
			ipg = get_random_number(0, 0xffffffff);
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
				
			ipg = string2value(argv[2]);
			ipg_inc_start = ipg;
			ipg_inc_step = string2value(argv[3]);
			ipg_random = 0;
			ipg_incremental = 1;
			have_ipg = 1;
			return 0;
		}
		
		ipg = (unsigned long)string2value(argv[1]);
		if (ipg >= 0xffffffff)
		{
			printf("Incorrect IPG (%X)!\n", ipg);
			printf("Must be in the range 0 ~ 0xFFFFFFFF\n");
			return -1;
		}
		
		have_ipg = 1;
		return 0;
	}
	return -1;
}

/*----------------------------------------------------------------------
* parse_LLID
	LLID [0x0000000c] or [random] or [inc start step] (inc 0 1 )
*----------------------------------------------------------------------*/
int parse_LLID(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			llid_random = 1;
			have_llid = 1;
			have_llid = 1;
			llid_incremental = 0;
			llid = get_random_number(0, 0xffff);
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
				
			llid = string2value(argv[2]);
			llid_inc_start = llid;
			llid_inc_step = string2value(argv[3]);
			llid_random = 0;
			llid_incremental = 1;
			have_llid = 1;
			return 0;
		}
		
		llid = (unsigned long)string2value(argv[1]);
		if (llid >= 0xffff)
		{
			printf("Incorrect LLID (%X)!\n", llid);
			printf("Must be in the range 0 ~ 0xFFFF\n");
			return -1;
		}
			
		have_llid = 1;
		return 0;
	}
	return -1;
}
#endif
/*----------------------------------------------------------------------
* parse_DA
	DA [00:50:c2:11:22:33] or [random] or [inc start step] (inc 0 1 )
*----------------------------------------------------------------------*/
static int parse_DA(int argc, unsigned char *argv[])
{
	int i;
	DESCRIPTION_INFO_T	*info = pInfo;

	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L2.da_random = 1;
		info->L2.have_da = 1;
		info->L2.da_incremental = 0;
		return 0;
	}

	if (argc == 9 && IS_INCREMENTAL(argv[1]))
	{
		for (i=0; i<6; i++)
			info->L2.da[i] = string2hex(argv[i+2]);
		//info->L2.da_inc_start = info->L2.da[5];	
		info->L2.da_inc_step = string2hex(argv[8]);
		info->L2.da_random = 0;
		info->L2.da_incremental = 1;
		info->L2.have_da = 1;
		return 0;	
	}

	if (argc != 7)
		return -1;
		
	for (i=0; i<6; i++)
		info->L2.da[i] = (unsigned char)string2hex(argv[i+1]);
		
	info->L2.have_da = 1;
#ifdef EPON	
//	if ((have_llid != 1) || (have_ipg != 1))
//	{
//		have_ipg = 0;
//		have_llid = 0;
//		llid = 0;
//		ipg = 0;
//	}	
#endif	
	return 0;
}

/*----------------------------------------------------------------------
* parse_SA
	SA [00:50c2:11:22:33] or [random] or [inc start step] (inc 2 4 )
*----------------------------------------------------------------------*/
static int parse_SA(int argc, unsigned char *argv[])
{
	int i;
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L2.sa_random = 1;
		info->L2.have_sa = 1;
		info->L2.sa_incremental = 0;
		return 0;
	}
#if 1
	if (argc == 9 && IS_INCREMENTAL(argv[1]))
	{
		for (i=0; i<6; i++)
			info->L2.sa[i] = string2hex(argv[i+2]);
		//info->L2.sa_inc_start = string2value(argv[2]);
		info->L2.sa_inc_step = string2value(argv[8]);
		info->L2.sa_random = 0;
		info->L2.sa_incremental = 1;
		info->L2.have_sa = 1;
		return 0;	
	}
#endif
	if (argc != 7)
		return -1;
		
	for (i=0; i<6; i++)
		info->L2.sa[i] = (unsigned char)string2hex(argv[i+1]);
		
	info->L2.have_sa = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_priority_sp
*	PRIORITY_SP [priority_sp] or [random] or [inc start step]
*----------------------------------------------------------------------*/
static int parse_priority_sp(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			info->L2.priority_random_sp = 1;
			info->L2.have_priority_sp = 1;
			info->L2.priority_sp_incremental = 0;
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			info->L2.vlan.priority_sp = string2value(argv[2]);
			info->L2.priority_sp_inc_start = info->L2.vlan.priority_sp;
			info->L2.priority_sp_inc_step = string2value(argv[3]);
			info->L2.priority_random_sp = 0;
			info->L2.have_priority_sp = 1;
			info->L2.priority_sp_incremental = 1;
			info->L2.p_sp_counter ++;
			return 0;
		}
		
		info->L2.vlan.priority_sp = (unsigned char)string2value(argv[1]);
		
		if (info->L2.vlan.priority_sp >= 8)
		{
			printf("Incorrect SP PRIORITY (%d)!\n", info->L2.vlan.priority_sp);
			printf("Must be in the range %d ~ %d\n", 0, 7);
			return -1;
		}
		
		info->L2.have_priority_sp = 1;
		return 0;
	}
	return -1;
}

/*----------------------------------------------------------------------
* parse_cif_sp
*----------------------------------------------------------------------*/
static int parse_cif_sp(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
	
	info->L2.vlan.cif_sp = (unsigned char)string2value(argv[1]);
	if (info->L2.vlan.cif_sp >= 2)
		return -1;
		
	info->L2.have_cif_sp = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_OUTER_VLAN   	outer vlan --> SP VLAN
*----------------------------------------------------------------------*/
static int parse_OUTER_VLAN(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.outer_vlan_id_random = 1;
		info->L2.have_outer_vlan_id = 1;
		return 0;
	}
	
	info->L2.vlan.outer_vlan_id = (unsigned short)string2value(argv[1]);
	if (info->L2.vlan.outer_vlan_id >= 4096)
		return -1;
		
	info->L2.have_outer_vlan_id = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_priority_ce
*	PRIORITY_CE [priority_ce] or [random] or [inc start step]
*----------------------------------------------------------------------*/
static int parse_priority_ce(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			info->L2.priority_random_ce = 1;
			info->L2.have_priority_ce = 1;
			info->L2.priority_ce_incremental = 0;
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			info->L2.vlan.priority_ce = string2value(argv[2]);
			info->L2.priority_ce_inc_start = info->L2.vlan.priority_ce;
			info->L2.priority_ce_inc_step = string2value(argv[3]);
			info->L2.priority_random_ce = 0;
			info->L2.have_priority_ce = 1;
			info->L2.priority_ce_incremental = 1;
			info->L2.p_ce_counter ++;
			return 0;
		}
		
		info->L2.vlan.priority_ce = (unsigned char)string2value(argv[1]);
		
		if (info->L2.vlan.priority_ce >= 8)
		{
			printf("Incorrect CE PRIORITY (%d)!\n", info->L2.vlan.priority_ce);
			printf("Must be in the range %d ~ %d\n", 0, 7);
			return -1;
		}
		
		info->L2.have_priority_ce = 1;
		return 0;
	}
	return -1;
}

/*----------------------------------------------------------------------
* parse_cif_ce
*----------------------------------------------------------------------*/
static int parse_cif_ce(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
	
	info->L2.vlan.cif_ce = (unsigned char)string2value(argv[1]);
	if (info->L2.vlan.cif_ce >= 2)
		return -1;
		
	info->L2.have_cif_ce = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_INNER_VLAN		inner vlan --> CE VLAN
*----------------------------------------------------------------------*/
static int parse_INNER_VLAN(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.inner_vlan_id_random = 1;
		info->L2.have_inner_vlan_id = 1;
		return 0;
	}
	
	info->L2.vlan.inner_vlan_id = (unsigned short)string2value(argv[1]);
	if (info->L2.vlan.inner_vlan_id >= 4096)
		return -1;
		
	info->L2.have_inner_vlan_id = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_priority
*	PRIORITY [priority] or [random] or [inc start step]
*----------------------------------------------------------------------*/
static int parse_priority(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			info->L2.priority_random = 1;
			info->L2.have_priority = 1;
			info->L2.priority_incremental = 0;
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			info->L2.vlan.priority = string2value(argv[2]);
			info->L2.priority_inc_start = info->L2.vlan.priority;
			info->L2.priority_inc_step = string2value(argv[3]);
			info->L2.priority_random = 0;
			info->L2.have_priority = 1;
			info->L2.priority_incremental = 1;
			info->L2.p_counter ++;
			return 0;
		}
		
		info->L2.vlan.priority = (unsigned char)string2value(argv[1]);
		
		if (info->L2.vlan.priority >= 8)
		{
			printf("Incorrect PRIORITY (%d)!\n", info->L2.vlan.priority);
			printf("Must be in the range of %d ~ %d\n", 0, 7);
			return -1;
		}
		
		info->L2.have_priority = 1;
		return 0;
	}
	return -1;
}

/*----------------------------------------------------------------------
* parse_cif
*----------------------------------------------------------------------*/
static int parse_cif(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
	
	info->L2.vlan.cif = (unsigned char)string2value(argv[1]);
	if (info->L2.vlan.cif >= 2)
		return -1;
		
	info->L2.have_cif = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_VLAN
*----------------------------------------------------------------------*/
static int parse_VLAN(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.vlan_id_random = 1;
		info->L2.have_vlan_id = 1;
		return 0;
	}
	
	info->L2.vlan.vlan_id = (unsigned short)string2value(argv[1]);
	if (info->L2.vlan.vlan_id >= 4096)
		return -1;
		
	info->L2.have_vlan_id = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_EtherType
*----------------------------------------------------------------------*/
static int parse_EtherType(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.ether_type_random = 1;
		info->L2.have_ether_type = 1;
		return 0;
	}
	
	info->L2.ether_type = (unsigned short)string2value(argv[1]);
	if (info->L2.ether_type < 0x600)
	{
		printf("Incorrect Ether Type (0x%X)!\n", info->L2.ether_type);
		printf("Ether Type  0x%X must be > 0x600\n",info->L2.ether_type);
		return -1;
	}
		
	info->L2.have_ether_type = 1;
	return 0;
}

#ifdef AG_802_1	
/*----------------------------------------------------------------------
* parse_802_1ag_md_level
* MD LEVEL [md level] or [random] or [inc start step]
*----------------------------------------------------------------------*/
static int parse_802_1ag_md_level(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			info->L2.md_level_random = 1;
			info->L2.have_md_level = 1;
			info->L2.md_level_incremental = 0;
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			info->L2.ag.md_level = string2value(argv[2]);
			info->L2.md_level_inc_start = info->L2.ag.md_level;
			info->L2.md_level_inc_step = string2value(argv[3]);
			info->L2.md_level_random = 0;
			info->L2.have_md_level = 1;
			info->L2.md_level_incremental = 1;
			info->L2.md_level_counter ++;
			return 0;
		}
		
		info->L2.ag.md_level = (unsigned char)string2value(argv[1]);
		
		if ((info->L2.ag.md_level >= 8) || (info->L2.ag.md_level >= 0x8))
		{
			printf("Incorrect MD LEVEL (%d)!\n", info->L2.ag.md_level);
			printf("Must be in the range of %d ~ %d\n", 0, 7);
			return -1;
		}
		
		info->L2.have_md_level = 1;
		return 0;
	}
	return -1;
}

/*----------------------------------------------------------------------
* parse_802_1ag_version
*----------------------------------------------------------------------*/
static int parse_802_1ag_version(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.version_random = 1;
		info->L2.have_verison = 1;
		return 0;
	}
	
	info->L2.ag.version = (unsigned char)string2value(argv[1]);
	if ((info->L2.ag.version  >= 32) || (info->L2.ag.version  >= 0x20))
	{
		printf("Incorrect Version (%d)!\n", info->L2.ag.version);
		printf("Must be in the range of %d ~ %d\n", 0, 32);
		return -1;
	}	
	info->L2.have_verison = 1;
	return 0;
	
}

/*----------------------------------------------------------------------
* parse_802_1ag_opcode
*----------------------------------------------------------------------*/
static int parse_802_1ag_opcode(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.opcode_random = 1;
		info->L2.have_opcode = 1;
		return 0;
	}
	
	info->L2.opcode = (unsigned char)string2value(argv[1]);
	if ((info->L2.opcode >= 256) || (info->L2.opcode >= 0x100))
	{
		printf("Incorrect Opcode (%d)!\n", info->L2.opcode);
		printf("Must be in the range of %d ~ %d\n", 0, 256);
		return -1;
	}	
		
	info->L2.have_opcode = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_802_1ag_flag
*----------------------------------------------------------------------*/
static int parse_802_1ag_flag(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.flag_random = 1;
		info->L2.have_flag = 1;
		return 0;
	}
	
	info->L2.flag = (unsigned char)string2value(argv[1]);
	if (info->L2.flag >= 256)
		return -1;
		
	info->L2.have_flag = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_802_1ag_first_tlv
*----------------------------------------------------------------------*/
static int parse_802_1ag_first_tlv(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.first_tlv_random = 1;
		info->L2.have_first_tlv = 1;
		return 0;
	}
	
	info->L2.first_tlv = (unsigned char)string2value(argv[1]);
	if (info->L2.first_tlv >= 256)
		return -1;
		
	info->L2.have_first_tlv = 1;
	return 0;
}

/*----------------------------------------------------------------------
* parse_802_1ag_transaction_id
* TRANSACTION [TRANSACTION ID] or [random] or [inc start step]
*----------------------------------------------------------------------*/
static int parse_802_1ag_transaction_id(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			info->L2.transaction_id_random = 1;
			info->L2.have_transaction_id = 1;
			info->L2.transaction_id_incremental = 0;
			return 0;
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			info->L2.transaction_id = string2value(argv[2]);
			info->L2.transaction_id_inc_start = info->L2.transaction_id;
			info->L2.transaction_id_inc_step = string2value(argv[3]);
			info->L2.transaction_id_random = 0;
			info->L2.have_transaction_id = 1;
			info->L2.transaction_id_incremental = 1;
			info->L2.transaction_id_counter ++;
			return 0;
		}
			
		info->L2.transaction_id = (unsigned long)string2value(argv[1]);
		
		if (info->L2.transaction_id >= 0xffffffff)
		return -1;
		
		info->L2.have_transaction_id = 1;
		return 0;
	}
	return -1;
	
#if 0	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.transaction_id_random = 1;
		info->L2.have_transaction_id = 1;
		return 0;
	}

	info->L2.transaction_id = (unsigned long)string2value(argv[1]);
	if (info->L2.transaction_id >= 0xffffffff)
		return -1;
		
	info->L2.have_transaction_id = 1;
	return 0;
#endif		
	
}
#endif

/*----------------------------------------------------------------------
* parse_SubType
*----------------------------------------------------------------------*/
static int parse_SubType(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.sub_type_random = 1;
		info->L2.have_sub_type = 1;
		return 0;
	}
	
	info->L2.sub_type = (unsigned short)string2value(argv[1]);
//	if (info->L2.sub_type < 0x600)
//		return -1;
		
	info->L2.have_sub_type = 1;
	return 0;
}
/*----------------------------------------------------------------------
* parse_PPPoE
*	PPPoE: [data-1], [data-2], [data-3]	
*			data-1 : version + Type
*			data-2 : Code
*			data-3 : session id
*----------------------------------------------------------------------*/
static int parse_PPPoE(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L2.pppoe_random = 1;
		info->L2.have_pppoe = 1;
		return 0;
	}
	
	if (argc != (3 + 1))
		return -1;
		
	info->L2.pppoe_ver_type = (unsigned char)string2value(argv[1]);
	info->L2.pppoe_code = (unsigned char)string2value(argv[2]);
	info->L2.pppoe_sid = (unsigned short)string2value(argv[3]);
	
	info->L2.have_pppoe = 1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_PPP
*----------------------------------------------------------------------*/
static int parse_PPP(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.ppp_random = 1;
		info->L2.have_ppp = 1;
		return 0;
	}
	
	info->L2.ppp_protocol = (unsigned short)string2value(argv[1]);
	info->L2.have_ppp = 1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_LLC
*----------------------------------------------------------------------*/
static int parse_LLC(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int i;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L2.llc_random = 1;
		info->L2.have_llc = 1;
		return 0;
	}
	
	if (argc != (LLC_DATA_BYTES + 1))
		return -1;
		
	for (i=0; i<LLC_DATA_BYTES; i++)
		info->L2.llc[i] = (unsigned char)string2value(argv[i+1]);
	
	info->L2.have_llc = 1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_SNAP
*----------------------------------------------------------------------*/
static int parse_SNAP(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	int i;
	
	if (argc == 2 && IS_RANDOM(argv[1]))
	{
		info->L2.snap_random = 1;
		info->L2.have_snap = 1;
		return 0;
	}
	
	if (argc != (SNAP_DATA_BYTES + 1))
		return -1;
		
	for (i=0; i<SNAP_DATA_BYTES; i++)
		info->L2.snap[i] = (unsigned char)string2value(argv[i+1]);
	
	info->L2.have_snap = 1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_pause
*----------------------------------------------------------------------*/
static int parse_pause_timer(int argc, unsigned char *argv[])
{
	DESCRIPTION_INFO_T	*info = pInfo;
	
	if (argc != 2)
		return -1;
		
	if (IS_RANDOM(argv[1]))
	{
		info->L2.pause_timer_random = 1;
		return 0;
	}
	
	info->L2.pause_timer = (unsigned short)string2value(argv[1]);
	
	return 0;
}
/*----------------------------------------------------------------------
* build_L2_packet
*----------------------------------------------------------------------*/
unsigned char *build_L2_packet(DESCRIPTION_INFO_T *info, unsigned char *buf, unsigned int *len)
{
	unsigned char		*cp;
	int					payload_length;
	int					i;
	unsigned short		flag = 0 ,flag_0 = 0 ,flag_1 = 0 ,flag_2 = 0;
	unsigned short		flag_sp = 0,flag_sp_0 = 0,flag_sp_1 = 0,flag_sp_2 = 0;
	unsigned short		flag_ce = 0,flag_ce_0 = 0,flag_ce_1 = 0,flag_ce_2 = 0;
#ifdef AG_802_1	
	unsigned char		flag_ag = 0 ,flag_ag_0 = 0,flag_ag_1 = 0;
#endif //AG_802_1

	payload_length = *len;
	cp = buf;
	
	if (info->L2.type == PKT_L2_PAUSE)
	{
		for (i=0; i<6; i++)
			PUSH_BYTE(cp, pause_frame[i]);
		for (i=0; i<6; i++)
			PUSH_BYTE(cp, 0);
		PUSH_WORD(cp, 0x8808);	// packet type
		PUSH_WORD(cp, 0x0001);	// opcode
		if (info->L2.pause_timer_random)
			info->L2.pause_timer = get_random_number(0, 0xffff);
		
		PUSH_WORD(cp, info->L2.pause_timer);	// timer
		
		*len = 60 - 18;
		info->pkt.length = 60;
		return cp;
	}
	// DA
	if (info->L2.da_incremental == 1 )
	{
		info->L2.da[5] += info->L2.da_inc_step;
		if (info->L2.da[5] > 0xfe)
		{
			info->L2.da[5] = 0xff;
			info->L2.da[4] += info->L2.da_inc_step;
		}
		if (info->L2.da[4] > 0xfe)
		{
			info->L2.da[4] = 0xff;
			info->L2.da[3] += info->L2.da_inc_step;
		}
		
		if (info->L2.da[3] > 0xfe)
		{
			info->L2.da[3] = 0xff;
			info->L2.da[2] += info->L2.da_inc_step;
		}
		
		if (info->L2.da[2] > 0xfe)
		{
			info->L2.da[2] = 0xff;
			info->L2.da[1] += info->L2.da_inc_step;
		}
		
		if (info->L2.da[1] > 0xfe)
		{
			info->L2.da[1] = 0xff;
			info->L2.da[0] += info->L2.da_inc_step;
		}
	}

	if (info->L2.da_random)
	{
		for (i=0; i<6; i++)
			info->L2.da[i] = get_random_number(0, 0xff);
	}
				
		for (i=0; i<6; i++)
			PUSH_BYTE(cp, info->L2.da[i]);
	
	// SA
	if (info->L2.sa_incremental == 1)
	{
		info->L2.sa[5] += info->L2.sa_inc_step;
		if (info->L2.sa[5] > 0xfe)
		{
			info->L2.sa[5] = 0xff;
			info->L2.sa[4] += info->L2.sa_inc_step;
		}
		if (info->L2.sa[4] > 0xfe)
		{
			info->L2.sa[4] = 0xff;
			info->L2.sa[3] += info->L2.sa_inc_step;
		}
		
		if (info->L2.sa[3] > 0xfe)
		{
			info->L2.sa[3] = 0xff;
			info->L2.sa[2] += info->L2.sa_inc_step;
		}
		
		if (info->L2.sa[2] > 0xfe)
		{
			info->L2.sa[2] = 0xff;
			info->L2.sa[1] += info->L2.sa_inc_step;
		}
		
		if (info->L2.sa[1] > 0xfe)
		{
			info->L2.sa[1] = 0xff;
			info->L2.sa[0] += info->L2.sa_inc_step;
		}
	}
	if (info->L2.sa_random)
	{
		for (i=0; i<6; i++)
			info->L2.sa[i] = get_random_number(0, 0xff);
	}
	for (i=0; i<6; i++)
		PUSH_BYTE(cp, info->L2.sa[i]);
		
	payload_length -= 12;
	
	//parse priority_sp 3 bits
	if (info->L2.have_priority_sp)
	{
		if (info->L2.priority_random_sp)
		{
			info->L2.vlan.priority_sp = get_random_number(0, 0x7);
		}
		else if (info->L2.priority_sp_incremental)
		{
			if (info->L2.p_sp_counter != 1)
				info->L2.vlan.priority_sp += info->L2.priority_sp_inc_step;
			else
				info->L2.vlan.priority_sp = info->L2.priority_sp_inc_start;	
			if (info->L2.vlan.priority_sp > 8)
				info->L2.vlan.priority_sp = info->L2.priority_sp_inc_start;
			info->L2.p_sp_counter = 0;	
		}
			flag_sp_0 = (info->L2.vlan.priority_sp << 13) & 0xf000 ;
	}
	
	
		
	//parse cif 1 bit
	if (info->L2.have_cif_sp)
	{
			flag_sp_2 = (info->L2.vlan.cif_sp << 12) & 0x1000 ;
	}
	
	
	//OUTER VLAN/SP_VLAN
	if (info->L2.have_outer_vlan_id)
	{
		PUSH_WORD(cp, SP_VLAN_TPID);
//		PUSH_WORD(cp, VLAN_TPID);
		if (info->L2.outer_vlan_id_random)
			info->L2.vlan.outer_vlan_id = get_random_number(0, 0x0fff);
		
		flag_sp_1=	 (info->L2.vlan.outer_vlan_id << 0) & 0x0fff ;
		flag_sp = flag_sp_0 | flag_sp_1 | flag_sp_2;
		PUSH_WORD(cp, flag_sp);
		payload_length -= 4;
	}
	
	//parse priority_ce 3 bits
	if (info->L2.have_priority_ce)
	{
	
		if (info->L2.priority_random_ce)
		{
			info->L2.vlan.priority_ce = get_random_number(0, 0x7);
		}
		else if (info->L2.priority_ce_incremental)
		{
			if (info->L2.p_ce_counter != 1)
				info->L2.vlan.priority_ce += info->L2.priority_ce_inc_step;
			else
				info->L2.vlan.priority_ce = info->L2.priority_ce_inc_start;
			if (info->L2.vlan.priority_ce > 8)
				info->L2.vlan.priority_ce = info->L2.priority_ce_inc_start;
			info->L2.p_ce_counter = 0;	
		}
			flag_ce_0 = (info->L2.vlan.priority_ce << 13) & 0xf000 ;
	}
	
	//parse cif 1 bit
	if (info->L2.have_cif_ce)
	{
			flag_ce_2 = (info->L2.vlan.cif_ce << 12) & 0x1000 ;
	}
	
	//INNER VLAN/CE_VLAN
	if (info->L2.have_inner_vlan_id)
	{
		PUSH_WORD(cp, VLAN_TPID);
		if (info->L2.inner_vlan_id_random)
			info->L2.vlan.inner_vlan_id = get_random_number(0, 0x0fff);
			
		flag_ce_1=	 (info->L2.vlan.inner_vlan_id << 0) & 0x0fff ;
		flag_ce = flag_ce_0 | flag_ce_1 | flag_ce_2;
		PUSH_WORD(cp, flag_ce);
		payload_length -= 4;
	}
	
	//parse priority 3 bits
	if (info->L2.have_priority)
	{
		if (info->L2.priority_random)
		{
			info->L2.vlan.priority = get_random_number(0, 0x7);
		}
		else if (info->L2.priority_incremental)
		{
			if (info->L2.p_counter != 1)
				info->L2.vlan.priority += info->L2.priority_inc_step;
			else
				info->L2.vlan.priority = info->L2.priority_inc_start;
			if (info->L2.vlan.priority > 8)
				info->L2.vlan.priority = info->L2.priority_inc_start;
			info->L2.p_counter = 0;
		}
		
			flag_0 = (info->L2.vlan.priority << 13) & 0xf000 ;
	}
	
	//parse cif 1 bit
	if (info->L2.have_cif)
	{
			flag_2 = (info->L2.vlan.cif << 12) & 0x1000 ;
	}
	
	//parse VLAN 12 bits
	if (info->L2.have_vlan_id )
	{
		PUSH_WORD(cp, VLAN_TPID);
		if (info->L2.vlan_id_random)
			info->L2.vlan.vlan_id = get_random_number(0, 0x0fff);
			
		flag_1=	 (info->L2.vlan.vlan_id << 0) & 0x0fff ;
		
		flag = flag_0 | flag_1 | flag_2;
		PUSH_WORD(cp, flag);
		payload_length -= 4;
		
	}
	
	switch (info->L2.type)
	{
		case PKT_L2_ETHERNET_II:
			{
				if (info->L2.ether_type_random)
					info->L2.ether_type = get_random_number(0x600, 0xffff);
				PUSH_WORD(cp, info->L2.ether_type);
				payload_length -= 2;
				
			}
			break;
		case PKT_L2_802_3:
			{
				payload_length -= 2;
				PUSH_WORD(cp, payload_length);
				if (info->L2.have_llc)
				{
					if (info->L2.llc_random)
					{
						for (i=0; i<LLC_DATA_BYTES; i++)
							  info->L2.llc[i] = get_random_number(0, 0xff);
					}
					memcpy((char *)cp, (char *)info->L2.llc, LLC_DATA_BYTES);
					cp += LLC_DATA_BYTES;
					payload_length -= LLC_DATA_BYTES;
				}
				if (info->L2.have_snap)
				{
					if (info->L2.snap_random)
					{
						for (i=0; i<SNAP_DATA_BYTES; i++)
							  info->L2.snap[i] = get_random_number(0, 0xff);
					}
					memcpy((char *)cp, (char *)info->L2.snap, SNAP_DATA_BYTES);
					cp += SNAP_DATA_BYTES;
					payload_length -= SNAP_DATA_BYTES;
					
					if (info->L2.ether_type_random)
						info->L2.ether_type = get_random_number(0x600, 0xffff);
					PUSH_WORD(cp, info->L2.ether_type);
					payload_length -= 2;
				}
			}
			break;
	}
	
#ifdef AG_802_1	
	//parse md level 3 bits
	if (info->L2.have_md_level)
	{
		if (info->L2.md_level_random)
		{
			info->L2.ag.md_level = get_random_number(0, 0x7);
		}
		else if (info->L2.md_level_incremental)
		{
			if (info->L2.md_level_counter != 1)
				info->L2.ag.md_level += info->L2.md_level_inc_step;
			else
				info->L2.ag.md_level = info->L2.md_level_inc_start;
			if (info->L2.ag.md_level > 8)
				info->L2.ag.md_level = info->L2.md_level_inc_start;
			info->L2.md_level_counter = 0;
		}
		
			flag_ag_0 = (info->L2.ag.md_level << 5) & 0xe0;
	}
	
	//parse Version 5 bits
	if (info->L2.have_verison )
	{
		if (info->L2.version_random)
			info->L2.ag.version = get_random_number(0, 0xff);
			
		flag_ag_1 = (info->L2.ag.version << 0) & 0x1f;
		
		flag_ag = flag_ag_0 | flag_ag_1 ;
//		printf("flag_ag_0 = 0x%X,flag_ag_1 = 0x%X,flag_ag = 0x%X\n",flag_ag_0,flag_ag_1,flag_ag);
		PUSH_BYTE(cp, flag_ag);
		payload_length -= 2;
	}
	
	if (info->L2.have_opcode )
	{
		if (info->L2.opcode_random)
			info->L2.opcode = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L2.opcode);
		payload_length -= 2;
	}
	
	if (info->L2.have_flag )
	{
		if (info->L2.flag_random)
			info->L2.flag = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L2.flag);
		payload_length -= 2;
	}
	
	if (info->L2.have_first_tlv )
	{
		if (info->L2.first_tlv_random)
			info->L2.first_tlv = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L2.first_tlv);
		payload_length -= 2;
	}
	
	//parse transaction id
	if (info->L2.have_transaction_id)
	{
		if (info->L2.transaction_id_random)
		{
			info->L2.transaction_id = get_random_number(0, 0xffffffff);
		}
		else if (info->L2.transaction_id_incremental)
		{
			if (info->L2.transaction_id_counter != 1)
				info->L2.transaction_id += info->L2.transaction_id_inc_step;
			else
				info->L2.transaction_id = info->L2.transaction_id_inc_start;
			if (info->L2.transaction_id > 0xfffffffe)
				info->L2.transaction_id = info->L2.transaction_id_inc_start;
			info->L2.transaction_id_counter = 0;
		}
		
		PUSH_DWORD(cp, info->L2.transaction_id);
		payload_length -= 4;
	}
	
#endif	
	
	if (info->L2.have_sub_type)
	{
		if (info->L2.pppoe_random)
			info->L2.sub_type_random = get_random_number(0, 0xff);
		PUSH_BYTE(cp, info->L2.sub_type);
		payload_length -= 2;
	}
	if (info->L2.have_pppoe)
	{
		if (info->L2.pppoe_random)
		{
			info->L2.pppoe_ver_type = get_random_number(0, 0xff);
			info->L2.pppoe_code = get_random_number(0, 0xff);
			info->L2.pppoe_sid = get_random_number(0, 0xffff);
		}
		PUSH_BYTE(cp, info->L2.pppoe_ver_type);
		PUSH_BYTE(cp, info->L2.pppoe_code);
		PUSH_WORD(cp, info->L2.pppoe_sid);
		
		payload_length -= 6;	// ver-type, code, sid, & length
		PUSH_WORD(cp, payload_length);
		
		if (info->L2.have_ppp)
		{
			if (info->L2.ppp_random)
				info->L2.ppp_protocol = get_random_number(0, 0xffff);
			PUSH_WORD(cp, info->L2.ppp_protocol);
			payload_length -= 2;
		}
	}
		
	*len = payload_length;
	
	return cp;
}

