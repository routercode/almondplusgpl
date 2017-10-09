/*******************************************************************************
 *
 * FILE NAME          : MxL241SF_OEM_Drv.c
 * 
 * AUTHOR             : Brenndon Lee
 * DATE CREATED       : 7/30/2009
 *
 * DESCRIPTION        : This file contains I2C driver functins that OEM should
 *                      implement for MxL241SF APIs
 *                             
 *******************************************************************************
 *                Copyright (c) 2006, MaxLinear, Inc.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "DP_TX_Reg.h"

typedef unsigned char        UINT8;
typedef unsigned short       UINT16;
typedef unsigned int         UINT32;
typedef unsigned long long   UINT64;
typedef char                 SINT8;
typedef short                SINT16;
typedef int                  SINT32;
typedef long long            SINT64;
typedef float                REAL32;
typedef double               REAL64;

typedef enum
{
  MXL_TRUE = 0,
  MXL_FALSE = 1,
} MXL_STATUS;

static char i2c_device[] = "/dev/i2c-0";

/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_WriteRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C write operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_WriteRegister2(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 RegData)
{
	int fd, ret;
	MXL_STATUS status = MXL_TRUE;
	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg i2cmsg;
	UINT8 buf[4];

	printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, RegData=0x%x\r\n", __func__,
			I2cSlaveAddr, RegAddr, RegData);	

	fd = open(i2c_device, O_RDWR);
	if (fd < 0 ) {
        	printf("open /dev/i2c-0 failed!!!\r\n");
        	return MXL_FALSE;
    	}

	buf[0] = (RegAddr & 0xFF00) >> 8;
	buf[1] = (RegAddr & 0x00FF);
	buf[2] = (RegData & 0xFF00) >> 8;
	buf[3] = (RegData & 0x00FF);
   
	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;

	i2cmsg.addr  = I2cSlaveAddr;
	i2cmsg.flags = 0;
	i2cmsg.len   = 4;
	i2cmsg.buf   = buf;

	if ((ret = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
    		perror("ioctl()");
    		fprintf(stderr,"ioctl returned %d\n", ret);
    		return MXL_FALSE;
	}
   
	close(fd);

  	return status;
}

MXL_STATUS Ctrl_WriteRegister(UINT8 I2cSlaveAddr, UINT8 RegAddr, UINT8 RegData)
{
        int fd, ret;
        MXL_STATUS status = MXL_TRUE;
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg i2cmsg;
        UINT8 buf[4];

        printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, RegData=0x%x\r\n", __func__,
                        I2cSlaveAddr, RegAddr, RegData);

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

/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_ReadRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C read operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_ReadRegister2(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 *DataPtr)
{
	int fd, ret;
	MXL_STATUS status = MXL_TRUE;
	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg i2cmsg[2];
	UINT8 buf[4];

	fd = open(i2c_device, O_RDWR);
	if (fd < 0 ) {
        	printf("open /dev/i2c-0 failed!!!\r\n");
        	return MXL_FALSE;
	}

	buf[0] = 0xff;
	buf[1] = 0xfb;
	buf[2] = (RegAddr & 0xFF00) >> 8;
	buf[3] = (RegAddr & 0x00FF);
   
	msg_rdwr.msgs = i2cmsg;
	msg_rdwr.nmsgs = 2;

	/* write register address */
	i2cmsg[0].addr  = I2cSlaveAddr;
	i2cmsg[0].flags = 0;
	i2cmsg[0].len   = 4;
	i2cmsg[0].buf   = buf;
   
	/* read value */
	i2cmsg[1].addr  = I2cSlaveAddr;
	i2cmsg[1].flags = I2C_M_RD;
	i2cmsg[1].len   = 2;
	i2cmsg[1].buf   = buf;
   
	if ((ret = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
    		perror("ioctl()");
    		fprintf(stderr,"ioctl returned %d\n", ret);
   		return MXL_FALSE;
	}
   
	*DataPtr = ((UINT16)buf[0] << 8) | buf[1];

	printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, *DataPtr=0x%x\n", __func__,
			I2cSlaveAddr, RegAddr, *DataPtr);

	close(fd);
	return status;
}

MXL_STATUS Ctrl_ReadRegister(UINT8 I2cSlaveAddr, UINT8 RegAddr, UINT8 *DataPtr)
{
        int fd, ret;
        MXL_STATUS status = MXL_TRUE;
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg i2cmsg[2];
        UINT8 buf[4];

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

        *DataPtr = ((UINT16)buf[0] << 8) | buf[1];

        printf("%s: I2cSlaveAddr=%d, RegAddr=0x%x, *DataPtr=0x%x\n", __func__,
                        I2cSlaveAddr, RegAddr, *DataPtr);

        close(fd);
        return status;
}

static void debug_read_all_registers(unsigned short i2c_addr)
{
        UINT16 regaddr;
        UINT16 data;
        int i;

        for (i = 0; i < 128; i++) {
                regaddr = i;
                if (Ctrl_ReadRegister(i2c_addr, regaddr, &data) == MXL_TRUE) {
                        printf("%s: regaddr=0x%x, data=0x%x\r\n", __func__, regaddr, data);
                }
                else {
                        printf("%s: regaddr=0x%x, Read Failed!!!\r\n", __func__, regaddr);
                }

        }
}

int main(int argc, char *argv[])
{
	unsigned short addr;
	unsigned short regoff;
	unsigned short value;

	if (argc < 4)
	{
		printf("usage: i2c_rw {addr} {read|write} {regoff} [value] [all]\n");
		exit(-1);
	}
	addr = atoi(argv[1]);
	regoff = atoi(argv[3]);
	if (!strcmp(argv[2], "read"))
	{
		if (argc > 4)
		{
			if (!strcmp(argv[4], "all"))
			{
				/* read all of the registers */
				printf("All of the register values will be printed out\n");
				debug_read_all_registers(addr);
				exit(0);
			}
		}
		if (Ctrl_ReadRegister(addr, regoff, &value) == MXL_FALSE)
		{
			printf("Read regoff=%d failed\n", regoff);
			exit(-1);	
		}
		printf("I2C addr=%d, regoff=0x%x, value=0x%x\n", addr, regoff, value);
		exit(0);		
		
	}	
	if (!strcmp(argv[2], "write"))
	{
		if (argc < 5)
		{
			printf("usage: i2c_rw {addr} {read|write} {regoff} [value] [all]\n");
                	exit(-1);
		}
		if (Ctrl_WriteRegister(addr, regoff, value) == MXL_FALSE)
		{
			 printf("Write regoff=%d failed!!!\n", regoff);
                         exit(-1);
		}

		printf("Write regoff=%d OK\n", regoff);
		if (Ctrl_ReadRegister(addr, regoff, &value) == MXL_FALSE)
                {
                        printf("Read regoff=%d back failed\n", regoff);
                        exit(-1);
                }
                printf("Read back I2C addr=%d, regoff=0x%x, value=0x%x\n", addr, regoff, value);	
		exit(0);
	}
	printf("Function does not support!!!\n");
}
