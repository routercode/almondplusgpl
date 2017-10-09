/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblACLAction.c
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

int fe_tbl_ACLRule_rule_valid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.rule_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_rule_valid() */
int fe_tbl_ACLRule_l2_l2_mac_da_0(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[0] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_0() */
int fe_tbl_ACLRule_l2_l2_mac_da_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[1] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_1() */
int fe_tbl_ACLRule_l2_l2_mac_da_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[2] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_2() */
int fe_tbl_ACLRule_l2_l2_mac_da_3(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[3] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_3() */
int fe_tbl_ACLRule_l2_l2_mac_da_4(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[4] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_4() */
int fe_tbl_ACLRule_l2_l2_mac_da_5(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da[5] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_5() */
int fe_tbl_ACLRule_l2_l2_mac_sa_0(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[0] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_0() */
int fe_tbl_ACLRule_l2_l2_mac_sa_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[1] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_1() */
int fe_tbl_ACLRule_l2_l2_mac_sa_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[2] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_2() */
int fe_tbl_ACLRule_l2_l2_mac_sa_3(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[3] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_3() */
int fe_tbl_ACLRule_l2_l2_mac_sa_4(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[4] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_4() */
int fe_tbl_ACLRule_l2_l2_mac_sa_5(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa[5] =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_5() */
int fe_tbl_ACLRule_l2_eth_type(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.eth_type =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_eth_type() */
int fe_tbl_ACLRule_l2_len_encoded(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.len_encoded =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_len_encoded() */
int fe_tbl_ACLRule_l2_tpid_1_vld(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_1_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_1_vld() */
int fe_tbl_ACLRule_l2_tpid_enc_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_enc_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_enc_1() */
int fe_tbl_ACLRule_l2_vid_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.vid_1 =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_vid_1() */
int fe_tbl_ACLRule_l2__8021p_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2._8021p_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2__8021p_1() */
int fe_tbl_ACLRule_l2_dei_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.dei_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_dei_1() */
int fe_tbl_ACLRule_l2_tpid_2_vld(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_2_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_2_vld() */
int fe_tbl_ACLRule_l2_tpid_enc_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_enc_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_enc_2() */
int fe_tbl_ACLRule_l2_vid_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.vid_2 =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_vid_2() */
int fe_tbl_ACLRule_l2__8021p_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2._8021p_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2__8021p_2() */
int fe_tbl_ACLRule_l2_dei_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.dei_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_dei_2() */
int fe_tbl_ACLRule_l2_l2_mac_da_mask(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_da_mask() */
int fe_tbl_ACLRule_l2_l2_mac_sa_mask(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.l2_mac_sa_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_l2_mac_sa_mask() */
int fe_tbl_ACLRule_l2_ethertype_mask(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.ethertype_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_ethertype_mask() */
int fe_tbl_ACLRule_l2_len_encoded_mask(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.len_encoded_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_len_encoded_mask() */
int fe_tbl_ACLRule_l2_tpid_1_vld_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_1_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_1_vld_mask() */
int fe_tbl_ACLRule_l2_tpid_enc_1_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_enc_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_enc_1_mask() */
int fe_tbl_ACLRule_l2_vid_1_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.vid_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_vid_1_mask() */
int fe_tbl_ACLRule_l2__8021p_1_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2._8021p_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2__8021p_1_mask() */
int fe_tbl_ACLRule_l2_dei_1_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.dei_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_dei_1_mask() */
int fe_tbl_ACLRule_l2_tpid_2_vld_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_2_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_2_vld_mask() */
int fe_tbl_ACLRule_l2_tpid_enc_2_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.tpid_enc_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_tpid_enc_2_mask() */
int fe_tbl_ACLRule_l2_vid_2_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.vid_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_vid_2_mask() */
int fe_tbl_ACLRule_l2__8021p_2_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2._8021p_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2__8021p_2_mask() */
int fe_tbl_ACLRule_l2_dei_2_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.dei_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_dei_2_mask() */
int fe_tbl_ACLRule_l2_da_an_mac_sel(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.da_an_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_da_an_mac_sel() */
int fe_tbl_ACLRule_l2_da_an_mac_sel_mask(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.da_an_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_da_an_mac_sel_mask() */
int fe_tbl_ACLRule_l2_da_an_mac_hit(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.da_an_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_da_an_mac_hit() */
int fe_tbl_ACLRule_l2_da_an_mac_hit_mask(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.da_an_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_da_an_mac_hit_mask() */
int fe_tbl_ACLRule_l2_sa_bng_mac_sel(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.sa_bng_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_sa_bng_mac_sel() */
int fe_tbl_ACLRule_l2_sa_bng_mac_sel_mask(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.sa_bng_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_sa_bng_mac_sel_mask() */
int fe_tbl_ACLRule_l2_sa_bng_mac_hit(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.sa_bng_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_sa_bng_mac_hit() */
int fe_tbl_ACLRule_l2_sa_bng_mac_hit_mask(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.sa_bng_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_sa_bng_mac_hit_mask() */
int fe_tbl_ACLRule_l2_pppoe_session_id_vld(char *pString,
					   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.pppoe_session_id_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_pppoe_session_id_vld() */
int fe_tbl_ACLRule_l2_pppoe_session_id_vld_mask(char *pString,
						ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.pppoe_session_id_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_pppoe_session_id_vld_mask() */
int fe_tbl_ACLRule_l2_pppoe_session_id(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.pppoe_session_id =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_pppoe_session_id() */
int fe_tbl_ACLRule_l2_pppoe_session_id_mask(char *pString,
					    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.pppoe_session_id_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_pppoe_session_id_mask() */
int fe_tbl_ACLRule_l2_ppp_protocol_vld(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.ppp_protocol_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_ppp_protocol_vld() */
int fe_tbl_ACLRule_l2_ppp_protocol_vld_mask(char *pString,
					    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.ppp_protocol_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_ppp_protocol_vld_mask() */
int fe_tbl_ACLRule_l2_ppp_protocol(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.ppp_protocol =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_ppp_protocol() */
int fe_tbl_ACLRule_l2_ppp_protocol_mask(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l2.ppp_protocol_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l2_ppp_protocol_mask() */

//
// fe->ACL_Rule->fe_acl_rule_l3_entry_s
//
int fe_tbl_ACLRule_l3_ip_vld(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_vld() */
int fe_tbl_ACLRule_l3_ip_ver(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_ver =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_ver() */
int fe_tbl_ACLRule_l3_da_0(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.da[0] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_da_0() */
int fe_tbl_ACLRule_l3_da_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.da[1] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_da_1() */
int fe_tbl_ACLRule_l3_da_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.da[2] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_da_2() */
int fe_tbl_ACLRule_l3_da_3(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.da[3] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_da_3() */
int fe_tbl_ACLRule_l3_sa_0(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.sa[0] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_sa_0() */
int fe_tbl_ACLRule_l3_sa_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.sa[1] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_sa_1() */
int fe_tbl_ACLRule_l3_sa_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.sa[2] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_sa_2() */
int fe_tbl_ACLRule_l3_sa_3(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.sa[3] =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_sa_3() */
int fe_tbl_ACLRule_l3_dscp(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.dscp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_dscp() */
int fe_tbl_ACLRule_l3_ecn(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ecn = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ecn() */
int fe_tbl_ACLRule_l3_proto(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.proto =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_proto() */
int fe_tbl_ACLRule_l3_fragment(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.fragment =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_fragment() */
int fe_tbl_ACLRule_l3_options(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.options =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_options() */
int fe_tbl_ACLRule_l3_ipv6_flow_label(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_flow_label =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_flow_label() */
int fe_tbl_ACLRule_l3_ttl_hoplimit(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ttl_hoplimit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ttl_hoplimit() */
int fe_tbl_ACLRule_l3_ip_vld_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_vld_mask() */
int fe_tbl_ACLRule_l3_ip_ver_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_ver_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_ver_mask() */
int fe_tbl_ACLRule_l3_ip_da_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_da_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_da_mask() */
int fe_tbl_ACLRule_l3_ip_sa_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_sa_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_sa_mask() */
int fe_tbl_ACLRule_l3_dscp_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.dscp_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_dscp_mask() */
int fe_tbl_ACLRule_l3_ecn_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ecn_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ecn_mask() */
int fe_tbl_ACLRule_l3_ip_proto_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_proto_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_proto_mask() */
int fe_tbl_ACLRule_l3_ip_fragment_mask(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_fragment_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_fragment_mask() */
int fe_tbl_ACLRule_l3_ip_options_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ip_options_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ip_options_mask() */
int fe_tbl_ACLRule_l3_ipv6_flow_label_mask(char *pString,
					   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_flow_label_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_flow_label_mask() */
int fe_tbl_ACLRule_l3_ttl_hoplimit_mask(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ttl_hoplimit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ttl_hoplimit_mask() */
int fe_tbl_ACLRule_l3_spi(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.spi =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_spi() */
int fe_tbl_ACLRule_l3_spi_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.spi_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_spi_mask() */
int fe_tbl_ACLRule_l3_spi_vld(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.spi_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_spi_vld() */
int fe_tbl_ACLRule_l3_spi_vld_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.spi_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_spi_vld_mask() */
int fe_tbl_ACLRule_l3_ipv6_ndp(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_ndp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_ndp() */
int fe_tbl_ACLRule_l3_ipv6_ndp_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_ndp_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_ndp_mask() */
int fe_tbl_ACLRule_l3_ipv6_hbh(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_hbh =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_hbh() */
int fe_tbl_ACLRule_l3_ipv6_hbh_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_hbh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_hbh_mask() */
int fe_tbl_ACLRule_l3_ipv6_rh(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_rh =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_rh() */
int fe_tbl_ACLRule_l3_ipv6_rh_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_rh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_rh_mask() */
int fe_tbl_ACLRule_l3_ipv6_doh(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_doh =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_doh() */
int fe_tbl_ACLRule_l3_ipv6_doh_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l3.ipv6_doh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l3_ipv6_doh_mask() */

//
// fe->ACL_Rule->fe_acl_rule_l4_entry_s
//
int fe_tbl_ACLRule_l4_l4_valid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.l4_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_l4_valid() */
int fe_tbl_ACLRule_l4_dp_lo(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.dp_lo =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_dp_lo() */
int fe_tbl_ACLRule_l4_dp_hi(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.dp_hi =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_dp_hi() */
int fe_tbl_ACLRule_l4_sp_lo(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.sp_lo =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_sp_lo() */
int fe_tbl_ACLRule_l4_sp_hi(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.sp_hi =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_sp_hi() */
int fe_tbl_ACLRule_l4_l4_valid_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.l4_valid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_l4_valid_mask() */
int fe_tbl_ACLRule_l4_l4_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.l4.l4_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_l4_l4_mask() */

//
// fe->ACL_Rule->fe_acl_misc_entry_s
//
int fe_tbl_ACLRule_misc_lspid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lspid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lspid() */
int fe_tbl_ACLRule_misc_orig_lspid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.orig_lspid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_orig_lspid() */
int fe_tbl_ACLRule_misc_fwd_type(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.fwd_type =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_fwd_type() */
int fe_tbl_ACLRule_misc_spl_pkt_vec(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.spl_pkt_vec =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_spl_pkt_vec() */
int fe_tbl_ACLRule_misc_class_hit(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.class_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_class_hit() */
int fe_tbl_ACLRule_misc_class_svidx(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.class_svidx =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_class_svidx() */
int fe_tbl_ACLRule_misc_lpm_hit(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lpm_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lpm_hit() */
int fe_tbl_ACLRule_misc_lpm_hit_idx(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lpm_hit_idx =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lpm_hit_idx() */
int fe_tbl_ACLRule_misc_hash_hit(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.hash_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_hash_hit() */
int fe_tbl_ACLRule_misc_hash_hit_idx(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.hash_hit_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_hash_hit_idx() */
int fe_tbl_ACLRule_misc_l7_field(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.l7_field =
	    (cs_uint32) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_l7_field() */
int fe_tbl_ACLRule_misc_lspid_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lspid_mask() */
int fe_tbl_ACLRule_misc_orig_lspid_mask(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.orig_lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_orig_lspid_mask() */
int fe_tbl_ACLRule_misc_fwd_type_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.fwd_type_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_fwd_type_mask() */
int fe_tbl_ACLRule_misc_spl_pkt_vec_mask(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.spl_pkt_vec_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_spl_pkt_vec_mask() */
int fe_tbl_ACLRule_misc_class_hit_mask(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.class_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_class_hit_mask() */
int fe_tbl_ACLRule_misc_class_svidx_mask(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.class_svidx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_class_svidx_mask() */
int fe_tbl_ACLRule_misc_lpm_hit_mask(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lpm_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lpm_hit_mask() */
int fe_tbl_ACLRule_misc_lpm_hit_idx_mask(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.lpm_hit_idx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_lpm_hit_idx_mask() */
int fe_tbl_ACLRule_misc_hash_hit_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.hash_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_hash_hit_mask() */
int fe_tbl_ACLRule_misc_hash_hit_idx_mask(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.hash_hit_idx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_hash_hit_idx_mask() */
int fe_tbl_ACLRule_misc_l7_field_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.l7_field_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_l7_field_mask() */
int fe_tbl_ACLRule_misc_flags_vec(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.flags_vec =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_flags_vec() */
int fe_tbl_ACLRule_misc_flags_vec_mask(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.flags_vec_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_flags_vec_mask() */
int fe_tbl_ACLRule_misc_flags_vec_or(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.flags_vec_or =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_flags_vec_or() */
int fe_tbl_ACLRule_misc_spl_pkt_vec_or(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.spl_pkt_vec_or =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_spl_pkt_vec_or() */
int fe_tbl_ACLRule_misc_recirc_idx(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.recirc_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_recirc_idx() */
int fe_tbl_ACLRule_misc_recirc_idx_mask(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.recirc_idx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_recirc_idx_mask() */
int fe_tbl_ACLRule_misc_ne_vec(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.ne_vec =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_ne_vec() */
int fe_tbl_ACLRule_misc_mc_idx(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.mc_idx =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_mc_idx() */
int fe_tbl_ACLRule_misc_mc_idx_mask(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.mc_idx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_mc_idx_mask() */
int fe_tbl_ACLRule_misc_sdb_drop(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.sdb_drop =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_sdb_drop() */
int fe_tbl_ACLRule_misc_sdb_drop_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.sdb_drop_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_sdb_drop_mask() */
int fe_tbl_ACLRule_misc_fwd_drop(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.fwd_drop =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_fwd_drop() */
int fe_tbl_ACLRule_misc_fwd_drop_mask(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.fwd_drop_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_fwd_drop_mask() */
int fe_tbl_ACLRule_misc_pktlen_rng_vec(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.pktlen_rng_vec =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_pktlen_rng_vec() */
int fe_tbl_ACLRule_misc_pktlen_rng_vec_mask(char *pString,
					    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.pktlen_rng_vec_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_pktlen_rng_vec_mask() */
int fe_tbl_ACLRule_misc_rsvd_879_878(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.misc.rsvd_879_878 =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_misc_rsvd_879_878() */
int fe_tbl_ACLRule_parity(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.rule.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLRule_parity() */


int fe_tbl_ACLAction_l2__8021p_1_vld(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_1_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_1_vld() */
int fe_tbl_ACLAction_l2__8021p_1(char *pString,
				 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_1() */
int fe_tbl_ACLAction_l2__8021p_1_pri(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_1_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_1_pri() */
int fe_tbl_ACLAction_l2_dei_1_vld(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_1_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_1_vld() */
int fe_tbl_ACLAction_l2_dei_1(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_1() */
int fe_tbl_ACLAction_l2_dei_1_pri(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_1_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_1_pri() */
int fe_tbl_ACLAction_l2__8021p_2_vld(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_2_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_2_vld() */
int fe_tbl_ACLAction_l2__8021p_2(char *pString,
				 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_2() */
int fe_tbl_ACLAction_l2__8021p_2_pri(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2._8021p_2_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2__8021p_2_pri() */
int fe_tbl_ACLAction_l2_dei_2_vld(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_2_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_2_vld() */
int fe_tbl_ACLAction_l2_dei_2(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_2() */
int fe_tbl_ACLAction_l2_dei_2_pri(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.dei_2_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_dei_2_pri() */
int fe_tbl_ACLAction_l2_first_vlan_cmd_vld(char *pString,
					   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.first_vlan_cmd_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_first_vlan_cmd_vld() */
int fe_tbl_ACLAction_l2_first_vlan_cmd(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.first_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_first_vlan_cmd() */
int fe_tbl_ACLAction_l2_first_vid(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.first_vid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_first_vid() */
int fe_tbl_ACLAction_l2_first_tpid_enc(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.first_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_first_tpid_enc() */
int fe_tbl_ACLAction_l2_first_vlan_cmd_pri(char *pString,
					   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.first_vlan_cmd_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_first_vlan_cmd_pri() */
int fe_tbl_ACLAction_l2_second_vlan_cmd_vld(char *pString,
					    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.second_vlan_cmd_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_second_vlan_cmd_vld() */
int fe_tbl_ACLAction_l2_second_vlan_cmd(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.second_vlan_cmd =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_second_vlan_cmd() */
int fe_tbl_ACLAction_l2_second_vid(char *pString,
				   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.second_vid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_second_vid() */
int fe_tbl_ACLAction_l2_second_tpid_enc(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.second_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_second_tpid_enc() */
int fe_tbl_ACLAction_l2_second_vlan_cmd_pri(char *pString,
					    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.second_vlan_cmd_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_second_vlan_cmd_pri() */
int fe_tbl_ACLAction_l2_mac_da_sa_replace_en_vld(char *pString,
						 ACL_INFO *
						 pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.mac_da_sa_replace_en_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_mac_da_sa_replace_en_vld() */
int fe_tbl_ACLAction_l2_mac_da_replace_en(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.mac_da_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_mac_da_replace_en() */
int fe_tbl_ACLAction_l2_mac_sa_replace_en(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.mac_sa_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_mac_sa_replace_en() */
int fe_tbl_ACLAction_l2_mac_da_sa_replace_en_pri(char *pString,
						 ACL_INFO *
						 pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.mac_da_sa_replace_en_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_mac_da_sa_replace_en_pri() */
int fe_tbl_ACLAction_l2_l2_idx(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.l2_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_l2_idx() */
int fe_tbl_ACLAction_l2_change_8021p_1_en(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.change_8021p_1_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_change_8021p_1_en() */
int fe_tbl_ACLAction_l2_change_dei_1_en(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.change_dei_1_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_change_dei_1_en() */
int fe_tbl_ACLAction_l2_change_8021p_2_en(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.change_8021p_2_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_change_8021p_2_en() */
int fe_tbl_ACLAction_l2_change_dei_2_en(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l2.change_dei_2_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l2_change_dei_2_en() */
int fe_tbl_ACLAction_l3_dscp_vld(char *pString,
				 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.dscp_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_dscp_vld() */
int fe_tbl_ACLAction_l3_dscp(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.dscp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_dscp() */
int fe_tbl_ACLAction_l3_dscp_pri(char *pString,
				 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.dscp_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_dscp_pri() */
int fe_tbl_ACLAction_l3_ip_sa_replace_en_vld(char *pString,
					     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_sa_replace_en_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_sa_replace_en_vld() */
int fe_tbl_ACLAction_l3_ip_sa_replace_en(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_sa_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_sa_replace_en() */
int fe_tbl_ACLAction_l3_ip_sa_replace_en_pri(char *pString,
					     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_sa_replace_en_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_sa_replace_en_pri() */
int fe_tbl_ACLAction_l3_ip_da_replace_en_vld(char *pString,
					     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_da_replace_en_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_da_replace_en_vld() */
int fe_tbl_ACLAction_l3_ip_da_replace_en(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_da_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_da_replace_en() */
int fe_tbl_ACLAction_l3_ip_da_replace_en_pri(char *pString,
					     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_da_replace_en_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_da_replace_en_pri() */
int fe_tbl_ACLAction_l3_ip_sa_idx(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_sa_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_sa_idx() */
int fe_tbl_ACLAction_l3_ip_da_idx(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.ip_da_idx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_ip_da_idx() */
int fe_tbl_ACLAction_l3_change_dscp_en(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.change_dscp_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_change_dscp_en() */
int fe_tbl_ACLAction_l3_decr_ttl_hoplimit_vld(char *pString,
					      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.decr_ttl_hoplimit_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_decr_ttl_hoplimit_vld() */
int fe_tbl_ACLAction_l3_decr_ttl_hoplimit(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.decr_ttl_hoplimit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_decr_ttl_hoplimit() */
int fe_tbl_ACLAction_l3_decr_ttl_hoplimit_pri(char *pString,
					      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l3.decr_ttl_hoplimit_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l3_decr_ttl_hoplimit_pri() */
int fe_tbl_ACLAction_l4_sp_replace_en_vld(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.sp_replace_en_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_sp_replace_en_vld() */
int fe_tbl_ACLAction_l4_sp_replace_en(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.sp_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_sp_replace_en() */
int fe_tbl_ACLAction_l4_sp(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.sp =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_sp() */
int fe_tbl_ACLAction_l4_sp_replace_en_pri(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.sp_replace_en_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_sp_replace_en_pri() */
int fe_tbl_ACLAction_l4_dp_replace_en_vld(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.dp_replace_en_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_dp_replace_en_vld() */
int fe_tbl_ACLAction_l4_dp_replace_en(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.dp_replace_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_dp_replace_en() */
int fe_tbl_ACLAction_l4_dp(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.dp =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_dp() */
int fe_tbl_ACLAction_l4_dp_replace_en_pri(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.l4.dp_replace_en_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_l4_dp_replace_en_pri() */
int fe_tbl_ACLAction_misc_voq_vld(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.voq_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_voq_vld() */
int fe_tbl_ACLAction_misc_voq_pri(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.voq_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_voq_pri() */
int fe_tbl_ACLAction_misc_voq(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.voq =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_voq() */
int fe_tbl_ACLAction_misc_ldpid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.ldpid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_ldpid() */
int fe_tbl_ACLAction_misc_cpucopy(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.cpucopy =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_cpucopy() */
int fe_tbl_ACLAction_misc_cpucopy_voq(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.cpucopy_voq =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_cpucopy_voq() */
int fe_tbl_ACLAction_misc_cpucopy_pri(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.cpucopy_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_cpucopy_pri() */
int fe_tbl_ACLAction_misc_mirror_vld(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mirror_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mirror_vld() */
int fe_tbl_ACLAction_misc_mirror_id(char *pString,
				    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mirror_id =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mirror_id() */
int fe_tbl_ACLAction_misc_mirror_id_pri(char *pString,
					ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mirror_id_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mirror_id_pri() */
int fe_tbl_ACLAction_misc_wred_cos_vld(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.wred_cos_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_wred_cos_vld() */
int fe_tbl_ACLAction_misc_wred_cos(char *pString,
				   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.wred_cos =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_wred_cos() */
int fe_tbl_ACLAction_misc_wred_cos_pri(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.wred_cos_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_wred_cos_pri() */
int fe_tbl_ACLAction_misc_pre_mark_vld(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.pre_mark_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_pre_mark_vld() */
int fe_tbl_ACLAction_misc_pre_mark(char *pString,
				   ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.pre_mark =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_pre_mark() */
int fe_tbl_ACLAction_misc_pre_mark_pri(char *pString,
				       ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.pre_mark_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_pre_mark_pri() */
int fe_tbl_ACLAction_misc_policer_id_vld(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.policer_id_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_policer_id_vld() */
int fe_tbl_ACLAction_misc_policer_id(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.policer_id =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_policer_id() */
int fe_tbl_ACLAction_misc_policer_id_pri(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.policer_id_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_policer_id_pri() */
int fe_tbl_ACLAction_misc_drop_permit_vld(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.drop_permit_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_drop_permit_vld() */
int fe_tbl_ACLAction_misc_drop(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.drop =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_drop() */
int fe_tbl_ACLAction_misc_permit(char *pString,
				 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.permit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_permit() */
int fe_tbl_ACLAction_misc_drop_permit_pri(char *pString,
					  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.drop_permit_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_drop_permit_pri() */
int fe_tbl_ACLAction_misc_fwdtype_vld(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.fwdtype_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_fwdtype_vld() */
int fe_tbl_ACLAction_misc_fwdtype(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.fwdtype =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_fwdtype() */
int fe_tbl_ACLAction_misc_fwdtype_pri(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.fwdtype_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_fwdtype_pri() */
int fe_tbl_ACLAction_misc_mcgid_vld(char *pString,
				    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mcgid_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mcgid_vld() */
int fe_tbl_ACLAction_misc_mcgid(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mcgid =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mcgid() */
int fe_tbl_ACLAction_misc_mcdid_pri(char *pString,
				    ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.mcdid_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_mcdid_pri() */
int fe_tbl_ACLAction_misc_keep_ts_vld(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.keep_ts_vld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_keep_ts_vld() */
int fe_tbl_ACLAction_misc_keep_ts(char *pString,
				  ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.keep_ts =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_keep_ts() */
int fe_tbl_ACLAction_misc_keep_ts_pri(char *pString,
				      ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.keep_ts_pri =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_keep_ts_pri() */
int fe_tbl_ACLAction_misc_voq_cpupid(char *pString,
				     ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.voq_cpupid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_voq_cpupid() */
int fe_tbl_ACLAction_misc_cpucopy_cpupid(char *pString,
					 ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.misc.cpucopy_cpupid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_misc_cpucopy_cpupid() */
int fe_tbl_ACLAction_parity(char *pString, ACL_INFO * pACLInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pACLInfo->tbl_ACL.action.parity =
	    (cs_boolean) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_ACLAction_parity() */

/*==========================================================================*/
NE_FIELD_T fe_tbl_ACL[] = {
	{"rule.rule_valid",			fe_tbl_ACLRule_rule_valid},

	{"rule.l2.l2_mac_da.0",		fe_tbl_ACLRule_l2_l2_mac_da_0},
	{"rule.l2.l2_mac_da.1",		fe_tbl_ACLRule_l2_l2_mac_da_1},
	{"rule.l2.l2_mac_da.2",		fe_tbl_ACLRule_l2_l2_mac_da_2},
	{"rule.l2.l2_mac_da.3",		fe_tbl_ACLRule_l2_l2_mac_da_3},
	{"rule.l2.l2_mac_da.4",		fe_tbl_ACLRule_l2_l2_mac_da_4},
	{"rule.l2.l2_mac_da.5",		fe_tbl_ACLRule_l2_l2_mac_da_5},
	{"rule.l2.l2_mac_sa.0",		fe_tbl_ACLRule_l2_l2_mac_sa_0},
	{"rule.l2.l2_mac_sa.1",		fe_tbl_ACLRule_l2_l2_mac_sa_1},
	{"rule.l2.l2_mac_sa.2",		fe_tbl_ACLRule_l2_l2_mac_sa_2},
	{"rule.l2.l2_mac_sa.3",		fe_tbl_ACLRule_l2_l2_mac_sa_3},
	{"rule.l2.l2_mac_sa.4",		fe_tbl_ACLRule_l2_l2_mac_sa_4},
	{"rule.l2.l2_mac_sa.5",		fe_tbl_ACLRule_l2_l2_mac_sa_5},
	{"rule.l2.eth_type",			fe_tbl_ACLRule_l2_eth_type},
	{"rule.l2.len_encoded",		fe_tbl_ACLRule_l2_len_encoded},
	{"rule.l2.tpid_1_vld",		fe_tbl_ACLRule_l2_tpid_1_vld},
	{"rule.l2.tpid_enc_1",		fe_tbl_ACLRule_l2_tpid_enc_1},
	{"rule.l2.vid_1",			fe_tbl_ACLRule_l2_vid_1},
	{"rule.l2._8021p_1",			fe_tbl_ACLRule_l2__8021p_1},
	{"rule.l2.dei_1",			fe_tbl_ACLRule_l2_dei_1},
	{"rule.l2.tpid_2_vld",		fe_tbl_ACLRule_l2_tpid_2_vld},
	{"rule.l2.tpid_enc_2",		fe_tbl_ACLRule_l2_tpid_enc_2},
	{"rule.l2.vid_2",			fe_tbl_ACLRule_l2_vid_2},
	{"rule.l2._8021p_2",			fe_tbl_ACLRule_l2__8021p_2},
	{"rule.l2.dei_2",			fe_tbl_ACLRule_l2_dei_2},
	{"rule.l2.l2_mac_da_mask",		fe_tbl_ACLRule_l2_l2_mac_da_mask},
	{"rule.l2.l2_mac_sa_mask",		fe_tbl_ACLRule_l2_l2_mac_sa_mask},
	{"rule.l2.ethertype_mask",		fe_tbl_ACLRule_l2_ethertype_mask},
	{"rule.l2.len_encoded_mask",		fe_tbl_ACLRule_l2_len_encoded_mask},
	{"rule.l2.tpid_1_vld_mask",		fe_tbl_ACLRule_l2_tpid_1_vld_mask},
	{"rule.l2.tpid_enc_1_mask",		fe_tbl_ACLRule_l2_tpid_enc_1_mask},
	{"rule.l2.vid_1_mask",		fe_tbl_ACLRule_l2_vid_1_mask},
	{"rule.l2._8021p_1_mask",		fe_tbl_ACLRule_l2__8021p_1_mask},
	{"rule.l2.dei_1_mask",		fe_tbl_ACLRule_l2_dei_1_mask},
	{"rule.l2.tpid_2_vld_mask",		fe_tbl_ACLRule_l2_tpid_2_vld_mask},
	{"rule.l2.tpid_enc_2_mask",		fe_tbl_ACLRule_l2_tpid_enc_2_mask},
	{"rule.l2.vid_2_mask",		fe_tbl_ACLRule_l2_vid_2_mask},
	{"rule.l2._8021p_2_mask",		fe_tbl_ACLRule_l2__8021p_2_mask},
	{"rule.l2.dei_2_mask",		fe_tbl_ACLRule_l2_dei_2_mask},
	{"rule.l2.da_an_mac_sel",		fe_tbl_ACLRule_l2_da_an_mac_sel},
	{"rule.l2.da_an_mac_sel_mask",	fe_tbl_ACLRule_l2_da_an_mac_sel_mask},
	{"rule.l2.da_an_mac_hit",		fe_tbl_ACLRule_l2_da_an_mac_hit},
	{"rule.l2.da_an_mac_hit_mask",	fe_tbl_ACLRule_l2_da_an_mac_hit_mask},
	{"rule.l2.sa_bng_mac_sel",		fe_tbl_ACLRule_l2_sa_bng_mac_sel},
	{"rule.l2.sa_bng_mac_sel_mask",	fe_tbl_ACLRule_l2_sa_bng_mac_sel_mask},
	{"rule.l2.sa_bng_mac_hit",		fe_tbl_ACLRule_l2_sa_bng_mac_hit},
	{"rule.l2.sa_bng_mac_hit_mask",	fe_tbl_ACLRule_l2_sa_bng_mac_hit_mask},
	{"rule.l2.pppoe_session_id_vld",	fe_tbl_ACLRule_l2_pppoe_session_id_vld},
	{"rule.l2.pppoe_session_id_vld_mask",fe_tbl_ACLRule_l2_pppoe_session_id_vld_mask},
	{"rule.l2.pppoe_session_id",		fe_tbl_ACLRule_l2_pppoe_session_id},
	{"rule.l2.pppoe_session_id_mask",	fe_tbl_ACLRule_l2_pppoe_session_id_mask},
	{"rule.l2.ppp_protocol_vld",		fe_tbl_ACLRule_l2_ppp_protocol_vld},
	{"rule.l2.ppp_protocol_vld_mask",	fe_tbl_ACLRule_l2_ppp_protocol_vld_mask},
	{"rule.l2.ppp_protocol",		fe_tbl_ACLRule_l2_ppp_protocol},
	{"rule.l2.ppp_protocol_mask",	fe_tbl_ACLRule_l2_ppp_protocol_mask},

	{"rule.l3.ip_vld",			fe_tbl_ACLRule_l3_ip_vld}, 
	{"rule.l3.ip_ver",			fe_tbl_ACLRule_l3_ip_ver},
	{"rule.l3.da.0",			fe_tbl_ACLRule_l3_da_0},
	{"rule.l3.da.1",			fe_tbl_ACLRule_l3_da_1},
	{"rule.l3.da.2",			fe_tbl_ACLRule_l3_da_2},
	{"rule.l3.da.3",			fe_tbl_ACLRule_l3_da_3}, 
	{"rule.l3.sa.0",			fe_tbl_ACLRule_l3_sa_0},
	{"rule.l3.sa.1",			fe_tbl_ACLRule_l3_sa_1},
	{"rule.l3.sa.2",			fe_tbl_ACLRule_l3_sa_2},
	{"rule.l3.sa.3",			fe_tbl_ACLRule_l3_sa_3},
	{"rule.l3.dscp",			fe_tbl_ACLRule_l3_dscp}, 
	{"rule.l3.ecn",			fe_tbl_ACLRule_l3_ecn},
	{"rule.l3.proto",			fe_tbl_ACLRule_l3_proto},
	{"rule.l3.fragment",			fe_tbl_ACLRule_l3_fragment},
	{"rule.l3.options",			fe_tbl_ACLRule_l3_options},
	{"rule.l3.ipv6_flow_label",		fe_tbl_ACLRule_l3_ipv6_flow_label},	
	{"rule.l3.ttl_hoplimit",		fe_tbl_ACLRule_l3_ttl_hoplimit},
	{"rule.l3.ip_vld_mask",		fe_tbl_ACLRule_l3_ip_vld_mask},
	{"rule.l3.ip_ver_mask",		fe_tbl_ACLRule_l3_ip_ver_mask},
	{"rule.l3.ip_da_mask",		fe_tbl_ACLRule_l3_ip_da_mask},
	{"rule.l3.ip_sa_mask",		fe_tbl_ACLRule_l3_ip_sa_mask}, 
	{"rule.l3.dscp_mask",		fe_tbl_ACLRule_l3_dscp_mask},
	{"rule.l3.ecn_mask",			fe_tbl_ACLRule_l3_ecn_mask},
	{"rule.l3.ip_proto_mask",		fe_tbl_ACLRule_l3_ip_proto_mask},
	{"rule.l3.ip_fragment_mask",		fe_tbl_ACLRule_l3_ip_fragment_mask},
	{"rule.l3.ip_options_mask",		fe_tbl_ACLRule_l3_ip_options_mask},	
	{"rule.l3.ipv6_flow_label_mask",	fe_tbl_ACLRule_l3_ipv6_flow_label_mask},
	{"rule.l3.ttl_hoplimit_mask",	fe_tbl_ACLRule_l3_ttl_hoplimit_mask},
	{"rule.l3.spi",			fe_tbl_ACLRule_l3_spi},
	{"rule.l3.spi_mask",			fe_tbl_ACLRule_l3_spi_mask},
	{"rule.l3.spi_vld",			fe_tbl_ACLRule_l3_spi_vld},
	{"rule.l3.spi_vld_mask",		fe_tbl_ACLRule_l3_spi_vld_mask},
	{"rule.l3.ipv6_ndp",			fe_tbl_ACLRule_l3_ipv6_ndp},
	{"rule.l3.ipv6_ndp_mask",		fe_tbl_ACLRule_l3_ipv6_ndp_mask},
	{"rule.l3.ipv6_hbh",			fe_tbl_ACLRule_l3_ipv6_hbh},
	{"rule.l3.ipv6_hbh_mask",		fe_tbl_ACLRule_l3_ipv6_hbh_mask},
	{"rule.l3.ipv6_rh",			fe_tbl_ACLRule_l3_ipv6_rh},
	{"rule.l3.ipv6_rh_mask",		fe_tbl_ACLRule_l3_ipv6_rh_mask},
	{"rule.l3.ipv6_doh",			fe_tbl_ACLRule_l3_ipv6_doh},
	{"rule.l3.ipv6_doh_mask",		fe_tbl_ACLRule_l3_ipv6_doh_mask},

	{"rule.l4.l4_valid",			fe_tbl_ACLRule_l4_l4_valid},
	{"rule.l4.dp_lo",			fe_tbl_ACLRule_l4_dp_lo},
	{"rule.l4.dp_hi",			fe_tbl_ACLRule_l4_dp_hi},
	{"rule.l4.sp_lo",			fe_tbl_ACLRule_l4_sp_lo},
	{"rule.l4.sp_hi",			fe_tbl_ACLRule_l4_sp_hi},
	{"rule.l4.l4_valid_mask",		fe_tbl_ACLRule_l4_l4_valid_mask},
	{"rule.l4.l4_mask",			fe_tbl_ACLRule_l4_l4_mask},

	{"rule.misc.lspid",			fe_tbl_ACLRule_misc_lspid},
	{"rule.misc.orig_lspid",		fe_tbl_ACLRule_misc_orig_lspid},
	{"rule.misc.fwd_type",		fe_tbl_ACLRule_misc_fwd_type},
	{"rule.misc.spl_pkt_vec",		fe_tbl_ACLRule_misc_spl_pkt_vec},
	{"rule.misc.class_hit",		fe_tbl_ACLRule_misc_class_hit},
	{"rule.misc.class_svidx",		fe_tbl_ACLRule_misc_class_svidx},
	{"rule.misc.lpm_hit",		fe_tbl_ACLRule_misc_lpm_hit},
	{"rule.misc.lpm_hit_idx",		fe_tbl_ACLRule_misc_lpm_hit_idx},
	{"rule.misc.hash_hit",		fe_tbl_ACLRule_misc_hash_hit},
	{"rule.misc.hash_hit_idx",		fe_tbl_ACLRule_misc_hash_hit_idx},
	{"rule.misc.l7_field",		fe_tbl_ACLRule_misc_l7_field},
	{"rule.misc.lspid_mask",		fe_tbl_ACLRule_misc_lspid_mask},
	{"rule.misc.orig_lspid_mask",	fe_tbl_ACLRule_misc_orig_lspid_mask},
	{"rule.misc.fwd_type_mask",		fe_tbl_ACLRule_misc_fwd_type_mask},
	{"rule.misc.spl_pkt_vec_mask",	fe_tbl_ACLRule_misc_spl_pkt_vec_mask},
	{"rule.misc.class_hit_mask",		fe_tbl_ACLRule_misc_class_hit_mask},
	{"rule.misc.class_svidx_mask",	fe_tbl_ACLRule_misc_class_svidx_mask},
	{"rule.misc.lpm_hit_mask",		fe_tbl_ACLRule_misc_lpm_hit_mask},
	{"rule.misc.lpm_hit_idx_mask",	fe_tbl_ACLRule_misc_lpm_hit_idx_mask},
	{"rule.misc.hash_hit_mask",		fe_tbl_ACLRule_misc_hash_hit_mask},
	{"rule.misc.hash_hit_idx_mask",	fe_tbl_ACLRule_misc_hash_hit_idx_mask},
	{"rule.misc.l7_field_mask",		fe_tbl_ACLRule_misc_l7_field_mask},
	{"rule.misc.flags_vec",		fe_tbl_ACLRule_misc_flags_vec},
	{"rule.misc.flags_vec_mask",		fe_tbl_ACLRule_misc_flags_vec_mask},
	{"rule.misc.flags_vec_or",		fe_tbl_ACLRule_misc_flags_vec_or},
	{"rule.misc.spl_pkt_vec_or",		fe_tbl_ACLRule_misc_spl_pkt_vec_or},
	{"rule.misc.recirc_idx",		fe_tbl_ACLRule_misc_recirc_idx},
	{"rule.misc.recirc_idx_mask",	fe_tbl_ACLRule_misc_recirc_idx_mask},
	{"rule.misc.ne_vec",			fe_tbl_ACLRule_misc_ne_vec},
	{"rule.misc.mc_idx",			fe_tbl_ACLRule_misc_mc_idx},
	{"rule.misc.mc_idx_mask",		fe_tbl_ACLRule_misc_mc_idx_mask},
	{"rule.misc.sdb_drop",		fe_tbl_ACLRule_misc_sdb_drop},
	{"rule.misc.sdb_drop_mask",		fe_tbl_ACLRule_misc_sdb_drop_mask},
	{"rule.misc.fwd_drop",		fe_tbl_ACLRule_misc_fwd_drop},
	{"rule.misc.fwd_drop_mask",		fe_tbl_ACLRule_misc_fwd_drop_mask},
	{"rule.misc.pktlen_rng_vec", 	fe_tbl_ACLRule_misc_pktlen_rng_vec},
	{"rule.misc.pktlen_rng_vec_mask", 	fe_tbl_ACLRule_misc_pktlen_rng_vec_mask},
	{"rule.misc.rsvd_879_878", 		fe_tbl_ACLRule_misc_rsvd_879_878},
	{"rule.parity", 			fe_tbl_ACLRule_parity},

	{"action.l2._8021p_1_vld", 		fe_tbl_ACLAction_l2__8021p_1_vld},
	{"action.l2._8021p_1", 		fe_tbl_ACLAction_l2__8021p_1},
	{"action.l2._8021p_1_pri", 		fe_tbl_ACLAction_l2__8021p_1_pri},
	{"action.l2.dei_1_vld", 		fe_tbl_ACLAction_l2_dei_1_vld},
	{"action.l2.dei_1", 			fe_tbl_ACLAction_l2_dei_1},
	{"action.l2.dei_1_pri", 		fe_tbl_ACLAction_l2_dei_1_pri},
	{"action.l2._8021p_2_vld", 		fe_tbl_ACLAction_l2__8021p_2_vld},
	{"action.l2._8021p_2", 		fe_tbl_ACLAction_l2__8021p_2},
	{"action.l2._8021p_2_pri", 		fe_tbl_ACLAction_l2__8021p_2_pri},
	{"action.l2.dei_2_vld", 		fe_tbl_ACLAction_l2_dei_2_vld},
	{"action.l2.dei_2", 			fe_tbl_ACLAction_l2_dei_2},
	{"action.l2.dei_2_pri", 		fe_tbl_ACLAction_l2_dei_2_pri},
	{"action.l2.first_vlan_cmd_vld", 	fe_tbl_ACLAction_l2_first_vlan_cmd_vld},
	{"action.l2.first_vlan_cmd", 		fe_tbl_ACLAction_l2_first_vlan_cmd},
	{"action.l2.first_vid", 		fe_tbl_ACLAction_l2_first_vid},
	{"action.l2.first_tpid_enc", 		fe_tbl_ACLAction_l2_first_tpid_enc},
	{"action.l2.first_vlan_cmd_pri", 	fe_tbl_ACLAction_l2_first_vlan_cmd_pri},
	{"action.l2.second_vlan_cmd_vld", 	fe_tbl_ACLAction_l2_second_vlan_cmd_vld},
	{"action.l2.second_vlan_cmd", 		fe_tbl_ACLAction_l2_second_vlan_cmd},
	{"action.l2.second_vid", 		fe_tbl_ACLAction_l2_second_vid},
	{"action.l2.second_tpid_enc", 		fe_tbl_ACLAction_l2_second_tpid_enc},
	{"action.l2.second_vlan_cmd_pri", 	fe_tbl_ACLAction_l2_second_vlan_cmd_pri},
	{"action.l2.mac_da_sa_replace_en_vld", fe_tbl_ACLAction_l2_mac_da_sa_replace_en_vld},
	{"action.l2.mac_da_replace_en", 	fe_tbl_ACLAction_l2_mac_da_replace_en},
	{"action.l2.mac_sa_replace_en", 	fe_tbl_ACLAction_l2_mac_sa_replace_en},
	{"action.l2.mac_da_sa_replace_en_pri", fe_tbl_ACLAction_l2_mac_da_sa_replace_en_pri},
	{"action.l2.l2_idx", 			fe_tbl_ACLAction_l2_l2_idx},
	{"action.l2.change_8021p_1_en", 	fe_tbl_ACLAction_l2_change_8021p_1_en},
	{"action.l2.change_dei_1_en", 		fe_tbl_ACLAction_l2_change_dei_1_en},
	{"action.l2.change_8021p_2_en", 	fe_tbl_ACLAction_l2_change_8021p_2_en},
	{"action.l2.change_dei_2_en", 		fe_tbl_ACLAction_l2_change_dei_2_en},
	{"action.l3.dscp_vld", 		fe_tbl_ACLAction_l3_dscp_vld},
	{"action.l3.dscp", 			fe_tbl_ACLAction_l3_dscp},
	{"action.l3.dscp_pri", 		fe_tbl_ACLAction_l3_dscp_pri},
	{"action.l3.ip_sa_replace_en_vld", 	fe_tbl_ACLAction_l3_ip_sa_replace_en_vld},
	{"action.l3.ip_sa_replace_en", 	fe_tbl_ACLAction_l3_ip_sa_replace_en},
	{"action.l3.ip_sa_replace_en_pri", 	fe_tbl_ACLAction_l3_ip_sa_replace_en_pri},
	{"action.l3.ip_da_replace_en_vld", 	fe_tbl_ACLAction_l3_ip_da_replace_en_vld},
	{"action.l3.ip_da_replace_en", 	fe_tbl_ACLAction_l3_ip_da_replace_en},
	{"action.l3.ip_da_replace_en_pri", 	fe_tbl_ACLAction_l3_ip_da_replace_en_pri},
	{"action.l3.ip_sa_idx", 		fe_tbl_ACLAction_l3_ip_sa_idx},
	{"action.l3.ip_da_idx", 		fe_tbl_ACLAction_l3_ip_da_idx},
	{"action.l3.change_dscp_en", 		fe_tbl_ACLAction_l3_change_dscp_en},
	{"action.l3.decr_ttl_hoplimit_vld", 	fe_tbl_ACLAction_l3_decr_ttl_hoplimit_vld},
	{"action.l3.decr_ttl_hoplimit", 	fe_tbl_ACLAction_l3_decr_ttl_hoplimit},
	{"action.l3.decr_ttl_hoplimit_pri", 	fe_tbl_ACLAction_l3_decr_ttl_hoplimit_pri},
	{"action.l4.sp_replace_en_vld", 	fe_tbl_ACLAction_l4_sp_replace_en_vld},
	{"action.l4.sp_replace_en", 		fe_tbl_ACLAction_l4_sp_replace_en},
	{"action.l4.sp", 			fe_tbl_ACLAction_l4_sp},
	{"action.l4.sp_replace_en_pri", 	fe_tbl_ACLAction_l4_sp_replace_en_pri},
	{"action.l4.dp_replace_en_vld", 	fe_tbl_ACLAction_l4_dp_replace_en_vld},
	{"action.l4.dp_replace_en", 		fe_tbl_ACLAction_l4_dp_replace_en},
	{"action.l4.dp", 			fe_tbl_ACLAction_l4_dp},
	{"action.l4.dp_replace_en_pri", 	fe_tbl_ACLAction_l4_dp_replace_en_pri},
	{"action.misc.voq_vld", 		fe_tbl_ACLAction_misc_voq_vld},
	{"action.misc.voq_pri", 		fe_tbl_ACLAction_misc_voq_pri},
	{"action.misc.voq", 			fe_tbl_ACLAction_misc_voq},
	{"action.misc.ldpid", 			fe_tbl_ACLAction_misc_ldpid},
	{"action.misc.cpucopy", 		fe_tbl_ACLAction_misc_cpucopy},
	{"action.misc.cpucopy_voq", 		fe_tbl_ACLAction_misc_cpucopy_voq},
	{"action.misc.cpucopy_pri", 		fe_tbl_ACLAction_misc_cpucopy_pri},
	{"action.misc.mirror_vld", 		fe_tbl_ACLAction_misc_mirror_vld},
	{"action.misc.mirror_id", 		fe_tbl_ACLAction_misc_mirror_id},
	{"action.misc.mirror_id_pri", 		fe_tbl_ACLAction_misc_mirror_id_pri},
	{"action.misc.wred_cos_vld", 		fe_tbl_ACLAction_misc_wred_cos_vld},
	{"action.misc.wred_cos", 		fe_tbl_ACLAction_misc_wred_cos},
	{"action.misc.wred_cos_pri", 		fe_tbl_ACLAction_misc_wred_cos_pri},
	{"action.misc.pre_mark_vld", 		fe_tbl_ACLAction_misc_pre_mark_vld},
	{"action.misc.pre_mark", 		fe_tbl_ACLAction_misc_pre_mark},
	{"action.misc.pre_mark_pri", 		fe_tbl_ACLAction_misc_pre_mark_pri},
	{"action.misc.policer_id_vld", 	fe_tbl_ACLAction_misc_policer_id_vld},
	{"action.misc.policer_id", 		fe_tbl_ACLAction_misc_policer_id},
	{"action.misc.policer_id_pri", 	fe_tbl_ACLAction_misc_policer_id_pri},
	{"action.misc.drop_permit_vld", 	fe_tbl_ACLAction_misc_drop_permit_vld},
	{"action.misc.drop", 			fe_tbl_ACLAction_misc_drop},
	{"action.misc.permit", 		fe_tbl_ACLAction_misc_permit},
	{"action.misc.drop_permit_pri", 	fe_tbl_ACLAction_misc_drop_permit_pri},
	{"action.misc.fwdtype_vld", 		fe_tbl_ACLAction_misc_fwdtype_vld},
	{"action.misc.fwdtype", 		fe_tbl_ACLAction_misc_fwdtype},
	{"action.misc.fwdtype_pri", 		fe_tbl_ACLAction_misc_fwdtype_pri},
	{"action.misc.mcgid_vld", 		fe_tbl_ACLAction_misc_mcgid_vld},
	{"action.misc.mcgid", 			fe_tbl_ACLAction_misc_mcgid},
	{"action.misc.mcdid_pri", 		fe_tbl_ACLAction_misc_mcdid_pri},
	{"action.misc.keep_ts_vld",		fe_tbl_ACLAction_misc_keep_ts_vld},
	{"action.misc.keep_ts", 		fe_tbl_ACLAction_misc_keep_ts},
	{"action.misc.keep_ts_pri", 		fe_tbl_ACLAction_misc_keep_ts_pri},
	{"action.misc.voq_cpupid", 		fe_tbl_ACLAction_misc_voq_cpupid},
	{"action.misc.cpucopy_cpupid", 	fe_tbl_ACLAction_misc_cpucopy_cpupid},
	{"action.parity", 			fe_tbl_ACLAction_parity},
	{0, 				0}
};
