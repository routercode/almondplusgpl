/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_lpm_drv_api.c
 * This file contains the API for LPM module.
 *
 *
 * $Id: cs_fe_table_lpm.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 */

#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/types.h>
#include "../ni/cs752x_ioctl.h"
#include "cs_fe_lpm_api.h"
#include "cs_os_abstraction_api.h"

#define LPM_INIT_TIMEOUT   1000     /* 1 second or trying count */

lpm_tbl_t lpm_table_info;

/*
* Convert mask to pattern
*/
cs_uint32 cs_lpm_gen_ip_mask_v4(cs_uint8 mask)
{
	cs_uint8 i;
	cs_uint32 mask_pattern=0;

	for (i = 0; i < 32; i++) {
		if (mask--)
			mask_pattern |= 1<<(31-i);
	}

	return mask_pattern;
}


/****************************************************************/
/* $rtn_hdr_start G2 LPM Module initialization					*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_init(cs_uint8 debug_mode)
/* INPUTS     : o debug mode 									*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* This API is used to init the LPM table.						*/
/* The debug_mode enable the search area are defined by 		*/
/* internal register instead of SDB.							*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	FETOP_LPM_MEM_STATUS_t lpm_mem_status;
	FETOP_LPM_MEM_INIT_t lpm_mem_init;
	FETOP_LPM_LPM_CONFIG_t lpm_config;
	cs_int32 i, try_loop = 0;

	do {
		lpm_mem_status.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_MEM_STATUS);
		try_loop++;
		cs_pal_delay_us(1000);
	} while((lpm_mem_status.wrd&0xFFFF) != 0x00FF && (try_loop<LPM_INIT_TIMEOUT));

	if (try_loop >= LPM_INIT_TIMEOUT) {
		cs_pal_printk("lpm_mem_status:0x%x\n", lpm_mem_status.wrd);
		cs_pal_printk("Something not ready to init!\n");
		return CS_ERROR;
	}

	try_loop = 0;
	lpm_mem_init.bf.lpm0_lower_start_init = 1;
	lpm_mem_init.bf.lpm0_upper_start_init = 1;
	lpm_mem_init.bf.lpm1_upper_start_init = 1;
	lpm_mem_init.bf.lpm1_lower_start_init = 1;
	lpm_mem_init.bf.hc0_lower_start_init = 1;
	lpm_mem_init.bf.hc1_lower_start_init = 1;
	lpm_mem_init.bf.hc0_upper_start_init = 1;
	lpm_mem_init.bf.hc1_upper_start_init = 1;

	cs_pal_wt_hw_phy_reg(lpm_mem_init.wrd, FETOP_LPM_MEM_INIT);
	do {
		lpm_mem_status.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_MEM_STATUS);
		try_loop++;
		cs_pal_delay_us(1000);
	} while (((lpm_mem_status.wrd & 0xFF00) != 0x0000 )
			&& (try_loop < LPM_INIT_TIMEOUT));

	if (try_loop >= LPM_INIT_TIMEOUT) {
		cs_pal_printk("lpm_mem_status:0x%x\n", lpm_mem_status.wrd);
		cs_pal_printk("Initialize fail !\n");
		return CS_ERROR;
	}

	lpm_config.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_LPM_CONFIG);
	lpm_config.bf.sdb_en = (debug_mode == 1)? 0:1;
	cs_pal_wt_hw_phy_reg(lpm_config.wrd, FETOP_LPM_LPM_CONFIG);

	lpm_table_info.init_done = 1;
	lpm_table_info.active_tlb = 1;

	/* Init lpm_table info structure */
	cs_pal_memset(&lpm_table_info, 0, sizeof(lpm_tbl_t));
	for (i = 0; i < MAX_LPM_ENTRY_CNT4; i++)
		lpm_table_info.lpm_entries[i].is_empty = 1;

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Get LPM information							*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_get_status(lpm_tbl_t *lpm_table)
/* INPUTS     : o Pointer to lpm table							*/
/* OUTPUTS    : o structure to include lpm table				*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Get lpm module status, returned whole table and some 		*/
/* statistic 													*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_uint16 i;
	lpm_entity_info_t entity;

	for (i = 0; i < MAX_LPM_ENTRY_CNT4; i++) {
		if (lpm_table_info.lpm_entries[i].is_empty == 0)
			cs_lpm_get_entity_by_idx(i, &entity);
	}

	cs_pal_memcpy(lpm_table, &lpm_table_info, sizeof(lpm_tbl_t));

	return CS_OK;
}


/****************************************************************/
/* $rtn_hdr_start add LPM rule									*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_add_entry(lpm_entity_info_t *entity)
/* INPUTS     : o Pointer to a rule entity						*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Add one lpm rule												*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_uint8 i, j;
	cs_uint32 ip1, ip2;
	cs_uint8 mask1, mask2;

	/* return error if reach max entry count */
	if ((lpm_table_info.v4_entry_cnt + lpm_table_info.v6_entry_cnt*4 + (entity->is_v6?4:1))>MAX_LPM_ENTRY_CNT4)
		return CS_ERROR;

	if (entity->is_v6) {	/* IPv6 LPM*/
		i = MAX_LPM_ENTRY_CNT4 - 1;
		/* find start position of mask group */
		while ((i >= (MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt)) && \
				lpm_table_info.lpm_entries[i].mask < entity->mask) {
			i--;
		}

		j = i;
		/* Ensure that there is no duplicated entry */
		mask1 = entity->mask;
		ip1 = entity->ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask1);
		for(;j>=(MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt);j--){
			if(entity->mask == lpm_table_info.lpm_entries[j].mask){
				if(entity->mask > 96){    /* Just compare addr32[0~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							(entity->ip_u.addr32[2]==lpm_table_info.lpm_entries[i].ip_u.addr32[2])&&
							(entity->ip_u.addr32[1]==lpm_table_info.lpm_entries[i].ip_u.addr32[1])&&
							((entity->ip_u.addr32[0]&cs_lpm_gen_ip_mask_v4(128-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[0]&cs_lpm_gen_ip_mask_v4(128-mask1))))
						return CS_ERROR;

				}
				else if(entity->mask > 64){	/* Compare addr32[1~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							(entity->ip_u.addr32[2]==lpm_table_info.lpm_entries[i].ip_u.addr32[2])&&
							((entity->ip_u.addr32[1]&cs_lpm_gen_ip_mask_v4(64-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[1]&cs_lpm_gen_ip_mask_v4(64-mask1))))
						return CS_ERROR;

				}
				else if (entity->mask > 32){       /* Compare addr32[2~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							((entity->ip_u.addr32[2]&cs_lpm_gen_ip_mask_v4(32-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[2]&cs_lpm_gen_ip_mask_v4(32-mask1))))
						return CS_ERROR;

				}
				else if(entity->mask > 0){         /* Compare addr32[3] */
					if((entity->ip_u.addr32[3]&cs_lpm_gen_ip_mask_v4(mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[3]&cs_lpm_gen_ip_mask_v4(mask1)))
						return CS_ERROR;
				}

 			}
		}

		/* No duplicated rule, let's add one */
		j = i;
		mask1 = entity->mask;
		mask2 = lpm_table_info.lpm_entries[j].mask;
		while((mask1==mask2) && (j>(MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt))){
			j--;
			mask2 = lpm_table_info.lpm_entries[j].mask;
		}

		/* insert at last entry of mask group */
		for(i=MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt-1; i>j ; i++)
			cs_pal_memcpy(&lpm_table_info.lpm_entries[i],&lpm_table_info.lpm_entries[i+1],sizeof(lpm_entity_info_t));

		entity->index = i;
		entity->is_empty = 0;
		entity->is_v6 = 1;

		cs_pal_memcpy(&lpm_table_info.lpm_entries[i],entity,sizeof(lpm_entity_info_t));

		lpm_table_info.v6_entry_cnt++;
	} else {             /* IPv4 LPM */
		i=0;
		/* find start position of mask group */
		while((i<lpm_table_info.v4_entry_cnt) && \
				lpm_table_info.lpm_entries[i].mask > entity->mask ){
			i++;
		}

		j = i;
		/* Ensure that there is no duplicated entry */
		mask1 = entity->mask;
		ip1 = entity->ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask1);
		for(;j<lpm_table_info.v4_entry_cnt;j++){
			mask2 = lpm_table_info.lpm_entries[j].mask;
			ip2 = lpm_table_info.lpm_entries[j].ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask2);;
			if((mask1==mask2) && (ip1==ip2)){            /* Same rule */
				return CS_ERROR;
			}
		}

		/* No duplicated rule, let's add one */
		j = i;
		mask1 = entity->mask;
		mask2 = lpm_table_info.lpm_entries[j].mask;
		while((mask1==mask2) && (j<lpm_table_info.v4_entry_cnt)){
			j++;
			mask2 = lpm_table_info.lpm_entries[j].mask;
		}

		/* insert at last entry of mask group */
		for(i=lpm_table_info.v4_entry_cnt; i>j ; i--)
			cs_pal_memcpy(&lpm_table_info.lpm_entries[i],&lpm_table_info.lpm_entries[i-1],sizeof(lpm_entity_info_t));

		entity->index = i;
		entity->is_empty = 0;
		entity->is_v6 = 0;

		cs_pal_memcpy(&lpm_table_info.lpm_entries[i],entity,sizeof(lpm_entity_info_t));

		lpm_table_info.v4_entry_cnt++;

   }

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Search rule									*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_int8 cs_lpm_search_entry(lpm_entity_info_t *entity)
/* INPUTS     : o rule entity									*/
/* OUTPUTS    : ----											*/
/* RETURNS    : rule index or Error Code						*/
/* DESCRIPTION:													*/
/* delete one lpm rule based on rule content.					*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_int8 i,j;
	cs_uint32 ip1,ip2;
	cs_uint8 mask1,mask2;

	if(entity->is_v6){         /* IPv6 LPM*/
		i = MAX_LPM_ENTRY_CNT4-1;
		/* find start position of mask group */
		while((i >= (MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt)) && \
				lpm_table_info.lpm_entries[i].mask < entity->mask )
			i--;

		j = i;
		/* compare entry */
		mask1 = entity->mask;
		ip1 = entity->ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask1);
		for(;j>=(MAX_LPM_ENTRY_CNT4-lpm_table_info.v6_entry_cnt);j--){
			if(entity->mask == lpm_table_info.lpm_entries[j].mask){
				if(entity->mask > 96){    /* Just compare addr32[0~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							(entity->ip_u.addr32[2]==lpm_table_info.lpm_entries[i].ip_u.addr32[2])&&
							(entity->ip_u.addr32[1]==lpm_table_info.lpm_entries[i].ip_u.addr32[1])&&
							((entity->ip_u.addr32[0]&cs_lpm_gen_ip_mask_v4(128-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[0]&cs_lpm_gen_ip_mask_v4(128-mask1))))
						return j;
				}
				else if(entity->mask > 64){	/* Compare addr32[1~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							(entity->ip_u.addr32[2]==lpm_table_info.lpm_entries[i].ip_u.addr32[2])&&
							((entity->ip_u.addr32[1]&cs_lpm_gen_ip_mask_v4(64-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[1]&cs_lpm_gen_ip_mask_v4(64-mask1))))
						return j;

				}
				else if (entity->mask > 32){       /* Compare addr32[2~3] */
					if((entity->ip_u.addr32[3]==lpm_table_info.lpm_entries[i].ip_u.addr32[3])&&
							((entity->ip_u.addr32[2]&cs_lpm_gen_ip_mask_v4(32-mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[2]&cs_lpm_gen_ip_mask_v4(32-mask1))))
						return j;
				}
				else if(entity->mask > 0){         /* Compare addr32[3] */
					if((entity->ip_u.addr32[3]&cs_lpm_gen_ip_mask_v4(mask1))==(lpm_table_info.lpm_entries[i].ip_u.addr32[3]&cs_lpm_gen_ip_mask_v4(mask1)))
						return j;
				}
			}
		}
	} else {             /* IPv4 LPM */
		i=0;
		/* find start position of mask group */
		while((i<lpm_table_info.v4_entry_cnt) && \
				lpm_table_info.lpm_entries[i].mask > entity->mask ){
			i++;
		}
		j = i;
		/* compare entry */
		mask1 = entity->mask;
		ip1 = entity->ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask1);
		for(;j<lpm_table_info.v4_entry_cnt;j++){
			mask2 = lpm_table_info.lpm_entries[j].mask;
			ip2 = lpm_table_info.lpm_entries[j].ip_u.addr32[0] & cs_lpm_gen_ip_mask_v4(mask2);;
			if((mask1==mask2) && (ip1==ip2)){            /* Same rule */
				return j;
			}
		}
	}

	return CS_ERROR;
}

/****************************************************************/
/* $rtn_hdr_start del LPM rule									*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_delete_entry(lpm_entity_info_t *entity)
/* INPUTS     : o Pointer to a rule entity						*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* delete one lpm rule based on rule content.					*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_int8 index;

	index = cs_lpm_search_entry(entity);

	if (index==CS_ERROR)        /* Not found */
		return CS_ERROR;
	else
		cs_lpm_delete_index((cs_uint8)index);

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start del LPM rule by index							*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_delete_index(cs_uint8 index)
/* INPUTS     : o index to delete								*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* delete one lpm rule based on the index.						*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	if(lpm_table_info.lpm_entries[index].is_empty == 1)
		return CS_ERROR;

	if(lpm_table_info.lpm_entries[index].is_v6==1)
		lpm_table_info.v6_entry_cnt--;
	else
		lpm_table_info.v4_entry_cnt--;

	cs_pal_memset(&lpm_table_info.lpm_entries[index],0,sizeof(lpm_entity_info_t));

	lpm_table_info.lpm_entries[index].is_empty = 1;

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Get rule entity by index						*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_get_entity_by_idx(cs_uint8 index, lpm_entity_info_t *entity)
/* INPUTS     : o Index number									*/
/*              o Pointer to a rule entity						*/
/* OUTPUTS    : o rule entity									*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Get rule entity based on given index.						*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_uint8 table;
	cs_int16 i=0;
	FETOP_LPM_HC_TABLE0_UPPER_REGF_ACCESS_t hc_access;
	FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA0_t hc_data0;
	FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA1_t hc_data1;
	FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA2_t hc_data2;
	FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA3_t hc_data3;

	if(lpm_table_info.lpm_entries[index].is_empty == 1)
		return CS_ERROR;
	table = lpm_table_info.active_tlb;

	cs_pal_memcpy(entity, &lpm_table_info.lpm_entries[index], sizeof(lpm_entity_info_t));

	if(entity->is_v6){ /* check if it's V6 */
		if(entity->index>(MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6/2-1)){ 	/* use upper table */
			hc_access.bf.ACCESS=1;
			hc_access.bf.w_rdn=0;
			hc_access.bf.parity_bypass=0;
			hc_access.bf.address = entity->index-(MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6/2) ;
			cs_pal_wt_hw_phy_reg(hc_access.wrd,(table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_UPPER_REGF_ACCESS);
			/* Wait read complete*/
			while((hc_access.bf.ACCESS==1)&&(i<LPM_INIT_TIMEOUT))
				hc_access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_UPPER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;  /* ACCESS can't complete */
			hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA0);
			hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA1);
			entity->hit_count = hc_data1.wrd;
			entity->hit_count = entity->hit_count<<32;
			entity->hit_count |= hc_data0.wrd;
			lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
		}
		else{                      /* use lower table */
			hc_access.bf.ACCESS=1;
			hc_access.bf.w_rdn=0;
			hc_access.bf.parity_bypass=0;
			hc_access.bf.address = entity->index-(MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6) ;
			cs_pal_wt_hw_phy_reg(hc_access.wrd,(table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_LOWER_REGF_ACCESS);
 			/* Wait read complete*/
			while((hc_access.bf.ACCESS==1)&&(i<LPM_INIT_TIMEOUT))
				hc_access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_LOWER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;  /* ACCESS can't complete */
			hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA0);
			hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA1);
			entity->hit_count = hc_data1.wrd;
			entity->hit_count = entity->hit_count<<32;
			entity->hit_count |= hc_data0.wrd;
			lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
		}

	}
	else {	/* IPv4 LPM */
		if(entity->index>(MAX_LPM_ENTRY_CNT4/2-1)){ 	/* use upper table */
			hc_access.bf.ACCESS=1;
			hc_access.bf.w_rdn=0;
			hc_access.bf.parity_bypass=0;
			hc_access.bf.address = (entity->index-MAX_LPM_ENTRY_CNT4/2)/4 ;
			cs_pal_wt_hw_phy_reg(hc_access.wrd,(table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_UPPER_REGF_ACCESS);
			/* Wait read complete*/
			while((hc_access.bf.ACCESS==1)&&(i<LPM_INIT_TIMEOUT))
				hc_access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_UPPER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;  /* ACCESS can't complete */

			switch (entity->index%4){
				case 0:
					hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA0);
					entity->hit_count = hc_data0.bf.up_hit_cnt00;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 1:
					hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA0);
					hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA1);
					entity->hit_count = (hc_data1.bf.up_hit_cnt01<<5)|hc_data0.bf.up_hit_cnt01;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 2:
					hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA1);
					hc_data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA2:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA2);
					entity->hit_count = (hc_data2.bf.up_hit_cnt02<<10)|hc_data1.bf.up_hit_cnt02;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 3:
					hc_data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA2:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA2);
					hc_data3.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_UPPER_REGF_DATA3:FETOP_LPM_HC_TABLE0_UPPER_REGF_DATA3);
					entity->hit_count = (hc_data3.bf.up_hit_cnt03<<15)|hc_data2.bf.up_hit_cnt03;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				default:
	   				/* Impossible to here */
					break;
 			}
		}
		else{                      /* use lower table */
			hc_access.bf.ACCESS=1;
			hc_access.bf.w_rdn=0;
			hc_access.bf.parity_bypass=0;
			hc_access.bf.address = entity->index/4 ;
			cs_pal_wt_hw_phy_reg(hc_access.wrd,(table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_LOWER_REGF_ACCESS);
			/* Wait read complete*/
			while((hc_access.bf.ACCESS==1)&&(i<LPM_INIT_TIMEOUT))
				hc_access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_HC_TABLE0_LOWER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;  /* ACCESS can't complete */

			switch (entity->index%4){
				case 0:
					hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA0);
					entity->hit_count = hc_data0.bf.up_hit_cnt00;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 1:
					hc_data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA0);
					hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA1);
					entity->hit_count = (hc_data1.bf.up_hit_cnt01<<5)|hc_data0.bf.up_hit_cnt01;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 2:
					hc_data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA1);
					hc_data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA2:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA2);
					entity->hit_count = (hc_data2.bf.up_hit_cnt02<<10)|hc_data1.bf.up_hit_cnt02;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				case 3:
					hc_data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA2:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA2);
					hc_data3.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_HC_TABLE1_LOWER_REGF_DATA3:FETOP_LPM_HC_TABLE0_LOWER_REGF_DATA3);
					entity->hit_count = (hc_data3.bf.up_hit_cnt03<<15)|hc_data2.bf.up_hit_cnt03;
					lpm_table_info.lpm_entries[index].hit_count = entity->hit_count;
					break;
				default:
	   				/* Impossible to here */
					break;
			}
		}
	}

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Get rule entity by partial information		*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_get_entity_by_rule(lpm_entity_info_t *entity)
/* INPUTS     : o Pointer to a rule entity						*/
/* OUTPUTS    : o rule entity									*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Get rule entity based on given parital information.			*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_int8 index;

	index = cs_lpm_search_entry(entity);

	if(index==CS_ERROR)        /* Not found */
		return CS_ERROR;

	if(lpm_table_info.lpm_entries[index].is_empty==0)
		cs_lpm_get_entity_by_idx((cs_uint8)index, entity);

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Flush whole lpm table to HW register			*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_flush_all(cs_uint8 table)
/* INPUTS     : o Table number to flush							*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Update SW table to HW LPM registers.							*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_uint8 i;

	/* FIXME? if flush_entity fails returns? */
	for(i=0;i<MAX_LPM_ENTRY_CNT4;i++){
		if(lpm_table_info.lpm_entries[i].is_empty==0)  /* Not empty */
			cs_lpm_flush_entity(&lpm_table_info.lpm_entries[i],table);
	}
	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start Flush entity to HW register					*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_flush_entity(lpm_entity_info_t *entity,cs_uint8 table)
/* INPUTS     : o Pointer to a rule entity						*/
/* INPUTS     : o Table number to flush							*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Flush entity to HW register.									*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	cs_int8 is_upper=0,addr=0;
	cs_int16 i=0;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_ACCESS_t access;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA8_t data8;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA7_t data7;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA6_t data6;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA5_t data5;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA4_t data4;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA3_t data3;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA2_t data2;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA1_t data1;
	FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA0_t data0;

	if(entity->is_v6){
		if(entity->index>(MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6/2-1)){               /* Upper table */
			addr = entity->index - (MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6/2);
			is_upper = 1;
			/* IPv6 will overwrite all colume, no need to read first */
			data0.wrd = 0;
			data1.wrd = 0;
			data2.wrd = 0;
			data3.wrd = 0;
			data4.wrd = 0;
			data5.wrd = 0;
			data6.wrd = 0;
			data7.wrd = 0;
			data8.wrd = 0;

	   		data0.bf.up_nh00 = ((entity->hash_index&0x01FFF)<<4)|(entity->priority & 0x0F);
			data3.bf.up_mask00 = entity->mask ;
			data3.bf.up_ip00 = entity->ip_u.addr32[0]&0x00003FFF;
			data4.bf.up_ip00 = entity->ip_u.addr32[0]&0x0003FFFF;
			data4.bf.up_ip01 = entity->ip_u.addr32[1]&0x000000FF;
			data5.bf.up_ip01 = entity->ip_u.addr32[1]&0x00FFFFFF;
			data5.bf.up_ip02 = entity->ip_u.addr32[2]&0x00000003;
			data6.bf.up_ip02 = entity->ip_u.addr32[2]&0x3FFFFFFF;
			data7.bf.up_ip03 = entity->ip_u.addr32[3]&0x0FFFFFFF;
			data8.bf.up_ip03 = entity->ip_u.addr32[3]&0x0000000F;

	   		cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA0);
			cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA1);
			cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA3);
			cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA4);
			cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA5);
			cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA6);
			cs_pal_wt_hw_phy_reg(data7.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA7);
			cs_pal_wt_hw_phy_reg(data8.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA8);

		}
		else{                               /* Lower table */
			is_upper = 0;
			addr = entity->index - (MAX_LPM_ENTRY_CNT4-MAX_LPM_ENTRY_CNT6);
			/* IPv6 will overwrite all colume, no need to read first */
			data0.wrd = 0;
			data1.wrd = 0;
			data2.wrd = 0;
			data3.wrd = 0;
			data4.wrd = 0;
			data5.wrd = 0;
			data6.wrd = 0;
			data7.wrd = 0;
			data8.wrd = 0;

	   		data0.bf.up_nh00 = ((entity->hash_index&0x01FFF)<<4)|(entity->priority & 0x0F);
			data3.bf.up_mask00 = entity->mask ;
			data3.bf.up_ip00 = entity->ip_u.addr32[0]&0x00003FFF;
			data4.bf.up_ip00 = entity->ip_u.addr32[0]&0x0003FFFF;
			data4.bf.up_ip01 = entity->ip_u.addr32[1]&0x000000FF;
			data5.bf.up_ip01 = entity->ip_u.addr32[1]&0x00FFFFFF;
			data5.bf.up_ip02 = entity->ip_u.addr32[2]&0x00000003;
			data6.bf.up_ip02 = entity->ip_u.addr32[2]&0x3FFFFFFF;
			data7.bf.up_ip03 = entity->ip_u.addr32[3]&0x0FFFFFFF;
			data8.bf.up_ip03 = entity->ip_u.addr32[3]&0x0000000F;

			cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA0);
			cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA1);
			cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA3);
			cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA4);
			cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA5);
			cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA6);
			cs_pal_wt_hw_phy_reg(data7.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA7);
			cs_pal_wt_hw_phy_reg(data8.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA8);
		}
	}
	else{
		if(entity->index>(MAX_LPM_ENTRY_CNT4/2-1)){               /* Upper table */
			is_upper = 1;
			addr = (entity->index-MAX_LPM_ENTRY_CNT4/2)/4 ;
			/* Read first then update necessary field */
			access.bf.address = addr;
			access.bf.parity_bypass=0;
			access.bf.w_rdn=0;
			access.bf.ACCESS=1;
			cs_pal_wt_hw_phy_reg(access.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_UPPER_REGF_ACCESS );
			/* Check if access complete */
			while((i<LPM_INIT_TIMEOUT) && (access.bf.ACCESS==1))
				access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_UPPER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;

			data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA0);
			data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA1);
			data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA2);
			data3.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA3);
			data4.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA4);
			data5.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA5);
			data6.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA6);
			data7.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA7);
			data8.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA8);

			switch(entity->index%4){
				case 0:
			   		data0.bf.up_nh00 = ((entity->hash_index&0x01FFF)<<4)|(entity->priority & 0x0F);
		   			data3.bf.up_mask00 = entity->mask ;
	   				data3.bf.up_ip00 = entity->ip_u.addr32[0]&0x3FFF;
   					data4.bf.up_ip00 = (entity->ip_u.addr32[0]>>14)&0x3FFFF;

					cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA0);
					cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA3);
					cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA4);

					break;
				case 1:
   					data0.bf.up_nh01 = ((entity->hash_index&0x01)<<4)|(entity->priority & 0x0F);
					data1.bf.up_nh01 = (entity->hash_index>>1)&0xFFF;
					data4.bf.up_mask01 = entity->mask;
					data4.bf.up_ip01 =  entity->ip_u.addr32[0]&0xFF;
					data5.bf.up_ip01 = (entity->ip_u.addr32[0]>>8)&0xFFFFFF;

					cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA0);
					cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA1);
					cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA4);
					cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA5);
					break;
				case 2:
   					data1.bf.up_nh02 = ((entity->hash_index&0x3F)<<4)|(entity->priority & 0x0F);
					data2.bf.up_nh02 = (entity->hash_index>>6)&0x7F;
					data5.bf.up_mask02 = entity->mask;
					data5.bf.up_ip02 =  entity->ip_u.addr32[0]&0x03;
					data6.bf.up_ip02 = (entity->ip_u.addr32[0]>>2)&0x3FFFFFFF;

					cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA1);
					cs_pal_wt_hw_phy_reg(data2.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA2);
					cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA5);
					cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA6);
					break;
				case 3:
   					data2.bf.up_nh03 = ((entity->hash_index&0x7FF)<<4)|(entity->priority & 0x0F);
					data3.bf.up_nh03 = (entity->hash_index>>11)&0x03;
					data6.bf.up_mask03 = entity->mask&0x03;
					data7.bf.up_mask03 = (entity->mask>>2)&0xF;
					data7.bf.up_ip03 =  entity->ip_u.addr32[0]&0x0FFFFFFF;
					data8.bf.up_ip03 = (entity->ip_u.addr32[0]>>28)&0x0F;

					cs_pal_wt_hw_phy_reg(data2.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA2);
					cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA3);
					cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA6);
					cs_pal_wt_hw_phy_reg(data7.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA7);
					cs_pal_wt_hw_phy_reg(data8.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_UPPER_REGF_DATA8);
					break;
				default:
					/* impossible to here */
					break;
			}
		}
		else{                               /* Lower table */
			/* Read first then update necessary field */
			is_upper = 0;
			addr = entity->index/4;
			access.bf.address = addr;
			access.bf.parity_bypass=0;
			access.bf.w_rdn=0;
			access.bf.ACCESS=1;
			cs_pal_wt_hw_phy_reg(access.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_LOWER_REGF_ACCESS );
			/* Check if access complete */
			while((i<LPM_INIT_TIMEOUT) && (access.bf.ACCESS==1))
				access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_LOWER_REGF_ACCESS);
			if(i>=LPM_INIT_TIMEOUT)
				return CS_ERROR;

			data0.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA0);
			data1.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA1);
			data2.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA2);
			data3.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA3);
			data4.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA4);
			data5.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA5);
			data6.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA6);
			data7.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA7);
			data8.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA8);

			switch(entity->index%4){
				case 0:
					data0.bf.up_nh00 = ((entity->hash_index&0x01FFF)<<4)|(entity->priority & 0x0F);
					data3.bf.up_mask00 = entity->mask ;
					data3.bf.up_ip00 = entity->ip_u.addr32[0]&0x3FFF;
					data4.bf.up_ip00 = (entity->ip_u.addr32[0]>>14)&0x3FFFF;

					cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA0);
					cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA3);
					cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA4);

					break;
			case 1:
					data0.bf.up_nh01 = ((entity->hash_index&0x01)<<4)|(entity->priority & 0x0F);
					data1.bf.up_nh01 = (entity->hash_index>>1)&0xFFF;
					data4.bf.up_mask01 = entity->mask;
					data4.bf.up_ip01 =  entity->ip_u.addr32[0]&0xFF;
					data5.bf.up_ip01 = (entity->ip_u.addr32[0]>>8)&0xFFFFFF;

					cs_pal_wt_hw_phy_reg(data0.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA0:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA0);
					cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA1);
					cs_pal_wt_hw_phy_reg(data4.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA4:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA4);
					cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA5);
					break;
			case 2:
					data1.bf.up_nh02 = ((entity->hash_index&0x3F)<<4)|(entity->priority & 0x0F);
					data2.bf.up_nh02 = (entity->hash_index>>6)&0x7F;
					data5.bf.up_mask02 = entity->mask;
					data5.bf.up_ip02 =  entity->ip_u.addr32[0]&0x03;
					data6.bf.up_ip02 = (entity->ip_u.addr32[0]>>2)&0x3FFFFFFF;

					cs_pal_wt_hw_phy_reg(data1.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA1:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA1);
					cs_pal_wt_hw_phy_reg(data2.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA2);
					cs_pal_wt_hw_phy_reg(data5.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA5:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA5);
					cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA6);
					break;
			case 3:
					data2.bf.up_nh03 = ((entity->hash_index&0x7FF)<<4)|(entity->priority & 0x0F);
					data3.bf.up_nh03 = (entity->hash_index>>11)&0x03;
					data6.bf.up_mask03 = entity->mask&0x03;
					data7.bf.up_mask03 = (entity->mask>>2)&0xF;
					data7.bf.up_ip03 =  entity->ip_u.addr32[0]&0x0FFFFFFF;
					data8.bf.up_ip03 = (entity->ip_u.addr32[0]>>28)&0x0F;

					cs_pal_wt_hw_phy_reg(data2.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA2:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA2);
					cs_pal_wt_hw_phy_reg(data3.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA3:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA3);
					cs_pal_wt_hw_phy_reg(data6.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA6:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA6);
					cs_pal_wt_hw_phy_reg(data7.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA7:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA7);
					cs_pal_wt_hw_phy_reg(data8.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_DATA8:FETOP_LPM_LPM_TABLE0_LOWER_REGF_DATA8);
					break;
			default:
					/* impossible to here */
					break;

			}
		}
	}

	access.bf.address = addr;
	access.bf.parity_bypass=0;
	access.bf.w_rdn=1;
	access.bf.ACCESS=1;
	i = 0;
	if(is_upper==1){
		cs_pal_wt_hw_phy_reg(access.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_UPPER_REGF_ACCESS );
		/* Check if access complete */
		while((i<LPM_INIT_TIMEOUT) && (access.bf.ACCESS==1))
   			access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_UPPER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_UPPER_REGF_ACCESS);
		if(i>=LPM_INIT_TIMEOUT)
			return CS_ERROR;           /* access not complete */
	}
	else{
		cs_pal_wt_hw_phy_reg(access.wrd,(table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_LOWER_REGF_ACCESS );
		/* Check if access complete */
		while((i<LPM_INIT_TIMEOUT) && (access.bf.ACCESS==1))
			access.wrd = cs_pal_rd_hw_phy_reg((table==2)? FETOP_LPM_LPM_TABLE1_LOWER_REGF_ACCESS:FETOP_LPM_LPM_TABLE0_LOWER_REGF_ACCESS);
		if(i>=LPM_INIT_TIMEOUT)
			return CS_ERROR;           /* access not complete */
	}

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start active lpm table								*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_active_table(cs_uint8 table)
/* INPUTS     : o table index to active							*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* LPM has two tables, one is active and another is shadow.		*/
/* The shadow table is for software update, LPM engine doesn't	*/
/* refer shadow table.											*/
/* when table be modified, we may need to change SDB table too.	*/
/* After SDB and other related table updated, we can swap 		*/
/* shadow table to active.										*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	FETOP_LPM_LPM_CONFIG_t lpm_config;
	FETOP_LPM_LPM_STATUS_t lpm_status;
	cs_int32 try_loop=0;

	/* Set Active table */
	lpm_config.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_LPM_CONFIG);
	lpm_config.bf.active_table = table + 1;
	cs_pal_wt_hw_phy_reg(lpm_config.wrd, FETOP_LPM_LPM_CONFIG);

	/* Poll Status register to check if change has applied to HW  */
	do{
		lpm_status.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_LPM_STATUS);
	}while((lpm_status.bf.lpm_status != (table+1)) && (try_loop<LPM_INIT_TIMEOUT) );

	if(try_loop>=LPM_INIT_TIMEOUT)
		return CS_ERROR;

	return CS_OK;
}

/****************************************************************/
/* $rtn_hdr_start disable lpm search							*/
/* CATEGORY   : Device											*/
/* ACCESS     : Public											*/
/* BLOCK      : LPM												*/
/* CHIP       : G2												*/
cs_status cs_lpm_disable(void)
/* INPUTS     : ----											*/
/* OUTPUTS    : ----											*/
/* RETURNS    : CS_OK or Error Code								*/
/* DESCRIPTION:													*/
/* Disable LPM engine, but keep all table content.				*/
/* $rtn_hdr_end													*/
/****************************************************************/
{
	FETOP_LPM_LPM_CONFIG_t lpm_config;
	FETOP_LPM_LPM_STATUS_t lpm_status;
	cs_int32 try_loop=0;

	/* Set Active table as 0 ==> LPM engine will pause search engine */
	lpm_config.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_LPM_CONFIG);
	lpm_config.bf.active_table = 0;
	cs_pal_wt_hw_phy_reg(lpm_config.wrd, FETOP_LPM_LPM_CONFIG);

	/* Poll Status register to check if change has applied to HW  */
	do{
		lpm_status.wrd = cs_pal_rd_hw_phy_reg(FETOP_LPM_LPM_STATUS);
		cs_pal_delay_us(1000);
	}while((lpm_status.bf.lpm_status != 0) && (try_loop<LPM_INIT_TIMEOUT) );

	if(try_loop>=LPM_INIT_TIMEOUT)
		return CS_ERROR;

	return CS_OK;
}

cs_status cs_ioctl_lpm(struct net_device *dev, void *pdata , unsigned short cmd)
{
	fe_lpm_entry_s *lpm_entry = (fe_lpm_entry_s *)pdata;
	struct lpm_entity_info entity;
	cs_pal_memset(&entity,0,sizeof(struct lpm_entity_info));

	entity.is_v6 = 0;
	entity.ip_u.addr32[0] = lpm_entry->ip_addr[0];
	entity.mask	= lpm_entry->mask[0];
	entity.priority = lpm_entry->priority[0];
	entity.hash_index = lpm_entry->result_idx[0];

	switch(cmd){
		case CMD_ADD:
			cs_lpm_add_entry(&entity);
			break;
		case CMD_DELETE:
			cs_lpm_delete_entry(&entity);
			break;
		case CMD_FLUSH:
			cs_lpm_flush_all(lpm_table_info.active_tlb);	/* active table */
			break;
		case CMD_GET:
			cs_lpm_get_entity_by_rule(&entity);
			break;
		case CMD_REPLACE:

			break;
		default:
			return CS_ERROR;
	}

	return 0;
}

