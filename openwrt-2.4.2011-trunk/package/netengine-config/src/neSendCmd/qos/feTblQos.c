/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblQos.c
   Date        : 2011-10-03
   Description : Cortina GoldenGate NetEngine configuration utility process
	             fe module Ingress Qos Table
   Author      : Ethan Chen <ethan.chen@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ne_defs.h>
#include "../neSendCmd.h"
#include "../misc.h"

/* fe->QOS_TABLE(fe_ingress_qos_table_entry_s) */

int fe_tbl_QOS_TABLE_vid(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE.vid = (cs_uint16) strtoul(pString, NULL, 0);
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_VID;
	return STATUS_SUCCESS;
} /* fe_tbl_QOS_TABLE_vid() */

int fe_tbl_QOS_TABLE_8021p(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE._8021p =(cs_uint8) strtoul(pString, NULL, 0);
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_8021P;
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_8012p() */

int fe_tbl_QOS_TABLE_dscp(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE.dscp =(cs_uint8) strtoul(pString, NULL, 0);
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_DSCP;
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_dscp() */

int fe_tbl_QOS_TABLE_sip(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{

	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}

	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pIngressQosInfo->tbl_QOS_TABLE.sip[i] = ipVal[i];
	}
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_SIP;

	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_sip() */

int fe_tbl_QOS_TABLE_dip(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{

	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}

	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pIngressQosInfo->tbl_QOS_TABLE.dip[i] = ipVal[i];
	}
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_DIP;
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_dip() */

int fe_tbl_QOS_TABLE_voq(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE.voq =(cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_voq() */
/*==================================================================== */
int fe_tbl_QOS_TABLE_lspid(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE.lspid =(cs_uint8) strtoul(pString, NULL, 0);
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_LSPID;
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_lspid() */

int fe_tbl_QOS_TABLE_delete(char *pString, INGRESS_QOS_TABLE_INFO * pIngressQosInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pIngressQosInfo->tbl_QOS_TABLE.delete =(cs_uint8) strtoul(pString, NULL, 0);
	pIngressQosInfo->tbl_QOS_TABLE.mask |= QOSMASK_DELETE;
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_TABLE_delete() */
/*==================================================================== */
//__u8 mask;

NE_FIELD_T fe_tbl_QOS_TABLE[] = {
	{"vid",                         	fe_tbl_QOS_TABLE_vid},
	{"8021p",							fe_tbl_QOS_TABLE_8021p},
	{"dscp",							fe_tbl_QOS_TABLE_dscp},
	{"sip",								fe_tbl_QOS_TABLE_sip},
	{"dip",								fe_tbl_QOS_TABLE_dip},
	{"voq",								fe_tbl_QOS_TABLE_voq},
	{"lspid",							fe_tbl_QOS_TABLE_lspid},
	{"delete",							fe_tbl_QOS_TABLE_delete},
	{0,									0}
};
