/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ne_cfg.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>
#include "ne_cfg.h"
#include "helpMsg.h"
#include "ni/ni.h"
#include "fe/fe.h"
#include "tm/tm.h"
#include "sch/sch.h"
#include "qm/qm.h"
#include "qos/ingress_qos.h" /*[ingress qos]add by ethan for ingress qos*/
#include "voqcntr/voq_counter.h"
#include "tunnel/tunnel.h"

#define SVERSION            "1.0"
char VersionInfo[] =
    "Cortina-Systems Inc. GoldenGate Configuration Utility " SVERSION " ["
    __DATE__ " " __TIME__ "]";
char *ModuleDef[] = { "ni", "fe", "tm", "sch", "qm", "inqos", "voqcntr", "tunnel", 0 };

PAREMETER_T Para;
PAREMETER_T *pParaInfo = &Para;

/*
 *====================================================================== 
 * 
 *                        Main Function Here 
 *
 *====================================================================== 
 */
int main(int argc, char **argv)
{
	int i, j;
	unsigned char GetModule = 0xFF;
	if (argc < 2) {
		fprintf(stderr, UsageMsg, VersionInfo, argv[0]);
		return 1;
	}
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-m") == 0) {
			for (j = 0; ModuleDef[j] != NULL; j++) {
				if (strcmp(argv[i + 1], ModuleDef[j]) == 0) {
					GetModule = j;
					break;
				}
			}
		}
	}
	if (GetModule == 0xFF) {
		fprintf(stderr, UsageMsg, VersionInfo, argv[0]);
		return 1;
	}

	/* Parser parameter by each module */
	bzero(pParaInfo, sizeof(PAREMETER_T));
	switch (GetModule) {
	case MODULE_NI:
		necfg_ni_mod(argc, argv);
		break;
	case MODULE_FE:
		necfg_fe_mod(argc, argv);
		break;
	case MODULE_TM:
		necfg_tm_mod(argc, argv);
		break;
	case MODULE_SCH:
		necfg_sch_mod(argc, argv);
		break;
	case MODULE_QM:
		necfg_qm_mod(argc, argv);
		break;
	case MODULE_Ingress_QOS:
		necfg_ingress_qos_mod(argc, argv);
		break;
	case MODULE_VOQ_COUNTER:
		necfg_voq_counter_mod(argc, argv);
		break;
        case MODULE_TUNNEL:
                necfg_tunnel_mod(argc, argv);
                break;
	} /*end switch(GetModule) */
	return 0;
} /*end main() */
