/******************************************************************************
	 Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblShaperVoq.c
   Date        : 2011-10-03
   Description : Cortina GoldenGate NetEngine configuration utility process 
	             fe module Shaper Voq Table
   Author      : Ethan Chen <ethan.chen@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../neSendCmd.h"
#include "../misc.h"

char *SP_DRR[] = {
	"SP",
	"DRR",
	0
};
/* fe->QOS_SHAPER_VOQ(fe_ingress_qos_shaper_voq_entry_s) */


int fe_tbl_QOS_SHAPER_VOQ_rate(char *pString, INGRESS_QOS_SHAPER_VOQ_TABLE_INFO * pSHAPERVoqInfo)
{
	
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERVoqInfo->tbl_SHAPER_VOQTABLE.rate = (cs_uint16) strtoul(pString, NULL, 0);

	
	return STATUS_SUCCESS;
} /* end fe_tbl_QOS_SHAPER_VOQ_rate() */
int fe_tbl_QOS_SHAPER_VOQ_voq_id(char *pString, INGRESS_QOS_SHAPER_VOQ_TABLE_INFO * pSHAPERVoqInfo)
{
			
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pSHAPERVoqInfo->tbl_SHAPER_VOQTABLE.voq_id = (cs_uint8) strtoul(pString, NULL, 0);	
	return STATUS_SUCCESS;
} /* fe_tbl_QOS_SHAPER_VOQ_voq_id() */

/*fix me, convert SP/DRR to enum QOS_PRIORITY_DEF defined in cs_ne_ioctl.h*/
int fe_tbl_QOS_SHAPER_VOQ_priority(char *pString, INGRESS_QOS_SHAPER_VOQ_TABLE_INFO * pSHAPERVoqInfo)
{
	int retStatus;	
	int i,hit=0;
  
  for (i = 0; SP_DRR[i] != NULL; i++) {
				if (strcasecmp(pString, SP_DRR[i]) == 0) {
					hit=1;					
					break;
				}
			}
	
	if(hit){
		//pSHAPERVoqInfo->tbl_SHAPER_VOQTABLE.sp_drr =(cs_uint8)strtoul(i, NULL, 0);	
		pSHAPERVoqInfo->tbl_SHAPER_VOQTABLE.sp_drr =(cs_uint8)i;	
		return STATUS_SUCCESS;
  }
  else{
  	return STATUS_FAILURE;
  }
} /* end fe_tbl_QOS_SHAPER_VOQ_priority() */


/*==================================================================== */

NE_FIELD_T fe_tbl_QOS_SHAPER_VOQ[] = {
	{"voq_id",                         	fe_tbl_QOS_SHAPER_VOQ_voq_id},
	{"priority",                				fe_tbl_QOS_SHAPER_VOQ_priority},
	{"rate",                            fe_tbl_QOS_SHAPER_VOQ_rate},	

	{0,                                     0}
};
