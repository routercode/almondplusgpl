//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 0.1 2007/11/15

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
#include "Customdef.h"

void main()
{
	BYTE fw_restart;
	BYTE c;

  FW_Start:

	fw_restart = 0;
	enable_debug_output = 1;
	debug_mode = 0;

	//InitMCU();    

	debug_printf("DP_HDMI TX FW Version is %.2f, ", DP_HDMI_TX_FW_VER);
	debug_printf("Build at: %s,%s\n", __DATE__, __TIME__);

	if (DP_TX_Chip_Located())
		debug_puts("Chip found");
	else {
		debug_puts("chip not found");
		while (1)
			CommandProcess();
	}

	/*if(!BIST_EN)
	   {
	   debug_puts("waiting Rx...");
	   delay_ms(2000);//wait for HDMI RX stable      
	   } */

	DP_TX_Initialization();
	DP_TX_InputSet(VIP_CSC_RGB, COLOR_8);

	DP_TX_Write_Reg(HDMI_TX_PORT0_ADDR, DP_COMMON_INT_MASK3, 0xaa);

	if (!SWITCH2)
		dp_tx_ssc_enable = 1;
	else
		dp_tx_ssc_enable = 0;

	if (!SWITCH3)
		dp_tx_hdcp_enable = 1;
	else
		dp_tx_hdcp_enable = 0;

	//use HW link training default
	USE_FW_LINK_TRAINING = 0;

	//disable auto reset 8b/10b encoder before sending link training patter 2
	RST_ENCODER = 1;

	bForceSelIndex = 0;

	//when set to 1, will adjust BW and lane count before link training
	bBW_Lane_Adjust = 0;

	if (!BIST_EN) {
		//debug_puts("PRBS pattern!");
		while (SWITCH4) {
			debug_printf("<%s:%d> SWITCH4=%d\n", __func__, __LINE__, (WORD)SWITCH4);
			DP_TX_PBBS7_Test();
			while (SWITCH3) {
				DP_TX_Insert_Err();
				CommandProcess();
				delay_ms(1000);
			}
			CommandProcess();
		}
	}
	debug_printf("%s: debug_mode=%d\r\n", __func__, (WORD)debug_mode); 
	while (1) {

		if (!debug_mode) {
			DP_TX_Task();
			DP_TX_Read_Reg(HDMI_TX_PORT0_ADDR, DP_COMMON_INT_MASK3, &c);
			if (c != 0xaa)
				goto FW_Start;
		}
		//VESA_Tming_Set();

		CommandProcess();

		//sleep(5);
	}
}

#if 0
void serial_isr(void) interrupt 4
{
	prot_isr();
	_NOP_;
}

void timer1_isr(void) interrupt 1
{
	timer_isr();
	_NOP_;
}
#endif
