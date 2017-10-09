//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14
#ifndef _UART_INT_H
#define _UART_INT_H

#include "I2C_intf.h"

#define CMD_READ 0x19
#define CMD_WRITE 0x2a
#define RET_ID	0x5b
#define ST_SUCCESS	0x00
#define ST_DEV_UNKNOWN	0xa0
#define ST_REG_UNKNOWN	0xa1
#define	ST_CMD_UNKNOWN	0xa6
#define FW_CMD 0x5c
#define Burn_CMD 0x98

/* for 14.7456MHz (29.4912MHz) crystal */
#define BAUD38400	0xff //0xfe //
#define BAUD19200	0xfe //0xfc //
#define BAUD9600	0xfc //0xf8 //

#define TERM_CMD_SIZE   16
extern char command_buf[512+1];
extern char command_buf_len;
extern BYTE term_cmd_valid;
extern char rx_count;
extern char tx_left;
extern BYTE cmd_valid;
extern BYTE ret_valid;
void init_serial(void);
void prot_isr(void);
void send_return(BYTE status, BYTE datah, 	BYTE datal);
void process_cmd(void);
void Burn(unsigned char c);
void verify();


#endif
