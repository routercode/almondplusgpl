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

#include "cal_hwHash.h"
#include "cs_crc.h"
unsigned char HHBuffer[FE_HASH_HASH_BYTES];
unsigned long HHBitMask[] =
{ 0x00000001, 0x00000003, 0x00000007, 0x0000000F, 
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF, 
  0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
  0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF, 
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
  0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF, 
  0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF, 
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};
void fe_tbl_HashHash_Fill_Fields(unsigned char *pDstBuf,
				 unsigned char *pSrcBuf,
				 unsigned short StartPos,
				 unsigned short TotalWidth)
{
	unsigned short StartByte = (unsigned short)StartPos >> 3;
	unsigned short StartBit = (unsigned short)StartPos % 8;
	unsigned char RemainBits = 0, MoveBitsCount = 0;
	unsigned char *pDstByte = NULL, *pSrcByte = NULL;
	RemainBits = 8 - StartBit;
	pDstByte = (unsigned char *)(pDstBuf + StartByte);
	pSrcByte = (unsigned char *)pSrcBuf;
	if (TotalWidth <= 8) {
		if (RemainBits >= TotalWidth) {
			*pDstByte |=
			    ((*pSrcByte) & HHBitMask[TotalWidth - 1]) <<
			    StartBit;
		} else {
			*pDstByte |= ((*pSrcByte) << StartBit);
			TotalWidth -= RemainBits;
			*(pDstByte + 1) |=
			    (((*pSrcByte) >> RemainBits) &
			     HHBitMask[TotalWidth - 1]);
		}
	} else if (TotalWidth <= 64) {
		while (TotalWidth) {
			if (TotalWidth >= 8)
				MoveBitsCount = 8;
			else
				MoveBitsCount = TotalWidth;
			
			*pDstByte |= ((*pSrcByte) << StartBit);
			*(pDstByte + 1) |=
			    (((*pSrcByte) >> RemainBits) &
			     HHBitMask[TotalWidth - RemainBits - 1]);
			TotalWidth -= MoveBitsCount;
			pDstByte++;
			pSrcByte++;
		} /*end while(TotalWidth) */
	} else if (TotalWidth == 128) {	/* 128 bits */
		int i = 0;
		for (i = 0; i < 16; i++) {
			*pDstByte |= ((*pSrcByte) << StartBit);
			*(pDstByte + 1) |=
			    (((*pSrcByte) >> RemainBits) & HHBitMask[StartBit]);
			pDstByte++;
			pSrcByte++;
		}
	} /*end if(TotalWidth <= 8) */
	return;
} /*end fe_tbl_HashHash_Fill_Fields() */

void fe_tbl_HashHash_do_bits_table(fe_sw_hash_t * swhash)
{
	bzero(HHBuffer, sizeof(HHBuffer));
	fe_tbl_HashHash_Fill_Fields(HHBuffer, swhash->mac_da,
				    cs_fe_hash_hash_fields[FE_HASH_DA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_DA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer, swhash->mac_sa,
				    cs_fe_hash_hash_fields[FE_HASH_SA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_SA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->eth_type)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_ETHERTYPE_RAW].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_ETHERTYPE_RAW].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->llc_type_enc)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_LLC_TYPE_ENC].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_LLC_TYPE_ENC].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ip_frag)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_IP_FRAGMENT].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_IP_FRAGMENT].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->tpid_enc_1)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_TPID_ENC_1].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_TPID_ENC_1].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->_8021p_1)),
				    cs_fe_hash_hash_fields[FE_HASH_8021P_1].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_8021P_1].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->dei_1)),
				    cs_fe_hash_hash_fields[FE_HASH_DEI_1].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_DEI_1].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->vid_1)),
				    cs_fe_hash_hash_fields[FE_HASH_VID_1].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_VID_1].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->revd_135)),
				    cs_fe_hash_hash_fields[FE_HASH_RSVD_135].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_RSVD_135].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->tpid_enc_2)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_TPID_ENC_2].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_TPID_ENC_2].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->_8021p_2)),
				    cs_fe_hash_hash_fields[FE_HASH_8021P_2].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_8021P_2].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->dei_2)),
				    cs_fe_hash_hash_fields[FE_HASH_DEI_2].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_DEI_2].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->vid_2)),
				    cs_fe_hash_hash_fields[FE_HASH_VID_2].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_VID_2].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)&swhash->da[0],
				    cs_fe_hash_hash_fields[FE_HASH_IP_DA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IP_DA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer, (unsigned char *)swhash->sa,
				    cs_fe_hash_hash_fields[FE_HASH_IP_SA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IP_SA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ip_prot)),
				    cs_fe_hash_hash_fields[FE_HASH_IP_PROT].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IP_PROT].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->dscp)),
				    cs_fe_hash_hash_fields[FE_HASH_DSCP].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_DSCP].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ecn)),
				    cs_fe_hash_hash_fields[FE_HASH_ECN].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_ECN].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->pktlen_rng_match_vector)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_PKTLEN_RNG_MATCH_VECTOR].
				    start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_PKTLEN_RNG_MATCH_VECTOR].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->ipv6_flow_label)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_IPV6_FLOW_LBL].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_IPV6_FLOW_LBL].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ip_version)),
				    cs_fe_hash_hash_fields[FE_HASH_IP_VER].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IP_VER].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ip_valid)),
				    cs_fe_hash_hash_fields[FE_HASH_IP_VLD].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IP_VLD].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->l4_dp)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_DP_EXACT].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_DP_EXACT].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->l4_sp)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_SP_EXACT].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_SP_EXACT].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->tcp_ctrl_flags)),
				    cs_fe_hash_hash_fields[FE_HASH_TCP_CTRL].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_TCP_CTRL].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->tcp_ecn_flags)),
				    cs_fe_hash_hash_fields[FE_HASH_TCP_ECN].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_TCP_ECN].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->l4_valid)),
				    cs_fe_hash_hash_fields[FE_HASH_L4_VLD].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_L4_VLD].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->sdbid)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_SDB_KEYRULE].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_SDB_KEYRULE].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->lspid)),
				    cs_fe_hash_hash_fields[FE_HASH_LSPID].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_LSPID].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->fwdtype)),
				    cs_fe_hash_hash_fields[FE_HASH_FWDTYPE].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_FWDTYPE].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->pppoe_session_id_valid)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_PPPOE_SESSION_ID_VLD].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_PPPOE_SESSION_ID_VLD].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->pppoe_session_id)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_PPPOE_SESSION_ID].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_PPPOE_SESSION_ID].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->mask_ptr_0_7)),
				    cs_fe_hash_hash_fields
				    [FE_SDB_HTUPL_MASK_PTR_0_7].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_SDB_HTUPL_MASK_PTR_0_7].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->mcgid)),
				    cs_fe_hash_hash_fields[FE_HASH_MCGID].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_MCGID].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->mc_idx)),
				    cs_fe_hash_hash_fields[FE_HASH_MCIDX].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_MCIDX].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->da_an_mac_sel)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_DA_AN_MAC_SEL].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_DA_AN_MAC_SEL].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->da_an_mac_hit)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_DA_AN_MAC_HIT].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_DA_AN_MAC_HIT].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->sa_bng_mac_sel)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_SA_BNG_MAC_SEL].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_SA_BNG_MAC_SEL].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->sa_bng_mac_hit)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_SA_BNG_MAC_HIT].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_SA_BNG_MAC_HIT].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->orig_lspid)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_ORIG_LSPID].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_ORIG_LSPID].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->recirc_idx)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_RECIRC_IDX].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_RECIRC_IDX].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->l7_field)),
				    cs_fe_hash_hash_fields[FE_HASH_L7_FIELD].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_L7_FIELD].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->l7_field_valid)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_L7_FIELD_VLD].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_L7_FIELD_VLD].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->hdr_a_flags_crcerr)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_HDR_A_FLAGS_CRCERR].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_HDR_A_FLAGS_CRCERR].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->l3_csum_err)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_L3_CHKSUM_ERR].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_L3_CHKSUM_ERR].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->l4_csum_err)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_CHKSUM_ERR].start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_L4_CHKSUM_ERR].total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char
				     *)(&(swhash->not_hdr_a_flags_stsvld)),
				    cs_fe_hash_hash_fields
				    [FE_HASH_NOT_HDR_A_FLAGS_STSVLD].
				    start_pos,
				    cs_fe_hash_hash_fields
				    [FE_HASH_NOT_HDR_A_FLAGS_STSVLD].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->hash_fid)),
				    cs_fe_hash_hash_fields[FE_HASH_FID].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_FID].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->mc_da)),
				    cs_fe_hash_hash_fields[FE_HASH_MC_DA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_MC_DA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->bc_da)),
				    cs_fe_hash_hash_fields[FE_HASH_BC_DA].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_BC_DA].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->spi_vld)),
				    cs_fe_hash_hash_fields[FE_HASH_SPI_VLD].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_SPI_VLD].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->spi_idx)),
				    cs_fe_hash_hash_fields[FE_HASH_SPI_IDX].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_SPI_IDX].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ipv6_ndp)),
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_NDP].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_NDP].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ipv6_hbh)),
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_HBH].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_HBH].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ipv6_rh)),
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_RH].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_RH].
				    total_width);
	fe_tbl_HashHash_Fill_Fields(HHBuffer,
				    (unsigned char *)(&(swhash->ipv6_doh)),
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_DOH].
				    start_pos,
				    cs_fe_hash_hash_fields[FE_HASH_IPV6_DOH].
				    total_width);
	return;
} /*end fe_tbl_HashHash_do_bits_table() */

cs_uint32 cs_hash_keygen_crc32(cs_uint8 * buff, cs_uint16 size)
{
	cs_uint32 crc32 = 0xFFFFFFFF;
	cs_uint16 i;
	cs_uint8 data;
	for (i = FE_HASH_HASH_BITS; i > 0; i--) {
		data = cs_getBit(buff, i - 1);
		crc32 = cs_update_crc_32(crc32, data);
	}
	return crc32;
}

/****************************************************************/
/* $rtn_hdr_start Calculate crc16 				*/
/* CATEGORY   : Device                                          */
/* ACCESS     : Public                                          */
/* BLOCK      : LPM                                          	*/
/* CHIP       : G2                                       	*/
/* INPUTS     : o polynomial                                    */
/*              o buff pointer                                  */
/*              o size                                          */
/* OUTPUTS    : ----     					*/
/* RETURNS    : CRC16		                                */
/* DESCRIPTION:                                                 */
/* This API is used to calculate CRC16 based on given polynomial*/
/* 0 : x^16+x^12+x^5+1 (also known as CCITT CRC-16)             */
/* 1 : x^14+x^4+1                                               */
/* 2 : x^14+x^8+x^5+x^4+1                                       */
/* 3 : x^14+x^11+x^9+x^8+x^5+x^4+1                              */
/* $rtn_hdr_end                                                 */
/****************************************************************/
cs_uint16 cs_hash_keygen_crc16(cs_uint8 polynomial, cs_uint8 * buff,
			       cs_uint16 size)
{
	cs_uint16 crc16 = 0x3FFF;
	cs_uint16 i;
	cs_uint8 data;
	if (polynomial == CRC16_CCITT) {
		crc16 = 0xFFFF;
	}
	for (i = FE_HASH_HASH_BITS; i > 0; i--) {
		data = cs_getBit(buff, i - 1);
		switch (polynomial) {
		case CRC16_CCITT:
			crc16 = cs_update_crc_ccitt(crc16, data);
			break;
		case CRC16_14_1:
			crc16 = cs_update_crc_14_1(crc16, data);
			break;
		case CRC16_14_2:
			crc16 = cs_update_crc_14_2(crc16, data);
			break;
		case CRC16_14_3:
			crc16 = cs_update_crc_14_3(crc16, data);
			break;
		default:
			crc16 = cs_update_crc_ccitt(crc16, data);
			break;
		} /*end switch() */
	} /*end for() */
	return crc16;
}

/******************************************************************************
 *
 *                          Global Function Here
 *
 *****************************************************************************/
cs_status fe_tbl_HashHash_cal_crc(fe_sw_hash_t * swhash, cs_uint32 * pCrc32,
				  cs_uint16 * pCrc16, cs_uint8 crc16_polynomial)
{
	fe_tbl_HashHash_do_bits_table(swhash);
	*pCrc32 = cs_hash_keygen_crc32(HHBuffer, sizeof(HHBuffer));
	*pCrc16 =
	    cs_hash_keygen_crc16(crc16_polynomial, HHBuffer, sizeof(HHBuffer));
	return CS_OK;
} /*end fe_tbl_HashHash_cal_crc() */
