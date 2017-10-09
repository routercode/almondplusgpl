/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblQOSRSLT.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module QOSRSLT Table
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
 * fe->QOSRSLT(fe_qos_result_entry_s)
 */
int fe_tbl_QOSRSLT_wred_cos(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.wred_cos =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_wred_cos() */
int fe_tbl_QOSRSLT_voq_cos(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.voq_cos =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_voq_cos() */
int fe_tbl_QOSRSLT_pol_cos(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.pol_cos =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_pol_cos() */
int fe_tbl_QOSRSLT_premark(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.premark =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_premark() */
int fe_tbl_QOSRSLT_change_dscp_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.change_dscp_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_change_dscp_en() */
int fe_tbl_QOSRSLT_dscp(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.dscp = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_dscp() */
int fe_tbl_QOSRSLT_dscp_markdown_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.dscp_markdown_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_dscp_markdown_en() */
int fe_tbl_QOSRSLT_marked_down_dscp(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.marked_down_dscp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_marked_down_dscp() */
int fe_tbl_QOSRSLT_ecn_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.ecn_en = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_ecn_en() */
int fe_tbl_QOSRSLT_top_802_1p(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.top_802_1p =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_top_802_1p() */
int fe_tbl_QOSRSLT_marked_down_top_802_1p(char *pString,
					  QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.marked_down_top_802_1p =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_marked_down_top_802_1p() */
int fe_tbl_QOSRSLT_top_8021p_markdown_en(char *pString,
					 QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.top_8021p_markdown_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_top_8021p_markdown_en() */
int fe_tbl_QOSRSLT_top_dei(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.top_dei =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_top_dei() */
int fe_tbl_QOSRSLT_marked_down_top_dei(char *pString,
				       QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.marked_down_top_dei =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_marked_down_top_dei() */
int fe_tbl_QOSRSLT_inner_802_1p(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.inner_802_1p =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_inner_802_1p() */
int fe_tbl_QOSRSLT_marked_down_inner_802_1p(char *pString,
					    QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.marked_down_inner_802_1p =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_marked_down_inner_802_1p() */
int fe_tbl_QOSRSLT_inner_8021p_markdown_en(char *pString,
					   QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.inner_8021p_markdown_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_inner_8021p_markdown_en() */
int fe_tbl_QOSRSLT_inner_dei(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.inner_dei =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_inner_dei() */
int fe_tbl_QOSRSLT_marked_down_inner_dei(char *pString,
					 QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.marked_down_inner_dei =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_marked_down_inner_dei() */
int fe_tbl_QOSRSLT_change_8021p_1_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.change_8021p_1_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_change_8021p_1_en() */
int fe_tbl_QOSRSLT_change_dei_1_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.change_dei_1_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_change_dei_1_en() */
int fe_tbl_QOSRSLT_change_8021p_2_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.change_8021p_2_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_change_8021p_2_en() */
int fe_tbl_QOSRSLT_change_dei_2_en(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.change_dei_2_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_change_dei_2_en() */
int fe_tbl_QOSRSLT_parity(char *pString, QOSRSLT_INFO * pQOSRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pQOSRSLTInfo->tbl_QOSRSLT.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_QOSRSLT_parity() */

/*====================================================================== */

NE_FIELD_T fe_tbl_QOSRSLT[] = {
	{"wred_cos",                    fe_tbl_QOSRSLT_wred_cos},
	{"voq_cos",                     fe_tbl_QOSRSLT_voq_cos},
	{"pol_cos",                     fe_tbl_QOSRSLT_pol_cos},
	{"premark",                     fe_tbl_QOSRSLT_premark},
	{"change_dscp_en",              fe_tbl_QOSRSLT_change_dscp_en},
	{"dscp",                        fe_tbl_QOSRSLT_dscp},
	{"dscp_markdown_en",            fe_tbl_QOSRSLT_dscp_markdown_en},
	{"marked_down_dscp",            fe_tbl_QOSRSLT_marked_down_dscp},
	{"ecn_en",                      fe_tbl_QOSRSLT_ecn_en},
	{"top_802_1p",                  fe_tbl_QOSRSLT_top_802_1p},
	{"marked_down_top_802_1p",      fe_tbl_QOSRSLT_marked_down_top_802_1p},
	{"top_8021p_markdown_en",       fe_tbl_QOSRSLT_top_8021p_markdown_en},
	{"top_dei",                     fe_tbl_QOSRSLT_top_dei},
	{"marked_down_top_dei",         fe_tbl_QOSRSLT_marked_down_top_dei},
	{"inner_802_1p",                fe_tbl_QOSRSLT_inner_802_1p},
	{"marked_down_inner_802_1p",    fe_tbl_QOSRSLT_marked_down_inner_802_1p},
	{"inner_8021p_markdown_en",     fe_tbl_QOSRSLT_inner_8021p_markdown_en},
	{"inner_dei",                   fe_tbl_QOSRSLT_inner_dei},
	{"marked_down_inner_dei",       fe_tbl_QOSRSLT_marked_down_inner_dei},
	{"change_8021p_1_en",           fe_tbl_QOSRSLT_change_8021p_1_en},
	{"change_dei_1_en",             fe_tbl_QOSRSLT_change_dei_1_en},
	{"change_8021p_2_en",           fe_tbl_QOSRSLT_change_8021p_2_en},
	{"change_dei_2_en",             fe_tbl_QOSRSLT_change_dei_2_en},
	{"parity",                      fe_tbl_QOSRSLT_parity},
	
	{0,                                     0}
};
