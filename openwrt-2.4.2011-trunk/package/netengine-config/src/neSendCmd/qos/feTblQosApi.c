/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblQosApi.c
   Date        : 2012-02-29
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module Ingress Qos API Table
   Author      : Eric Wang <eric.wang@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/ 
    
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ne_defs.h>
#include "../neSendCmd.h"
#include "../misc.h"
char *ModuleDef[] = { "ni", "fe", "tm", "sch", "qm", "inqos", 0 };

INGRESS_QOS_SUB_CMD_T inqos_subcmd_def[] = {
	{"set_mode",			CS_QOS_INGRESS_SET_MODE},
	{"get_mode",			CS_QOS_INGRESS_GET_MODE},
	{"print_mode",			CS_QOS_INGRESS_PRINT_MODE},
	
	{"set_port_param",		CS_QOS_INGRESS_SET_PORT_PARAM},
	{"get_port_param",		CS_QOS_INGRESS_GET_PORT_PARAM},
	{"print_port_param",		CS_QOS_INGRESS_PRINT_PORT_PARAM},
	
	{"set_queue_scheduler",		CS_QOS_INGRESS_SET_QUEUE_SCHEDULER},
	{"get_queue_scheduler",		CS_QOS_INGRESS_GET_QUEUE_SCHEDULER},
	{"print_queue_scheduler",	CS_QOS_INGRESS_PRINT_QUEUE_SCHEDULER},
	{"print_queue_scheduler_of_port", CS_QOS_INGRESS_PRINT_QUEUE_SCHEDULER_OF_PORT},
	
	{"set_queue_size",		CS_QOS_INGRESS_SET_QUEUE_SIZE},
	{"get_queue_size",		CS_QOS_INGRESS_GET_QUEUE_SIZE},
	{"print_queue_size",		CS_QOS_INGRESS_PRINT_QUEUE_SIZE},
	{"print_queue_size_of_port",	CS_QOS_INGRESS_PRINT_QUEUE_SIZE_OF_PORT},
	
	{"set_value_queue_mapping",	CS_QOS_INGRESS_SET_VALUE_QUEUE_MAPPING},
	{"get_value_queue_mapping",	CS_QOS_INGRESS_GET_VALUE_QUEUE_MAPPING},
	{"print_value_queue_mapping",	CS_QOS_INGRESS_PRINT_VALUE_QUEUE_MAPPING},

	{"set_arp_policer",		CS_QOS_INGRESS_SET_ARP_POLICER},
	{"reset_arp_policer",		CS_QOS_INGRESS_RESET_ARP_POLICER},
	{"get_arp_policer",		CS_QOS_INGRESS_GET_ARP_POLICER},
	{"print_arp_policer",		CS_QOS_INGRESS_PRINT_ARP_POLICER},

	{"set_pkt_type_policer",	CS_QOS_INGRESS_SET_PKT_TYPE_POL},
	{"reset_pkt_type_policer",	CS_QOS_INGRESS_RESET_PKT_TYPE_POL},
	{"get_pkt_type_policer",	CS_QOS_INGRESS_GET_PKT_TYPE_POL},
	{"print_pkt_type_policer",	CS_QOS_INGRESS_PRINT_PKT_TYPE_POL},
	{"print_pkt_type_policer_port",	CS_QOS_INGRESS_PRINT_PKT_TYPE_POL_PORT},
	{"print_pkt_type_policer_all",	CS_QOS_INGRESS_PRINT_PKT_TYPE_POL_ALL},

	{NULL,				0}
};

int inqos_api_subcmd(char *pString,
		     INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info) 
{
	int i;
	inqos_api_tbl_info->tbl_inqos_api.sub_cmd = CS_QOS_INGRESS_MAX;
	for (i = 0; inqos_subcmd_def[i].name != NULL; i++) {
		if (strcmp(pString, inqos_subcmd_def[i].name) == 0) {
			inqos_api_tbl_info->tbl_inqos_api.sub_cmd =
			    inqos_subcmd_def[i].id;
			break;
		}
	}
	if (inqos_api_tbl_info->tbl_inqos_api.sub_cmd >= CS_QOS_INGRESS_MAX)
		return STATUS_ERR_PARA_GETVALUE;
	return STATUS_SUCCESS;
} /* inqos_api_subcmd() */


/***** MODE *****/ 
int inqos_api_mode(char *pString,
		   INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.mode.mode =
		(cs_uint8)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_mode() */


/***** PORT_PARAM *****/ 
int inqos_api_port_param_port_id(char *pString,
				 INGRESS_QOS_API_TABLE_INFO *
				 inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.port_param.port_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_port_param_port_id() */
int inqos_api_port_param_burst_size(char *pString,
				    INGRESS_QOS_API_TABLE_INFO *
				    inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.port_param.burst_size =
	    (cs_uint16) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_port_param_burst_size() */
int inqos_api_port_param_rate(char *pString,
				INGRESS_QOS_API_TABLE_INFO *
				inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.port_param.rate =
	    (cs_uint32) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_port_param_rate() */


/***** QUEUE_SCHEDULER  *****/ 
int inqos_api_queue_sch_port_id(char *pString,
				INGRESS_QOS_API_TABLE_INFO *
				inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_scheduler.port_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_sch_port_id() */
int inqos_api_queue_sch_queue_id(char *pString,
				   INGRESS_QOS_API_TABLE_INFO *
				   inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_scheduler.queue_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_sch_queue_id() */
int inqos_api_queue_sch_priority(char *pString,
				   INGRESS_QOS_API_TABLE_INFO *
				   inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_scheduler.priority =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_sch_priority() */
int inqos_api_queue_sch_weight(char *pString,
				 INGRESS_QOS_API_TABLE_INFO *
				 inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_scheduler.weight =
	    (cs_uint32) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_sch_weight() */
int inqos_api_queue_sch_rate(char *pString,
			       INGRESS_QOS_API_TABLE_INFO *
			       inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_scheduler.rate =
	    (cs_uint32) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_sch_rate() */


/***** QUEUE_SIZE  *****/ 
int inqos_api_queue_size_port_id(char *pString,
				 INGRESS_QOS_API_TABLE_INFO *
				 inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_size.port_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_size_port_id() */
int inqos_api_queue_size_queue_id(char *pString,
				    INGRESS_QOS_API_TABLE_INFO *
				    inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_size.queue_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_size_queue_id() */
int inqos_api_queue_size_rsrv_size(char *pString,
				     INGRESS_QOS_API_TABLE_INFO *
				     inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_size.rsrv_size =
	    (cs_uint32) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_size_rsrv_size() */
int inqos_api_queue_size_max_size(char *pString,
				    INGRESS_QOS_API_TABLE_INFO *
				    inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_size.max_size =
	    (cs_uint32) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_size_max_size() */


/***** QUEUE_MAPPING  *****/ 
int inqos_api_queue_mapping_value(char *pString,
				  INGRESS_QOS_API_TABLE_INFO *
				  inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_mapping.value =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_mapping_value() */
int inqos_api_queue_mapping_queue_id(char *pString,
				       INGRESS_QOS_API_TABLE_INFO *
				       inqos_api_tbl_info) 
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.queue_mapping.queue_id =
	    (cs_uint8) strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
}				/* inqos_api_queue_mapping_queue_id() */


/***** ARP_POLICER *****/
int inqos_api_arp_policer_cir(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.arp_policer.cir =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_arp_policer_cir */
int inqos_api_arp_policer_cbs(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.arp_policer.cbs =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_arp_policer_cbs */
int inqos_api_arp_policer_pir(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.arp_policer.pir =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_arp_policer_pir */
int inqos_api_arp_policer_pbs(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.arp_policer.pbs =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_arp_policer_pbs */

/***** PKT_TYPE_POLICER *****/
int inqos_api_pkt_type_policer_portid(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.port_id =
		(cs_uint8)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_portid */
int inqos_api_pkt_type_policer_pkt_type(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.pkt_type =
		(cs_uint8)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_pkt_type */
int inqos_api_pkt_type_policer_cir(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.cir =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_cir */
int inqos_api_pkt_type_policer_cbs(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.cbs =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_cbs */
int inqos_api_pkt_type_policer_pir(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.pir =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_pir */
int inqos_api_pkt_type_policer_pbs(char *pString,
		INGRESS_QOS_API_TABLE_INFO *inqos_api_tbl_info)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	inqos_api_tbl_info->tbl_inqos_api.pkt_type_policer.pbs =
		(cs_uint32)strtoul(pString, NULL, 0);;
	return STATUS_SUCCESS;
} /* inqos_api_pkt_type_policer_pbs */

NE_FIELD_T inqos_qos_api[] = {
	{"api",				inqos_api_subcmd},
		
	{"mode",			inqos_api_mode},
		
	{"port_param.port_id",		inqos_api_port_param_port_id},
	{"port_param.burst_size",	inqos_api_port_param_burst_size},
	{"port_param.rate",		inqos_api_port_param_rate},
	
	{"queue_sch.port_id",		inqos_api_queue_sch_port_id},
	{"queue_sch.queue_id",		inqos_api_queue_sch_queue_id},
	{"queue_sch.priority",		inqos_api_queue_sch_priority},
	{"queue_sch.weight",		inqos_api_queue_sch_weight},
	{"queue_sch.rate",		inqos_api_queue_sch_rate},
		
	{"queue_size.port_id",		inqos_api_queue_size_port_id},
	{"queue_size.queue_id",		inqos_api_queue_size_queue_id},
	{"queue_size.rsrv_size",	inqos_api_queue_size_rsrv_size},
	{"queue_size.max_size",		inqos_api_queue_size_max_size},
	
	{"queue_mapping.value",		inqos_api_queue_mapping_value},
	{"queue_mapping.queue_id",	inqos_api_queue_mapping_queue_id},

	{"arp_policer.cir",		inqos_api_arp_policer_cir},
	{"arp_policer.cbs",		inqos_api_arp_policer_cbs},
	{"arp_policer.pir",		inqos_api_arp_policer_pir},
	{"arp_policer.pbs",		inqos_api_arp_policer_pbs},

	{"pkt_type_policer.port_id",	inqos_api_pkt_type_policer_portid},
	{"pkt_type_policer.pkt_type",	inqos_api_pkt_type_policer_pkt_type},
	{"pkt_type_policer.cir",	inqos_api_pkt_type_policer_cir},
	{"pkt_type_policer.cbs",	inqos_api_pkt_type_policer_cbs},
	{"pkt_type_policer.pir",	inqos_api_pkt_type_policer_pir},
	{"pkt_type_policer.pbs",	inqos_api_pkt_type_policer_pbs},

	{0,				0}
};

