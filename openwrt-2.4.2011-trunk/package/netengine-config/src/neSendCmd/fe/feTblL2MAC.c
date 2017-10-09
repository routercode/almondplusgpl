/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblL2MAC.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module L2 MAC Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->L2_MAC(fe_l2_addr_pair_entry_s) */

int fe_tbl_L2MAC_mac_sa_0(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[0] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_0() */
int fe_tbl_L2MAC_mac_sa_1(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[1] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_1() */
int fe_tbl_L2MAC_mac_sa_2(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[2] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_2() */
int fe_tbl_L2MAC_mac_sa_3(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[3] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_3() */
int fe_tbl_L2MAC_mac_sa_4(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[4] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_4() */
int fe_tbl_L2MAC_mac_sa_5(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_sa[5] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_sa_5() */
int fe_tbl_L2MAC_sa_count(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.sa_count = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_sa_count() */
int fe_tbl_L2MAC_mac_da_0(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[0] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_0() */
int fe_tbl_L2MAC_mac_da_1(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[1] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_1() */
int fe_tbl_L2MAC_mac_da_2(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[2] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_2() */
int fe_tbl_L2MAC_mac_da_3(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[3] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_3() */
int fe_tbl_L2MAC_mac_da_4(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[4] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_4() */
int fe_tbl_L2MAC_mac_da_5(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.mac_da[5] = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_mac_da_5() */
int fe_tbl_L2MAC_da_count(char *pString, L2_MAC_INFO * pL2MACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pL2MACInfo->tbl_L2MAC.da_count = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_L2MAC_da_count() */

/*==================================================================== */

NE_FIELD_T fe_tbl_L2_MAC[] = {
	{"mac_sa.0",                             fe_tbl_L2MAC_mac_sa_0},
	{"mac_sa.1",                             fe_tbl_L2MAC_mac_sa_1},
	{"mac_sa.2",                             fe_tbl_L2MAC_mac_sa_2},
	{"mac_sa.3",                             fe_tbl_L2MAC_mac_sa_3},
	{"mac_sa.4",                             fe_tbl_L2MAC_mac_sa_4},
	{"mac_sa.5",                             fe_tbl_L2MAC_mac_sa_5},
	{"sa_count",                             fe_tbl_L2MAC_sa_count},
	{"mac_da.0",                             fe_tbl_L2MAC_mac_da_0},
	{"mac_da.1",                             fe_tbl_L2MAC_mac_da_1},
	{"mac_da.2",                             fe_tbl_L2MAC_mac_da_2},
	{"mac_da.3",                             fe_tbl_L2MAC_mac_da_3},
	{"mac_da.4",                             fe_tbl_L2MAC_mac_da_4},
	{"mac_da.5",                             fe_tbl_L2MAC_mac_da_5},
	{"da_count",                             fe_tbl_L2MAC_da_count},

	{0,                                     0}
};
