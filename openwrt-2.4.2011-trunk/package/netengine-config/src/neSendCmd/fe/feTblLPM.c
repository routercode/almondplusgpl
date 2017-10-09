/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblLPM.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
	             fe module LPM Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->LPM-->fe_lpm_entry_s */

int fe_tbl_LPM_ip_addr_0(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.ip_addr[0] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_ip_addr_0() */
int fe_tbl_LPM_ip_addr_1(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.ip_addr[1] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_ip_addr_1() */
int fe_tbl_LPM_ip_addr_2(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.ip_addr[2] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_ip_addr_2() */
int fe_tbl_LPM_ip_addr_3(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.ip_addr[3] = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_ip_addr_3() */

int fe_tbl_LPM_mask(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.mask = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_mask() */

int fe_tbl_LPM_priority(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.priority = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_priority() */

int fe_tbl_LPM_result_idx(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.result_idx = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_result_idx() */

int fe_tbl_LPM_ipv6(char *pString, LPM_INFO * pLPMInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPMInfo->tbl_LPM.ipv6 = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPM_ipv6() */


/*==================================================================== */

NE_FIELD_T fe_tbl_LPM[] = {
	{"ip_addr.0",                           fe_tbl_LPM_ip_addr_0},
	{"ip_addr.1",                           fe_tbl_LPM_ip_addr_1},
	{"ip_addr.2",                           fe_tbl_LPM_ip_addr_2},
	{"ip_addr.3",                           fe_tbl_LPM_ip_addr_3},
	{"mask",                              	fe_tbl_LPM_mask},
	{"priority",                          	fe_tbl_LPM_priority},
	{"result_idx",                        	fe_tbl_LPM_result_idx},
	{"ipv6",                        		fe_tbl_LPM_ipv6},
	{0,                                     0}
};

