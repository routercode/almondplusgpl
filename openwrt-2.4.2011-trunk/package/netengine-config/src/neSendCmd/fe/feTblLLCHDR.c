/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblLLCHDR.c
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
int fe_tbl_LLCHDR_llc_hdr(char *pString, LLC_HDR_INFO * pLLCHDRInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pLLCHDRInfo->tbl_LLCHDR.llc_hdr = (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
}				/* end fe_tbl_LLCHDR_llc_hdr() */
int fe_tbl_LLCHDR_valid(char *pString, LLC_HDR_INFO * pLLCHDRInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pLLCHDRInfo->tbl_LLCHDR.valid = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
}				/* end fe_tbl_LLCHDR_valid() */

/*==========================================================================*/
NE_FIELD_T fe_tbl_LLC_HDR[] = {
	{"llc_hdr", fe_tbl_LLCHDR_llc_hdr}
	,
	{"valid", fe_tbl_LLCHDR_valid}
	,
	{0, 0}
};
