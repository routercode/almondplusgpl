/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ne_defs.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility general
                 definitions file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     :

 *****************************************************************************/

#ifndef _NE_DEFS_H_
#define _NE_DEFS_H_

#define BIT_15   0x8000
#define BIT_14   0x4000
#define BIT_13   0x2000
#define BIT_12   0x1000
#define BIT_11   0x0800
#define BIT_10   0x0400
#define BIT_9    0x0200
#define BIT_8    0x0100
#define BIT_7    0x0080
#define BIT_6    0x0040
#define BIT_5    0x0020
#define BIT_4    0x0010
#define BIT_3    0x0008
#define BIT_2    0x0004
#define BIT_1    0x0002
#define BIT_0    0x0001

typedef enum {
	STATUS_SUCCESS,
	STATUS_FAILURE,
	STATUS_ERR_PARA_STARTCMD,
	STATUS_ERR_PARA_STOPCMD,
	STATUS_ERR_PARA_GETTABLE,
	STATUS_ERR_PARA_GETCOMMAND,
	STATUS_ERR_PARA_GETFIELD,
	STATUS_ERR_PARA_GETVALUE,
	STATUS_ERR_PARA_GETBYPASS,
	STATUS_ERR_PARA,

	STATUS_ERR_SOCKET,
	STATUS_ERR_BIND,

	STATUS_ERR_OPEN_DEVICE,
	STATUS_ERR_IOCTL,
	STATUS_ERR_MAC_FORMAT,
	STATUS_ERR_IP_FORMAT,
	STATUS_ERR_FORMAT,

	STATUS_NONE = 0xFF,
} STATUS;

typedef enum {
	PARAMASK_STARTCMD = BIT_0,
	PARAMASK_STOPCMD = BIT_1,
	PARAMASK_GETTABLE = BIT_2,
	PARAMASK_GETCOMMAND = BIT_3,
	PARAMASK_GETFIELD = BIT_4,
	PARAMASK_GETVALUE = BIT_5,
	PARAMASK_GETBYPASS = BIT_6,
} PARA_MASK_DEF;

/*[begin][ingress qos]add by ethan for ingress qos*/
typedef enum {
	QOSMASK_DIP = BIT_0,
	QOSMASK_SIP = BIT_1,
	QOSMASK_DSCP = BIT_2,
	QOSMASK_8021P = BIT_3,
	QOSMASK_VID = BIT_4,
	QOSMASK_LSPID = BIT_5,
	QOSMASK_DELETE = BIT_6,
} INGRESS_QOS_MASK_DEF;
/*[end][ingress qos]add by ethan for ingress qos*/
#endif	/* _NE_DEFS_H_ */
