/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : voq_counter.c
   Date        : 2011-09-20
   Description : Cortina GoldenGate NetEngine configuration utility VOQ Counter module
   Author      : Wen Hsu <whsu@cortina-systems.com>
   Remarks     :

 *****************************************************************************/


#include <stdio.h>

#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>
#include "../ne_cfg.h"
#include "../helpMsg.h"

char *voq_counter_TableDef[] = {
	"voqcntr_api",
	0
};
char *voq_counter_CommandDef[] = {"set", "reset",0 };

int voq_cntl_CheckParas(char *pStr)
{
	/* Check Start Command Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_STARTCMD)	{
		if ((pParaInfo->StartCmd != 1) ||
		     (pParaInfo->GetParaMask & PARAMASK_STOPCMD) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETTABLE)) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETCOMMAND))) {
			fprintf(stderr, VoqcntrStartHelp, pStr);
			return STATUS_ERR_PARA_STARTCMD;
		}
		pParaInfo->StopCmd = 0;
	}

	/* check Stop Command Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_STOPCMD) {
		if ((pParaInfo->StopCmd != 1) ||
		     (pParaInfo->GetParaMask & PARAMASK_STARTCMD) ||
		     (!(pParaInfo->GetParaMask & PARAMASK_GETTABLE))) {
			fprintf(stderr, VoqcntrStopHelp, pStr);
			return STATUS_ERR_PARA_STOPCMD;
		}
		pParaInfo->StartCmd = 0;
	}

	/* check Table Parameter */
	if (pParaInfo->GetParaMask & PARAMASK_GETTABLE) {
		unsigned char table = pParaInfo->GetTable;
		pParaInfo->GetTable = table + CS_IOCTL_TBL_VOQ_COUNTER_API;
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
		unsigned char command =  pParaInfo->GetCommand;
		if ((command + CMD_VOQ_COUNTER_SET) >= CMD_VOQ_COUNTER_MAX) {
			fprintf(stderr, VoqcntrCommandHelp, pStr);
			return STATUS_ERR_PARA_GETCOMMAND;
		}
		pParaInfo->GetCommand = command + CMD_VOQ_COUNTER_SET;	
	
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
* ROUTINE NAME - necfg_voq_counter_mod
*-----------------------------------------------------------------------
* DESCRIPTION:
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED
*----------------------------------------------------------------------*/
int necfg_voq_counter_mod(int argc, char **argv)
{
	int ret;

	/* Parser Paremeters in Parser.c*/
	if ((ret = ParserOpt(pParaInfo, argc, argv, voq_counter_TableDef,
			   voq_counter_CommandDef)) != STATUS_SUCCESS)
	{
		if ((ret == STATUS_ERR_PARA_GETFIELD)
		     || (ret == STATUS_ERR_PARA_GETVALUE))
			fprintf(stderr, FieldValueHelp, argv[0]);
		else
			fprintf(stderr, UsageMsg, VersionInfo, argv[0]);
		return ret;
	}

	/* Check Paremeters */
	if ((ret = voq_cntl_CheckParas(argv[0])) != STATUS_SUCCESS) {
		printf("[%s:%d]: Error %d\n", __FILE__, __LINE__, ret);
		return ret;
	}
	pParaInfo->Module = MODULE_VOQ_COUNTER;
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

