/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : misc.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility misc function
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <ne_defs.h>
int CheckParas(char *pString)
{
	int i, strLen;
	if (pString == NULL)
		return STATUS_ERR_FORMAT;

	strLen = strlen(pString);
	if (strncmp(pString, "0x", 2) == 0) {
		for (i = 2; i < strLen; i++) {
			if (!(isxdigit(*(pString + i))))
				return STATUS_ERR_FORMAT;
		}
	} else {
		for (i = 0; i < strLen; i++) {
			if (!(isdigit(*(pString + i))))
				return STATUS_ERR_FORMAT;
		}
	}
	return STATUS_SUCCESS;
} /*end CheckParas() */
int ParseMAC(char macStr[], unsigned int macVal[])
{
	int ret = 0;
	if (macStr == NULL)
		return STATUS_ERR_MAC_FORMAT;

	if (strlen(macStr) == 17) {
		if (macStr[2] != ':' || macStr[5] != ':' || macStr[8] != ':'
		    || macStr[11] != ':' || macStr[14] != ':')
			return STATUS_ERR_MAC_FORMAT;

		ret = sscanf(macStr, "%2x:%2x:%2x:%2x:%2x:%2x",
			   (unsigned int *)&macVal[0],
			   (unsigned int *)&macVal[1],
			   (unsigned int *)&macVal[2],
			   (unsigned int *)&macVal[3],
			   (unsigned int *)&macVal[4],
			   (unsigned int *)&macVal[5]);
		if (ret != 6)
			return STATUS_ERR_MAC_FORMAT;

		return STATUS_SUCCESS;
	} else if (strlen(macStr) == 12) {
		ret = sscanf(macStr, "%2x%2x%2x%2x%2x%2x",
			   (unsigned int *)&macVal[0],
			   (unsigned int *)&macVal[1],
			   (unsigned int *)&macVal[2],
			   (unsigned int *)&macVal[3],
			   (unsigned int *)&macVal[4],
			   (unsigned int *)&macVal[5]);
		if (ret != 6)
			return STATUS_ERR_MAC_FORMAT;

		return STATUS_SUCCESS;
	} else {
		return STATUS_ERR_MAC_FORMAT;
	}
} /* end ParseMAC() */
int ParseIP(char ipStr[], unsigned char ipVal[])
{
	char *delim = ".";
	char *pStr;
	int i, idx = 0;
	pStr = strtok(ipStr, delim);
	while (pStr != NULL) {
		for (i = 0; i < strlen(pStr); i++) {
			if (!(isdigit(*(pStr + i))))
				return STATUS_ERR_FORMAT;
		}
		ipVal[idx] = (unsigned char)strtoul(pStr, NULL, 10);
		idx++;
		pStr = strtok(NULL, delim);
	} 
	
	return STATUS_SUCCESS;
} /*end ParseIP() */

int ParseIPv6(char ipStr[], unsigned int ipVal[])
{
        char *delim = ":";
        char *pStr;
        int i, idx = 0;
        pStr = strtok(ipStr, delim);
        while (pStr != NULL) {
                for (i = 0; i < strlen(pStr); i++) {
                        if (!(isxdigit(*(pStr + i))))
                                return STATUS_ERR_FORMAT;
                }
                ipVal[idx] = (unsigned int)strtoul(pStr, NULL, 16);
                idx++;
                pStr = strtok(NULL, delim);
        }
	

        return STATUS_SUCCESS;
} /*end ParseIP() */


