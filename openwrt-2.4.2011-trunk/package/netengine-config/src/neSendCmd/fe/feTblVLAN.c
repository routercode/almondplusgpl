/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblVLAN.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module VLAN Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->VLAN(fe_vlan_entry_s) */

int fe_tbl_VLAN_vlan_member(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_member = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_member() */
int fe_tbl_VLAN_vlan_egress_untagged(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_egress_untagged =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_egress_untagged() */
int fe_tbl_VLAN_vlan_fid(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_fid = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_fid() */
int fe_tbl_VLAN_vlan_first_vlan_cmd(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_first_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_first_vlan_cmd() */
int fe_tbl_VLAN_vlan_first_vid(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_first_vid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_first_vid() */
int fe_tbl_VLAN_vlan_first_tpid_enc(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_first_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_first_tpid_enc() */
int fe_tbl_VLAN_vlan_second_vlan_cmd(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_second_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_second_vlan_cmd() */
int fe_tbl_VLAN_vlan_second_vid(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_second_vid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_second_vid() */
int fe_tbl_VLAN_vlan_second_tpid_enc(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_second_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_second_tpid_enc() */
int fe_tbl_VLAN_vlan_mcgid(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.vlan_mcgid = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_vlan_mcgid() */
int fe_tbl_VLAN_parity(char *pString, VLAN_INFO * pVLANInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVLANInfo->tbl_VLAN.parity = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VLAN_parity() */

/*==================================================================== */

NE_FIELD_T fe_tbl_VLAN[] = {
	{"vlan_member",                         fe_tbl_VLAN_vlan_member},
	{"vlan_egress_untagged",                fe_tbl_VLAN_vlan_egress_untagged},
	{"vlan_fid",                            fe_tbl_VLAN_vlan_fid},
	{"vlan_first_vlan_cmd",                 fe_tbl_VLAN_vlan_first_vlan_cmd},
	{"vlan_first_vid",                      fe_tbl_VLAN_vlan_first_vid},
	{"vlan_first_tpid_enc",                 fe_tbl_VLAN_vlan_first_tpid_enc},
	{"vlan_second_vlan_cmd",                fe_tbl_VLAN_vlan_second_vlan_cmd},
	{"vlan_second_vid",                     fe_tbl_VLAN_vlan_second_vid},
	{"vlan_second_tpid_enc",                fe_tbl_VLAN_vlan_second_tpid_enc},
	{"vlan_mcgid",                          fe_tbl_VLAN_vlan_mcgid},
	{"parity",                              fe_tbl_VLAN_parity},

	{0,                                     0}
};
