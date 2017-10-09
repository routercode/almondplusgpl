/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblClassifier.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
                 fe module Classifier Table
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../neSendCmd.h"
#include "../misc.h"

/*
 * fe->Classifier
 */
int fe_tbl_Classifier_sdb_idx(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.sdb_idx =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_sdb_idx() */
int fe_tbl_Classifier_rule_priority(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.rule_priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_rule_priority() */
int fe_tbl_Classifier_entry_valid(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.entry_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_entry_valid() */
int fe_tbl_Classifier_parity(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_parity() */

/* */
/* fe->Classifier->port */
/* */
int fe_tbl_Classifier_port_lspid(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.lspid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_lspid() */
int fe_tbl_Classifier_port_hdr_a_orig_lspid(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.hdr_a_orig_lspid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_hdr_a_orig_lspid() */
int fe_tbl_Classifier_port_fwd_type(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.fwd_type =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_fwd_type() */
int fe_tbl_Classifier_port_hdr_a_flags_crcerr(char *pString,
					      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.hdr_a_flags_crcerr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_hdr_a_flags_crcerr() */
int fe_tbl_Classifier_port_l3_csum_err(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.l3_csum_err =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_l3_csum_err() */
int fe_tbl_Classifier_port_l4_csum_err(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.l4_csum_err =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_l4_csum_err() */
int fe_tbl_Classifier_port_not_hdr_a_flags_stsvld(char *pString,
						  CLASSIFIER_INFO *
						  pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.not_hdr_a_flags_stsvld =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_not_hdr_a_flags_stsvld() */
int fe_tbl_Classifier_port_lspid_mask(char *pString,
				      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_lspid_mask() */
int fe_tbl_Classifier_port_hdr_a_orig_lspid_mask(char *pString,
						 CLASSIFIER_INFO *
						 pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.hdr_a_orig_lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_hdr_a_orig_lspid_mask() */
int fe_tbl_Classifier_port_fwd_type_mask(char *pString,
					 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.fwd_type_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_fwd_type_mask() */
int fe_tbl_Classifier_port_hdr_a_flags_crcerr_mask(char *pString,
						   CLASSIFIER_INFO *
						   pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.hdr_a_flags_crcerr_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_hdr_a_flags_crcerr_mask() */
int fe_tbl_Classifier_port_l3_csum_err_mask(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.l3_csum_err_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_l3_csum_err_mask() */
int fe_tbl_Classifier_port_l4_csum_err_mask(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.l4_csum_err_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_l4_csum_err_mask() */
int fe_tbl_Classifier_port_not_hdr_a_flags_stsvld_mask(char *pString,
						       CLASSIFIER_INFO *
						       pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.not_hdr_a_flags_stsvld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_not_hdr_a_flags_stsvld_mask() */
int fe_tbl_Classifier_port_mcgid(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.mcgid =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_mcgid() */
int fe_tbl_Classifier_port_mcgid_mask(char *pString,
				      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.port.mcgid_mask =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_port_mcgid_mask() */

/*
 * fe->Classifier->fe_class_entry_l2_s
 */
int fe_tbl_Classifier_l2_tpid_enc_1(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_1() */
int fe_tbl_Classifier_l2_vid_1(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.vid_1 =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_vid_1() */
int fe_tbl_Classifier_l2__8021p_1(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2._8021p_1 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2__8021p_1() */
int fe_tbl_Classifier_l2_tpid_enc_2(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_2() */
int fe_tbl_Classifier_l2_vid_2(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.vid_2 =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_vid_2() */
int fe_tbl_Classifier_l2__8021p_2(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2._8021p_2 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2__8021p_2() */
int fe_tbl_Classifier_l2_tpid_enc_1_msb_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_1_msb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_1_msb_mask() */
int fe_tbl_Classifier_l2_tpid_enc_1_lsb_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_1_lsb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_1_lsb_mask() */
int fe_tbl_Classifier_l2_vid_1_mask(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.vid_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_vid_1_mask() */
int fe_tbl_Classifier_l2__8021p_1_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2._8021p_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2__8021p_1_mask() */
int fe_tbl_Classifier_l2_tpid_enc_2_msb_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_2_msb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_2_msb_mask() */
int fe_tbl_Classifier_l2_tpid_enc_2_lsb_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.tpid_enc_2_lsb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_tpid_enc_2_lsb_mask() */
int fe_tbl_Classifier_l2_vid_2_mask(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.vid_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_vid_2_mask() */
int fe_tbl_Classifier_l2__8021p_2_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2._8021p_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2__8021p_2_mask() */
int fe_tbl_Classifier_l2_da_an_mac_sel(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.da_an_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_da_an_mac_sel() */
int fe_tbl_Classifier_l2_da_an_mac_hit(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.da_an_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_da_an_mac_hit() */
int fe_tbl_Classifier_l2_sa_bng_mac_sel(char *pString,
					CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.sa_bng_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_sa_bng_mac_sel() */
int fe_tbl_Classifier_l2_sa_bng_mac_hit(char *pString,
					CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.sa_bng_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_sa_bng_mac_hit() */
int fe_tbl_Classifier_l2_da_an_mac_sel_mask(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.da_an_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_da_an_mac_sel_mask() */
int fe_tbl_Classifier_l2_da_an_mac_hit_mask(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.da_an_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_da_an_mac_hit_mask() */
int fe_tbl_Classifier_l2_sa_bng_mac_sel_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.sa_bng_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_sa_bng_mac_sel_mask */
int fe_tbl_Classifier_l2_sa_bng_mac_hit_mask(char *pString,
					     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.sa_bng_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_sa_bng_mac_hit_mask() */
int fe_tbl_Classifier_l2_ethertype_enc(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.ethertype_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_ethertype_enc() */
int fe_tbl_Classifier_l2_ethertype_enc_mask(char *pString,
					    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.ethertype_enc_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_ethertype_enc_mask() */
int fe_tbl_Classifier_l2_da(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	unsigned int tmpMac[6];
	int i, retStatus = STATUS_FAILURE;
	retStatus = ParseMAC(pString, tmpMac);
	if (retStatus == STATUS_SUCCESS) {
		for (i = 0; i < 6; i++) {
			pClassifierInfo->tbl_Classifier.l2.da[i] =
			    (cs_uint8) tmpMac[i];
		}
	}
	return retStatus;
} /*end fe_tbl_Classifier_l2_da() */
int fe_tbl_Classifier_l2_sa(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	unsigned int tmpMac[6];
	int i, retStatus = STATUS_FAILURE;
	retStatus = ParseMAC(pString, tmpMac);
	if (retStatus == STATUS_SUCCESS) {
		for (i = 0; i < 6; i++) {
			pClassifierInfo->tbl_Classifier.l2.sa[i] =
			    (cs_uint8) tmpMac[i];
		}
	}
	return retStatus;
} /*end fe_tbl_Classifier_l2_da_mask() */
int fe_tbl_Classifier_l2_da_mask(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_da_mask() */
int fe_tbl_Classifier_l2_sa_mask(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.sa_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_sa_mask() */
int fe_tbl_Classifier_l2_mcast_da(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.mcast_da =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_mcast_da() */
int fe_tbl_Classifier_l2_bcast_da(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.bcast_da =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_bcast_da() */
int fe_tbl_Classifier_l2_mcast_da_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.mcast_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_mcast_da_mask() */
int fe_tbl_Classifier_l2_bcast_da_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.bcast_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_bcast_da_mask() */
int fe_tbl_Classifier_l2_len_encoded(char *pString,
				     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.len_encoded =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_len_encoded() */
int fe_tbl_Classifier_l2_len_encoded_mask(char *pString,
					  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l2.len_encoded_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l2_len_encoded_mask() */

/*
 * fe->Classifier->fe_class_entry_l3_s
 */
int fe_tbl_Classifier_l3_dscp(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.dscp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_dscp() */
int fe_tbl_Classifier_l3_ecn(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ecn =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ecn() */
int fe_tbl_Classifier_l3_ip_prot(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_prot =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_prot() */
int fe_tbl_Classifier_l3_ip_da(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}
	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pClassifierInfo->tbl_Classifier.l3.da[i] = ipVal[i];
	}
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_da() */
int fe_tbl_Classifier_l3_ip_sa(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}
	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pClassifierInfo->tbl_Classifier.l3.sa[i] = ipVal[i];
	}
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_sa() */
int fe_tbl_Classifier_l3_ip_valid(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_valid() */
int fe_tbl_Classifier_l3_ip_ver(char *pString,
				CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_ver =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_ver() */
int fe_tbl_Classifier_l3_ip_frag(char *pString,
				 CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_frag =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_frag() */
int fe_tbl_Classifier_l3_dscp_mask(char *pString,
				   CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.dscp_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_dscp_mask() */
int fe_tbl_Classifier_l3_ecn_mask(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ecn_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ecn_mask() */
int fe_tbl_Classifier_l3_ip_prot_mask(char *pString,
				      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_prot_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_prot_mask() */
int fe_tbl_Classifier_l3_ip_sa_mask(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_sa_mask =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_sa_mask() */
int fe_tbl_Classifier_l3_ip_da_mask(char *pString,
				    CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_da_mask =
	    (cs_uint16) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_da_mask() */
int fe_tbl_Classifier_l3_ip_valid_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_valid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_valid_mask() */
int fe_tbl_Classifier_l3_ip_ver_mask(char *pString,
				     CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_ver_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_ver_mask() */
int fe_tbl_Classifier_l3_ip_frag_mask(char *pString,
				      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.ip_frag_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_ip_frag_mask() */
int fe_tbl_Classifier_l3_spi(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.spi =
	    (cs_uint32) strtoul(pString, NULL, 16);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_spi() */
int fe_tbl_Classifier_l3_spi_valid(char *pString,
				   CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.spi_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_spi_valid() */
int fe_tbl_Classifier_l3_spi_mask(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.spi_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_spi_mask() */
int fe_tbl_Classifier_l3_spi_valid_mask(char *pString,
					CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l3.spi_valid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l3_spi_valid_mask() */

/*
 * fe->Classifier->fe_class_entry_l4_s
 */
int fe_tbl_Classifier_l4_l4_sp(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l4.l4_sp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l4_l4_sp() */
int fe_tbl_Classifier_l4_l4_dp(char *pString, CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l4.l4_dp =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l4_l4_dp() */
int fe_tbl_Classifier_l4_l4_valid(char *pString,
				  CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l4.l4_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l4_l4_valid() */
int fe_tbl_Classifier_l4_l4_port_mask(char *pString,
				      CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l4.l4_port_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l4_l4_port_mask() */
int fe_tbl_Classifier_l4_l4_valid_mask(char *pString,
				       CLASSIFIER_INFO * pClassifierInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pClassifierInfo->tbl_Classifier.l4.l4_valid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_Classifier_l4_l4_valid_mask() */

/*======================================================================== */

NE_FIELD_T fe_tbl_Classifier[] = {
	{"sdb_idx",			fe_tbl_Classifier_sdb_idx},
	{"rule_priority",		fe_tbl_Classifier_rule_priority},
	{"entry_valid",			fe_tbl_Classifier_entry_valid},
	{"parity",			fe_tbl_Classifier_parity},

	{"port.lspid",			fe_tbl_Classifier_port_lspid},
	{"port.hdr_a_orig_lspid",	fe_tbl_Classifier_port_hdr_a_orig_lspid},
	{"port.fwd_type",		fe_tbl_Classifier_port_fwd_type},
	{"port.hdr_a_flags_crcerr",	fe_tbl_Classifier_port_hdr_a_flags_crcerr},
	{"port.l3_csum_err",		fe_tbl_Classifier_port_l3_csum_err},
	{"port.l4_csum_err",		fe_tbl_Classifier_port_l4_csum_err},
	{"port.not_hdr_a_flags_stsvld",	fe_tbl_Classifier_port_not_hdr_a_flags_stsvld},
	{"port.lspid_mask",		fe_tbl_Classifier_port_lspid_mask},
	{"port.hdr_a_orig_lspid_mask",	fe_tbl_Classifier_port_hdr_a_orig_lspid_mask},
	{"port.fwd_type_mask",		fe_tbl_Classifier_port_fwd_type_mask},
	{"port.hdr_a_flags_crcerr_mask",fe_tbl_Classifier_port_hdr_a_flags_crcerr_mask},
	{"port.l3_csum_err_mask",	fe_tbl_Classifier_port_l3_csum_err_mask},
	{"port.l4_csum_err_mask",	fe_tbl_Classifier_port_l4_csum_err_mask},
	{"port.not_hdr_a_flags_stsvld_mask", fe_tbl_Classifier_port_not_hdr_a_flags_stsvld_mask},
	{"port.mcgid",			fe_tbl_Classifier_port_mcgid},
	{"port.mcgid_mask",		fe_tbl_Classifier_port_mcgid_mask},

	{"l2.tpid_enc_1",		fe_tbl_Classifier_l2_tpid_enc_1},
	{"l2.vid_1",			fe_tbl_Classifier_l2_vid_1},
	{"l2._8021p_1",			fe_tbl_Classifier_l2__8021p_1},
	{"l2.tpid_enc_2",		fe_tbl_Classifier_l2_tpid_enc_2},
	{"l2.vid_2",			fe_tbl_Classifier_l2_vid_2},
	{"l2._8021p_2",			fe_tbl_Classifier_l2__8021p_2},
	{"l2.tpid_enc_1_msb_mask",	fe_tbl_Classifier_l2_tpid_enc_1_msb_mask},
	{"l2.tpid_enc_1_lsb_mask",	fe_tbl_Classifier_l2_tpid_enc_1_lsb_mask},
	{"l2.vid_1_mask",		fe_tbl_Classifier_l2_vid_1_mask},
	{"l2._8021p_1_mask",		fe_tbl_Classifier_l2__8021p_1_mask},
	{"l2.tpid_enc_2_msb_mask",	fe_tbl_Classifier_l2_tpid_enc_2_msb_mask},
	{"l2.tpid_enc_2_lsb_mask",	fe_tbl_Classifier_l2_tpid_enc_2_lsb_mask},
	{"l2.vid_2_mask",		fe_tbl_Classifier_l2_vid_2_mask},
	{"l2._8021p_2_mask",		fe_tbl_Classifier_l2__8021p_2_mask},
	{"l2.da_an_mac_sel",		fe_tbl_Classifier_l2_da_an_mac_sel},
	{"l2.da_an_mac_hit",		fe_tbl_Classifier_l2_da_an_mac_hit},
	{"l2.sa_bng_mac_sel",		fe_tbl_Classifier_l2_sa_bng_mac_sel},
	{"l2.sa_bng_mac_hit",		fe_tbl_Classifier_l2_sa_bng_mac_hit},
	{"l2.da_an_mac_sel_mask",	fe_tbl_Classifier_l2_da_an_mac_sel_mask},
	{"l2.da_an_mac_hit_mask",	fe_tbl_Classifier_l2_da_an_mac_hit_mask},
	{"l2.sa_bng_mac_sel_mask",	fe_tbl_Classifier_l2_sa_bng_mac_sel_mask},
	{"l2.sa_bng_mac_hit_mask",	fe_tbl_Classifier_l2_sa_bng_mac_hit_mask},
	{"l2.ethertype_enc",		fe_tbl_Classifier_l2_ethertype_enc},
	{"l2.ethertype_enc_mask",	fe_tbl_Classifier_l2_ethertype_enc_mask},
	{"l2.da",			fe_tbl_Classifier_l2_da},
	{"l2.sa",			fe_tbl_Classifier_l2_sa},
	{"l2.da_mask",			fe_tbl_Classifier_l2_da_mask},
	{"l2.sa_mask",			fe_tbl_Classifier_l2_sa_mask},
	{"l2.mcast_da",			fe_tbl_Classifier_l2_mcast_da},
	{"l2.bcast_da",			fe_tbl_Classifier_l2_bcast_da},
	{"l2.mcast_da_mask",		fe_tbl_Classifier_l2_mcast_da_mask},
	{"l2.bcast_da_mask",		fe_tbl_Classifier_l2_bcast_da_mask},
	{"l2.len_encoded",		fe_tbl_Classifier_l2_len_encoded},
	{"l2.len_encoded_mask",		fe_tbl_Classifier_l2_len_encoded_mask},

	{"l3.dscp",			fe_tbl_Classifier_l3_dscp},
	{"l3.ecn",			fe_tbl_Classifier_l3_ecn},
	{"l3.ip_prot",			fe_tbl_Classifier_l3_ip_prot},
	{"l3.ip_da",			fe_tbl_Classifier_l3_ip_da},
	{"l3.ip_sa",			fe_tbl_Classifier_l3_ip_sa},
	{"l3.ip_valid",			fe_tbl_Classifier_l3_ip_valid},
	{"l3.ip_ver",			fe_tbl_Classifier_l3_ip_ver},
	{"l3.ip_frag",			fe_tbl_Classifier_l3_ip_frag},
	{"l3.dscp_mask",		fe_tbl_Classifier_l3_dscp_mask},
	{"l3.ecn_mask",			fe_tbl_Classifier_l3_ecn_mask},
	{"ip_prot_mask",		fe_tbl_Classifier_l3_ip_prot_mask},
	{"l3.ip_sa_mask",		fe_tbl_Classifier_l3_ip_sa_mask},
	{"l3.ip_da_mask",		fe_tbl_Classifier_l3_ip_da_mask},
	{"l3.ip_valid_mask",		fe_tbl_Classifier_l3_ip_valid_mask},
	{"l3.ip_ver_mask",		fe_tbl_Classifier_l3_ip_ver_mask},
	{"l3.ip_frag_mask",		fe_tbl_Classifier_l3_ip_frag_mask},
	{"l3.spi",			fe_tbl_Classifier_l3_spi},
	{"l3.spi_valid",		fe_tbl_Classifier_l3_spi_valid},
	{"l3.spi_mask",			fe_tbl_Classifier_l3_spi_mask},
	{"l3.spi_valid_mask",		fe_tbl_Classifier_l3_spi_valid_mask},

	{"l4.l4_sp",			fe_tbl_Classifier_l4_l4_sp},
	{"l4.l4_dp",			fe_tbl_Classifier_l4_l4_dp},
	{"l4.l4_valid",			fe_tbl_Classifier_l4_l4_valid},
	{"l4.l4_port_mask",		fe_tbl_Classifier_l4_l4_port_mask},
	{"l4.l4_valid_mask",		fe_tbl_Classifier_l4_l4_valid_mask},

	{0,					0}
};
