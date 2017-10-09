/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblVoQPolicer.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module VoQ Policer Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"


/* fe->VoQ_Policer(fe_voq_pol_entry_s) */

int fe_tbl_VoQPolicer_voq_base(char *pString,
			       VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.voq_base =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_voq_base() */
int fe_tbl_VoQPolicer_pol_base(char *pString,
			       VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.pol_base =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_pol_base() */
int fe_tbl_VoQPolicer_cpu_pid(char *pString, VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.cpu_pid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_cpu_pid() */
int fe_tbl_VoQPolicer_ldpid(char *pString, VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.ldpid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_ldpid() */
int fe_tbl_VoQPolicer_pppoe_session_id(char *pString,
				       VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.pppoe_session_id =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_pppoe_session_id() */
int fe_tbl_VoQPolicer_cos_nop(char *pString, VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.cos_nop =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_cos_nop() */
int fe_tbl_VoQPolicer_parity(char *pString, VOQ_POLICER_INFO * pVoQPolicerInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pVoQPolicerInfo->tbl_VoQPolicer.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_VoQPolicer_parity() */

/*==================================================================== */

NE_FIELD_T fe_tbl_VoQ_Policer[] = {
	{"voq_base",                            fe_tbl_VoQPolicer_voq_base},
	{"pol_base",                            fe_tbl_VoQPolicer_pol_base},
	{"cpu_pid",                             fe_tbl_VoQPolicer_cpu_pid},
	{"ldpid",                               fe_tbl_VoQPolicer_ldpid},
	{"pppoe_session_id",                    fe_tbl_VoQPolicer_pppoe_session_id},
	{"cos_nop",                             fe_tbl_VoQPolicer_cos_nop},
	{"parity",                              fe_tbl_VoQPolicer_parity},

	{0,                                     0}
};
