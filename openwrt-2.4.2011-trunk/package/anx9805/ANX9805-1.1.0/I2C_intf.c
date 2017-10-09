//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "I2C_intf.h"
#include "mcu.h"
//#include "P89v662.h"
#include "timer.h"

static char i2c_device[] = "/dev/i2c-0";

MXL_STATUS DP_TX_Write_Reg(BYTE I2cSlaveAddr, BYTE RegAddr, BYTE RegData)
{
        int fd, ret;
        MXL_STATUS status = MXL_TRUE;
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg i2cmsg;
        unsigned char buf[4];

	I2cSlaveAddr /= 2;
        //printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, RegData=0x%x\r\n", __func__,
        //                I2cSlaveAddr, RegAddr, RegData);
        //printf("%s: [%2x:%2x]=%2x\n", __func__, I2cSlaveAddr, RegAddr, RegData);


        fd = open(i2c_device, O_RDWR);
        if (fd < 0 ) {
                printf("open /dev/i2c-0 failed!!!\r\n");
                return MXL_FALSE;
        }

        buf[0] = RegAddr;
        buf[1] = RegData;

        msg_rdwr.msgs = &i2cmsg;
        msg_rdwr.nmsgs = 1;

        i2cmsg.addr  = I2cSlaveAddr;
        i2cmsg.flags = 0;
        i2cmsg.len   = 2;
        i2cmsg.buf   = buf;

        if ((ret = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
                perror("ioctl()");
                fprintf(stderr,"ioctl returned %d\n", ret);
                return MXL_FALSE;
        }

        close(fd);

        return status;
}

MXL_STATUS DP_TX_Read_Reg(BYTE I2cSlaveAddr, BYTE RegAddr, BYTE *DataPtr)
{
 	 int fd, ret;
        MXL_STATUS status = MXL_TRUE;
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg i2cmsg[2];
        unsigned char buf[4];

	I2cSlaveAddr /= 2;
	//printf("%s: I2cSlaveAddr=0x%x, RegAddr=0x%x\r\n", __func__, I2cSlaveAddr, RegAddr);

        fd = open(i2c_device, O_RDWR);
        if (fd < 0 ) {
                printf("open /dev/i2c-0 failed!!!\r\n");
                return MXL_FALSE;
        }

        buf[0] = RegAddr;

        msg_rdwr.msgs = i2cmsg;
        msg_rdwr.nmsgs = 2;

        /* write register address */
        i2cmsg[0].addr  = I2cSlaveAddr;
        i2cmsg[0].flags = 0;
        i2cmsg[0].len   = 1;
        i2cmsg[0].buf   = buf;

        /* read value */
        i2cmsg[1].addr  = I2cSlaveAddr;
        i2cmsg[1].flags = I2C_M_RD;
        i2cmsg[1].len   = 1;
        i2cmsg[1].buf   = buf;

        if ((ret = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
                perror("ioctl()");
                fprintf(stderr,"ioctl returned %d\n", ret);
                return MXL_FALSE;
        }

        *DataPtr = buf[0];

        //printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, *DataPtr=0x%x\n", __func__,
        //                I2cSlaveAddr, RegAddr, *DataPtr);
        //printf("%s: [%2x:%2x]=%2x\n", __func__, I2cSlaveAddr, RegAddr, *DataPtr);

        close(fd);
        return status;

}

void anx9805_dump_reg(BYTE dev_addr, BYTE base, BYTE len)
{
	BYTE reg_val;
	int cnt;

	for (cnt=base; cnt<(base+len); cnt++) {
		DP_TX_Read_Reg(dev_addr, (BYTE)cnt, &reg_val);
		debug_printf("[%02x:%02x]=%02x\n", (WORD) dev_addr, (WORD) cnt, (WORD) reg_val);
	}
	if (len > 1) {
		debug_printf("\n");
	}
}

#ifdef MCCS_SUPPORT
BYTE DP_TX_I2C_WriteMulti(BYTE dev_addr, BYTE offset, BYTE bCount,pByte pacBuffer)
{
	BYTE result;
	BYTE i;

	S1CON = 0x40;
	
	STA = 1;
	while (!SI) {
		_NOP_;
	}	 
	STA = 0;	
	result = 1;
	if(S1STA != 0x08)
		goto write_stop;//plus start and stop

		
	S1DAT = dev_addr;
	SI = 0;
	while (!SI) {
		_NOP_;
	}  
	result = 2;
	if(S1STA != 0x18)
		goto write_stop;
	S1DAT = offset;
	SI = 0;
	while (!SI) {
		_NOP_;
	}  
	result = 3;
	if(S1STA != 0x28)
		goto write_stop;

	//start to write data
	for(i= 0; i<bCount;i++,pacBuffer++)
	{
		S1DAT = *pacBuffer;
		SI = 0;
		while (!SI) 
		{
			_NOP_;
		}  
		result = 4;
		if(S1STA != 0x28)
			goto write_stop;
	}
	result = 0;

write_stop:
	
	STO = 1;	
	SI = 0;
	while (STO)
			_NOP_;

	return result;

}

BYTE DP_TX_I2C_ReadMulti(BYTE dev_addr,  BYTE bCount,pByte pacBuffer)
{
	BYTE result;
	BYTE i;
	
	EX1 = 0;
	S1CON = 0x40;

		
	STO = 1;	
	SI = 0;
	while (STO)
			_NOP_;//stop  


	STA = 1;
	while (!SI) {
		_NOP_;
	}	 
	STA = 0;	
	result = 2;
	if(S1STA != 0x08)
		goto read_stop;//start 

	
	S1DAT = (dev_addr | 0x01);

	SI = 0;
	while (!SI) {
		_NOP_;//_NOP_;
	}	   
	result = 4;

	if(S1STA != 0x40) 
		goto read_stop;

	for(i= 0; i<bCount;i++,pacBuffer++)
	{
		if(i< bCount-1)
			AA = 1;
		else
			AA = 0;

		SI = 0;
		while (!SI) 
		{
			_NOP_;
		}

		*pacBuffer = S1DAT;
	}
	

	result = 0;
read_stop:
	STO = 1;
	
	SI = 0;

	while (STO)
			_NOP_;

	return result;

}

#endif

unsigned char S2STA_TMP;
unsigned char S2DAT_TMP;
unsigned char S2DAT_TX;


