//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0 2006/07/14
#ifndef _MCU_H
#define _MCU_H

#include "compiler.h"

#define SBUF S0BUF

#define MC12429_M0 P2_0
#define MC12429_M1 P2_1
#define MC12429_M2 P2_2
#define MC12429_M3 P2_3
#define MC12429_M4 P2_4
#define MC12429_M5 P2_5
#define MC12429_M6 P2_6
#define MC12429_M7 P2_7
#define MC12429_M8 P0_4
#define MC12429_N0 P0_5
#define MC12429_N1 P0_6
#define MC12429_LOAD P0_7

//#define BIST_EN P1_4
#define BIST_EN 0				// Joe20110922

#define DP_TX_Resetn_Pin P3_2
#define DP_TX_Hotplug_pin P3_4
#define DP_TX_interrupt_pin ~P3_3
#define DP_TX_Dev_Sel 1

//#define SWITCH1 P0_3
#define SWITCH1 0	// Joe20111011
#define SWITCH2 P0_2
#define SWITCH3 P0_1
//#define SWITCH4 P0_0
#define SWITCH4 0	// Joe20110923

#define Force_Video_Resolution P0_0

//#define PRBS7 P0_0

//char putchar(char c);
void InitMCU(void);
void print_char1(BYTE c);
void print_char(BYTE c);
void print_byte(char *s, BYTE b);

extern BYTE enable_debug_output;
extern BYTE debug_mode;
extern BYTE restart_system;
extern char number;
extern BYTE video_interface_selected;
//extern BYTE VESA_timing_CMD_Set;
//extern BYTE bakup_VESA_timing_CMD_Set;
void debug_puts(char *s);
void debug_printf(char *s, ...);
void decode_term_str(void);
int hex_to_int(char *s);
long hex_to_long(char *s);
void proc_term_cmd();
void CommandProcess();
#if 0
void VESA_Tming_Set(void);
#endif

void DP_TX_VESA_Format_Resolution(void);
void nbc12429_setting(int frequency);

#endif
