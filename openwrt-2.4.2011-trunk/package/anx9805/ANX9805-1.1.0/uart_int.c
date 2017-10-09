//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14

#include <stdio.h>
#include "compiler.h"
#include "uart_int.h"
#include "mcu.h"
#include "I2C_intf.h"
#include "P89v662.h"
#include "DP_TX_DRV.h"
#include "DP_TX_Reg.h"


char command_buf[16];
char command_buf_len;
BYTE term_cmd_valid;
BYTE fetching_term_cmd;
BYTE uart_buf[7];
char rx_count;
char tx_left;
BYTE cmd_valid;
BYTE ret_valid;
BYTE fw_cmd_got;
BYTE burn_cmd_got;
unsigned char block_cnt;
unsigned char length_of_lastblock;
unsigned data_cnt;
unsigned char offset_tmp;
unsigned char current_block;
unsigned char cmd_type;
unsigned char cmd_mode;
unsigned char block;
unsigned char from_block;

void init_serial(void)
{
    RCAP2H = 0xff;
    RCAP2L = 0xe8;//for 14.7456mhz
   //RCAP2L = 0xd0;//for 29.4mhz

    TCLK = 1;
    RCLK = 1;
    CT2 = 0;
    TR2 = 1;
    EXEN2 = 0;
    CP = 0;
    
    S0CON = 0x50;
    TI = 1;
    RI = 0;
    ES0 = 1;

    rx_count = 0;
    tx_left = 0;
    cmd_valid = 0;
    ret_valid = 0;
    term_cmd_valid = 0;
    fetching_term_cmd = 0;

	fw_cmd_got = 0;
    burn_cmd_got = 0;
    block_cnt = 0;
    length_of_lastblock = 0;
    data_cnt = 0;
    offset_tmp = 0;
    current_block = 0;
    cmd_type = 0;
    cmd_mode = 0;
    block = 0;
    from_block = 0;
}

void prot_isr(void)
{
    BYTE c;

    if (TI) 
    {
        TI = 0;	
        if (ret_valid)
        if (tx_left)
        S0BUF = uart_buf[--tx_left];
        else
        ret_valid = 0;

    }
    if (RI) 
    {
    	RI = 0;
		uart_buf[data_cnt] = S0BUF;
		
		if(uart_buf[0] == FW_CMD)
		{
			fw_cmd_got = 1;
			burn_cmd_got = 0;
		}
		else if(uart_buf[0] == Burn_CMD)
		{
			fw_cmd_got = 0;
			burn_cmd_got = 1;
		}
		else
		{
			data_cnt = 0;
		}

		if(burn_cmd_got)
		{
			data_cnt++;
			Burn(uart_buf[data_cnt - 1]);
		}
		
		if(fw_cmd_got)
		{
	        enable_debug_output = 0;
	        c = S0BUF;
	        if (fetching_term_cmd) 
	        {
	            if (c == '\r' || c == '\n') 
	            {
	                term_cmd_valid = 1;
	                fetching_term_cmd = 0;
	            } 
	            else if (c == 0x08) 
	                command_buf_len--;
	            else if (command_buf_len < TERM_CMD_SIZE-1) 
	            {
	                command_buf[command_buf_len++] = c;
	            } 
	            else
	                c = '*';
	            enable_debug_output = 1;
	            if (c == '\r')
	                putchar('\n');
	            else
	                putchar(c);
	            enable_debug_output = 0;
	        } 
	        else 
	        {
	            if ((rx_count != 0) || (c == CMD_READ) || (c == CMD_WRITE))
	                uart_buf[rx_count++] = c;
	            else if(c == '\\') 
	            {
	                fetching_term_cmd = 1;
	                command_buf_len = 0;
	                enable_debug_output = 1;
	                putchar(c);
	                enable_debug_output = 0;
	            }

	            if (rx_count == 7) 
	            {
	                cmd_valid = 1;
	                rx_count = 0;
	            }
	        }
    	}
    }
}

void send_return(BYTE status, BYTE datah, BYTE datal)
{
    uart_buf[2] = status;
    uart_buf[1] = datah;
    uart_buf[0] = datal;
    tx_left = 3;
    S0BUF = RET_ID;
    ret_valid = 1;
}

BYTE clock0_reg, clock1_reg;
WORD clock;
void process_cmd(void)
{
    BYTE c,uc;
    //WORD clk;

    uc = 0;
    if (uart_buf[0] == CMD_WRITE)
    {
        if(uart_buf[2] == 0xff)
        {
            c = 0;
            uc = uart_buf[6];
            if(uart_buf[4] == 0x00)
                clock0_reg = uc;
            else if(uart_buf[4] == 0x01)
            {
                clock1_reg = uc;
                clock = clock1_reg;
                clock = clock << 8;
                clock = clock | clock0_reg;
                nbc12429_setting(clock/2);
            }
	    else if(uart_buf[4] == 0x02)
		debug_mode = 1;
        }
        else
            c = DP_TX_Write_Reg(uart_buf[2], uart_buf[4], uart_buf[6]);
    }
    else if (uart_buf[0] == CMD_READ) 
    {
        if(uart_buf[2] == 0xff)
        {
            c = 0;
            if(uart_buf[4] == 0x00)
                uc = clock0_reg;
            else if(uart_buf[4] == 0x01)
                uc = clock1_reg;
        }
        else
            c = DP_TX_Read_Reg(uart_buf[2] , uart_buf[4], &uc);
    }
    
    switch (c) 
    {
        case 1: 
            c = ST_DEV_UNKNOWN;
            break;
        case 2:
            c = ST_REG_UNKNOWN;
            break;
        case 3: 
            c = ST_DEV_UNKNOWN;
            break;
        case 4:
            c = ST_REG_UNKNOWN;
            break;            
    }
    send_return(c, 0x00, uc);
}

void Burn(unsigned char c)
{
	if((data_cnt - 1) == 0)
		return;

	EA = 0;
	
	if((data_cnt - 1) == 6)
	{
		data_cnt = 6;
		
		if(cmd_type == 0x55)	//write cmd
		{
			debug_mode = 1;
			if(cmd_mode == 0x01)		//burn eeprom mode
			{
				//i2c_write_ee_reg(current_block, offset_tmp, c);
				//DP_TX_Write_Reg( current_block, offset_tmp, c); //Add by ljx 8-18				
				DP_TX_Write_Reg(0xa0|(current_block << 1), offset_tmp, c);//Add by ljx 8-18
				
				//send respone
				EA = 0;
				S0BUF = 0xff;
				while(!TI);
				TI = 0;
				EA = 1;
			}
			else//burn ocm mode
			{
				;
			}
			if(block_cnt == 0)
			{
				if(offset_tmp == length_of_lastblock)
				{
//					debug_puts("a:Finish!");
					data_cnt = 0;
					length_of_lastblock = 0;
					block_cnt = 0;
					offset_tmp = 0;
					current_block = 0;
					cmd_mode = 0;
					cmd_type = 0;
					burn_cmd_got = 0;
					from_block = 0;
					debug_mode = 0;
				}
				else
					offset_tmp ++;
			}
			else
			{
				if((current_block == (block_cnt + from_block)) && (offset_tmp == length_of_lastblock))
				{
//					debug_puts("b:Finish!");
					data_cnt = 0;
					length_of_lastblock = 0;
					block_cnt = 0;
					offset_tmp = 0;
					current_block = 0;
					cmd_mode = 0;
					cmd_type = 0;
					burn_cmd_got = 0;
					from_block = 0;
					debug_mode = 0;
				}
				else
				{
					offset_tmp ++;
					if(offset_tmp == 0)
					{
						current_block ++;
						DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_EXTRA_ADDR_REG, ((0xa0|(current_block << 1)) >> 1));
					}
				}
			}
		}
	}

	else if((data_cnt - 1) == 5)
	{
		length_of_lastblock = c;
//		if(S2_1)
//		debug_printf("length_of_lastblock: %.2x \n",(unsigned int)length_of_lastblock);
		if(cmd_type == 0xaa)
			verify();
	}
	else if((data_cnt - 1) == 4) 
	{
		from_block = c;
		current_block = from_block;
//		if(S2_1)
//		debug_printf("from_block: %.2x \n",(unsigned int)from_block);
	}
	
	else if((data_cnt - 1) == 3) 
	{
		block_cnt = c;
//		if(S2_1)
//		debug_printf("block_cnt: %.2x \n",(unsigned int)block_cnt);
	}
	else if((data_cnt - 1) == 2) 
	{
		cmd_type = c;
//		if(S2_1)
//		debug_printf("cmd_type: %.2x \n",(unsigned int)cmd_type);
	}
	else if((data_cnt - 1) == 1) 
	{
		cmd_mode = c;
//		if(S2_1)
//		debug_printf("cmd_mode: %.2x \n",(unsigned int)cmd_mode);
		/*if(cmd_mode == 0x40)//request otp page count
		{
			cmd_mode = 0;
			data_cnt = 0;
			//send otp page count
			EA = 0;
			S0BUF = otp_page_count;
			while(!TI);
			TI = 0;
			EA = 1;
			return;
		}*/
	}

	EA = 1;
}
void verify()
{
	unsigned char tx_data;
	unsigned int i,j;

	debug_mode = 1;
	
	if(block_cnt == 0)
	{
		for(i = 0; i < (length_of_lastblock + 1); i ++)
		{
//			if(cmd_mode)//eeprom mode
//				i2c_read_ee_reg(0, i, &tx_data);

			//DP_TX_Read_Reg(0, i, &tx_data);//Add by ljx 08-18
			DP_TX_Read_Reg(0xa0,i,&tx_data);//Add by ljx 08-18
			
			// ANX9850 has no OCM, so fellowing code is unuseful.
			/*			
			else //ocm mode
				tx_data = DP_TCON_Read_OTP(from_block, i);
			*/
			EA = 0;
			S0BUF = tx_data;
			while(!TI);
			TI = 0;
			EA = 1;
			//debug_printf("\n current_block %.2x, offset: %.2x,data: %.2x \n",(unsigned int)current_block,(unsigned int)i,(unsigned int)tx_data);
		}
	}
	else
	{
		for(i = from_block; i <= (block_cnt - 1 + from_block); i ++)
		{
			DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_EXTRA_ADDR_REG, ((0xa0 |(i << 1)) >> 1));
			for(j = 0; j < 256; j ++)
			{
//				if(cmd_mode)//eeprom mode
//					i2c_read_ee_reg(i, j, &tx_data);

				//DP_TX_Read_Reg(i, j, &tx_data);//Add by ljx 08-18
				DP_TX_Read_Reg(0xa0 |(i << 1), j, &tx_data);//Add by ljx 08-18


				// ANX9850 has no OCM, so fellowing code is unuseful.
				/*
				else //ocm mode
					tx_data = DP_TCON_Read_OTP(i, j);
				*/
				EA = 0;
				S0BUF = tx_data;
				while(!TI);
				TI = 0;
				EA = 1;
				//debug_printf("\n current_block %.2x, offset: %.2x,data: %.2x \n",(unsigned int)i,(unsigned int)j,(unsigned int)tx_data);
			}
		}
		
		DP_TX_Write_Reg(DP_TX_PORT0_ADDR, DP_TX_EXTRA_ADDR_REG, ((0xa0 |(block_cnt << 1)) >> 1));
		for(i = 0; i < (length_of_lastblock + 1); i ++)
		{
//			if(cmd_mode)//eeprom mode
//				i2c_read_ee_reg(block_cnt, i, &tx_data);

			//DP_TX_Read_Reg(block_cnt, i, &tx_data);//Add by ljx 08-18
			DP_TX_Read_Reg(0xa0 |(block_cnt << 1), i, &tx_data);//Add by ljx 08-18

			// ANX9850 has no OCM, so fellowing code is unuseful.
			/*
			else //ocm mode
				tx_data = DP_TCON_Read_OTP(block_cnt + from_block, i);
			*/
			EA = 0;
			S0BUF = tx_data;
			while(!TI);
			TI = 0;
			EA = 1;
			//debug_printf("\n current_block %.2x, offset: %.2x,data: %.2x \n",(unsigned int)block_cnt,(unsigned int)i,(unsigned int)tx_data);
		}
	}

	data_cnt = 0;
	length_of_lastblock = 0;
	block_cnt = 0;
	offset_tmp = 0;
	current_block = 0;
	cmd_mode = 0;
	cmd_type = 0;
	burn_cmd_got = 0;
	from_block = 0;
	debug_mode = 0;
}

