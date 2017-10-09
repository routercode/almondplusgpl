/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblHashCheck.c
   Date        : 2012-12-18
   Description : Cortina GoldenGate NetEngine configuration utility process
	             fe module Hash Check Table
   Author      : Eric Wang <eric.wang@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"

/*
 * fe->Hash_Check->fe_hash_check_entry_s
 */
int fe_tbl_HashCheck_l4_sp_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_l4_sp_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_l4_sp_en() */
int fe_tbl_HashCheck_l4_dp_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_l4_dp_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_l4_dp_en() */
int fe_tbl_HashCheck_mac_sa_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_mac_sa_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_mac_sa_en() */
int fe_tbl_HashCheck_mac_da_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_mac_da_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_mac_da_en() */
int fe_tbl_HashCheck_ip_sa_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_ip_sa_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_ip_sa_en() */
int fe_tbl_HashCheck_ip_da_en(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_ip_da_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_ip_da_en() */
int fe_tbl_HashCheck_l4_sp_to_be_chk(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_l4_sp_to_be_chk =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_l4_sp_to_be_chk() */
int fe_tbl_HashCheck_l4_dp_to_be_chk(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_l4_dp_to_be_chk =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_l4_dp_to_be_chk() */
int fe_tbl_HashCheck_l2_chk_idx(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_l2_check_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_l2_chk_idx() */
int fe_tbl_HashCheck_ip_sa_check_idx(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_ip_sa_check_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_ip_sa_check_idx() */
int fe_tbl_HashCheck_ip_da_check_idx(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_ip_da_check_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_ip_da_check_idx() */
int fe_tbl_HashCheck_parity(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_parity() */
int fe_tbl_HashCheck_reserved(char *pString, HASH_CHECK_INFO * pHashCheckInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pHashCheckInfo->tbl_HashCheck.check_reserved =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashCheck_reserved() */






/*====================================================================== */

NE_FIELD_T fe_tbl_Hash_Check[] = {
	{"l4_sp_en",			fe_tbl_HashCheck_l4_sp_en},
	{"l4_dp_en",			fe_tbl_HashCheck_l4_dp_en},
	{"mac_sa_en",			fe_tbl_HashCheck_mac_sa_en},
	{"mac_da_en",			fe_tbl_HashCheck_mac_da_en},
	{"ip_sa_en",			fe_tbl_HashCheck_ip_sa_en},
	{"ip_da_en",			fe_tbl_HashCheck_ip_da_en},
	{"l4_sp_to_be_chk",		fe_tbl_HashCheck_l4_sp_to_be_chk},
	{"l4_dp_to_be_chk",		fe_tbl_HashCheck_l4_dp_to_be_chk},
	{"l2_chk_idx",			fe_tbl_HashCheck_l2_chk_idx},
	{"ip_sa_chk_idx",		fe_tbl_HashCheck_ip_sa_check_idx},
	{"ip_da_chk_idx",		fe_tbl_HashCheck_ip_da_check_idx},

	{0,				0}
};
