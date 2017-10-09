/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : fe.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility fe module
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     :

 *****************************************************************************/


#include <stdio.h>

#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>
#include "../ne_cfg.h"
#include "../helpMsg.h"
char *fe_TableDef[] = {
	"Classifier",
	"SDB",
	"Hash_Mask",
	"LPM",
	"Hash_Match",
	"FWDRSLT",
	"QOSRSLT",
	"L3_IP",
	"L2_MAC",
	"VoQ_Policer",
	"LPB",
	"AN_BNG_MAC",
	"Port_Range",
	"VLAN",
	"ACL_Rule",
	"ACL_Action",
	"PE_VOQ_DROP",
	"ETYPE",
	"LLC_HDR",
	"FVLAN",
	"Hash_Hash",
	"Double_Check",
	"Pktlen_Range",
	0
};
char *fe_CommandDef[] = { "add", "delete", "flush",
			  "get", "replace", "init", 0 };

int CheckParas(char *pStr)
{

	/* Check Start Command Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_STARTCMD)	{
		if ((pParaInfo->StartCmd != 1) ||
		     (pParaInfo->GetParaMask & PARAMASK_STOPCMD) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETTABLE)) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETCOMMAND))) {
			fprintf(stderr, StartHelp, pStr);
			return STATUS_ERR_PARA_STARTCMD;
		}
		pParaInfo->StopCmd = 0;
	}

	/* check Stop Command Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_STOPCMD) {
		if ((pParaInfo->StopCmd != 1) ||
		     (pParaInfo->GetParaMask & PARAMASK_STARTCMD) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETTABLE))) {
			fprintf(stderr, StopHelp, pStr);
			return STATUS_ERR_PARA_STOPCMD;
		}
		pParaInfo->StartCmd = 0;
	}

	/* check Table Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_GETTABLE) {
		if (pParaInfo->GetTable >= CS_IOCTL_FE_TBL_MAX) {
			fprintf(stderr, StartHelp, pStr);
			return STATUS_ERR_PARA_GETTABLE;
		}
	}

	/* NOTE: Did NOT check Field and Value content here */
	/* check Field and Value Parameters */
	if (((pParaInfo->GetParaMask & PARAMASK_GETFIELD)
		 && (!(pParaInfo->GetParaMask & PARAMASK_GETVALUE)))
		|| ((!(pParaInfo->GetParaMask & PARAMASK_GETFIELD))
		     && (pParaInfo->GetParaMask & PARAMASK_GETVALUE))) {
		fprintf(stderr, FieldValueHelp, pStr);
		return STATUS_ERR_PARA;
	}

	/* check Command Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_GETCOMMAND) {
		if (pParaInfo->GetCommand >= CS_IOCTL_CMD_MAX) {
			fprintf(stderr, CommandHelp, pStr);
			return STATUS_ERR_PARA_GETCOMMAND;
		}
	}

	/* check Bypass Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_GETBYPASS) {
		if (pParaInfo->GetBypass > 1) {
			fprintf(stderr, BypassHelp, pStr);
			return STATUS_ERR_PARA_GETBYPASS;
		}
	}
	return STATUS_SUCCESS;
} /*end CheckPara() */


/*----------------------------------------------------------------------
* ROUTINE NAME - necfg_fe_mod
*-----------------------------------------------------------------------
* DESCRIPTION:
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED
*----------------------------------------------------------------------*/
int necfg_fe_mod(int argc, char **argv)
{
	int ret;

	/* Parser Paremeters */
	if ((ret = ParserOpt(pParaInfo, argc, argv, fe_TableDef,
			   fe_CommandDef)) != STATUS_SUCCESS)
	{
		if ((ret == STATUS_ERR_PARA_GETFIELD)
		     || (ret == STATUS_ERR_PARA_GETVALUE))
			fprintf(stderr, FieldValueHelp, argv[0]);
		else
			fprintf(stderr, UsageMsg, VersionInfo, argv[0]);
		return ret;
	}

	/* Check Paremeters */
	if ((ret = CheckParas(argv[0])) != STATUS_SUCCESS) {
		printf("[%s:%d]: Error %d\n", __FILE__, __LINE__, ret);
		return ret;
	}
	pParaInfo->Module = MODULE_FE;
	pParaInfo->Status = STATUS_NONE;
	if ((ret = SendCommand((char *)pParaInfo, sizeof(PAREMETER_T))) == 0)
		ret = pParaInfo->Status;

	if (ret != STATUS_SUCCESS)
	{
		printf("[%s:%d]: Status %d, pParaInfo->Status %d\n", __FILE__,
			__LINE__, ret, pParaInfo->Status);
	}
	return ret;
} /*end necfg_fe_mod() */
