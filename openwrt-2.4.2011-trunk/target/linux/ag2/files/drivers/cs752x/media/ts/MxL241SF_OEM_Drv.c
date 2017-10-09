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

#include "MxL241SF_OEM_Drv.h"

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

MXL_STATUS Ctrl_WriteRegister(MXL_I2C_CONTROLLER I2c, UINT16 RegAddr, UINT16 RegData)
{
	MXL_STATUS status = MXL_TRUE;

	uint8_t buf[4] = {
		RegAddr >> 8,
		RegAddr & 0xff,
		RegData >> 8,
		RegData & 0xff
	};

	struct i2c_msg msg = {
		.addr = I2c.SlaveAddr,
		.flags = 0,
		.buf = buf,
		.len = sizeof(buf)
	};

	if (i2c_transfer(I2c.Priv, &msg, 1) != 1)
		status = MXL_FALSE;

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

MXL_STATUS Ctrl_ReadRegister(MXL_I2C_CONTROLLER I2c, UINT16 RegAddr, UINT16 *DataPtr)
{
	MXL_STATUS status = MXL_TRUE;

	uint8_t buf[4] = {
		0xff,
		0xfb,
		RegAddr >> 8,
		RegAddr & 0xff
	};

	struct i2c_msg msg[2] = {
		{
			.addr = I2c.SlaveAddr,
			.flags = 0,
			.buf = buf,
			.len = sizeof(buf)
		},
		{
			.addr = I2c.SlaveAddr,
			.flags = I2C_M_RD,
			.buf = buf,
			.len = sizeof(*DataPtr)
		}
	};

	if (i2c_transfer(I2c.Priv, msg, 2) != 2)
		status = MXL_FALSE;

	*DataPtr = ((UINT16)buf[0] << 8) | buf[1];

	return status;
}
