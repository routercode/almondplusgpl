/******************************************************************************
	 Copyright (c) 2012, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : voqCntrApi.c
   Date        : 2012-08-09
   Description : Cortina GoldenGate NetEngine configuration utility process
	             VOQ Counter API Table
   Author      : Wen Hsu <whsu@cortina-systems.com>
   Remarks     :

 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ne_defs.h>
#include "../neSendCmd.h"
#include "../misc.h"

VOQ_COUNTER_SUB_CMD_T voqcntr_subcmd_def[] = {
	{"add",			CS_VOQ_COUNTER_ADD},
	{"delete",		CS_VOQ_COUNTER_DELETE},
	{"get",			CS_VOQ_COUNTER_GET},
	{"print",		CS_VOQ_COUNTER_PRINT},
	{"set_read_mode",	CS_VOQ_COUNTER_SET_READ_MODE},
	{"get_read_mode",	CS_VOQ_COUNTER_GET_READ_MODE},
	{"print_read_mode",	CS_VOQ_COUNTER_PRINT_READ_MODE},
	{NULL,		0}
};

int voqcntr_api_subcmd(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *voqcntr_api_tbl_info)
{
	int i;

	voqcntr_api_tbl_info->api.sub_cmd = CS_VOQ_COUNTER_MAX;
	for (i = 0; voqcntr_subcmd_def[i].name != NULL; i++) {
		if (strcmp(pString, voqcntr_subcmd_def[i].name) == 0) {
			voqcntr_api_tbl_info->api.sub_cmd =
			    voqcntr_subcmd_def[i].id;
			break;
		}
	}
	if (voqcntr_api_tbl_info->api.sub_cmd >= CS_VOQ_COUNTER_MAX)
		return STATUS_ERR_PARA_GETVALUE;
	return STATUS_SUCCESS;
} /* voqcntr_api_subcmd */

int voqcntr_api_param_voq_id(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.voq_id =
		(unsigned char)strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_voq_id */

int voqcntr_api_param_pkts(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.pkts =
		(unsigned int)strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_pkts */

int voqcntr_api_param_pkts_mark(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.pkts_mark =
		(unsigned int)strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_pkts_mark */

int voqcntr_api_param_pkts_drop(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.pkts_drop =
		(unsigned int)strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_pkts_drop */

int voqcntr_api_param_bytes(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.bytes =
		(unsigned long long)strtoull(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_bytes */

int voqcntr_api_param_bytes_mark(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.bytes_mark =
		(unsigned long long)strtoull(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_bytes_mark */

int voqcntr_api_param_bytes_drop(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *VOQ_COUNTER_API_TABLE_INFO)
{
	int retStatus;

	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	VOQ_COUNTER_API_TABLE_INFO->api.param.bytes_drop =
		(unsigned long long)strtoull(pString, NULL, 0);
	return STATUS_SUCCESS;
} /* voqcntr_api_param_bytes_drop */

VOQ_COUNTER_SUB_CMD_T voqcntr_read_mode_def[] = {
	{"no_clear",	CS_VOQ_COUNTER_READ_MODE_NO_CLEAR},
	{"clear_all",	CS_VOQ_COUNTER_READ_MODE_CLEAR_ALL},
	{"clear_msb",	CS_VOQ_COUNTER_READ_MODE_CLEAR_MSB},
	{NULL,		CS_VOQ_COUNTER_READ_MODE_NO_CLEAR}
};

int voqcntr_api_read_mode(char *pString,
		VOQ_COUNTER_API_TABLE_INFO *voqcntr_api_tbl_info)
{
	int i;

	voqcntr_api_tbl_info->api.sub_cmd = CS_VOQ_COUNTER_MAX;
	for (i = 0; voqcntr_read_mode_def[i].name != NULL; i++) {
		if (strcmp(pString, voqcntr_read_mode_def[i].name) == 0) {
			voqcntr_api_tbl_info->api.sub_cmd =
			    voqcntr_read_mode_def[i].id;
			break;
		}
	}
	if (voqcntr_api_tbl_info->api.read_mode >= CS_VOQ_COUNTER_READ_MODE_MAX)
		return STATUS_ERR_PARA_GETVALUE;
	return STATUS_SUCCESS;
} /* voqcntr_api_subcmd */

NE_FIELD_T voqcntr_api[] = {
	{"api",				voqcntr_api_subcmd},

	{"param.voq_id",		voqcntr_api_param_voq_id},

	{"param.pkts",			voqcntr_api_param_pkts},
	{"param.pkts_mark",		voqcntr_api_param_pkts_mark},
	{"param.pkts_drop",		voqcntr_api_param_pkts_drop},

	{"param.bytes",			voqcntr_api_param_bytes},
	{"param.bytes_mark",		voqcntr_api_param_bytes_mark},
	{"param.bytes_drop",		voqcntr_api_param_bytes_drop},
	{"read_mode",			voqcntr_api_read_mode},
	{0,				0}
};

