/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : parser.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <ne_defs.h>
#include <linux/cs_ne_ioctl.h>
#include "ne_cfg.h"
#include "helpMsg.h"
const char *AllOpts = "hsem:t:f:v:c:b:";

static inline int get_idx(PAREMETER_T * pPara, int argc, char **argv, char **TableDef,
	      char **CommandDef, int i)
{
	int j;
	for (j = 0; j < argc; j++) {
		if (strcmp(argv[j], CommandDef[i]) == 0) {
			if (j+2 >= argc)
				return STATUS_FAILURE;
			pPara-> idx_start = (unsigned short) strtoul(argv[j + 1], NULL, 0);
			pPara-> idx_end = (unsigned short) strtoul(argv[j + 2], NULL, 0);
			j = argc;
		}
	}
	return STATUS_SUCCESS;	
}

static inline int set_idx(PAREMETER_T * pPara, int argc, char **argv, char **TableDef,
	      char **CommandDef, int i)
{
	int j;
	for (j = 0; j < argc; j++) {
		if (strcmp(argv[j], CommandDef[i]) == 0) {
			if (j+1 >= argc)
				return STATUS_FAILURE;
			pPara-> idx_start = (unsigned short) strtoul(argv[j + 1], NULL, 0);
			j = argc;
		}
	}
	return STATUS_SUCCESS;	
}

int ParserOpt(PAREMETER_T * pPara, int argc, char **argv, char **TableDef,
	      char **CommandDef)
{
	int i, oc, ret = STATUS_SUCCESS;

	/* Don't print error message */
	opterr = 0;

	/* Parameter get */
	while ((oc = getopt(argc, argv, AllOpts)) != -1) {
		switch (oc) {
		case 's':
			pPara->GetParaMask |= PARAMASK_STARTCMD;
			pPara->StartCmd = 1;
			break;
		case 'e':
			pPara->GetParaMask |= PARAMASK_STOPCMD;
			pPara->StopCmd = 1;
			break;
		case 't':
			pPara->GetParaMask |= PARAMASK_GETTABLE;
			pPara->GetTable = 0xFF;
			for (i = 0; TableDef[i] != NULL; i++) {
				if (strcmp(optarg, TableDef[i]) == 0) {
					pPara->GetTable = i;
					break;
				}
			}
			break;
		case 'f':
			if (pPara->GetParaMask & PARAMASK_GETFIELD)
				return STATUS_ERR_PARA_GETFIELD;

			pPara->GetParaMask |= PARAMASK_GETFIELD;
			strcpy(pPara->GetField, optarg);
			break;
		case 'v':
			if (pPara->GetParaMask & PARAMASK_GETVALUE)
				return STATUS_ERR_PARA_GETVALUE;

			pPara->GetParaMask |= PARAMASK_GETVALUE;
			strcpy(pPara->GetValue, optarg);
			break;
		case 'c':
			pPara->GetParaMask |= PARAMASK_GETCOMMAND;
			pPara->GetCommand = 0xFF;
			for (i = 0; CommandDef[i] != NULL; i++) {
				if (strcmp(optarg, CommandDef[i]) == 0) {
					pPara->GetCommand = i;
					if (pPara->GetCommand == CMD_GET) {
						ret = get_idx(pPara, argc, argv, 
							TableDef, CommandDef,
							i);
						if (ret != STATUS_SUCCESS)
							return ret;
					} else if (pPara->GetCommand == CMD_ADD ||
						pPara->GetCommand == CMD_DELETE) {
						set_idx(pPara, argc, argv, 
							TableDef, CommandDef,
							i);
						/*idx is a optional parameter*/
					}
					break;
				}
			}
			break;
		case 'b':
			pPara->GetParaMask |= PARAMASK_GETBYPASS;
			pPara->GetBypass = strtoul(optarg, NULL, 10);
			break;
		case 'm':
			break;
		default:
			ret = STATUS_FAILURE;
		} /*end switch() */
	} /*end while() */
	return ret;
} /*end ParserOpt() */
