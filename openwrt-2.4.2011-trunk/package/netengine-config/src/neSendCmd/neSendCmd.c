/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : neSendCmd.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration IOCTL file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include <ipc.h>
#include "neSendCmd.h"
#include "neSend_ioctl.h"
#include "misc.h"
#include "fe/feTblHashHash.h"
#include "fe/cal_hwHash.h"


/* Defined for FE all tables */

CLASSIFIER_INFO ClassifierInfo;
SDB_INFO SDBInfo;
HASH_MASK_INFO HashMaskInfo;
LPM_INFO LPMInfo;
HASH_MATCH_INFO HashMatchInfo;
FWDRSLT_INFO FWDRSLTInfo;
QOSRSLT_INFO QOSRSLTInfo;
L3_IP_INFO L3IPInfo;
L2_MAC_INFO L2MACInfo;
VOQ_POLICER_INFO VoQPolicerInfo;
LPB_INFO LPBInfo;
AN_BNG_MAC_INFO ANBNGMACInfo;
PORT_RANGE_INFO PortRangeInfo;
VLAN_INFO VLANInfo;
ACL_INFO ACLInfo;
PE_VOQ_DROP_INFO PEVOQDROPInfo;
ETYPE_INFO ETYPEInfo;
LLC_HDR_INFO LLCHDRInfo;
FVLAN_INFO FVLANInfo;
HASH_HASH_INFO HashHashInfo;
PKTLEN_RANGE_INFO PktlenRangeInfo;
CLASSIFIER_INFO *pClassifierInfo = &ClassifierInfo;
SDB_INFO *pSDBInfo = &SDBInfo;
HASH_MASK_INFO *pHashMaskInfo = &HashMaskInfo;
LPM_INFO *pLPMInfo = &LPMInfo;
HASH_MATCH_INFO *pHashMatchInfo = &HashMatchInfo;
FWDRSLT_INFO *pFWDRSLTInfo = &FWDRSLTInfo;
QOSRSLT_INFO *pQOSRSLTInfo = &QOSRSLTInfo;
L3_IP_INFO *pL3IPInfo = &L3IPInfo;
L2_MAC_INFO *pL2MACInfo = &L2MACInfo;
VOQ_POLICER_INFO *pVoQPolicerInfo = &VoQPolicerInfo;
LPB_INFO *pLPBInfo = &LPBInfo;
AN_BNG_MAC_INFO *pANBNGMACInfo = &ANBNGMACInfo;
PORT_RANGE_INFO *pPortRangeInfo = &PortRangeInfo;
VLAN_INFO *pVLANInfo = &VLANInfo;
ACL_INFO *pACLInfo = &ACLInfo;
PE_VOQ_DROP_INFO *pPEVOQDROPInfo = &PEVOQDROPInfo;
ETYPE_INFO *pETYPEInfo = &ETYPEInfo;
LLC_HDR_INFO *pLLCHDRInfo = &LLCHDRInfo;
FVLAN_INFO *pFVLANInfo = &FVLANInfo;
HASH_HASH_INFO *pHashHashInfo = &HashHashInfo;
fe_hash_hash_s feHashHash;
fe_hash_hash_s *pfeHashHash = &feHashHash;
HASH_CHECK_INFO HashCheckInfo;
HASH_CHECK_INFO *pHashCheckInfo = &HashCheckInfo;
PKTLEN_RANGE_INFO *pPktlenRangeInfo = &PktlenRangeInfo;

//[begin][ingress qos]add by ethan for ingress qos
INGRESS_QOS_TABLE_INFO InQosTableInfo;
INGRESS_QOS_SHAPER_VOQ_TABLE_INFO InQosShaperVoqTableInfo;
INGRESS_QOS_SHAPER_PORT_TABLE_INFO InQosShaperPortTableInfo;
INGRESS_QOS_API_TABLE_INFO InQosApiTableInfo;

INGRESS_QOS_TABLE_INFO *pInQosTableInfo = &InQosTableInfo;
INGRESS_QOS_SHAPER_VOQ_TABLE_INFO *pInQosShaperVoqTableInfo = &InQosShaperVoqTableInfo;
INGRESS_QOS_SHAPER_PORT_TABLE_INFO *pInQosShaperPortTableInfo = &InQosShaperPortTableInfo;
INGRESS_QOS_API_TABLE_INFO *pInQosApiTableInfo = &InQosApiTableInfo;
//[end][ingress qos]add by ethan for ingress qos

VOQ_COUNTER_API_TABLE_INFO VoqCntrApiTableInfo;
VOQ_COUNTER_API_TABLE_INFO *pVoqCntrApiTableInfo = &VoqCntrApiTableInfo;

TUNNEL_IPLIP_API_TABLE_INFO TunnelIplipApiTableInfo;
TUNNEL_IPLIP_API_TABLE_INFO *pTunnelIplipApiTableInfo = &TunnelIplipApiTableInfo;

/*-----------------------------------------------------------------
* ROUTINE NAME - PrintParas       
*------------------------------------------------------------------
* FUNCTION: This function is the print function for received parameters
*
* INPUT:    TimeInternal   *pTime
* OUTPUT:   NONE
* RETURN:   NONE
* NOTE:     
*----------------------------------------------------------------*/
void PrintParas(PAREMETER_T * pParaInfo)
{
	printf("\n");
	printf("*********************************\n");
	printf("GetParaMask 0x%8.8x\n", pParaInfo->GetParaMask);
	printf("StartCmd    %d\n", pParaInfo->StartCmd);
	printf("StopCmd     %d\n", pParaInfo->StopCmd);
	printf("GetTable    %d\n", pParaInfo->GetTable);
	printf("GetCommand  %d\n", pParaInfo->GetCommand);
	printf("GetField    %s\n", pParaInfo->GetField);
	printf("GetValue    %s\n", pParaInfo->GetValue);
	printf("GetBypass   %d\n", pParaInfo->GetBypass);
	printf("*********************************\n");
	usleep(300000);
	return;
}				/*end PrintParas() */

/*----------------------------------------------------------------------
* ROUTINE NAME - FindCommand
*-----------------------------------------------------------------------
* DESCRIPTION: Find the command in command table
* INPUT      : char *name
*              NE_FIELD_T *neField
* OUTPUT     : NE_FIELD_T *neField 
*              0: command is not found
*----------------------------------------------------------------------*/
NE_FIELD_T *FindCommand(char *name, NE_FIELD_T * neField)
{
	int i, total, len = strlen(name);
	NE_FIELD_T *me = NULL, *exact = NULL, *tmp;
	tmp = neField;
	exact = (NE_FIELD_T *) 0;
	total = 0;
	while (neField->name) {
		char *srce = name;
		char *dest = neField->name;
		for (i = 0; i < len; i++) {
			if (tolower(*dest) != tolower(*srce)) 
				break;

			dest++;
			srce++;
		}
		if (i == len) {
			if (i == strlen(neField->name))
				exact = neField;
			
			total++;
			me = neField;
		}
		neField++;
	} /*end while() */
	if (total > 1) 
		return ((NE_FIELD_T *) exact);	/* duplicate */
	
	if (total == 1) 
		return (me);
	else {
		printf("Possible fields:\n");
		while (tmp->name) {
			printf("\t%s\n", tmp->name);
			tmp++;
		}
		return ((NE_FIELD_T *) 0);
	}
	
} /*end FindCommand() */

/*-----------------------------------------------------------------
* ROUTINE NAME - DoStartCmd       
*----------------------------------------------------------------*/
void DoStartCmd(PAREMETER_T * pParaInfo)
{
	PrintParas(pParaInfo);
	switch (pParaInfo->GetTable) {
	case CS_IOTCL_TBL_CLASSIFIER:
		bzero(pClassifierInfo, sizeof(CLASSIFIER_INFO));
		pClassifierInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pClassifierInfo->ne_hdr.len = sizeof(CLASSIFIER_INFO);
		pClassifierInfo->cmd_Hdr.Module = pParaInfo->Module;
		pClassifierInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pClassifierInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pClassifierInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pClassifierInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pClassifierInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_SDB:
		bzero(pSDBInfo, sizeof(SDB_INFO));
		pSDBInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pSDBInfo->ne_hdr.len = sizeof(SDB_INFO);
		pSDBInfo->cmd_Hdr.Module = pParaInfo->Module;
		pSDBInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pSDBInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pSDBInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pSDBInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pSDBInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_HASH_MASK:
		bzero(pHashMaskInfo, sizeof(HASH_MASK_INFO));
		pHashMaskInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pHashMaskInfo->ne_hdr.len = sizeof(HASH_MASK_INFO);
		pHashMaskInfo->cmd_Hdr.Module = pParaInfo->Module;
		pHashMaskInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pHashMaskInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pHashMaskInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pHashMaskInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pHashMaskInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_LPM:
		bzero(pLPMInfo, sizeof(LPM_INFO));
		pLPMInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pLPMInfo->ne_hdr.len = sizeof(LPM_INFO);
		pLPMInfo->cmd_Hdr.Module = pParaInfo->Module;
		pLPMInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pLPMInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pLPMInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pLPMInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pLPMInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_HASH_MATCH:
		bzero(pHashMatchInfo, sizeof(HASH_MATCH_INFO));
		pHashMatchInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pHashMatchInfo->ne_hdr.len = sizeof(HASH_MATCH_INFO);
		pHashMatchInfo->cmd_Hdr.Module = pParaInfo->Module;
		pHashMatchInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pHashMatchInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pHashMatchInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pHashMatchInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pHashMatchInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_FWDRSLT:
		bzero(pFWDRSLTInfo, sizeof(FWDRSLT_INFO));
		pFWDRSLTInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pFWDRSLTInfo->ne_hdr.len = sizeof(FWDRSLT_INFO);
		pFWDRSLTInfo->cmd_Hdr.Module = pParaInfo->Module;
		pFWDRSLTInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pFWDRSLTInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pFWDRSLTInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pFWDRSLTInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pFWDRSLTInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_QOSRSLT:
		bzero(pQOSRSLTInfo, sizeof(QOSRSLT_INFO));
		pQOSRSLTInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pQOSRSLTInfo->ne_hdr.len = sizeof(QOSRSLT_INFO);
		pQOSRSLTInfo->cmd_Hdr.Module = pParaInfo->Module;
		pQOSRSLTInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pQOSRSLTInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pQOSRSLTInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pQOSRSLTInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pQOSRSLTInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_L3_IP:
		bzero(pL3IPInfo, sizeof(L3_IP_INFO));
		pL3IPInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pL3IPInfo->ne_hdr.len = sizeof(L3_IP_INFO);
		pL3IPInfo->cmd_Hdr.Module = pParaInfo->Module;
		pL3IPInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pL3IPInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pL3IPInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pL3IPInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pL3IPInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_L2_MAC:
		bzero(pL2MACInfo, sizeof(L2_MAC_INFO));
		pL2MACInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pL2MACInfo->ne_hdr.len = sizeof(L2_MAC_INFO);
		pL2MACInfo->cmd_Hdr.Module = pParaInfo->Module;
		pL2MACInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pL2MACInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pL2MACInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pL2MACInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pL2MACInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_VOQ_POLICER:
		bzero(pVoQPolicerInfo, sizeof(VOQ_POLICER_INFO));
		pVoQPolicerInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pVoQPolicerInfo->ne_hdr.len = sizeof(VOQ_POLICER_INFO);
		pVoQPolicerInfo->cmd_Hdr.Module = pParaInfo->Module;
		pVoQPolicerInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pVoQPolicerInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pVoQPolicerInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pVoQPolicerInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pVoQPolicerInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_LPB:
		bzero(pLPBInfo, sizeof(LPB_INFO));
		pLPBInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pLPBInfo->ne_hdr.len = sizeof(LPB_INFO);
		pLPBInfo->cmd_Hdr.Module = pParaInfo->Module;
		pLPBInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pLPBInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pLPBInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pLPBInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pLPBInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_AN_BNG_MAC:
		bzero(pANBNGMACInfo, sizeof(AN_BNG_MAC_INFO));
		pANBNGMACInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pANBNGMACInfo->ne_hdr.len = sizeof(AN_BNG_MAC_INFO);
		pANBNGMACInfo->cmd_Hdr.Module = pParaInfo->Module;
		pANBNGMACInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pANBNGMACInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pANBNGMACInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pANBNGMACInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pANBNGMACInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_PORT_RANGE:
		bzero(pPortRangeInfo, sizeof(PORT_RANGE_INFO));
		pPortRangeInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pPortRangeInfo->ne_hdr.len = sizeof(PORT_RANGE_INFO);
		pPortRangeInfo->cmd_Hdr.Module = pParaInfo->Module;
		pPortRangeInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pPortRangeInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pPortRangeInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pPortRangeInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pPortRangeInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_VLAN:
		bzero(pVLANInfo, sizeof(VLAN_INFO));
		pVLANInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pVLANInfo->ne_hdr.len = sizeof(VLAN_INFO);
		pVLANInfo->cmd_Hdr.Module = pParaInfo->Module;
		pVLANInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pVLANInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pVLANInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pVLANInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pVLANInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_ACL_RULE:
	case CS_IOCTL_TBL_ACL_ACTION:
		bzero(pACLInfo, sizeof(ACL_INFO));
		pACLInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pACLInfo->ne_hdr.len = sizeof(ACL_INFO);
		pACLInfo->cmd_Hdr.Module = pParaInfo->Module;
		pACLInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pACLInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pACLInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pACLInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pACLInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_PE_VOQ_DROP:
		bzero(pPEVOQDROPInfo, sizeof(PE_VOQ_DROP_INFO));
		pPEVOQDROPInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pPEVOQDROPInfo->ne_hdr.len = sizeof(PE_VOQ_DROP_INFO);
		pPEVOQDROPInfo->cmd_Hdr.Module = pParaInfo->Module;
		pPEVOQDROPInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pPEVOQDROPInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pPEVOQDROPInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pPEVOQDROPInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pPEVOQDROPInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_ETYPE:
		bzero(pETYPEInfo, sizeof(ETYPE_INFO));
		pETYPEInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pETYPEInfo->ne_hdr.len = sizeof(ETYPE_INFO);
		pETYPEInfo->cmd_Hdr.Module = pParaInfo->Module;
		pETYPEInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pETYPEInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pETYPEInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pETYPEInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pETYPEInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_LLC_HDR:
		bzero(pLLCHDRInfo, sizeof(LLC_HDR_INFO));
		pLLCHDRInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pLLCHDRInfo->ne_hdr.len = sizeof(LLC_HDR_INFO);
		pLLCHDRInfo->cmd_Hdr.Module = pParaInfo->Module;
		pLLCHDRInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pLLCHDRInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pLLCHDRInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pLLCHDRInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pLLCHDRInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_FVLAN:
		bzero(pFVLANInfo, sizeof(FVLAN_INFO));
		pFVLANInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pFVLANInfo->ne_hdr.len = sizeof(FVLAN_INFO);
		pFVLANInfo->cmd_Hdr.Module = pParaInfo->Module;
		pFVLANInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pFVLANInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pFVLANInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pFVLANInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pFVLANInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_HASH_HASH:
		bzero(pHashHashInfo, sizeof(HASH_HASH_INFO));
		pHashHashInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pHashHashInfo->ne_hdr.len = sizeof(HASH_HASH_INFO);
		pHashHashInfo->cmd_Hdr.Module = pParaInfo->Module;
		pHashHashInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pHashHashInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pHashHashInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pHashHashInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pHashHashInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
	case CS_IOCTL_TBL_HASH_CHECK:
		bzero(pHashCheckInfo, sizeof(HASH_CHECK_INFO));
		pHashCheckInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pHashCheckInfo->ne_hdr.len = sizeof(HASH_CHECK_INFO);
		pHashCheckInfo->cmd_Hdr.Module = pParaInfo->Module;
		pHashCheckInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pHashCheckInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pHashCheckInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pHashCheckInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pHashCheckInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;
//[begin][ingress qos]add by ethan for ingress qos	
	case CS_IOCTL_TBL_INGRESS_QOS_TABLE:		
		bzero(pInQosTableInfo, sizeof(INGRESS_QOS_TABLE_INFO));
		pInQosTableInfo->ne_hdr.cmd = NE_INGRESS_QOS_IOCTL;
		pInQosTableInfo->ne_hdr.len = sizeof(INGRESS_QOS_TABLE_INFO);
		pInQosTableInfo->cmd_Hdr.Module = pParaInfo->Module;
		pInQosTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pInQosTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pInQosTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pInQosTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pInQosTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		
		break;
	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_VOQ_TABLE:
		bzero(pInQosShaperVoqTableInfo, sizeof(INGRESS_QOS_SHAPER_VOQ_TABLE_INFO));
		pInQosShaperVoqTableInfo->ne_hdr.cmd = NE_INGRESS_QOS_IOCTL;
		pInQosShaperVoqTableInfo->ne_hdr.len = sizeof(INGRESS_QOS_SHAPER_VOQ_TABLE_INFO);
		pInQosShaperVoqTableInfo->cmd_Hdr.Module = pParaInfo->Module;
		pInQosShaperVoqTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pInQosShaperVoqTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pInQosShaperVoqTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pInQosShaperVoqTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pInQosShaperVoqTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		//setup default value	
		pInQosShaperVoqTableInfo->tbl_SHAPER_VOQTABLE.sp_drr=0xff;		
		break;	
		
	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_PORT_TABLE:
		bzero(pInQosShaperPortTableInfo, sizeof(INGRESS_QOS_SHAPER_PORT_TABLE_INFO));
		pInQosShaperPortTableInfo->ne_hdr.cmd = NE_INGRESS_QOS_IOCTL;
		pInQosShaperPortTableInfo->ne_hdr.len = sizeof(INGRESS_QOS_SHAPER_PORT_TABLE_INFO);
		pInQosShaperPortTableInfo->cmd_Hdr.Module = pParaInfo->Module;
		pInQosShaperPortTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pInQosShaperPortTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pInQosShaperPortTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pInQosShaperPortTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pInQosShaperPortTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;		
		break;	
		
//[end][ingress qos]add by ethan for ingress qos		

	case CS_IOCTL_TBL_INGRESS_QOS_API:
		bzero(pInQosApiTableInfo, sizeof(INGRESS_QOS_API_TABLE_INFO));
		pInQosApiTableInfo->ne_hdr.cmd = NE_INGRESS_QOS_IOCTL;
		pInQosApiTableInfo->ne_hdr.len = sizeof(INGRESS_QOS_API_TABLE_INFO);
		pInQosApiTableInfo->cmd_Hdr.Module = pParaInfo->Module;
		pInQosApiTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pInQosApiTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pInQosApiTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pInQosApiTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pInQosApiTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;		
		break;	

	case CS_IOCTL_TBL_VOQ_COUNTER_API:
		bzero(pVoqCntrApiTableInfo, sizeof(VOQ_COUNTER_API_TABLE_INFO));
		pVoqCntrApiTableInfo->ne_hdr.cmd = NE_VOQ_COUNTER_IOCTL;
		pVoqCntrApiTableInfo->ne_hdr.len = sizeof(VOQ_COUNTER_API_TABLE_INFO);
		pVoqCntrApiTableInfo->cmd_Hdr.Module = pParaInfo->Module;
		pVoqCntrApiTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pVoqCntrApiTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pVoqCntrApiTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pVoqCntrApiTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pVoqCntrApiTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;		
		break;	

	case CS_IOCTL_TBL_TUNNEL_IPLIP_API:
                bzero(pTunnelIplipApiTableInfo, sizeof(TUNNEL_IPLIP_API_TABLE_INFO));
                pTunnelIplipApiTableInfo->ne_hdr.cmd = NE_TUNNEL_IOCTL;
                pTunnelIplipApiTableInfo->ne_hdr.len = sizeof(TUNNEL_IPLIP_API_TABLE_INFO);
                pTunnelIplipApiTableInfo->cmd_Hdr.Module = pParaInfo->Module;
               	pTunnelIplipApiTableInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
                pTunnelIplipApiTableInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
                pTunnelIplipApiTableInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
                pTunnelIplipApiTableInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
                pTunnelIplipApiTableInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
                PrintParas(pParaInfo);
		break;
		
	case CS_IOCTL_TBL_PKTLEN:
		bzero(pPktlenRangeInfo, sizeof(PKTLEN_RANGE_INFO));
		pPktlenRangeInfo->ne_hdr.cmd = NE_FE_IOCTL;
		pPktlenRangeInfo->ne_hdr.len = sizeof(PKTLEN_RANGE_INFO);
		pPktlenRangeInfo->cmd_Hdr.Module = pParaInfo->Module;
		pPktlenRangeInfo->cmd_Hdr.table_id = pParaInfo->GetTable;
		pPktlenRangeInfo->cmd_Hdr.cmd = pParaInfo->GetCommand;
		pPktlenRangeInfo->cmd_Hdr.Bypass = pParaInfo->GetBypass;
		pPktlenRangeInfo->cmd_Hdr.idx_start = pParaInfo->idx_start;
		pPktlenRangeInfo->cmd_Hdr.idx_end = pParaInfo->idx_end;
		break;

	} /*end switch(pParaInfo->GetTable) */
	return;
} /*end DoStartCmd() */

/*-----------------------------------------------------------------
* ROUTINE NAME - DoStopCmd       
*----------------------------------------------------------------*/
int DoStopCmd(PAREMETER_T * pParaInfo)
{
	int retStatus = STATUS_FAILURE;
	void *pBuffer = NULL;

	switch (pParaInfo->GetTable) {
	case CS_IOTCL_TBL_CLASSIFIER:
		pBuffer = (void *)pClassifierInfo;
		break;
	case CS_IOCTL_TBL_SDB:
		pBuffer = (void *)pSDBInfo;
		break;
	case CS_IOCTL_TBL_HASH_MASK:
		pBuffer = (void *)pHashMaskInfo;
		break;
	case CS_IOCTL_TBL_LPM:
		pBuffer = (void *)pLPMInfo;
		break;
	case CS_IOCTL_TBL_HASH_MATCH:
		pBuffer = (void *)pHashMatchInfo;
		break;
	case CS_IOCTL_TBL_FWDRSLT:
		pBuffer = (void *)pFWDRSLTInfo;
		break;
	case CS_IOCTL_TBL_QOSRSLT:
		pBuffer = (void *)pQOSRSLTInfo;
		break;
	case CS_IOCTL_TBL_L3_IP:
		pBuffer = (void *)pL3IPInfo;
		break;
	case CS_IOCTL_TBL_L2_MAC:
		pBuffer = (void *)pL2MACInfo;
		break;
	case CS_IOCTL_TBL_VOQ_POLICER:
		pBuffer = (void *)pVoQPolicerInfo;
		break;
	case CS_IOCTL_TBL_LPB:
		pBuffer = (void *)pLPBInfo;
		break;
	case CS_IOCTL_TBL_AN_BNG_MAC:
		pBuffer = (void *)pANBNGMACInfo;
		break;
	case CS_IOCTL_TBL_PORT_RANGE:
		pBuffer = (void *)pPortRangeInfo;
		break;
	case CS_IOCTL_TBL_VLAN:
		pBuffer = (void *)pVLANInfo;
		break;
	case CS_IOCTL_TBL_ACL_RULE:
	case CS_IOCTL_TBL_ACL_ACTION:
		pBuffer = (void *)pACLInfo;
		break;
	case CS_IOCTL_TBL_PE_VOQ_DROP:
		pBuffer = (void *)pPEVOQDROPInfo;
		break;
	case CS_IOCTL_TBL_ETYPE:
		pBuffer = (void *)pETYPEInfo;
		break;
	case CS_IOCTL_TBL_LLC_HDR:
		pBuffer = (void *)pLLCHDRInfo;
		break;
	case CS_IOCTL_TBL_FVLAN:
		pBuffer = (void *)pFVLANInfo;
		break;
	case CS_IOCTL_TBL_HASH_HASH:

		/*calculate hash key */
		fe_tbl_HashHash_cal_crc(&(pfeHashHash->swhash),
					&(pHashHashInfo->crc32),
					&(pHashHashInfo->crc16), CRC16_CCITT);
		pBuffer = (void *)pHashHashInfo;
		break;
	case CS_IOCTL_TBL_HASH_CHECK:
		pBuffer = (void *)pHashCheckInfo;
		break;
	//[begin][ingress qos]add by ethan for ingress qos		
	case CS_IOCTL_TBL_INGRESS_QOS_TABLE:
		pBuffer = (void *) pInQosTableInfo;
		break;
	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_VOQ_TABLE:
		pBuffer = (void *) pInQosShaperVoqTableInfo;
		break;
	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_PORT_TABLE:
		pBuffer = (void *) pInQosShaperPortTableInfo;
		break;
	case CS_IOCTL_TBL_INGRESS_QOS_API:
		pBuffer = (void *) pInQosApiTableInfo;
		break;
	case CS_IOCTL_TBL_VOQ_COUNTER_API:
		pBuffer = (void *) pVoqCntrApiTableInfo;
		break;
	case CS_IOCTL_TBL_TUNNEL_IPLIP_API:
                pBuffer = (void *) pTunnelIplipApiTableInfo;
                break;
	case CS_IOCTL_TBL_PKTLEN:
		pBuffer = (void *)pPktlenRangeInfo;
		break;

	} /*end switch(pParaInfo->GetTable) */
	if (pBuffer) 
		retStatus = IOCTL_NetEngine(pBuffer);

	return retStatus;
} /*end DoStopCmd() */

/*-----------------------------------------------------------------
* ROUTINE NAME - DoStopCmd       
*----------------------------------------------------------------*/
int DoFieldnValueCmd(PAREMETER_T * pParaInfo)
{
	int retStatus = STATUS_FAILURE;
	NE_FIELD_T *neField;
	switch (pParaInfo->GetTable) {
	case CS_IOTCL_TBL_CLASSIFIER:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Classifier[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
							pClassifierInfo);
		}
		break;
	case CS_IOCTL_TBL_SDB:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_SDB[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pSDBInfo);
		}
		break;
	case CS_IOCTL_TBL_HASH_MASK:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Hash_Mask[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pHashMaskInfo);
		}
		break;
	case CS_IOCTL_TBL_LPM:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_LPM[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pLPMInfo);
		}
		break;
	case CS_IOCTL_TBL_HASH_MATCH:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Hash_Match[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					    		pHashMatchInfo);
		}
		break;
	case CS_IOCTL_TBL_FWDRSLT:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_FWDRSLT[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pFWDRSLTInfo);
		}
		break;
	case CS_IOCTL_TBL_QOSRSLT:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_QOSRSLT[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pQOSRSLTInfo);
		}
		break;
	case CS_IOCTL_TBL_L3_IP:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_L3_IP[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pL3IPInfo);
		}
		break;
	case CS_IOCTL_TBL_L2_MAC:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_L2_MAC[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pL2MACInfo);
		}
		break;
	case CS_IOCTL_TBL_VOQ_POLICER:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_VoQ_Policer[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					    		pVoQPolicerInfo);
		}
		break;
	case CS_IOCTL_TBL_LPB:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_LPB[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pLPBInfo);
		}
		break;
	case CS_IOCTL_TBL_AN_BNG_MAC:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_AN_BNG_MAC[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pANBNGMACInfo);
		}
		break;
	case CS_IOCTL_TBL_PORT_RANGE:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Port_Range[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					    		pPortRangeInfo);
		}
		break;
	case CS_IOCTL_TBL_VLAN:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_VLAN[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pVLANInfo);
		}
		break;
	case CS_IOCTL_TBL_ACL_RULE:
	case CS_IOCTL_TBL_ACL_ACTION:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_ACL[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
							pACLInfo);
		}
		break;
	case CS_IOCTL_TBL_PE_VOQ_DROP:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_PE_VOQ_DROP[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					    		pPEVOQDROPInfo);
		}
		break;
	case CS_IOCTL_TBL_ETYPE:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_ETYPE[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pETYPEInfo);
		}
		break;
	case CS_IOCTL_TBL_LLC_HDR:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_LLC_HDR[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pLLCHDRInfo);
		}
		break;
	case CS_IOCTL_TBL_FVLAN:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_FVLAN[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pFVLANInfo);
		}
		break;
	case CS_IOCTL_TBL_HASH_HASH:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_HASH_HASH[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pfeHashHash);
		}
		break;
	case CS_IOCTL_TBL_HASH_CHECK:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Hash_Check[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
							pHashCheckInfo);
		}
		break;
		
		//[begin][ingress qos]add by ethan for ingress qos		
	case CS_IOCTL_TBL_INGRESS_QOS_TABLE:			
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_QOS_TABLE[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pInQosTableInfo);
						
		}	
		break;

	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_VOQ_TABLE:		
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_QOS_SHAPER_VOQ[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pInQosShaperVoqTableInfo);
		}
		break;

	case CS_IOCTL_TBL_INGRESS_QOS_SHAPER_PORT_TABLE:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_QOS_SHAPER_PORT[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pInQosShaperPortTableInfo);
		}
		break;
		//[end][ingress qos]add by ethan for ingress qos		

	case CS_IOCTL_TBL_INGRESS_QOS_API:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & inqos_qos_api[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue, 
							pInQosApiTableInfo);
		}
		break;	
		
	case CS_IOCTL_TBL_VOQ_COUNTER_API:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & voqcntr_api[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					pVoqCntrApiTableInfo);
		}
		break;

	case CS_IOCTL_TBL_TUNNEL_IPLIP_API:
                neField = FindCommand(pParaInfo->GetField,
                                (NE_FIELD_T *) & tunnel_iplip_api[0]);
                if (neField && neField->action) {
                        retStatus = neField->action(pParaInfo->GetValue,
                                        pTunnelIplipApiTableInfo);
                }
                break;	
	case CS_IOCTL_TBL_PKTLEN:
		neField = FindCommand(pParaInfo->GetField,
				(NE_FIELD_T *) & fe_tbl_Pktlen_Range[0]);
		if (neField && neField->action) {
			retStatus = neField->action(pParaInfo->GetValue,
					pPktlenRangeInfo);
		}
		break;
	} /*end switch(pParaInfo->GetTable) */
	printf("pParaInfo->GetTable=%d", pParaInfo->GetTable);
	return retStatus;
} /*end DoFieldnValueCmd() */

/*-----------------------------------------------------------------
* ROUTINE NAME - ProcessCmd       
*----------------------------------------------------------------*/
int ProcessCmd(PAREMETER_T * pParaInfo)
{
	int ret;
	if (pParaInfo->StartCmd == 1) {
		DoStartCmd(pParaInfo);
		return STATUS_SUCCESS;
	}			/*end if(pParaInfo->StartCmd == 1) */
	if (pParaInfo->StopCmd == 1) {
		ret = DoStopCmd(pParaInfo);
		
		if (ret == STATUS_SUCCESS)
			show_result(pParaInfo);
		
		return ret;
	}			/*end if(pParaInfo->StopCmd == 1) */
	return DoFieldnValueCmd(pParaInfo);
}				/*end ProcessCmd() */

/*-----------------------------------------------------------------
* ROUTINE NAME - show_result       
*----------------------------------------------------------------*/
int show_result(PAREMETER_T * pParaInfo)
{
	switch (pParaInfo->GetTable) {
	case CS_IOCTL_TBL_INGRESS_QOS_API:
		switch (pInQosApiTableInfo->tbl_inqos_api.sub_cmd) {
		case CS_QOS_INGRESS_SET_MODE:
			printf("cs_qos_ingress_set_mode(): ret = %d\n"
				"\t mode =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.mode.mode);
			break;
		case CS_QOS_INGRESS_GET_MODE:
			printf("cs_qos_ingress_get_mode(): ret = %d\n"
				"\t mode =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.mode.mode);
			break;
		case CS_QOS_INGRESS_PRINT_MODE:
			printf("cs_qos_ingress_print_mode(): ret = %d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret);
			break;
		
		case CS_QOS_INGRESS_SET_PORT_PARAM:
			printf("cs_qos_ingress_set_port_param(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t burst_size =\t%d\n"
				"\t rate =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.port_param.port_id,
				pInQosApiTableInfo->tbl_inqos_api.port_param.burst_size,
				pInQosApiTableInfo->tbl_inqos_api.port_param.rate);
			break;
		case CS_QOS_INGRESS_GET_PORT_PARAM:
			printf("cs_qos_ingress_get_port_param(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t burst_size =\t%d\n"
				"\t rate =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.port_param.port_id,
				pInQosApiTableInfo->tbl_inqos_api.port_param.burst_size,
				pInQosApiTableInfo->tbl_inqos_api.port_param.rate);
			break;
		case CS_QOS_INGRESS_PRINT_PORT_PARAM:
			printf("cs_qos_ingress_print_port_param(): ret = %d\n"
				"\t port_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.port_param.port_id);
			break;
		
		case CS_QOS_INGRESS_SET_QUEUE_SCHEDULER:
			printf("cs_qos_ingress_set_queue_scheduler(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n"
				"\t priority =\t%d\n"
				"\t weight =\t%d\n"
				"\t rate =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.queue_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.priority,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.weight,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.rate);
			break;
		case CS_QOS_INGRESS_GET_QUEUE_SCHEDULER:
			printf("cs_qos_ingress_get_queue_scheduler(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n"
				"\t priority =\t%d\n"
				"\t weight =\t%d\n"
				"\t rate =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.queue_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.priority,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.weight,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.rate);
			break;
		case CS_QOS_INGRESS_PRINT_QUEUE_SCHEDULER:
			printf("cs_qos_ingress_print_queue_scheduler(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.queue_id);
			break;
		case CS_QOS_INGRESS_PRINT_QUEUE_SCHEDULER_OF_PORT:
			printf("cs_qos_ingress_print_queue_scheduler_of_port(): ret = %d\n"
				"\t port_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_scheduler.port_id);
			break;
		
		case CS_QOS_INGRESS_SET_QUEUE_SIZE:
			printf("cs_qos_ingress_set_queue_size(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n"
				"\t rsrv_size =\t%d\n"
				"\t max_size =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.queue_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.rsrv_size,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.max_size);
			break;
		case CS_QOS_INGRESS_GET_QUEUE_SIZE:
			printf("cs_qos_ingress_get_queue_size(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n"
				"\t rsrv_size =\t%d\n"
				"\t max_size =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.queue_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.rsrv_size,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.max_size);
			break;
		case CS_QOS_INGRESS_PRINT_QUEUE_SIZE:
			printf("cs_qos_ingress_print_queue_size(): ret = %d\n"
				"\t port_id =\t%d\n"
				"\t queue_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.port_id,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.queue_id);
			break;
		case CS_QOS_INGRESS_PRINT_QUEUE_SIZE_OF_PORT:
			printf("cs_qos_ingress_print_queue_size_of_port(): ret = %d\n"
				"\t port_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_size.port_id);
			break;
		
		case CS_QOS_INGRESS_SET_VALUE_QUEUE_MAPPING:
			printf("cs_qos_ingress_set_value_queue_mapping(): ret = %d\n"
				"\t value =\t%d\n"
				"\t queue_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_mapping.value,
				pInQosApiTableInfo->tbl_inqos_api.queue_mapping.queue_id);
			break;
		case CS_QOS_INGRESS_GET_VALUE_QUEUE_MAPPING:
			printf("cs_qos_ingress_get_value_queue_mapping(): ret = %d\n"
				"\t value =\t%d\n"
				"\t queue_id =\t%d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret,
				pInQosApiTableInfo->tbl_inqos_api.queue_mapping.value,
				pInQosApiTableInfo->tbl_inqos_api.queue_mapping.queue_id);
			break;
		case CS_QOS_INGRESS_PRINT_VALUE_QUEUE_MAPPING:
			printf("cs_qos_ingress_print_value_queue_mapping(): ret = %d\n",
				pInQosApiTableInfo->tbl_inqos_api.ret);
			break;
		
		default:
			return STATUS_SUCCESS;
	
		}
		break;	
		
	}
	return STATUS_SUCCESS;
}	/*end show_result() */

/*-----------------------------------------------------------------
* ROUTINE NAME - ExecNESendCmd       
*------------------------------------------------------------------
* FUNCTION: This function is the main program of neSendCmd
*
* INPUT:    NONE
* OUTPUT:   NONE
* RETURN:   NONE
* NOTE:     
*----------------------------------------------------------------*/
int ExecNESendCmd(void)
{
	int sockfd, len, sendlen = sizeof(PAREMETER_T);
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	char Buffer[sizeof(PAREMETER_T)];
	PAREMETER_T *pParaInfo;
	printf("neSendCmd: Start\n");
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("neSendCmd: Socket error\n");
		return STATUS_ERR_SOCKET;
	}
	/*build address data structure */
	bzero((char *)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(IPC_SERVER_PORT);

	/*setup passive open */
	if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
		printf("neSendCmd: Bind error\n");
		close(sockfd);
		return STATUS_ERR_BIND;
	} /*end if((bind(s, struct sockaddr *)&sin, sizeof(sin))) < 0) */
	while (1) {
		bzero(Buffer, sizeof(Buffer));
		addr_len = sizeof(struct sockaddr_in);
		len = recvfrom(sockfd, Buffer, sizeof(Buffer), 0, 
				(struct sockaddr *)&addr, &addr_len);
		if (addr.sin_addr.s_addr == LOCAL_HOST) {
			pParaInfo = (PAREMETER_T *) Buffer;

			/* Print received parameters */
			/* PrintParas(pParaInfo); */

			/* Check "Stop neSendCmd" */
			if (pParaInfo->neSendCmdQuit) {
				printf("neSendCmd: Receive neSendCmdQuit\n");
				break;
			}
			/* Process Command */
			pParaInfo->Status = ProcessCmd(pParaInfo);

			/* Send back command status  */
			sendto(sockfd, Buffer, sendlen, 0,
				(struct sockaddr *)&addr, addr_len);
		} /*end if(addr.sin_addr.s_addr == LOCAL_HOST) */
	} /*end while(go) */
	close(sockfd);
	printf("neSendCmd: Left\n");
	return 0;
}

/*-----------------------------------------------------------------
*                       Main Function Here       
*----------------------------------------------------------------*/
int main(void)
{
	if (fork() == 0)
		return ExecNESendCmd();
	else 
		return 0;
	
} /*end main() */
