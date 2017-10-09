/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblFVLAN.c
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


/* fe->FVLAN(fe_flow_vlan_entry_s) */

int fe_tbl_FVLAN_first_vlan_cmd(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.first_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_first_vlan_cmd() */
int fe_tbl_FVLAN_first_vid(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.first_vid = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_first_vid() */
int fe_tbl_FVLAN_first_tpid_enc(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.first_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_first_tpid_enc() */
int fe_tbl_FVLAN_second_vlan_cmd(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.second_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_second_vlan_cmd() */
int fe_tbl_FVLAN_second_vid(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.second_vid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_second_vid() */
int fe_tbl_FVLAN_second_tpid_enc(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.second_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_second_tpid_enc() */
int fe_tbl_FVLAN_parity(char *pString, FVLAN_INFO * pFVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFVLANInfo->tbl_FVLAN.parity = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FVLAN_parity() */

/*==================================================================== */

NE_FIELD_T fe_tbl_FVLAN[] = {
	{"first_vlan_cmd",                      fe_tbl_FVLAN_first_vlan_cmd},
	{"first_vid",                           fe_tbl_FVLAN_first_vid},
	{"first_tpid_enc",                      fe_tbl_FVLAN_first_tpid_enc},
	{"second_vlan_cmd",                     fe_tbl_FVLAN_second_vlan_cmd},
	{"second_vid",                          fe_tbl_FVLAN_second_vid},
	{"second_tpid_enc",                     fe_tbl_FVLAN_second_tpid_enc},
	{"parity",                              fe_tbl_FVLAN_parity},

	{0,                                     0}
};
