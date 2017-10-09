//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14
#ifndef _TIMER_H
#define _TIMER_H

#include "compiler.h"
#include "I2C_intf.h"

/* for 29MHz (//14.7456MHz) crystal */
#define TINTVAL_8MS	0xb3 //0xda
#define TINTVAL_4MS	0xda //0xed
#define TINTVAL_2MS	0xed //0xf6
#define TINTVAL_1MS	0xf6 //0xfb

#define UNIT_MS	0x4cc

#define start_timer() do { TR0 = 1; } while (0)
#define stop_timer() do { TR0 = 0; } while (0)

// for 29MHz (//14.7456MHz) crystal, Atmel AT89C51xx series
#define DELAY_FACTOR    444  //222

extern char timer_done;
extern WORD timer_tick;
extern BYTE hh, mm, ss;

void init_timer(BYTE intval);
void timer_isr(void);
void delay_ms(int n);
void timer2_isr(void);
unsigned long get_timer_tick(void);
void show_cur_time(void);
void delay_us(unsigned int c);


#endif
