/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : misc.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility misc function 
                 header file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef _MISC__H_
#define _MISC__H_

int CheckParas(char *pString);
int ParseMAC(char macStr[], unsigned int macVal[]);
int ParseIP(char *pbuf, unsigned char *pip);
int ParseIPv6(char *pbuf, unsigned int *pip);
#endif /* _MISC__H_ */
