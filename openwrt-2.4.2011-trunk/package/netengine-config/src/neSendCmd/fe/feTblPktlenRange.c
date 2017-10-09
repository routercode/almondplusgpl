/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblPktlenRange.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module Port Range Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->Port_Range(fe_port_range_entry_s) */

int fe_tbl_PktlenRange_low(char *pString, PKTLEN_RANGE_INFO * pPktlenRangeInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pPktlenRangeInfo->tbl_PktlenRange.low =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_PortRange_sp_dp_low() */
int fe_tbl_PktlenRange_high(char *pString, PKTLEN_RANGE_INFO * pPktlenRangeInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pPktlenRangeInfo->tbl_PktlenRange.high =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_PortRange_sp_dp_high() */
int fe_tbl_PktlenRange_valid(char *pString, PKTLEN_RANGE_INFO * pPktlenRangeInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pPktlenRangeInfo->tbl_PktlenRange.valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_PortRange_valid() */

/*==================================================================== */

NE_FIELD_T fe_tbl_Pktlen_Range[] = {
	{"pktlen_low",                           fe_tbl_PktlenRange_low},
	{"pktlen_high",                          fe_tbl_PktlenRange_high},
	{"valid",                                fe_tbl_PktlenRange_valid},

	{0,                                     0}
};

