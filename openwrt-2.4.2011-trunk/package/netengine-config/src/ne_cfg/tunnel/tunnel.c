/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : iplip.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility ni module
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/ 
    
    
#include <stdio.h>
    
#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>
#include "../ne_cfg.h"
#include "../helpMsg.h"

char *tunnel_TableDef[] = {
        "iplip_api",
        0
};

char *tunnel_CommandDef[] = {"set", 0};

int tunnel_CheckParas(char *pStr)
{

        /* Check Start Command Parameter */
        if (pParaInfo->GetParaMask & PARAMASK_STARTCMD) {
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
		unsigned char table = pParaInfo->GetTable;
		pParaInfo->GetTable = table + CS_IOCTL_TBL_TUNNEL_IPLIP_API;
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
* ROUTINE NAME - necfg_tunnel_mod
*-----------------------------------------------------------------------
* DESCRIPTION:
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED
*----------------------------------------------------------------------*/ 
int necfg_tunnel_mod(int argc, char **argv) 
{
	int ret;

        /* Parser Paremeters in Parser.c*/
        if ((ret = ParserOpt(pParaInfo, argc, argv, tunnel_TableDef,
                           tunnel_CommandDef)) != STATUS_SUCCESS)
        {
                if ((ret == STATUS_ERR_PARA_GETFIELD)
                     || (ret == STATUS_ERR_PARA_GETVALUE))
                        fprintf(stderr, FieldValueHelp, argv[0]);
                else
                        fprintf(stderr, UsageMsg, VersionInfo, argv[0]);
                return ret;
        }

        /* Check Paremeters */
        if ((ret = tunnel_CheckParas(argv[0])) != STATUS_SUCCESS) {
                printf("[%s:%d]: Error %d\n", __FILE__, __LINE__, ret);
                return ret;
        }
        pParaInfo->Module = MODULE_TUNNEL;
        pParaInfo->Status = STATUS_NONE;

        if ((ret = SendCommand((char *)pParaInfo, sizeof(PAREMETER_T))) == 0)
                ret = pParaInfo->Status;

        if (ret != STATUS_SUCCESS)
        {
                printf("[%s:%d]: Status %d, pParaInfo->Status %d\n", __FILE__,
                        __LINE__, ret, pParaInfo->Status);
        }
        return ret;

} /*end necfg_tunnel__mod() */
