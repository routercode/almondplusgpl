/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblANBNGMAC.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module AN BNG MAC Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->AN_BNG_MAC(fe_an_bng_mac_entry_s) */

int fe_tbl_ANBNGMAC_mac_0(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[0] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_0() */
int fe_tbl_ANBNGMAC_mac_1(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[1] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_1() */
int fe_tbl_ANBNGMAC_mac_2(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[2] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_2() */
int fe_tbl_ANBNGMAC_mac_3(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[3] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_3() */
int fe_tbl_ANBNGMAC_mac_4(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[4] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_4() */
int fe_tbl_ANBNGMAC_mac_5(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.mac[5] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_mac_5() */
int fe_tbl_ANBNGMAC_sa_da(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.sa_da =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_sa_da() */
int fe_tbl_ANBNGMAC_pspid(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.pspid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_pspid() */
int fe_tbl_ANBNGMAC_pspid_mask(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.pspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_pspid_mask() */
int fe_tbl_ANBNGMAC_valid(char *pString, AN_BNG_MAC_INFO * pANBNGMACInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pANBNGMACInfo->tbl_ANBNGMAC.valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ANBNGMAC_valid() */

/*==================================================================== */

NE_FIELD_T fe_tbl_AN_BNG_MAC[] = {
	{"mac.0",                             fe_tbl_ANBNGMAC_mac_0},
	{"mac.1",                             fe_tbl_ANBNGMAC_mac_1},
	{"mac.2",                             fe_tbl_ANBNGMAC_mac_2},
	{"mac.3",                             fe_tbl_ANBNGMAC_mac_3},
	{"mac.4",                             fe_tbl_ANBNGMAC_mac_4},
	{"mac.5",                             fe_tbl_ANBNGMAC_mac_5},
	{"sa_da",                             fe_tbl_ANBNGMAC_sa_da},
	{"pspid",                             fe_tbl_ANBNGMAC_pspid},
	{"pspid_mask",                        fe_tbl_ANBNGMAC_pspid_mask},
	{"valid",                             fe_tbl_ANBNGMAC_valid},

	{0,                                     0}
};
