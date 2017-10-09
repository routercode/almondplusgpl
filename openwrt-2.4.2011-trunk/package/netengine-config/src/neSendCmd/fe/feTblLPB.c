/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblLPB.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
                 fe module LPB Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"

/*
 * fe->LPB(fe_lpb_entry_t)
 */
int fe_tbl_LPB_lspid(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.lspid = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_lspid() */
int fe_tbl_LPB_pvid(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.pvid = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_pvid() */
int fe_tbl_LPB_pvid_tpid_enc(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.pvid_tpid_enc = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_pvid_tpid_enc() */
int fe_tbl_LPB_olspid_en(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.olspid_en = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_olspid_en() */
int fe_tbl_LPB_olspid(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.olspid = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_olspid() */
int fe_tbl_LPB_olspid_preserve_en(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.olspid_preserve_en =
			(cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_olspid_preserve_en() */
int fe_tbl_LPB_parity(char *pString, LPB_INFO * pLPBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pLPBInfo->tbl_LPB.parity = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_LPB_parity() */

/*====================================================================== */

NE_FIELD_T fe_tbl_LPB[]	= {
	{"lspid",			fe_tbl_LPB_lspid},
	{"pvid",			fe_tbl_LPB_pvid},
	{"pvid_tpid_enc",		fe_tbl_LPB_pvid_tpid_enc},
	{"olspid_en",			fe_tbl_LPB_olspid_en},
	{"olspid",			fe_tbl_LPB_olspid},
	{"olspid_preserve_en",		fe_tbl_LPB_olspid_preserve_en},
	{"parity",			fe_tbl_LPB_parity},

	{0,				0}
};
