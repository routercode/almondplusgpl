/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblHash_Match.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
	             fe module Hash Match Table
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
 * fe->Hash_Match->fe_hash_entry_s
 */
int fe_tbl_HashMatch_crc32_0(char *pString, HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.crc32_0 =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_crc32_0() */
int fe_tbl_HashMatch_crc32_1(char *pString, HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.crc32_1 =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_crc32_1() */
int fe_tbl_HashMatch_result_index0(char *pString,
				   HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.result_index0 =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_result_index0() */
int fe_tbl_HashMatch_result_index1(char *pString,
				   HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.result_index1 =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_result_index1() */
int fe_tbl_HashMatch_mask_ptr0(char *pString, HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.mask_ptr0 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_mask_ptr0() */
int fe_tbl_HashMatch_mask_ptr1(char *pString, HASH_MATCH_INFO * pHashMatchInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashMatchInfo->tbl_HashMatch.mask_ptr1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMatch_mask_ptr1() */

/*====================================================================== */

NE_FIELD_T fe_tbl_Hash_Match[] = {
	{"crc32_0",			fe_tbl_HashMatch_crc32_0},
	{"crc32_1",			fe_tbl_HashMatch_crc32_1},
	{"result_index0",		fe_tbl_HashMatch_result_index0},
	{"result_index1",		fe_tbl_HashMatch_result_index1},
	{"mask_ptr0",			fe_tbl_HashMatch_mask_ptr0},
	{"mask_ptr1",			fe_tbl_HashMatch_mask_ptr1},

	{0,				0}
};
