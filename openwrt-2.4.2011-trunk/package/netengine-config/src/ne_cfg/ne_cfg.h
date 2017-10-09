/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ne_cfg.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility header file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef _NE_CFG_H_
#define _NE_CFG_H_

#include <ipc.h>

extern char VersionInfo[];
extern PAREMETER_T *pParaInfo;
extern char *TableDef[];
extern char *FieldDef[];
extern char *CommandDef[];

extern int ParserOpt(PAREMETER_T * pPara, int argc, char **argv,
		     char **TableDef, char **CommandDef);
extern unsigned long SendCommand(char *pBuffer, unsigned int SendLen);

#endif /* _NE_CFG_H_ */
