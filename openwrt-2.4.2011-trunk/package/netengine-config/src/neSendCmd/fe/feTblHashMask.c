/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblHashMask.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
				 fe module Hash Mask Table
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
 * fe->fe_hash_mask_entry_t
 */
int fe_tbl_HashMask_mac_da_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.mac_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_mac_da_mask() */
int fe_tbl_HashMask_mac_sa_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.mac_sa_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l2_mac_mask_mac_sa() */
int fe_tbl_HashMask_ethertype_mask(char *pString,
				   HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ethertype_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ethertype_mask() */
int fe_tbl_HashMask_llc_type_enc_msb_mask(char *pString,
					  HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.llc_type_enc_msb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_llc_type_enc_msb_mask() */
int fe_tbl_HashMask_llc_type_enc_lsb_mask(char *pString,
					  HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.llc_type_enc_lsb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_llc_type_enc_lsb_mask() */
int fe_tbl_HashMask_tpid_enc_1_msb_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tpid_enc_1_msb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tpid_enc_1_msb_mask() */
int fe_tbl_HashMask_tpid_enc_1_lsb_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tpid_enc_1_lsb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tpid_enc_1_lsb_mask() */
int fe_tbl_HashMask__8021p_1_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask._8021p_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask__8021p_1_mask() */
int fe_tbl_HashMask_dei_1_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.dei_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_dei_1_mask() */
int fe_tbl_HashMask_vid_1_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.vid_1_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_vid_1_mask() */
int fe_tbl_HashMask_tpid_enc_2_msb_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tpid_enc_2_msb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tpid_enc_2_msb_mask() */
int fe_tbl_HashMask_tpid_enc_2_lsb_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tpid_enc_2_lsb_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tpid_enc_2_lsb_mask() */
int fe_tbl_HashMask__8021p_2_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask._8021p_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask__8021p_2_mask() */
int fe_tbl_HashMask_dei_2_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.dei_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_dei_2_mask() */
int fe_tbl_HashMask_vid_2_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.vid_2_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_vid_2_mask() */
int fe_tbl_HashMask_ip_da_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_da_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_da_mask() */
int fe_tbl_HashMask_ip_sa_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_sa_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_sa_mask() */
int fe_tbl_HashMask_ip_prot_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_prot_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_prot_mask() */
int fe_tbl_HashMask_dscp_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.dscp_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_dscp_mask() */
int fe_tbl_HashMask_ecn_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ecn_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ecn_mask() */
int fe_tbl_HashMask_ip_fragment_mask(char *pString,
				     HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_fragment_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_fragment_mask() */
int fe_tbl_HashMask_keygen_poly_sel(char *pString,
				    HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.keygen_poly_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_keygen_poly_sel() */
int fe_tbl_HashMask_ipv6_flow_lbl_mask(char *pString,
				       HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ipv6_flow_lbl_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ipv6_flow_lbl_mask() */
int fe_tbl_HashMask_ip_ver_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_ver_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_ver_mask() */
int fe_tbl_HashMask_ip_vld_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ip_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ip_vld_mask() */
int fe_tbl_HashMask_l4_ports_rngd(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l4_ports_rngd =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l4_ports_rngd() */
int fe_tbl_HashMask_l4_dp_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l4_dp_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l4_dp_mask() */
int fe_tbl_HashMask_l4_sp_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l4_sp_mask =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l4_sp_mask() */
int fe_tbl_HashMask_tcp_ctrl_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tcp_ctrl_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tcp_ctrl_mask() */
int fe_tbl_HashMask_tcp_ecn_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.tcp_ecn_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_tcp_ecn_mask() */
int fe_tbl_HashMask_l4_vld_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l4_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l4_vld_mask() */
int fe_tbl_HashMask_lspid_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_lspid_mask() */
int fe_tbl_HashMask_fwdtype_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.fwdtype_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_fwdtype_mask() */
int fe_tbl_HashMask_pppoe_session_id_vld_mask(char *pString,
					      HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.pppoe_session_id_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_pppoe_session_id_vld_mask() */
int fe_tbl_HashMask_pppoe_session_id_mask(char *pString,
					  HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.pppoe_session_id_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_pppoe_session_id_mask() */
int fe_tbl_HashMask_rsvd_109(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.rsvd_109 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_rsvd_109() */
int fe_tbl_HashMask_recirc_idx_mask(char *pString,
				    HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.recirc_idx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_recirc_idx_mask() */
int fe_tbl_HashMask_mcidx_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.mcidx_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_mcidx_mask() */
int fe_tbl_HashMask_mc_da_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.mc_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_mc_da_mask() */
int fe_tbl_HashMask_bc_da_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.bc_da_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_bc_da_mask() */
int fe_tbl_HashMask_da_an_mac_sel_mask(char *pString,
				       HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.da_an_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_da_an_mac_sel_mask() */
int fe_tbl_HashMask_da_an_mac_hit_mask(char *pString,
				       HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.da_an_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_da_an_mac_hit_mask() */
int fe_tbl_HashMask_orig_lspid_mask(char *pString,
				    HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.orig_lspid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_orig_lspid_mask() */
int fe_tbl_HashMask_l7_field_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l7_field_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l7_field_mask() */
int fe_tbl_HashMask_l7_field_vld_mask(char *pString,
				      HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l7_field_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l7_field_vld_mask() */
int fe_tbl_HashMask_l7_field_sel(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l7_field_sel =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l7_field_sel() */
int fe_tbl_HashMask_hdr_a_flags_crcerr_mask(char *pString,
					    HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.hdr_a_flags_crcerr_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_hdr_a_flags_crcerr_mask() */
int fe_tbl_HashMask_l3_chksum_err_mask(char *pString,
				       HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l3_chksum_err_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l3_chksum_err_mask() */
int fe_tbl_HashMask_l4_chksum_err_mask(char *pString,
				       HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.l4_chksum_err_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_l4_chksum_err_mask() */
int fe_tbl_HashMask_not_hdr_a_flags_stsvld_mask(char *pString,
						HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.not_hdr_a_flags_stsvld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_not_hdr_a_flags_stsvld_mask() */
int fe_tbl_HashMask_hash_fid_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.hash_fid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_hash_fid_mask() */
int fe_tbl_HashMask_sa_bng_mac_sel_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.sa_bng_mac_sel_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_sa_bng_mac_sel_mask() */
int fe_tbl_HashMask_sa_bng_mac_hit_mask(char *pString,
					HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.sa_bng_mac_hit_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_sa_bng_mac_hit_mask() */
int fe_tbl_HashMask_spi_vld_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.spi_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_spi_vld_mask() */
int fe_tbl_HashMask_spi_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.spi_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_spi_mask() */
int fe_tbl_HashMask_ipv6_ndp_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ipv6_ndp_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ipv6_ndp_mask() */
int fe_tbl_HashMask_ipv6_hbh_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ipv6_hbh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ipv6_hbh_mask() */
int fe_tbl_HashMask_ipv6_rh_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ipv6_rh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ipv6_rh_mask() */
int fe_tbl_HashMask_ipv6_doh_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ipv6_doh_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ipv6_doh_mask() */
int fe_tbl_HashMask_ppp_protocol_vld_mask(char *pString,
					  HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ppp_protocol_vld_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ppp_protocol_vld_mask() */
int fe_tbl_HashMask_ppp_protocol_mask(char *pString,
				      HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.ppp_protocol_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_ppp_protocol_mask() */
int fe_tbl_HashMask_pktlen_rng_match_vector_mask(char *pString,
						 HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.pktlen_rng_match_vector_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_pktlen_rng_match_vector_mask() */
int fe_tbl_HashMask_mcgid_mask(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.mcgid_mask =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_mcgid_mask() */
int fe_tbl_HashMask_parity(char *pString, HASH_MASK_INFO * pHashMaskInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pHashMaskInfo->tbl_HashMask.parity =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* end fe_tbl_HashMask_parity() */

/*==========================================================================*/
NE_FIELD_T fe_tbl_Hash_Mask[] = {
	{"mac_da_mask",			fe_tbl_HashMask_mac_da_mask},
	{"mac_sa_mask",			fe_tbl_HashMask_mac_sa_mask},
	{"ethertype_mask",		fe_tbl_HashMask_ethertype_mask},
	{"llc_type_enc_msb_mask",	fe_tbl_HashMask_llc_type_enc_msb_mask},
	{"llc_type_enc_lsb_mask",	fe_tbl_HashMask_llc_type_enc_lsb_mask},
	{"tpid_enc_1_msb_mask",		fe_tbl_HashMask_tpid_enc_1_msb_mask},
	{"tpid_enc_1_lsb_mask",		fe_tbl_HashMask_tpid_enc_1_lsb_mask},
	{"_8021p_1_mask",		fe_tbl_HashMask__8021p_1_mask},
	{"dei_1_mask",			fe_tbl_HashMask_dei_1_mask},
	{"vid_1_mask",			fe_tbl_HashMask_vid_1_mask},
	{"tpid_enc_2_msb_mask",		fe_tbl_HashMask_tpid_enc_2_msb_mask},
	{"tpid_enc_2_lsb_mask",		fe_tbl_HashMask_tpid_enc_2_lsb_mask},
	{"_8021p_2_mask",		fe_tbl_HashMask__8021p_2_mask},
	{"dei_2_mask",			fe_tbl_HashMask_dei_2_mask},
	{"vid_2_mask",			fe_tbl_HashMask_vid_2_mask},
	{"ip_da_mask",			fe_tbl_HashMask_ip_da_mask},
	{"ip_sa_mask",			fe_tbl_HashMask_ip_sa_mask},
	{"ip_prot_mask",		fe_tbl_HashMask_ip_prot_mask},
	{"dscp_mask",			fe_tbl_HashMask_dscp_mask},
	{"ecn_mask",			fe_tbl_HashMask_ecn_mask},
	{"ip_fragment_mask",		fe_tbl_HashMask_ip_fragment_mask},
	{"keygen_poly_sel",		fe_tbl_HashMask_keygen_poly_sel},
	{"ipv6_flow_lbl_mask",		fe_tbl_HashMask_ipv6_flow_lbl_mask},
	{"ip_ver_mask",			fe_tbl_HashMask_ip_ver_mask},
	{"ip_vld_mask",			fe_tbl_HashMask_ip_vld_mask},
	{"l4_ports_rngd",		fe_tbl_HashMask_l4_ports_rngd},
	{"l4_dp_mask",			fe_tbl_HashMask_l4_dp_mask},
	{"l4_sp_mask",			fe_tbl_HashMask_l4_sp_mask},
	{"tcp_ctrl_mask",		fe_tbl_HashMask_tcp_ctrl_mask},
	{"tcp_ecn_mask",		fe_tbl_HashMask_tcp_ecn_mask},
	{"l4_vld_mask",			fe_tbl_HashMask_l4_vld_mask},
	{"lspid_mask",			fe_tbl_HashMask_lspid_mask},
	{"fwdtype_mask",		fe_tbl_HashMask_fwdtype_mask},
	{"pppoe_session_id_vld_mask",	fe_tbl_HashMask_pppoe_session_id_vld_mask},
	{"pppoe_session_id_mask",	fe_tbl_HashMask_pppoe_session_id_mask},
	{"rsvd_109",			fe_tbl_HashMask_rsvd_109},
	{"recirc_idx_mask",		fe_tbl_HashMask_recirc_idx_mask},
	{"mcidx_mask",			fe_tbl_HashMask_mcidx_mask},
	{"mc_da_mask",			fe_tbl_HashMask_mc_da_mask},
	{"bc_da_mask",			fe_tbl_HashMask_bc_da_mask},
	{"da_an_mac_sel_mask",		fe_tbl_HashMask_da_an_mac_sel_mask},
	{"da_an_mac_hit_mask",		fe_tbl_HashMask_da_an_mac_hit_mask},
	{"orig_lspid_mask",		fe_tbl_HashMask_orig_lspid_mask},
	{"l7_field_mask",		fe_tbl_HashMask_l7_field_mask},
	{"l7_field_vld_mask",		fe_tbl_HashMask_l7_field_vld_mask},
	{"hdr_a_flags_crcerr_mask",	fe_tbl_HashMask_hdr_a_flags_crcerr_mask},
	{"l3_chksum_err_mask",		fe_tbl_HashMask_l3_chksum_err_mask},
	{"l4_chksum_err_mask",		fe_tbl_HashMask_l4_chksum_err_mask},
	{"not_hdr_a_flags_stsvld_mask",	fe_tbl_HashMask_not_hdr_a_flags_stsvld_mask},
	{"hash_fid_mask",		fe_tbl_HashMask_hash_fid_mask},
	{"l7_field_sel",		fe_tbl_HashMask_l7_field_sel},
	{"sa_bng_mac_sel_mask",		fe_tbl_HashMask_sa_bng_mac_sel_mask},
	{"sa_bng_mac_hit_mask",		fe_tbl_HashMask_sa_bng_mac_hit_mask},
	{"spi_vld_mask",		fe_tbl_HashMask_spi_vld_mask},
	{"spi_mask",			fe_tbl_HashMask_spi_mask},
	{"ipv6_ndp_mask",		fe_tbl_HashMask_ipv6_ndp_mask},
	{"ipv6_hbh_mask",		fe_tbl_HashMask_ipv6_hbh_mask},
	{"ipv6_rh_mask",		fe_tbl_HashMask_ipv6_rh_mask},
	{"ipv6_doh_mask",		fe_tbl_HashMask_ipv6_doh_mask},
	{"ppp_protocol_vld_mask",	fe_tbl_HashMask_ppp_protocol_vld_mask},
	{"ppp_protocol_mask",		fe_tbl_HashMask_ppp_protocol_mask},
	{"pktlen_rng_match_vector_mask", fe_tbl_HashMask_pktlen_rng_match_vector_mask},
	{"mcgid_mask",			fe_tbl_HashMask_mcgid_mask},
	{"parity",			fe_tbl_HashMask_parity},
	{0, 				0}
};
