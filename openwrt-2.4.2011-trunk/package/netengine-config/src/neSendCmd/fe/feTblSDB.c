/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblSDB.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility process
				 fe module SDB Table
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
 * fe->SDB->fe_sdb_tuple_entry_s
 */
int fe_tbl_SDB_sdb_tuple_0_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[0].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_0_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_0_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[0].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_0_priority() */
int fe_tbl_SDB_sdb_tuple_0_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[0].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_0_enable() */
int fe_tbl_SDB_sdb_tuple_1_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[1].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_1_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_1_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[1].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_1_priority() */
int fe_tbl_SDB_sdb_tuple_1_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[1].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_1_enable() */
int fe_tbl_SDB_sdb_tuple_2_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[2].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_2_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_2_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[2].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_2_priority() */
int fe_tbl_SDB_sdb_tuple_2_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[2].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_2_enable() */
int fe_tbl_SDB_sdb_tuple_3_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[3].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_3_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_3_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[3].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_3_priority() */
int fe_tbl_SDB_sdb_tuple_3_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[3].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_3_enable() */
int fe_tbl_SDB_sdb_tuple_4_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[4].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_4_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_4_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[4].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_4_priority() */
int fe_tbl_SDB_sdb_tuple_4_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[4].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_4_enable() */
int fe_tbl_SDB_sdb_tuple_5_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[5].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_5_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_5_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[5].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_5_priority() */
int fe_tbl_SDB_sdb_tuple_5_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[5].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_5_enable() */
int fe_tbl_SDB_sdb_tuple_6_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[6].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_6_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_6_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[6].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_6_priority() */
int fe_tbl_SDB_sdb_tuple_6_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[6].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_6_enable() */
int fe_tbl_SDB_sdb_tuple_7_mask_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[7].mask_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_7_mask_ptr() */
int fe_tbl_SDB_sdb_tuple_7_priority(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[7].priority =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_7_priority() */
int fe_tbl_SDB_sdb_tuple_7_enable(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_tuple[7].enable =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_tuple_7_enable() */
int fe_tbl_SDB_sdb_lpm4_0_start_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[0].start_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm4_0_start_ptr() */
int fe_tbl_SDB_sdb_lpm4_0_end_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[0].end_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm4_0_end_ptr() */
int fe_tbl_SDB_sdb_lpm4_0_lpm_ptr_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[0].lpm_ptr_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm_0_lpm_ptr_en() */
int fe_tbl_SDB_sdb_lpm4_1_start_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[1].start_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm4_1_start_ptr() */
int fe_tbl_SDB_sdb_lpm4_1_end_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[1].end_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm_1_end_ptr() */
int fe_tbl_SDB_sdb_lpm4_1_lpm_ptr_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v4[1].lpm_ptr_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm4_1_lpm_ptr_en() */
int fe_tbl_SDB_sdb_lpm6_0_start_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[0].start_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_0_start_ptr() */
int fe_tbl_SDB_sdb_lpm6_0_end_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[0].end_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_0_end_ptr() */
int fe_tbl_SDB_sdb_lpm6_0_lpm_ptr_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[0].lpm_ptr_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_0_lpm_ptr_en() */
int fe_tbl_SDB_sdb_lpm6_1_start_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[1].start_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_1_start_ptr() */
int fe_tbl_SDB_sdb_lpm6_1_end_ptr(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[1].end_ptr =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_1_end_ptr() */
int fe_tbl_SDB_sdb_lpm6_1_lpm_ptr_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.sdb_lpm_v6[1].lpm_ptr_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_sdb_lpm6_1_lpm_ptr_en() */

/*
 * fe->SDB
 */
int fe_tbl_SDB_lpm_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.lpm_en = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_lpm_en() */

/*
 * fe->SDB->fe_sdb_flow_entry_t.fe_sdb_entry_misc_t
 */
int fe_tbl_SDB_uu_flow_flow_index(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.uu_flowidx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_uu_flow_flow_index() */
int fe_tbl_SDB_bc_flow_flow_index(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.bc_flowidx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_bc_flow_flow_index() */
int fe_tbl_SDB_um_flow_flow_index(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.um_flowidx =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_um_flow_flow_index() */
int fe_tbl_SDB_misc_rsvd_202(char *pString,
						  SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.rsvd_202 =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_rsvd_202() */
int fe_tbl_SDB_misc_ttl_hop_limit_zero_discard_en(char *pString,
						  SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.ttl_hop_limit_zero_discard_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_ttl_hop_limit_zero_discard_en() */
int fe_tbl_SDB_misc_key_rule(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.key_rule = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_key_rule() */
int fe_tbl_SDB_misc_drop(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.drop = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_drop() */
int fe_tbl_SDB_misc_egr_vln_ingr_mbrshp_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.egr_vln_ingr_mbrshp_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_egr_vln_ingr_mbrshp_en() */

/*
 * fe->SDB->fe_sdb_pvid_field_t
 */
int fe_tbl_SDB_pvid_pvid(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.pvid.pvid = (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_pvid_pvid() */
int fe_tbl_SDB_pvid_pvid_tpid_enc(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.pvid.pvid_tpid_enc =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_pvid_pvid_tpid_enc() */
int fe_tbl_SDB_pvid_pvid_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.pvid.pvid_en = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_pvid_pvid_en() */

/*
 * fe->SDB->fe_sdb_entry_misc_t
 */
int fe_tbl_SDB_misc_use_egrlen_pkttype_policer(char *pString,
					       SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.use_egrlen_pkttype_policer =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_use_egrlen_pkttype_policer() */
int fe_tbl_SDB_misc_use_egrlen_src_policer(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.use_egrlen_src_policer =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_use_egrlen_src_policer() */
int fe_tbl_SDB_misc_use_egrlen_flow_policer(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.use_egrlen_flow_policer =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_use_egrlen_flow_policer() */

/*
 * fe->SDB->fe_sdb_vlan_entry_t
 */
int fe_tbl_SDB_vlan_vlan_ingr_membership_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.vlan.vlan_ingr_membership_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_vlan_vlan_ingr_membership_en() */
int fe_tbl_SDB_vlan_vlan_egr_membership_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.vlan.vlan_egr_membership_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_vlan_vlan_egr_membership_en() */
int fe_tbl_SDB_vlan_vlan_egr_untag_chk_en(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.vlan.vlan_egr_untag_chk_en =
	    (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_vlan_vlan_egr_untag_chk_en() */

/*
 * fe->SDB->fe_sdb_flow_entry_t.fe_sdb_entry_misc_t
 */
int fe_tbl_SDB_misc_acl_dsbl(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.acl_dsbl =
	    (cs_boolean) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_misc_acl_dsbl() */
int fe_tbl_SDB_misc_hash_sts_update_ctrl(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.misc.hash_sts_update_ctrl =
	    (cs_boolean) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_acl_dsbl() */
int fe_tbl_SDB_parity(char *pString, SDB_INFO * pSDBInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;
	pSDBInfo->tbl_SDB.parity = (cs_uint8) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /*end fe_tbl_SDB_parity() */

/*=========================================================================*/
NE_FIELD_T fe_tbl_SDB[] = {
	{"sdb_tuple.0.mask_ptr",	fe_tbl_SDB_sdb_tuple_0_mask_ptr},
	{"sdb_tuple.0.priority",	fe_tbl_SDB_sdb_tuple_0_priority},
	{"sdb_tuple.0.enable",		fe_tbl_SDB_sdb_tuple_0_enable},
	{"sdb_tuple.1.mask_ptr",	fe_tbl_SDB_sdb_tuple_1_mask_ptr},
	{"sdb_tuple.1.priority",	fe_tbl_SDB_sdb_tuple_1_priority},
	{"sdb_tuple.1.enable",		fe_tbl_SDB_sdb_tuple_1_enable},
	{"sdb_tuple.2.mask_ptr",	fe_tbl_SDB_sdb_tuple_2_mask_ptr},
	{"sdb_tuple.2.priority",	fe_tbl_SDB_sdb_tuple_2_priority},
	{"sdb_tuple.2.enable",		fe_tbl_SDB_sdb_tuple_2_enable},
	{"sdb_tuple.3.mask_ptr",	fe_tbl_SDB_sdb_tuple_3_mask_ptr},
	{"sdb_tuple.3.priority",	fe_tbl_SDB_sdb_tuple_3_priority},
	{"sdb_tuple.3.enable",		fe_tbl_SDB_sdb_tuple_3_enable},
	{"sdb_tuple.4.mask_ptr",	fe_tbl_SDB_sdb_tuple_4_mask_ptr},
	{"sdb_tuple.4.priority",	fe_tbl_SDB_sdb_tuple_4_priority},
	{"sdb_tuple.4.enable",		fe_tbl_SDB_sdb_tuple_4_enable},
	{"sdb_tuple.5.mask_ptr",	fe_tbl_SDB_sdb_tuple_5_mask_ptr},
	{"sdb_tuple.5.priority",	fe_tbl_SDB_sdb_tuple_5_priority},
	{"sdb_tuple.5.enable",		fe_tbl_SDB_sdb_tuple_5_enable},
	{"sdb_tuple.6.mask_ptr",	fe_tbl_SDB_sdb_tuple_6_mask_ptr},
	{"sdb_tuple.6.priority",	fe_tbl_SDB_sdb_tuple_6_priority},
	{"sdb_tuple.6.enable",		fe_tbl_SDB_sdb_tuple_6_enable},
	{"sdb_tuple.7.mask_ptr",	fe_tbl_SDB_sdb_tuple_7_mask_ptr},
	{"sdb_tuple.7.priority",	fe_tbl_SDB_sdb_tuple_7_priority},
	{"sdb_tuple.7.enable",		fe_tbl_SDB_sdb_tuple_7_enable},
	{"sdb_lpm4.0.start_ptr",	fe_tbl_SDB_sdb_lpm4_0_start_ptr},
	{"sdb_lpm4.0.end_ptr",		fe_tbl_SDB_sdb_lpm4_0_end_ptr},
	{"sdb_lpm4.0.lpm_ptr_en",	fe_tbl_SDB_sdb_lpm4_0_lpm_ptr_en},
	{"sdb_lpm4.1.start_ptr",	fe_tbl_SDB_sdb_lpm4_1_start_ptr},
	{"sdb_lpm4.1.end_ptr",		fe_tbl_SDB_sdb_lpm4_1_end_ptr},
	{"sdb_lpm4.1.lpm_ptr_en",	fe_tbl_SDB_sdb_lpm4_1_lpm_ptr_en},
	{"sdb_lpm6.0.start_ptr",	fe_tbl_SDB_sdb_lpm6_0_start_ptr},
	{"sdb_lpm6.0.end_ptr",		fe_tbl_SDB_sdb_lpm6_0_end_ptr},
	{"sdb_lpm6.0.lpm_ptr_en",	fe_tbl_SDB_sdb_lpm6_0_lpm_ptr_en},
	{"sdb_lpm6.1.start_ptr",	fe_tbl_SDB_sdb_lpm6_1_start_ptr},
	{"sdb_lpm6.1.end_ptr",		fe_tbl_SDB_sdb_lpm6_1_end_ptr},
	{"sdb_lpm6.1.lpm_ptr_en",	fe_tbl_SDB_sdb_lpm6_1_lpm_ptr_en},
	{"lpm_en",			fe_tbl_SDB_lpm_en},
	{"misc.uu_flowidx",		fe_tbl_SDB_uu_flow_flow_index},
	{"misc.bc_flowidx",		fe_tbl_SDB_bc_flow_flow_index},
	{"misc.um_flow.flowidx",	fe_tbl_SDB_um_flow_flow_index},
	{"misc.rsvd_202",		fe_tbl_SDB_misc_rsvd_202},
	{"misc.ttl_hop_limit_zero_discard_en", fe_tbl_SDB_misc_ttl_hop_limit_zero_discard_en},
	{"misc.key_rule",		fe_tbl_SDB_misc_key_rule},
	{"misc.drop",			fe_tbl_SDB_misc_drop},
	{"misc.egr_vln_ingr_mbrshp_en",	fe_tbl_SDB_misc_egr_vln_ingr_mbrshp_en},
	{"pvid.pvid",			fe_tbl_SDB_pvid_pvid},
	{"pvid.pvid_tpid_enc",		fe_tbl_SDB_pvid_pvid_tpid_enc},
	{"pvid.pvid_en",		fe_tbl_SDB_pvid_pvid_en},
	{"misc.use_egrlen_pkttype_policer", fe_tbl_SDB_misc_use_egrlen_pkttype_policer},
	{"misc.use_egrlen_src_policer",	fe_tbl_SDB_misc_use_egrlen_src_policer},
	{"misc.use_egrlen_flow_policer",fe_tbl_SDB_misc_use_egrlen_flow_policer},
	{"vlan.vlan_ingr_membership_en",fe_tbl_SDB_vlan_vlan_ingr_membership_en},
	{"vlan.vlan_egr_membership_en",	fe_tbl_SDB_vlan_vlan_egr_membership_en},
	{"vlan.vlan_egr_untag_chk_en",	fe_tbl_SDB_vlan_vlan_egr_untag_chk_en},
	{"misc.acl_dsbl",		fe_tbl_SDB_misc_acl_dsbl},
	{"misc.hash_sts_update_ctrl",	fe_tbl_SDB_misc_hash_sts_update_ctrl},
	{"parity",			fe_tbl_SDB_parity},
	{0, 				0}
};
