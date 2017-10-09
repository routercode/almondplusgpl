//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14

#ifndef _COMPILER_H
#define _COMPILER_H

#if 0
#include <stdio.h>
//#include <reg52.h>
#include <intrins.h>
#include <absacc.h>
#endif

#define P0_0  0x80
#define P0_1  0x81
#define P0_2  0x82
#define P0_3  0x83
#define P0_4  0x84
#define P0_5  0x85
#define P0_6  0x86
#define P0_7  0x87

#define P1_0  0x90
#define P1_1  0x91
#define P1_2  0x92
#define P1_3  0x93
#define P1_4  0x94
#define P1_5  0x95
#define P1_6  0x96
#define P1_7  0x97

#define P2_0  0xa0
#define P2_1  0xa1
#define P2_2  0xa2
#define P2_3  0xa3
#define P2_4  0xa4
#define P2_5  0xa5
#define P2_6  0xa6
#define P2_7  0xa7

#define P3_0  0xb0
#define P3_1  0xb1
#define P3_2  0xb2
#define P3_3  0xb3
#define P3_4  0xb4
#define P3_5  0xb5
#define P3_6  0xb6
#define P3_7  0xb7

#define ACC_0  0xe0
#define ACC_1  0xe1
#define ACC_2  0xe2
#define ACC_3  0xe3
#define ACC_4  0xe4
#define ACC_5  0xe5
#define ACC_6  0xe6
#define ACC_7  0xe7

#define _NOP_	_nop_()
#define PUTCHAR_TYPE	char

#endif
