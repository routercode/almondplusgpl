/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : feTblPEVOQDROP.c
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

//
// fe->PE_VOQ_DROP->fe_pe_voq_drp_entry_s
//
int fe_tbl_PEVOQDROP_voq_id(char *pString, PE_VOQ_DROP_INFO * pPEVOQDROPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pPEVOQDROPInfo->tbl_PEVOQDROP.voq_id =
	    (cs_uint16) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
}				//end fe_tbl_PEVOQDROP_voq_id()
int fe_tbl_PEVOQDROP_f_drop_enbl(char *pString,
				 PE_VOQ_DROP_INFO * pPEVOQDROPInfo)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS) {
		return retStatus;
	}
	pPEVOQDROPInfo->tbl_PEVOQDROP.f_drop_enbl =
	    (cs_boolean) strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
}				//end fe_tbl_PEVOQDROP_f_drop_enbl()

//==================================================================================
//
NE_FIELD_T fe_tbl_PE_VOQ_DROP[] = {
	{
	 "voq_id", fe_tbl_PEVOQDROP_voq_id}
	, {
	   "f_drop_enbl", fe_tbl_PEVOQDROP_f_drop_enbl}
	, {
	   0, 0}
};
