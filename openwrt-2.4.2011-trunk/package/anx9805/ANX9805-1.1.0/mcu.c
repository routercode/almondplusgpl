// ---------------------------------------------------------------------------
// Analogix Confidential Strictly Private
//
// $RCSfile: mcu.c,v $
// $Revision: 1.1 $
// $Author: jhsu $
// $Date: 2011/10/06 07:38:24 $
//
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ---------------------------------------------------------------------------
// Copyright 2004-2007 (c) Analogix 
//
//Analogix owns the sole copyright to this software. Under international
// copyright laws you (1) may not make a copy of this software except for
// the purposes of maintaining a single archive copy, (2) may not derive
// works herefrom, (3) may not distribute this work to others. These rights
// are provided for information clarification, other restrictions of rights
// may apply as well.
//
// This is an unpublished work.
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> WARRANTEE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ---------------------------------------------------------------------------
// Analogix  MAKES NO WARRANTY OF ANY KIND WITH REGARD TO THE USE OF
// THIS SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
// PURPOSE.
// ---------------------------------------------------------------------------

//  DP_TX Demo Firmware on SST89V58RD2
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "uart_int.h"
#include "mcu.h"
#include "timer.h"
#include "mcu.h"
#include "I2C_intf.h"
#include "DP_TX_DRV.h"
#include "DP_TX_Reg.h"
//#include "P89v662.h"

#ifdef SDCC
xdata at 0xffff char program_done;
xdata at 0xfffe char stdout_port;
xdata at 0xfff0 char init_done;
#endif

#define MAX_BUF_CNT 6
BYTE Byte_Buf[MAX_BUF_CNT];

BYTE link_training_parameter_selected = 0;
BYTE video_interface_selected = 0;
BYTE debug_mode;
BYTE restart_system;
//BYTE VESA_timing_CMD_Set = 0;
//BYTE bakup_VESA_timing_CMD_Set = 0;

char argc, number;
char *argv[12];
char *string_bkp[3];

BYTE vesa_is_interlaced;
BYTE vesa_v_sync_polarity;
BYTE vesa_h_sync_polarity;
unsigned int vesa_pclk;
unsigned int vesa_h_total_length;
unsigned int vesa_h_active_length;
unsigned int vesa_h_front_porch;
unsigned int vesa_h_sync_width;
unsigned int vesa_h_back_porch;
unsigned int vesa_v_total_length;
unsigned int vesa_v_active_length;
unsigned int vesa_v_front_porch;
unsigned int vesa_v_sync_width;
unsigned int vesa_v_back_porch;

BYTE enable_debug_output;
BYTE bist_timing_set_cnt;
BYTE input_done = 0;

#ifdef MCCS_SUPPORT
BYTE bBuff[16];
#endif

char command_buf[512 + 1];
char command_buf_len;

#if 0
void InitMCU()
{
	number = 0;
	bist_timing_set_cnt = 0;
	init_serial();
	init_timer(TINTVAL_8MS);
	start_timer();
	EX1 = 0;
	EA = 1;						// enable all interrupt
	// ES2 = 1;
	IEN1 = 0x02;
	S2CON = 0x44;

	//Enable load of clock generator
	MC12429_LOAD = 0;

	P3_4 = 1;					//make these pins to input state.
	P0_3 = 1;					//make these pins to input state.
	P0_2 = 1;					//make these pins to input state.
	P0_1 = 1;					//make these pins to input state.
	P0_0 = 1;					//make these pins to input state.
	P1_7 = 1;					//make these pins to input state.

	DP_TX_Resetn_Pin = 0;
	delay_ms(2);
	DP_TX_Resetn_Pin = 1;
}

PUTCHAR_TYPE putchar(char c)
{
	if (!enable_debug_output)
		return c;
	EA = 0;
	if (c == '\n') {
		SBUF = 0x0d;
		while (!TI) ;
		TI = 0;
	}
	SBUF = c;
	while (!TI) ;
	TI = 0;
	EA = 1;
	return c;
}
#endif

void debug_puts(char *s)
{
	if (enable_debug_output)
		puts(s);
}

void debug_printf(char *s, ...)
{
	if (enable_debug_output) {
		va_list v;
		va_start(v, s);
		vprintf(s, v);
	}
}

void decode_term_str(void)
{
	char *s, *s1;
	char space;

	s = command_buf;
	s1 = s;
	argc = 0;
	if (*s) {
		space = 0;
		argv[argc++] = s;
		while (*s) {
			if (*s == ' ') {
				*s = 0;
				if (!space) {
					space = 1;
				}
			} else {
				if (space)
					argv[argc++] = s;
				space = 0;
			}
			s++;
		}
	}
}

int hex_to_int(char *s)
{
	int r;

	r = 0;
	while (*s) {
		if (*s >= '0' && *s <= '9')
			r = (r << 4) + *s - '0';
		if (*s >= 'a' && *s <= 'f')
			r = (r << 4) + *s - 'a' + 10;
		if (*s >= 'A' && *s <= 'F')
			r = (r << 4) + *s - 'A' + 10;
		s++;
	}
	return r;
}

long hex_to_long(char *s)
{
	long r;
	char c;

	r = 0;
	while (c = *s) {
		r = (r << 4) + c;
		if (c <= '9')
			r = r - '0';
		else
			r = r - 87;			// - 'a' + 10
		s++;
	}
	return r;
}

/*
int dec_to_int(char *s)
{
    int r;
    
    r = 0;
    while (*s) 
    {
        if (*s >= '0' && *s <= '9')
            r = r*10 + *s - '0';
        s++;
    }
    return r;
}
*/

void proc_term_cmd()
{
	BYTE c, c1;
	//WORD errl,errh;
	int j;
	int i;
	//BYTE ac;

	if (argc == 0)
		return;
	if (!strcmp(argv[0], "dumpedid")) {
		for (j = 0; j < 256; j++) {
			DP_TX_Read_Reg(0xa0, 0x00 + j, &c);
			debug_printf("edid[0x%.2x] = 0x%.2x\n", (WORD) (j + 0x00), (WORD) c);
		}
	}
	/*
	   else if(!strcmp(argv[0], "edidtest")) 
	   {
	   DP_TX_EDID_Read();
	   }
	 */
	//else if(!strcmp(argv[0], "burn")) 
	//burn_cmd_got = 1;
	//debug_Aaron
	else if (!strcmp(argv[0], "sys_stat")) {
		printf("dp_tx_system_state=%d\r\n", dp_tx_system_state);
	} else if (!strcmp(argv[0], "set_stat")) {
		dp_tx_system_state = hex_to_int(argv[1]);
		printf("dp_tx_system_state=%d\r\n", dp_tx_system_state);
	}

	else if (!strcmp(argv[0], "fwlton")) {
		USE_FW_LINK_TRAINING = 1;
		if (dp_tx_system_state > DP_TX_WAIT_HOTPLUG)
			DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	} else if (!strcmp(argv[0], "fwltoff")) {
		USE_FW_LINK_TRAINING = 0;
		if (dp_tx_system_state > DP_TX_WAIT_HOTPLUG)
			DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	} else if (!strcmp(argv[0], "rerd")) {
		for (i = 0; i < 1000; i++) {
			DP_TX_Read_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), &c1);
			debug_printf("[%s]=%.2x\n", argv[2], (unsigned int)c1);
		}
	}

	else if (!strcmp(argv[0], "rst8b_en")) {
		RST_ENCODER = 1;
		if (dp_tx_system_state > DP_TX_WAIT_HOTPLUG)
			DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	} else if (!strcmp(argv[0], "rst8b_dis")) {
		RST_ENCODER = 0;
		if (dp_tx_system_state > DP_TX_WAIT_HOTPLUG)
			DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	} else if (!strcmp(argv[0], "bist")) {
		//Force_Video_Resolution = 1;
		c = (BYTE) hex_to_int(argv[1]);
		if ((c > 5) | (c < 0)) {
			debug_puts("range is 0~5.\n");
			return;
		}
		bForceSelIndex = c;
		debug_printf("set index 0x%.2x\n", (WORD) bForceSelIndex);

		if (dp_tx_system_state > DP_TX_WAIT_HOTPLUG)
			DP_TX_Set_System_State(DP_TX_CONFIG_VIDEO);

	} else if (!strcmp(argv[0], "pwr")) {
		if (!strcmp(argv[1], "0")) {
			Byte_Buf[0] = 1;
			DP_TX_AUX_DPCDWrite_Bytes(0x000600, 0x01, Byte_Buf);
			delay_ms(1);
			Byte_Buf[0] = 1;
			DP_TX_AUX_DPCDWrite_Bytes(0x000600, 0x01, Byte_Buf);
			delay_ms(1);
			Byte_Buf[0] = 1;
			DP_TX_AUX_DPCDWrite_Bytes(0x000600, 0x01, Byte_Buf);
			delay_ms(1);
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_LANE0_SET_REG, 0x00);
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_LANE1_SET_REG, 0x00);
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_LANE2_SET_REG, 0x00);
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_LANE3_SET_REG, 0x00);
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_LINK_TRAINING_CTRL_REG,
							DP_TX_LINK_TRAINING_CTRL_EN);
			debug_puts("quit power saving");
		} else if (!strcmp(argv[1], "1")) {
			Byte_Buf[0] = 2;
			DP_TX_AUX_DPCDWrite_Bytes(0x000600, 0x01, Byte_Buf);
			debug_puts("entry power saving");
		}
	} else if (!strcmp(argv[0], "bwadjust")) {
		if (hex_to_int(argv[1]) == 0x00)
			bBW_Lane_Adjust = 0;
		if (hex_to_int(argv[1]) == 0x01)
			bBW_Lane_Adjust = 1;
		DP_TX_Set_System_State(DP_TX_CONFIG_VIDEO);

	} else if (!strcmp(argv[0], "sscon")) {
		DP_TX_CONFIG_SSC();

	} else if (!strcmp(argv[0], "show")) {
		DP_TX_Show_Infomation();
	}

	else if (!strcmp(argv[0], "ioar")) {
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_EXTRA_ADDR_REG, (hex_to_int(argv[1]) >> 1));
		DP_TX_Read_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), &c);
		debug_printf("I2C [0x%.2x] = 0x%.2x\n", (WORD) (hex_to_int(argv[2])), (WORD) c);
	} else if (!strcmp(argv[0], "ioaw")) {
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_EXTRA_ADDR_REG, (hex_to_int(argv[1]) >> 1));
		DP_TX_Write_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), hex_to_int(argv[3]));
		delay_ms(10);
		DP_TX_Read_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), &c);
		debug_printf(" Read back I2C[0x%.2x] = 0x%.2x\n", (WORD) (hex_to_int(argv[2])), (WORD) c);
	}

	else if (!strcmp(argv[0], "debugon"))
		debug_mode = 1;
	else if (!strcmp(argv[0], "debugoff"))
		debug_mode = 0;

	else if (!strcmp(argv[0], "dpcdr")) {
		DP_TX_AUX_DPCDRead_Bytes(hex_to_long(argv[1]), 1, Byte_Buf);
		//c = Byte_Buf[0];
		debug_printf("AUX:0x%2x\n", (WORD) Byte_Buf[0]);

	} else if (!strcmp(argv[0], "dpcdw")) {
		Byte_Buf[0] = (BYTE) hex_to_int(argv[2]);
		DP_TX_AUX_DPCDWrite_Bytes(hex_to_long(argv[1]), 1, Byte_Buf);
		DP_TX_AUX_DPCDRead_Bytes(hex_to_long(argv[1]), 1, Byte_Buf);
		//c = Byte_Buf[0];
		debug_printf("Read back:0x%2x\n", (WORD) Byte_Buf[0]);
	} else if (!strcmp(argv[0], "ddcr")) {
		DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_SLV_ADDR_REG, hex_to_int(argv[1]));
		DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_ACCNUM0_REG, 0x01);
		DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_ACCNUM1_REG, 0x00);
		//Clear FIFO
		DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_ACC_CMD_REG, 0x05);

		for (i = 0; i < hex_to_int(argv[3]); i++) {
			//c = HDMI_Read_EDID_BYTE(hex_to_int(argv[1])+i); 
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_SLV_OFFADDR_REG,
							(hex_to_int(argv[2]) + i));
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_ACC_CMD_REG, HDMI_DDC_ACC_DDC_READ);
			delay_ms(3);
			DP_TX_Read_Reg(HDMI_TX_PORT1_ADDR, HDMI_DDC_FIFO_ACC_REG, &c);
			debug_printf("edid[0x%.2x] = 0x%.2x\n", (WORD) (i + hex_to_int(argv[2])), (WORD) c);
			//debug_printf("edid:0x%2x\n", (WORD)c);
		}
	}
	/*else if(!strcmp(argv[0], "sscon"))
	   {
	   DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	   dp_tx_ssc_enable = 1;
	   }
	   else if (!strcmp(argv[0], "hdmi")){
	   mode_hdmi_or_dvi = 1;
	   DP_TX_Set_System_State(DP_TX_CONFIG_VIDEO);
	   }
	   else if (!strcmp(argv[0], "dvi")){
	   mode_hdmi_or_dvi = 0;
	   DP_TX_Set_System_State(DP_TX_CONFIG_VIDEO);
	   } */
#if 0
	else if (!strcmp(argv[0], "prbs")) {
		DP_TX_Power_On();
		//set CR pattern
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_PTN_SET_REG, 0x01);
		c = 0x01;
		while (c) {
			DP_TX_AUX_DPCDRead_Bytes(0x00, 0x02, 0x02, 1, Byte_Buf);
			c = Byte_Buf[0];
			if (c & 0x11)
				c = 0x00;
			else
				c = 0x01;
		}
		//set PRBS pattren
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_TRAINING_PTN_SET_REG, 0x0c);
		Byte_Buf[0] = 0x0c;
		DP_TX_AUX_DPCDWrite_Bytes(0x00, 0x01, 0x02, 1, Byte_Buf);
		debug_puts("PRBS");

	} else if (!strcmp(argv[0], "insert")) {
		debug_puts("Insert err");
		for (i = 0; i < hex_to_int(argv[1]); i++) {
			DP_TX_Insert_Err();
			delay_ms(1000);
		}
	}
	/*else if (!strcmp(argv[0], "errchk"))
	   {  
	   DP_TX_AUX_DPCDRead_Bytes(0x00, 0x01 ,0x02,1,ByteBuf);
	   c = ByteBuf[0];
	   DP_TX_AUX_DPCDWrite_Byte(0x00, 0x01 ,0x02, (c & 0x3f));
	   DP_TX_AUX_DPCDRead_Bytes(0x00, 0x01, 0x01,1,ByteBuf);   
	   c = ByteBuf[0];
	   c = c & 0x07;        
	   for(i = 0; i < c; i++)
	   {
	   j = i << 1;             
	   DP_TX_AUX_DPCDRead_Bytes(0x00, 0x02, (0x10 + j),2,ByteBuf);
	   errh = ByteBuf[1];
	   if(errh & 0x80)
	   {
	   errl = ByteBuf[0];                       
	   errh = (errh &0x7f) << 8;
	   errl = errh + errl;
	   debug_printf(" Err of Lane[%d] = %d\n",i, errl);
	   }              
	   }
	   } */

	else if (!strcmp(argv[0], "bpc")) {
		video_bpc = dec_to_int(argv[1]);
		DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
	}
#endif
#ifdef ERR_CHK_ENABLE
	else if (!strcmp(argv[0], "errchk")) {
		debug_puts("check symbol error rate...");

		DP_TX_AUX_DPCDRead_Bytes(0x000102, 1, ByteBuf);
		ByteBuf[0] &= 0x3f;		//select disparity error and illegal symbol error
		DP_TX_AUX_DPCDWrite_Bytes(0x000102, 1, ByteBuf);

		//Get lane count
		DP_TX_AUX_DPCDRead_Bytes(0x000101, 1, ByteBuf);
		c = ByteBuf[0];
		c = c & 0x07;
		for (i = 0; i < c; i++) {
			j = i << 1;
			DP_TX_AUX_DPCDRead_Bytes(0x000210 + j, 2, ByteBuf);
			errh = ByteBuf[1];
			if (errh & 0x80) {
				errl = ByteBuf[0];
				errh = (errh & 0x7f) << 8;
				errl = errh + errl;
				debug_printf(" Err of Lane[%d] = %d\n", i, errl);
			}
		}
	}
#endif
	else if (!strcmp(argv[0], "rd") == 1) {
		DP_TX_Read_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), &c1);
		debug_printf("[%s]=%.2x\n", argv[2], (unsigned int)c1);
	} else if (!strcmp(argv[0], "wr") == 1) {
		DP_TX_Write_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), hex_to_int(argv[3]));
		debug_printf("[%s]=%s\n", argv[2], argv[3]);
		DP_TX_Read_Reg(hex_to_int(argv[1]), hex_to_int(argv[2]), &c);
		debug_printf("read back: [%s]=%.2x\n", argv[2], (unsigned int)c);
	}

	else if (!strcmp(argv[0], "dump")) {
		debug_printf("      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F");
		for (i = 0; i <= 255; i++) {
			DP_TX_Read_Reg(hex_to_int(argv[1]), i, &c1);
			if ((i & 0x0f) == 0)
				//debug_printf("\n rp0: [%.2x]  %.2x  ", (unsigned int)i, (unsigned int)c1);
				debug_printf("\n[%.2x]  %.2x  ", i, (unsigned int)c1);
			else
				debug_printf("%.2x  ", (unsigned int)c1);
			if ((i & 0x0f) == 0x0f)
				debug_printf("\n-------------------------------------");
		}

		debug_printf("\n");
	}

	else if (!strcmp(argv[0], "dumpdpcd")) {
		debug_printf("              0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F");
		//DP_TX_Read_Reg(DP_TX_PORT0_ADDR, DP_TX_AUX_ADDR_19_16_REG, &c);
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_AUX_ADDR_19_16_REG, (hex_to_int(argv[1]) & 0x0f));
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_AUX_ADDR_15_8_REG, hex_to_int(argv[2]));
		for (i = 0; i <= 255; i++) {
			DP_TX_Read_Reg(DP_TX_PORT1_ADDR, i, &c1);
			if ((i & 0x0f) == 0)
				debug_printf("\n dpcd: [%.2x]  %.2x  ", (unsigned int)i, (unsigned int)c1);
			//debug_printf("\n dpcd%s: [%.2x]  %.2x  ", argv[2],(unsigned int)i, (unsigned int)c1);
			else
				debug_printf("%.2x  ", (unsigned int)c1);
			if ((i & 0x0f) == 0x0f)
				debug_printf("\n");
		}
		debug_printf("\n");
	}

	else if (!strcmp(argv[0], "videoin")) {
		c = hex_to_int(argv[1]);
		c1 = hex_to_int(argv[2]);

		if (c == VIP_CSC_RGB)
			debug_puts("input color space is RGB\n");
		else if (c == VIP_CSC_YCBCR422)
			debug_puts("input color space is YCBCR422\n");
		else if (c == VIP_CSC_YCBCR444)
			debug_puts("input color space is YCBCR444\n");
		else {
			debug_puts("invalid color space\n");
			return;
		}

		if (c1 == COLOR_6)
			debug_puts("input color depth is 6\n");
		else if (c1 == COLOR_8)
			debug_puts("input color depth is 8\n");
		else if (c1 == COLOR_10)
			debug_puts("input color depth is 10\n");
		else if (c1 == COLOR_12)
			debug_puts("input color depth is 12\n");
		else {
			debug_puts("invalid color depth\n");
			return;
		}

		DP_TX_InputSet(c, c1);

		DP_TX_Set_System_State(DP_TX_CONFIG_VIDEO);

	}
#if 0
	else if (!strcmp(argv[0], "map"))	//For YCBCR422 24 bit test
	{
		for (i = 0; i < 8; i++) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, 0x40 + i, 0x04 + i);
		}

		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, 0x48, 0x10);
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, 0x49, 0x11);
		for (i = 0; i < 6; i++) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, 0x4a + i, 0x18 + i);
		}

		for (i = 0; i < 8; i++) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, 0x50 + i, 0x22 + i);
		}
	}
#endif

#ifdef MCCS_SUPPORT
	else if (!strcmp(argv[0], "setbright"))	//set brightness
	{
		//Syntax:
		//start + 0x6E + 0x51 + 0x84 + 0x03 + 0x10 + argv[1] + argv[2] + Checksum +Stop
		//0x6E  Destination address
		//0x51  Source Address
		//0x84  bit7 = 1, control/status message
		//      bit7 = 0, Data stream message
		//      bit 0-6, Data length
		//0x03  Set VCP Feature command
		//0x10  VCP Op code--Brightness
		//argv[1]   high byte
		//argv[2]   low byte
		//Checksum =0x6E^0x51^0x84^0x03^0x10^argv[1]^argv[2]

		bBuff[0] = 0x84;
		bBuff[1] = 0x03;		//Primary op code, set vcp feature
		bBuff[2] = 0x10;		//VCP version

		bBuff[3] = (BYTE) (hex_to_int(argv[1]));	//brightness high byte
		bBuff[4] = (BYTE) (hex_to_int(argv[2]));	//brightness low byte

		bBuff[5] = 0x51 ^ 0x6e ^ bBuff[0] ^ bBuff[1] ^ bBuff[2] ^ bBuff[3] ^ bBuff[4];

		DP_TX_I2C_WriteMulti(0x6E, 0x51, 6, bBuff);

	} else if (!strcmp(argv[0], "setctr"))	//set contrast
	{
		//Syntax:
		//start + 0x6E + 0x51 + 0x84 + 0x03 + 0x12 + argv[1] + argv[2] + Checksum +Stop
		//0x6E  Destination address
		//0x51  Source Address
		//0x84  bit7 = 1, control/status message
		//      bit7 = 0, Data stream message
		//      bit 0-6, Data length
		//0x03  Set VCP Feature command
		//0x10  VCP Op code--Brightness
		//argv[1]   high byte
		//argv[2]   low byte
		//Checksum =0x6E^0x51^0x84^0x03^0x10^argv[1]^argv[2]

		bBuff[0] = 0x84;
		bBuff[1] = 0x03;		//Primary op code, set vcp feature
		bBuff[2] = 0x12;		//VCP version

		bBuff[3] = (BYTE) (hex_to_int(argv[1]));	//brightness high byte
		bBuff[4] = (BYTE) (hex_to_int(argv[2]));	//brightness low byte

		bBuff[5] = 0x51 ^ 0x6e ^ bBuff[0] ^ bBuff[1] ^ bBuff[2] ^ bBuff[3] ^ bBuff[4];

		DP_TX_I2C_WriteMulti(0x6E, 0x51, 6, bBuff);
	} else if (!strcmp(argv[0], "vcpver"))	//VCP Version
	{
		bBuff[0] = 0x82;
		bBuff[1] = 0x01;		//Primary op code, get vcp feature
		bBuff[2] = 0xdf;		//VCP version

		bBuff[3] = 0x51 ^ 0x6e ^ bBuff[0] ^ bBuff[1] ^ bBuff[2];

		DP_TX_I2C_WriteMulti(0x6E, 0x51, 4, bBuff);
		delay_ms(125);			// this delay is necessary
		DP_TX_I2C_ReadMulti(0x6f, 0x0b, bBuff);

		debug_printf("VCP version %d . %d\n", (WORD) bBuff[8], (WORD) bBuff[9]);
	} else if (!strcmp(argv[0], "getctr"))	//Get current contrast
	{

		bBuff[0] = 0x82;
		bBuff[1] = 0x01;		//Primary op code, get vcp feature
		bBuff[2] = 0x12;		//contrast

		bBuff[3] = 0x51 ^ 0x6e ^ bBuff[0] ^ bBuff[1] ^ bBuff[2];

		DP_TX_I2C_WriteMulti(0x6E, 0x51, 4, bBuff);
		delay_ms(125);			// this delay is necessary

		DP_TX_I2C_ReadMulti(0x6f, 0x0b, bBuff);

		debug_printf("Current contrast %.2x . %.2x\n", (WORD) bBuff[8], (WORD) bBuff[9]);
	} else if (!strcmp(argv[0], "getbrt"))	//Get current brightness
	{
		bBuff[0] = 0x82;
		bBuff[1] = 0x01;		//Primary op code, get vcp feature
		bBuff[2] = 0x10;		//brightness

		bBuff[3] = 0x51 ^ 0x6e ^ bBuff[0] ^ bBuff[1] ^ bBuff[2];

		DP_TX_I2C_WriteMulti(0x6E, 0x51, 4, bBuff);
		delay_ms(125);			// this delay is necessary

		DP_TX_I2C_ReadMulti(0x6f, 0x0b, bBuff);

		debug_printf("Current brightness %.2x . %.2x\n", (WORD) bBuff[8], (WORD) bBuff[9]);

	}
#ifdef HDMI_COLOR_DEBUG
	else if (!strcmp(argv[0], "cs")) {
		DP_TX_Video_Disable();
		DP_TX_Read_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL2_REG, &c);
		if (hex_to_int(argv[1]) == 0x00)
			c &= 0xfc;
		if (hex_to_int(argv[1]) == 0x01)
			c = (c & 0xfc) | 0x01;
		if (hex_to_int(argv[1]) == 0x02)
			c = (c & 0xfc) | 0x02;

		if (hex_to_int(argv[2]) == 0x00) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL2_REG, (c & 0x8f));
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL1_REG, 0x85);

			DP_TX_Read_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, &c);
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, c & 0x03);

			ANX9805_VIDEO_Mapping_8();
		}

		if (hex_to_int(argv[2]) == 0x01) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL2_REG, ((c & 0x9f) | 0x10));
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL1_REG, 0x85);

			DP_TX_Read_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, &c);
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, ((c & 0x03) | 0xd0));

			ANX9805_VIDEO_Mapping_8();
		}

		if (hex_to_int(argv[2]) == 0x02) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL2_REG, ((c & 0xaf) | 0x20));
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL1_REG, 0x85);

			DP_TX_Read_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, &c);
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, ((c & 0x03) | 0xd4));

			ANX9805_VIDEO_Mapping_10();
		}
		if (hex_to_int(argv[2]) == 0x03) {
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL2_REG, ((c & 0xbf) | 0x30));
			DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL1_REG, 0x85);

			DP_TX_Read_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, &c);
			DP_TX_Write_Reg(HDMI_TX_PORT1_ADDR, HDMI_GNRL_CTRL_PKT_REG, ((c & 0x03) | 0xd8));

			ANX9805_VIDEO_Mapping_12();
		}

		delay_ms(500);
		DP_TX_Show_Infomation();

	}
#endif

#endif

	else
		debug_printf("unknown cmd: %s\n", argv[0]);
}

void CommandProcess()
{
	printf("Input a command=");
	gets(command_buf);
	printf("\n");
	printf("command=%s\n", command_buf);
	enable_debug_output = 1;
	decode_term_str();
	proc_term_cmd();
	enable_debug_output = 1;
}

#if 0
void CommandProcess()
{
	if (cmd_valid) {
		cmd_valid = 0;
		process_cmd();
	}
	if (term_cmd_valid) {
		term_cmd_valid = 0;
		command_buf[command_buf_len] = 0;
		enable_debug_output = 1;
		decode_term_str();
		proc_term_cmd();
		enable_debug_output = 1;
	}
}

void VESA_Tming_Set(void)
{
	while (bist_timing_set_cnt) {
		input_done = 0;
		switch (bist_timing_set_cnt) {
#if 1
		case 15:
			debug_puts("Input video pixel clock:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 14:
			debug_puts("Input video type: 1-interlace, 0-progressive");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 13:
			debug_puts("Input Vsync Polarity: 1-low is active, 0-high is active");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 12:
			debug_puts("Input Hsync Polarity: 1-low is active, 0-high is active");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 11:
			debug_puts("Input H total resolution:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 10:
			debug_puts("Input H active resolution:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 9:
			debug_puts("Input H front porch:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 8:
			debug_puts("Input Hsync width:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 7:
			debug_puts("Input H back porch:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 6:
			debug_puts("Input V total resolution:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 5:
			debug_puts("Input V active resolution:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 4:
			debug_puts("Input V front porch:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 3:
			debug_puts("Input Vsync width:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 2:
			debug_puts("Input V back porch:");
			bist_timing_set_cnt--;
			while (!input_done)
				CommandProcess();
			break;
		case 1:
			bist_timing_set_cnt--;
			nbc12429_setting(vesa_pclk);
			DP_TX_Set_System_State(DP_TX_LINK_TRAINING);
			//DP_TX_VESA_Format_Resolution();
			//delay_ms(200);
			//DP_TX_Show_Infomation();
			//debug_puts("Type debugoff to go back to normal state");
			//debug_mode = 1;
			break;
#endif
		default:
			break;
		}
	}
}

void DP_TX_VESA_Format_Resolution(void)
{
	WORD dp_tx_bist_data;
	BYTE c;

	dp_tx_bist_data = vesa_is_interlaced;
	DP_TX_Read_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, &c);
	if (dp_tx_bist_data == 0) {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, (c & (~DP_TX_VID_CTRL10_I_SCAN)));
	} else {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, (c | DP_TX_VID_CTRL10_I_SCAN));
	}

	//Vsync Polarity set
	dp_tx_bist_data = vesa_v_sync_polarity;
	DP_TX_Read_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, &c);
	if (dp_tx_bist_data == 1) {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, (c | DP_TX_VID_CTRL10_VSYNC_POL));
	} else {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG,
						(c & (~DP_TX_VID_CTRL10_VSYNC_POL)));
	}

	//Hsync Polarity set
	dp_tx_bist_data = vesa_h_sync_polarity;
	DP_TX_Read_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, &c);
	if (dp_tx_bist_data == 1) {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG, (c | DP_TX_VID_CTRL10_HSYNC_POL));
	} else {
		DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VID_CTRL10_REG,
						(c & (~DP_TX_VID_CTRL10_HSYNC_POL)));
	}

	//H total length set
	dp_tx_bist_data = vesa_h_total_length;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_TOTAL_PIXELL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_TOTAL_PIXELH_REG, (dp_tx_bist_data >> 8));

	//H active length set
	dp_tx_bist_data = vesa_h_active_length;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_ACT_PIXELL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_ACT_PIXELH_REG, (dp_tx_bist_data >> 8));

	//H front porth width set
	dp_tx_bist_data = vesa_h_front_porch;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HF_PORCHL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HF_PORCHH_REG, (dp_tx_bist_data >> 8));

	//H sync width set
	dp_tx_bist_data = vesa_h_sync_width;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HSYNC_CFGL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HSYNC_CFGH_REG, (dp_tx_bist_data >> 8));

	//H back porth width set
	dp_tx_bist_data = vesa_h_back_porch;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HB_PORCHL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_HB_PORCHH_REG, (dp_tx_bist_data >> 8));

	//V total length set
	dp_tx_bist_data = vesa_v_total_length;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_TOTAL_LINEL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_TOTAL_LINEH_REG, (dp_tx_bist_data >> 8));

	//V active length set
	dp_tx_bist_data = vesa_v_active_length;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_ACT_LINEL_REG, dp_tx_bist_data);
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_ACT_LINEH_REG, (dp_tx_bist_data >> 8));

	//V front porth width set
	dp_tx_bist_data = vesa_v_front_porch;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VF_PORCH_REG, dp_tx_bist_data);

	//V sync width set
	dp_tx_bist_data = vesa_v_sync_width;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VSYNC_CFG_REG, dp_tx_bist_data);

	//V back porth width set
	dp_tx_bist_data = vesa_v_back_porch;
	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_TX_VB_PORCH_REG, dp_tx_bist_data);
}
#endif

#if 0
void nbc12429_setting(int frequency)
{
	int m_setting;
	//BYTE x,y;
	debug_printf("set pclk: %d\n", frequency);

	if ( /* frequency>=25 && */ frequency <= 50) {
		// N = 8
		MC12429_N0 = 1;
		MC12429_N1 = 1;

		m_setting = frequency << 3;

		if (mode_dp) {
			if ((dp_tx_lane_count != 0x01) && (DP_TX_Video_Input.bColordepth != COLOR_12)) {
				m_setting = frequency << 2;
			}
		}

		MC12429_M0 = (m_setting & 0x001);
		MC12429_M1 = (m_setting & 0x002);
		MC12429_M2 = (m_setting & 0x004);
		MC12429_M3 = (m_setting & 0x008);
		MC12429_M4 = (m_setting & 0x010);
		MC12429_M5 = (m_setting & 0x020);
		MC12429_M6 = (m_setting & 0x040);
		MC12429_M7 = (m_setting & 0x080);
		MC12429_M8 = (m_setting & 0x100);

	} else if (frequency > 50 && frequency <= 110) {
		// N = 4
		MC12429_N0 = 0;
		MC12429_N1 = 1;

		if (mode_dp) {
			if ((dp_tx_lane_count != 0x01) && (DP_TX_Video_Input.bColordepth != COLOR_12)) {
				MC12429_N0 = 1;
				MC12429_N1 = 1;
			}
		}
		m_setting = frequency << 2;

		MC12429_M0 = (m_setting & 0x001);
		MC12429_M1 = (m_setting & 0x002);
		MC12429_M2 = (m_setting & 0x004);
		MC12429_M3 = (m_setting & 0x008);
		MC12429_M4 = (m_setting & 0x010);
		MC12429_M5 = (m_setting & 0x020);
		MC12429_M6 = (m_setting & 0x040);
		MC12429_M7 = (m_setting & 0x080);
		MC12429_M8 = (m_setting & 0x100);

	} else if (frequency > 110 && frequency <= 200) {
		// N = 2
		MC12429_N0 = 1;
		MC12429_N1 = 0;

		if (mode_dp) {
			if ((dp_tx_lane_count != 0x01) && (DP_TX_Video_Input.bColordepth != COLOR_12)) {
				MC12429_N0 = 0;
				MC12429_N1 = 1;
			}
		}
		m_setting = frequency << 1;

		MC12429_M0 = (m_setting & 0x001);
		MC12429_M1 = (m_setting & 0x002);
		MC12429_M2 = (m_setting & 0x004);
		MC12429_M3 = (m_setting & 0x008);
		MC12429_M4 = (m_setting & 0x010);
		MC12429_M5 = (m_setting & 0x020);
		MC12429_M6 = (m_setting & 0x040);
		MC12429_M7 = (m_setting & 0x080);
		MC12429_M8 = (m_setting & 0x100);
	} else if (frequency > 200 && frequency <= 400) {
		// N = 1
		MC12429_N0 = 0;
		MC12429_N1 = 0;
		if (mode_dp) {
			if ((dp_tx_lane_count != 0x01) && (DP_TX_Video_Input.bColordepth != COLOR_12)) {
				MC12429_N0 = 1;
				MC12429_N1 = 0;
			}
		}
		m_setting = frequency;

		MC12429_M0 = (m_setting & 0x001);
		MC12429_M1 = (m_setting & 0x002);
		MC12429_M2 = (m_setting & 0x004);
		MC12429_M3 = (m_setting & 0x008);
		MC12429_M4 = (m_setting & 0x010);
		MC12429_M5 = (m_setting & 0x020);
		MC12429_M6 = (m_setting & 0x040);
		MC12429_M7 = (m_setting & 0x080);
		MC12429_M8 = (m_setting & 0x100);
	} else
		debug_puts("Wrong value given!");
}

#endif /* 0 */
