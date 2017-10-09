//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14

#ifndef _DP_TX_I2C_INTF_H
#define _DP_TX_I2C_INTF_H

#include "Customdef.h"
#include "compiler.h"

typedef unsigned char BYTE;
//typedef bit BIT;
typedef unsigned int WORD;

typedef unsigned char *pByte;
//#define DVI_TX_PORT0_ADDR	0x72  
//#define DVI_TX_PORT1_ADDR	0x7A  
//#define DP_TX_PORT0_ADDR   0x70
//#define DP_TX_PORT1_ADDR   0x74

typedef enum
{
  MXL_TRUE = 0,
  MXL_FALSE = 1,
} MXL_STATUS;

MXL_STATUS DP_TX_Write_Reg(BYTE I2cSlaveAddr, BYTE RegAddr, BYTE RegData);
MXL_STATUS DP_TX_Read_Reg(BYTE I2cSlaveAddr, BYTE RegAddr, BYTE *DataPtr);

#ifdef MCCS_SUPPORT
BYTE DP_TX_I2C_WriteMulti(BYTE dev_addr, BYTE offset, BYTE bCount,pByte pacBuffer);
BYTE DP_TX_I2C_ReadMulti(BYTE dev_addr, BYTE bCount,pByte pacBuffer);
#endif

#endif

