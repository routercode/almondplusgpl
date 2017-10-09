/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblFWDRSLT.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
                 fe module FWDRSLT Table
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
 * fe->FWDRSLT->fe_fwd_result_l2
 */
int fe_tbl_FWDRSLT_l2_mac_sa_replace_en(char *pString,
					FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.mac_sa_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_mac_sa_replace_en() */
int fe_tbl_FWDRSLT_l2_mac_da_replace_en(char *pString,
					FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.mac_da_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_mac_da_replace_en() */
int fe_tbl_FWDRSLT_l2_l2_index(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.l2_index =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_l2_index() */
int fe_tbl_FWDRSLT_l2_mcgid(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.mcgid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_mcgid() */
int fe_tbl_FWDRSLT_l2_mcgid_valid(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.mcgid_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_mcgid_valid() */
int fe_tbl_FWDRSLT_l2_flow_vlan_op_en(char *pString,
				      FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.flow_vlan_op_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_flow_vlan_op_en() */
int fe_tbl_FWDRSLT_l2_flow_vlan_index(char *pString,
				      FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.flow_vlan_index =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_flow_vlan_index() */
int fe_tbl_FWDRSLT_l2_pppoe_encap_en(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.pppoe_encap_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_pppoe_encap_en() */
int fe_tbl_FWDRSLT_l2_pppoe_decap_en(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l2.pppoe_decap_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l2_pppoe_decap_en() */

/*
 * fe->FWDRSLT-->fe_fwd_result_l3
 */
int fe_tbl_FWDRSLT_l3_ip_sa_replace_en(char *pString,
				       FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l3.ip_sa_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l3_ip_sa_replace_en() */
int fe_tbl_FWDRSLT_l3_ip_da_replace_en(char *pString,
				       FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l3.ip_da_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l3_ip_da_replace_en() */
int fe_tbl_FWDRSLT_l3_ip_sa_index(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l3.ip_sa_index =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l3_ip_sa_index() */
int fe_tbl_FWDRSLT_l3_ip_da_index(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l3.ip_da_index =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l3_ip_da_index() */
int fe_tbl_FWDRSLT_l3_decr_ttl_hoplimit(char *pString,
					FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l3.decr_ttl_hoplimit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l3_decr_ttl_hoplimit() */

/*
 * fe->FWDRSLT->fe_fwd_result_l4
 */
int fe_tbl_FWDRSLT_l4_sp_replace_en(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l4.sp_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l4_sp_replace_en() */
int fe_tbl_FWDRSLT_l4_dp_replace_en(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l4.dp_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l4_dp_replace_en() */
int fe_tbl_FWDRSLT_l4_sp(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l4.sp = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l4_sp() */
int fe_tbl_FWDRSLT_l4_dp(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.l4.dp = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_l4_dp() */

/*
 * fe->FWDRSLT->fe_fwd_result_dest
 */
int fe_tbl_FWDRSLT_dest_pol_policy(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.dest.pol_policy =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_dest_pol_policy() */
int fe_tbl_FWDRSLT_dest_voq_policy(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.dest.voq_policy =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_dest_voq_policy() */
int fe_tbl_FWDRSLT_dest_voq_pol_table_index(char *pString,
					    FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.dest.voq_pol_table_index =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_dest_voq_pol_table_index() */

/*
 * fe->FWDRSLT->fe_fwd_result_act
 */
int fe_tbl_FWDRSLT_act_fwd_type_valid(char *pString,
				      FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.act.fwd_type_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_act_fwd_type_valid() */
int fe_tbl_FWDRSLT_act_fwd_type(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.act.fwd_type =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_act_fwd_type() */
int fe_tbl_FWDRSLT_act_drop(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.act.drop =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_act_drop() */

/*
 * fe->FWDRSLT
 */
int fe_tbl_FWDRSLT_acl_dsbl(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.acl_dsbl =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_acl_dsbl() */
int fe_tbl_FWDRSLT_parity(char *pString, FWDRSLT_INFO * pFWDRSLTInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pFWDRSLTInfo->tbl_FWDRSLT.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_FWDRSLT_parity() */

/*======================================================================= */

NE_FIELD_T fe_tbl_FWDRSLT[] = {
	{"l2.mac_sa_replace_en",	fe_tbl_FWDRSLT_l2_mac_sa_replace_en},
	{"l2.mac_da_replace_en",	fe_tbl_FWDRSLT_l2_mac_da_replace_en},
	{"l2.l2_index",			fe_tbl_FWDRSLT_l2_l2_index},
	{"l2.mcgid",			fe_tbl_FWDRSLT_l2_mcgid},
	{"l2.mcgid_valid",		fe_tbl_FWDRSLT_l2_mcgid_valid},
	{"l2.flow_vlan_op_en",		fe_tbl_FWDRSLT_l2_flow_vlan_op_en},
	{"l2.flow_vlan_index",		fe_tbl_FWDRSLT_l2_flow_vlan_index},
	{"l2.pppoe_encap_en",		fe_tbl_FWDRSLT_l2_pppoe_encap_en},
	{"l2.pppoe_decap_en",		fe_tbl_FWDRSLT_l2_pppoe_decap_en},

	{"l3.ip_sa_replace_en",		fe_tbl_FWDRSLT_l3_ip_sa_replace_en},
	{"l3.ip_da_replace_en",		fe_tbl_FWDRSLT_l3_ip_da_replace_en},
	{"l3.ip_sa_index",		fe_tbl_FWDRSLT_l3_ip_sa_index},
	{"l3.ip_da_index",		fe_tbl_FWDRSLT_l3_ip_da_index},
	{"l3.decr_ttl_hoplimit",	fe_tbl_FWDRSLT_l3_decr_ttl_hoplimit},

	{"l4.sp_replace_en",		fe_tbl_FWDRSLT_l4_sp_replace_en},
	{"l4.dp_replace_en",		fe_tbl_FWDRSLT_l4_dp_replace_en},
	{"l4.sp",			fe_tbl_FWDRSLT_l4_sp},
	{"l4.dp",			fe_tbl_FWDRSLT_l4_dp},

	{"dest.pol_policy",		fe_tbl_FWDRSLT_dest_pol_policy},
	{"dest.voq_policy",		fe_tbl_FWDRSLT_dest_voq_policy},
	{"dest.voq_pol_table_index",	fe_tbl_FWDRSLT_dest_voq_pol_table_index},

	{"act.fwd_type_valid",		fe_tbl_FWDRSLT_act_fwd_type_valid},
	{"act.fwd_type",		fe_tbl_FWDRSLT_act_fwd_type},
	{"act.drop",			fe_tbl_FWDRSLT_act_drop},

	{"acl_dsbl",			fe_tbl_FWDRSLT_acl_dsbl},
	{"parity",			fe_tbl_FWDRSLT_parity},

	{0,				0}
};
