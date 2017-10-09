/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblETYPE.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module ACL Action Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->ETYPE(fe_eth_type_entry_s) */

int fe_tbl_ETYPE_ether_type(char *pString, ETYPE_INFO * pETYPEInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pETYPEInfo->tbl_ETYPE.ether_type =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ETYPE_ether_type() */
int fe_tbl_ETYPE_valid(char *pString, ETYPE_INFO * pETYPEInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pETYPEInfo->tbl_ETYPE.valid = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ETYPE_valid() */

/*==================================================================== */

NE_FIELD_T fe_tbl_ETYPE[] = {
	{"ether_type",                          fe_tbl_ETYPE_ether_type},
	{"valid",                               fe_tbl_ETYPE_valid},

	{0,                                     0}
};
