/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblL3IP.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module L3 IP Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->L3_IP(fe_l3_addr_entry_s) */

int fe_tbl_L3IP_ip_addr_0(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.ip_addr[0] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_ip_addr_0() */
int fe_tbl_L3IP_ip_addr_1(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.ip_addr[1] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_ip_addr_1() */
int fe_tbl_L3IP_ip_addr_2(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.ip_addr[2] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_ip_addr_2() */
int fe_tbl_L3IP_ip_addr_3(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.ip_addr[3] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_ip_addr_3() */
int fe_tbl_L3IP_count_0(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.count[0] = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_count_0() */
int fe_tbl_L3IP_count_1(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.count[1] = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_count_1() */
int fe_tbl_L3IP_count_2(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.count[2] = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_count_2() */
int fe_tbl_L3IP_count_3(char *pString, L3_IP_INFO * pL3IPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL3IPInfo->tbl_L3IP.count[3] = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L3IP_count_3() */

/*==================================================================== */

NE_FIELD_T fe_tbl_L3_IP[] = {
	{"ip_addr.0",                           fe_tbl_L3IP_ip_addr_0},
	{"ip_addr.1",                           fe_tbl_L3IP_ip_addr_1},
	{"ip_addr.2",                           fe_tbl_L3IP_ip_addr_2},
	{"ip_addr.3",                           fe_tbl_L3IP_ip_addr_3},
	{"count.0",                             fe_tbl_L3IP_count_0},
	{"count.1",                             fe_tbl_L3IP_count_1},
	{"count.2",                             fe_tbl_L3IP_count_2},
	{"count.3",                             fe_tbl_L3IP_count_3},

	{0,                                     0}
};
