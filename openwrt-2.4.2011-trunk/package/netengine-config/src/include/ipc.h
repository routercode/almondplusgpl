/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ipc.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility IPC
                 header file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef _IPC_H_
#define _IPC_H_

#define IPC_SERVER_PORT 		5500
#define IPC_SERVER_IP   		"127.0.0.1"
#define LOCAL_HOST			0x0100007F

#pragma pack(1)
typedef struct _PARAMETERS_ {
	unsigned int GetParaMask;	// reference PARA_MASK_DEF
	unsigned char Module;		// reference MODULE_DEF
	unsigned char Status;		// reference STATUS
	unsigned char neSendCmdQuit;
	unsigned char Reserved;
	unsigned char StartCmd;		// 0: NONE, 1: Start command
	unsigned char StopCmd;		// 0: NONE, 1: Stop command
	unsigned char GetTable;		// reference xx_TABLE_DEF
	unsigned char GetCommand;	// reference COMMAND_DEF
	char GetField[64];
	char GetValue[32];
	int GetBypass;			// 0: Disable, 1: Enable
	unsigned short idx_start;	// defined for command GET
	unsigned short idx_end;		// defined for command GET
} PAREMETER_T;

#pragma pack()

#endif				/* _IPC_H_ */
