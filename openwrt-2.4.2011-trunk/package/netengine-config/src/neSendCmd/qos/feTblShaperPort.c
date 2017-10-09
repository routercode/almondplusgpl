/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblShaperport.c
   Date        : 2011-10-03
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module Shaper Port Table
   Author      : Ethan Chen <ethan.chen@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->QOS_SHAPER_PORT(fe_ingress_qos_shaper_port_entry_s) */


int fe_tbl_QOS_SHAPER_PORT_port_id(char *pString, INGRESS_QOS_SHAPER_PORT_TABLE_INFO * pSHAPERPortInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERPortInfo->tbl_SHAPER_PORTTABLE.port_id = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_SHAPER_PORT_port_id() */
int fe_tbl_QOS_SHAPER_PORT_st_rate(char *pString, INGRESS_QOS_SHAPER_PORT_TABLE_INFO * pSHAPERPortInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERPortInfo->tbl_SHAPER_PORTTABLE.st_rate = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* fe_tbl_QOS_SHAPER_PORT_st_rate() */

int fe_tbl_QOS_SHAPER_PORT_lt_rate(char *pString, INGRESS_QOS_SHAPER_PORT_TABLE_INFO * pSHAPERPortInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERPortInfo->tbl_SHAPER_PORTTABLE.lt_rate =(cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_SHAPER_PORT_lt_rate() */

int fe_tbl_QOS_SHAPER_PORT_burst_size(char *pString, INGRESS_QOS_SHAPER_PORT_TABLE_INFO * pSHAPERPortInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERPortInfo->tbl_SHAPER_PORTTABLE.burst_size =(cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_SHAPER_PORT_burst_size() */
/*==================================================================== */

	
NE_FIELD_T fe_tbl_QOS_SHAPER_PORT[] = {
	{"port_id",                       fe_tbl_QOS_SHAPER_PORT_port_id},
	{"st_rate",                				fe_tbl_QOS_SHAPER_PORT_st_rate},
	{"lt_rate",                       fe_tbl_QOS_SHAPER_PORT_lt_rate},	
	{"burst_size",                    fe_tbl_QOS_SHAPER_PORT_burst_size},
	{0,                                     0}
};
