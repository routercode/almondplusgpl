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
#include "feTblHashHash.h"


/* fe->Hash_Mash */

int fe_tbl_HashHash_mac_da(char *pString, fe_hash_hash_s * pfeHashHash)
{
	unsigned int tmpMac[6];
	int i, retStatus = STATUS_FAILURE;
	retStatus = ParseMAC(pString, tmpMac);
	if (retStatus == STATUS_SUCCESS) {
		for (i = 0; i < 6; i++) {
			pfeHashHash->swhash.mac_da[i] = (cs_uint8) tmpMac[i];
		}
	}
#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mac_da:\n");

#endif /* DBG_HASH_FIELD */
	return retStatus;
} /* end fe_tbl_HashHash_mac_da() */
int fe_tbl_HashHash_mac_sa(char *pString, fe_hash_hash_s * pfeHashHash)
{
	unsigned int tmpMac[6];
	int i, retStatus = STATUS_FAILURE;
	retStatus = ParseMAC(pString, tmpMac);
	if (retStatus == STATUS_SUCCESS) {
		for (i = 0; i < 6; i++) {
			pfeHashHash->swhash.mac_sa[i] = (cs_uint8) tmpMac[i];
		}
	}
#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mac_sa:\n");

#endif /* DBG_HASH_FIELD */
	return retStatus;
} /* end fe_tbl_HashHash_mac_sa() */
int fe_tbl_HashHash_eth_type(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.eth_type = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_eth_type:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_eth_type() */
int fe_tbl_HashHash_llc_type_enc(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.llc_type_enc = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_llc_type_enc:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_llc_type_enc() */
int fe_tbl_HashHash_ip_frag(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ip_frag = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_revd_115:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_revd_115() */
int fe_tbl_HashHash_tpid_enc_1(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.tpid_enc_1 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_tpid_enc_1:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_tpid_enc_1() */
int fe_tbl_HashHash__8021p_1(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash._8021p_1 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash__8021p_1:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash__8021p_1() */
int fe_tbl_HashHash_dei_1(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.dei_1 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_dei_1:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_dei_1() */
int fe_tbl_HashHash_vid_1(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.vid_1 = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_vid_1:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_vid_1() */
int fe_tbl_HashHash_revd_135(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.revd_135 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_revd_135:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_revd_135() */
int fe_tbl_HashHash_tpid_enc_2(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.tpid_enc_2 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_tpid_enc_2:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_tpid_enc_2() */
int fe_tbl_HashHash__8021p_2(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash._8021p_2 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash__8021p_2:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash__8021p_2() */
int fe_tbl_HashHash_dei_2(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.dei_2 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_dei_2:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_dei_2() */
int fe_tbl_HashHash_vid_2(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.vid_2 = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_vid_2:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_vid_2() */
int fe_tbl_HashHash_da(char *pString, fe_hash_hash_s * pfeHashHash)
{
	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}
	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pfeHashHash->swhash.da[i] = ipVal[i];
	}

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_da:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_da() */
int fe_tbl_HashHash_sa(char *pString, fe_hash_hash_s * pfeHashHash)
{
	unsigned char ipVal[4];
	int i;
	if (pString == NULL) {
		return STATUS_FAILURE;
	}
	ParseIP(pString, ipVal);
	for (i = 0; i < 4; i++) {
		pfeHashHash->swhash.sa[i] = ipVal[i];
	}

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_sa:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_sa() */
int fe_tbl_HashHash_ip_prot(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ip_prot = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ip_prot:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ip_prot() */
int fe_tbl_HashHash_dscp(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.dscp = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_dscp:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_dscp() */
int fe_tbl_HashHash_ecn(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ecn = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ecn:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ecn() */
int fe_tbl_HashHash_pktlen_rng_match_vector(char *pString,
					    fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.pktlen_rng_match_vector =
	    (cs_uint32) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\fe_tbl_HashHash_pktlen_rng_match_vector:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_pktlen_rng_match_vector() */
int fe_tbl_HashHash_ipv6_flow_label(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ipv6_flow_label =
	    (cs_uint32) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ipv6_flow_label:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ipv6_flow_label() */
int fe_tbl_HashHash_ip_version(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ip_version = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ip_version:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ip_version() */
int fe_tbl_HashHash_ip_valid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ip_valid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ip_valid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ip_valid() */
int fe_tbl_HashHash_l4_dp(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l4_dp = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l4_dp:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l4_dp() */
int fe_tbl_HashHash_l4_sp(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l4_sp = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l4_sp:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l4_sp() */
int fe_tbl_HashHash_tcp_ctrl_flags(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.tcp_ctrl_flags =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_tcp_ctrl_flags:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_tcp_ctrl_flags() */
int fe_tbl_HashHash_tcp_ecn_flags(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.tcp_ecn_flags =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_tcp_ecn_flags:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_tcp_ecn_flags() */
int fe_tbl_HashHash_l4_valid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l4_valid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l4_valid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l4_valid() */
int fe_tbl_HashHash_sdbid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.sdbid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_sdbid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_sdbid() */
int fe_tbl_HashHash_lspid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.lspid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_lspid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_lspid() */
int fe_tbl_HashHash_fwdtype(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.fwdtype = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_fwdtype:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_fwdtype() */
int fe_tbl_HashHash_pppoe_session_id_valid(char *pString,
					   fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.pppoe_session_id_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_pppoe_session_id_valid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_pppoe_session_id_valid() */
int fe_tbl_HashHash_pppoe_session_id(char *pString,
				     fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.pppoe_session_id =
	    (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_pppoe_session_id:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_pppoe_session_id() */
int fe_tbl_HashHash_mask_ptr_0_7(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.mask_ptr_0_7 = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mask_ptr_0_7:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_mask_ptr_0_7() */
int fe_tbl_HashHash_mcgid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.mcgid = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mcgid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_mcgid() */
int fe_tbl_HashHash_mc_idx(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.mc_idx = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mc_idx:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_mc_idx() */
int fe_tbl_HashHash_da_an_mac_sel(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.da_an_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_da_an_mac_sel:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_da_an_mac_sel() */
int fe_tbl_HashHash_da_an_mac_hit(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.da_an_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_da_an_mac_hit:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_da_an_mac_hit() */
int fe_tbl_HashHash_sa_bng_mac_sel(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.sa_bng_mac_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_sa_bng_mac_sel:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_sa_bng_mac_sel() */
int fe_tbl_HashHash_sa_bng_mac_hit(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.sa_bng_mac_hit =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_sa_bng_mac_hit:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_sa_bng_mac_hit() */
int fe_tbl_HashHash_orig_lspid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.orig_lspid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_orig_lspid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_orig_lspid() */
int fe_tbl_HashHash_recirc_idx(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.recirc_idx = (cs_uint32) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_recirc_idx:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_recirc_idx() */
int fe_tbl_HashHash_l7_field(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l7_field = (cs_uint16) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l7_field:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l7_field() */
int fe_tbl_HashHash_l7_field_valid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l7_field_valid =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l7_field_valid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l7_field_valid() */
int fe_tbl_HashHash_hdr_a_flags_crcerr(char *pString,
				       fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.hdr_a_flags_crcerr =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_hdr_a_flags_crcerr:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_hdr_a_flags_crcerr() */
int fe_tbl_HashHash_l3_csum_err(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l3_csum_err = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l3_csum_err:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l3_csum_err() */
int fe_tbl_HashHash_l4_csum_err(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.l4_csum_err = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_l4_csum_err:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_l4_csum_err() */
int fe_tbl_HashHash_not_hdr_a_flags_stsvld(char *pString,
					   fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.not_hdr_a_flags_stsvld =
	    (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_not_hdr_a_flags_stsvld:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_not_hdr_a_flags_stsvld() */
int fe_tbl_HashHash_hash_fid(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.hash_fid = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_hash_fid:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_hash_fid() */
int fe_tbl_HashHash_mc_da(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.mc_da = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_mc_da:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_mc_da() */
int fe_tbl_HashHash_bc_da(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.bc_da = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_bc_da:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_bc_da() */
int fe_tbl_HashHash_spi_vld(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.spi_vld = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_spi_vld:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_spi_vld() */
int fe_tbl_HashHash_spi_idx(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.spi_idx = (cs_uint32) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_spi_idx:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_spi_idx() */
int fe_tbl_HashHash_ipv6_ndp(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ipv6_ndp = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ipv6_ndp:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ipv6_ndp() */
int fe_tbl_HashHash_ipv6_hbh(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ipv6_hbh = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ipv6_hbh:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ipv6_hbh() */
int fe_tbl_HashHash_ipv6_rh(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ipv6_rh = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ipv6_rh:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ipv6_rh() */
int fe_tbl_HashHash_ipv6_doh(char *pString, fe_hash_hash_s * pfeHashHash)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pfeHashHash->swhash.ipv6_doh = (cs_uint8) strtoul(pString, NULL, 0);

#ifdef DBG_HASH_FIELD
	printf("\nfe_tbl_HashHash_ipv6_doh:\n");

#endif /* DBG_HASH_FIELD */
	return STATUS_SUCCESS;
} /* end fe_tbl_HashHash_ipv6_doh() */

/*==================================================================== */

NE_FIELD_T fe_tbl_HASH_HASH[] =	{
	{"mac_da",			fe_tbl_HashHash_mac_da},
	{"mac_sa",			fe_tbl_HashHash_mac_sa},
	{"eth_type",			fe_tbl_HashHash_eth_type},
	{"llc_type_enc",		fe_tbl_HashHash_llc_type_enc},
	{"ip_frag",			fe_tbl_HashHash_ip_frag},

	{"tpid_enc_1",			fe_tbl_HashHash_tpid_enc_1},
	{"_8021p_1",			fe_tbl_HashHash__8021p_1},
	{"dei_1",			fe_tbl_HashHash_dei_1},
	{"vid_1",			fe_tbl_HashHash_vid_1},
	{"revd_135",			fe_tbl_HashHash_revd_135},

	{"tpid_enc_2",			fe_tbl_HashHash_tpid_enc_2},
	{"_8021p_2",			fe_tbl_HashHash__8021p_2},
	{"dei_2",			fe_tbl_HashHash_dei_2},
	{"vid_2",			fe_tbl_HashHash_vid_2},
	{"da",				fe_tbl_HashHash_da},
	{"sa",				fe_tbl_HashHash_sa},
	{"ip_prot",			fe_tbl_HashHash_ip_prot},
	{"dscp",			fe_tbl_HashHash_dscp},
	{"ecn",				fe_tbl_HashHash_ecn},
	{"pktlen_rng_match_vector",	fe_tbl_HashHash_pktlen_rng_match_vector},

	{"ipv6_flow_label",		fe_tbl_HashHash_ipv6_flow_label},
	{"ip_version",			fe_tbl_HashHash_ip_version},
	{"ip_valid",			fe_tbl_HashHash_ip_valid},
	{"l4_dp",			fe_tbl_HashHash_l4_dp},
	{"l4_sp",			fe_tbl_HashHash_l4_sp},
	{"tcp_ctrl_flags",		fe_tbl_HashHash_tcp_ctrl_flags},
	{"tcp_ecn_flags",		fe_tbl_HashHash_tcp_ecn_flags},
	{"l4_valid",			fe_tbl_HashHash_l4_valid},
	{"sdbid",			fe_tbl_HashHash_sdbid},
	{"lspid",			fe_tbl_HashHash_lspid},
	{"fwdtype",			fe_tbl_HashHash_fwdtype},
	{"pppoe_session_id_valid",	fe_tbl_HashHash_pppoe_session_id_valid},
	{"pppoe_session_id",		fe_tbl_HashHash_pppoe_session_id},
	{"mask_ptr_0_7",		fe_tbl_HashHash_mask_ptr_0_7},
	{"mcgid",			fe_tbl_HashHash_mcgid},
	{"mc_idx",			fe_tbl_HashHash_mc_idx},
	{"da_an_mac_sel",		fe_tbl_HashHash_da_an_mac_sel},
	{"da_an_mac_hit",		fe_tbl_HashHash_da_an_mac_hit},
	{"sa_bng_mac_sel",		fe_tbl_HashHash_sa_bng_mac_sel},
	{"sa_bng_mac_hit",		fe_tbl_HashHash_sa_bng_mac_hit},
	{"orig_lspid",			fe_tbl_HashHash_orig_lspid},
	{"recirc_idx",			fe_tbl_HashHash_recirc_idx},
	{"l7_field",			fe_tbl_HashHash_l7_field},
	{"l7_field_valid",		fe_tbl_HashHash_l7_field_valid},
	{"hdr_a_flags_crcerr",		fe_tbl_HashHash_hdr_a_flags_crcerr},
	{"l3_csum_err",			fe_tbl_HashHash_l3_csum_err},
	{"l4_csum_err",			fe_tbl_HashHash_l4_csum_err},
	{"not_hdr_a_flags_stsvld",	fe_tbl_HashHash_not_hdr_a_flags_stsvld},
	{"hash_fid",			fe_tbl_HashHash_hash_fid},
	{"mc_da",			fe_tbl_HashHash_mc_da},
	{"bc_da",			fe_tbl_HashHash_bc_da},
	{"spi_vld",			fe_tbl_HashHash_spi_vld},
	{"spi_idx",			fe_tbl_HashHash_spi_idx},
	{"ipv6_ndp",			fe_tbl_HashHash_ipv6_ndp},
	{"ipv6_hbh",			fe_tbl_HashHash_ipv6_hbh},
	{"ipv6_rh",			fe_tbl_HashHash_ipv6_rh},
	{"ipv6_doh",			fe_tbl_HashHash_ipv6_doh},

	{0,				0}
};

/*//================================================================================== */
/*// */
/*void PrintHashHash(fe_hash_hash_s *pfeHashHash) */
/*{ */
/*    printf("\n"); */
/*    printf("***********************************************\n"); */
/*    printf("mac_da              : %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", pfeHashHash->swhash.mac_da[0], pfeHashHash->swhash.mac_da[1] */
/*                                           , pfeHashHash->swhash.mac_da[2], pfeHashHash->swhash.mac_da[3] */
/*                                           , pfeHashHash->swhash.mac_da[4], pfeHashHash->swhash.mac_da[5]); */
/*    printf("mac_sa              : %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", pfeHashHash->swhash.mac_sa[0], pfeHashHash->swhash.mac_sa[1] */
/*                                           , pfeHashHash->swhash.mac_sa[2], pfeHashHash->swhash.mac_sa[3] */
/*                                           , pfeHashHash->swhash.mac_sa[4], pfeHashHash->swhash.mac_sa[5]); */
/*    printf("eth_type            : 0x%4.4x\n", pfeHashHash->swhash.eth_type); */
/*    printf("llc_type_enc        : 0x%2.2x\n", pfeHashHash->swhash.llc_type_enc); */
/*    printf("revd_115            : 0x%2.2x\n", pfeHashHash->swhash.revd_115); */

/*    printf("tpid_enc_1          : 0x%2.2x\n", pfeHashHash->swhash.tpid_enc_1); */
/*    printf("_8021p_1            : 0x%2.2x\n", pfeHashHash->swhash._8021p_1); */
/*    printf("dei_1               : 0x%2.2x\n", pfeHashHash->swhash.dei_1); */
/*    printf("vid_1               : 0x%4.4x\n", pfeHashHash->swhash.vid_1); */
/*    printf("revd_135            : 0x%2.2x\n", pfeHashHash->swhash.revd_135); */

/*    printf("tpid_enc_2          : 0x%2.2x\n", pfeHashHash->swhash.tpid_enc_2); */
/*    printf("_8021p_2            : 0x%2.2x\n", pfeHashHash->swhash._8021p_2); */
/*    printf("dei_2               : 0x%2.2x\n", pfeHashHash->swhash.dei_2); */
/*    printf("vid_2               : 0x%4.4x\n", pfeHashHash->swhash.vid_2); */
/*    printf("da.0                : 0x%8.8x\n", (int)pfeHashHash->swhash.da[0]); */
/*    printf("da.1                : 0x%8.8x\n", (int)pfeHashHash->swhash.da[1]); */
/*    printf("da.2                : 0x%8.8x\n", (int)pfeHashHash->swhash.da[2]); */
/*    printf("da.3                : 0x%8.8x\n", (int)pfeHashHash->swhash.da[3]); */
/*    printf("sa.0                : 0x%8.8x\n", (int)pfeHashHash->swhash.sa[0]); */
/*    printf("sa.1                : 0x%8.8x\n", (int)pfeHashHash->swhash.sa[1]); */
/*    printf("sa.2                : 0x%8.8x\n", (int)pfeHashHash->swhash.sa[2]); */
/*    printf("sa.3                : 0x%8.8x\n", (int)pfeHashHash->swhash.sa[3]); */
/*    printf("ip_prot             : 0x%2.2x\n", pfeHashHash->swhash.ip_prot); */
/*    printf("dscp                : 0x%2.2x\n", pfeHashHash->swhash.dscp); */
/*    printf("ecn                 : 0x%2.2x\n", pfeHashHash->swhash.ecn); */
/*    printf("ip_frag             : 0x%2.2x\n", pfeHashHash->swhash.ip_frag); */
/*    printf("revd_428            : 0x%2.2x\n", pfeHashHash->swhash.revd_428); */
/*    printf("revd_430_429        : 0x%2.2x\n", pfeHashHash->swhash.revd_430_429); */

/*    printf("ipv6_flow_label     : 0x%8.8x\n", (int)pfeHashHash->swhash.ipv6_flow_label); */
/*    printf("ip_version          : 0x%2.2x\n", pfeHashHash->swhash.ip_version); */
/*    printf("ip_valid            : 0x%2.2x\n", pfeHashHash->swhash.ip_valid); */
/*    printf("l4_dp               : 0x%4.4x\n", pfeHashHash->swhash.l4_dp); */
/*    printf("l4_sp               : 0x%4.4x\n", pfeHashHash->swhash.l4_sp); */
/*    printf("tcp_ctrl_flags      : 0x%2.2x\n", pfeHashHash->swhash.tcp_ctrl_flags); */
/*    printf("tcp_ecn_flags       : 0x%2.2x\n", pfeHashHash->swhash.tcp_ecn_flags); */
/*    printf("l4_valid            : 0x%2.2x\n", pfeHashHash->swhash.l4_valid); */
/*    printf("sdbid               : 0x%2.2x\n", pfeHashHash->swhash.sdbid); */
/*    printf("lspid               : 0x%2.2x\n", pfeHashHash->swhash.lspid); */
/*    printf("fwdtype             : 0x%2.2x\n", pfeHashHash->swhash.fwdtype); */
/*    printf("pppoe_session_id_valid : 0x%2.2x\n", pfeHashHash->swhash.pppoe_session_id_valid); */
/*    printf("pppoe_session_id    : 0x%4.4x\n", pfeHashHash->swhash.pppoe_session_id); */
/*    printf("mask_ptr_0_7        : 0x%2.2x\n", pfeHashHash->swhash.mask_ptr_0_7); */
/*    printf("mcgid               : 0x%4.4x\n", pfeHashHash->swhash.mcgid); */
/*    printf("mc_idx              : 0x%2.2x\n", pfeHashHash->swhash.mc_idx); */
/*    printf("da_an_mac_sel       : 0x%2.2x\n", pfeHashHash->swhash.da_an_mac_sel); */
/*    printf("da_an_mac_hit       : 0x%2.2x\n", pfeHashHash->swhash.da_an_mac_hit); */
/*    printf("sa_bng_mac_sel      : 0x%2.2x\n", pfeHashHash->swhash.sa_bng_mac_sel); */
/*    printf("sa_bng_mac_hit      : 0x%2.2x\n", pfeHashHash->swhash.sa_bng_mac_hit); */
/*    printf("orig_lspid          : 0x%2.2x\n", pfeHashHash->swhash.orig_lspid); */
/*    printf("recirc_idx          : 0x%8.8x\n", (int)pfeHashHash->swhash.recirc_idx); */
/*    printf("l7_field            : 0x%4.4x\n", pfeHashHash->swhash.l7_field); */
/*    printf("l7_field_valid      : 0x%2.2x\n", pfeHashHash->swhash.l7_field_valid); */
/*    printf("hdr_a_flags_crcerr  : 0x%2.2x\n", pfeHashHash->swhash.hdr_a_flags_crcerr); */
/*    printf("l3_csum_err         : 0x%2.2x\n", pfeHashHash->swhash.l3_csum_err); */
/*    printf("l4_csum_err         : 0x%2.2x\n", pfeHashHash->swhash.l4_csum_err); */
/*    printf("not_hdr_a_flags_stsvld : 0x%2.2x\n", pfeHashHash->swhash.not_hdr_a_flags_stsvld); */
/*    printf("hash_fid            : 0x%2.2x\n", pfeHashHash->swhash.hash_fid); */
/*    printf("mc_da               : 0x%2.2x\n", pfeHashHash->swhash.mc_da); */
/*    printf("bc_da               : 0x%2.2x\n", pfeHashHash->swhash.bc_da); */
/*    printf("spi_vld             : 0x%2.2x\n", pfeHashHash->swhash.spi_vld); */
/*    printf("spi_idx             : 0x%4.4x\n", (int)pfeHashHash->swhash.spi_idx); */
/*    printf("ipv6_ndp            : 0x%2.2x\n", pfeHashHash->swhash.ipv6_ndp); */
/*    printf("ipv6_hbh            : 0x%2.2x\n", pfeHashHash->swhash.ipv6_hbh); */
/*    printf("ipv6_rh             : 0x%2.2x\n", pfeHashHash->swhash.ipv6_rh); */
/*    printf("ipv6_doh            : 0x%2.2x\n", pfeHashHash->swhash.ipv6_doh); */
/*    printf("***********************************************\n"); */
/*     */
/*    return; */
/*}//end PrintHashHash() */
