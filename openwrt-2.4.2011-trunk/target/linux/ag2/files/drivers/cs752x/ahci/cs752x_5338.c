/*
* 	si5338.c

* Storm audio driver
*
* Copyright (c) 2000 Middle Huang
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License.
*
* History:
* 

*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>	/** for i2c **/
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/sound.h>
#include <linux/soundcard.h>
#include <linux/dma-mapping.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>


#include "cs752x_5338.h"


#define PFX "Cortina: "

#define DEBUG
#ifdef DEBUG
# define DPRINTK printk
#else
# define DPRINTK(x,...)
#endif

#define LOCK_MASK 0x15
#define LOS_MASK 0x04



static int si5338_reg_i2c_write(struct i2c_client *client, u8 reg, u8 value)
{
	struct i2c_msg msg;
	unsigned char buf[2];


	buf[0] = reg;
	buf[1] = value;

	msg.addr = client->addr;
	msg.buf = buf;
	msg.len = 2;
	msg.flags= 0;

	return i2c_transfer(client->adapter, &msg, 1);
}

static int si5338_reg_i2c_read(struct i2c_client *client, u8 reg, u8 *value_p)
{
	struct i2c_msg msg[2];


	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = &reg;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = value_p;
	msg[1].len = 1;

	return i2c_transfer(client->adapter, msg, 2);
}

void si5338_clk_init(struct i2c_client *client)
{
	static u16 counter;
   	static u8 curr_chip_val, clear_curr_val, clear_new_val, combined, reg;	
   	static reg_data *reg_store;
   	
   	
   	printk("%s : client->addr %x\n",__func__,client->addr);
	
	if(client->addr == 0x70)	
		reg_store = reg_store_gen2;
	else
		reg_store = reg_store_gen1;
	
	/**----------------------------------------------------------------
	 See Si5338 datasheet Figure 9 for more details on this procedure
	
	 delay added to wait for Si5338 to be ready to communicate 
	 after turning on
	**/
	mdelay(12);

	si5338_reg_i2c_write(client, 230, 0x10);     /** OEB_ALL = 1 **/
	si5338_reg_i2c_write(client, 241, 0xE5);     /** DIS_LOL = 1 **/

	/** for all the register values in the Reg_Store array 
	 get each value and mask and apply it to the Si5338
   	 Temporary counter variable used in for loops
   	**/
	for(counter=0; counter<NUM_REGS_MAX; counter++){
		
		if(reg_store[counter].Reg_Mask != 0x00) { 

			if(reg_store[counter].Reg_Mask == 0xFF) { 
				/**  do a write transaction only 
				 since the mask is all ones			
				**/
				si5338_reg_i2c_write(client, reg_store[counter].Reg_Addr, reg_store[counter].Reg_Val);
			
			} else {		
				/** do a read-modify-write **/
				si5338_reg_i2c_read(client, reg_store[counter].Reg_Addr, &curr_chip_val);
				clear_curr_val = curr_chip_val & ~(reg_store[counter].Reg_Mask);
				clear_new_val = (reg_store[counter].Reg_Val) & (reg_store[counter].Reg_Mask);
				combined = clear_new_val | clear_curr_val;		
				si5338_reg_i2c_write(client, (reg_store[counter].Reg_Addr), combined);
			}
		}
	}

	/**  check LOS alarm for the xtal input 
	 on IN1 and IN2 (and IN3 if necessary) - 
	 change this mask if using inputs on IN4, IN5, IN6
	**/
	si5338_reg_i2c_read(client, 218, &reg); 
	reg &= LOS_MASK;
	
	while(reg != 0){
		si5338_reg_i2c_read(client, 218, &reg);			
		reg &= LOS_MASK;
		schedule();
	}
	
	si5338_reg_i2c_read(client, 49, &reg);
	reg &= 0x7F;
	/**
	si5338_reg_i2c_write(client, 49, si5338_reg_i2c_read(client, 49) & 0x7F); //FCAL_OVRD_EN = 0
	**/
	si5338_reg_i2c_write(client, 49, reg); /** FCAL_OVRD_EN = 0 **/
	
	si5338_reg_i2c_write(client, 246, 2);                      /** soft reset **/
	si5338_reg_i2c_write(client, 241, 0x65);                   /** DIS_LOL = 0 **/

	/**  wait for Si5338 to be ready after calibration (ie, soft reset) **/
	mdelay(30);
	
	/**
	//make sure the device locked by checking PLL_LOL and SYS_CAL
	si5338_reg_i2c_read(client, 218, &reg);
	reg &= LOCK_MASK;
	while(reg != 0){
		si5338_reg_i2c_read(client, 218, &reg);
		reg &= LOCK_MASK;
		schedule();
	}
	**/
	
	/** copy FCAL values **/
	si5338_reg_i2c_read(client, 235, &reg);
	si5338_reg_i2c_write(client, 45, reg);
	si5338_reg_i2c_read(client, 236, &reg);
	si5338_reg_i2c_write(client, 46, reg);
	/**  clear bits 0 and 1 from 47 and 
	// combine with bits 0 and 1 from 237
	**/
	
	si5338_reg_i2c_read(client, 47, &combined);
	combined &= 0xFC;
	si5338_reg_i2c_read(client, 237, &reg);
	reg &= 3;
	reg |= combined;
	/** reg = (si5338_reg_i2c_read(client, 47) & 0xFC) | (si5338_reg_i2c_read(client, 237) & 3);  **/
	
	si5338_reg_i2c_write(client, 47, reg);
	
	si5338_reg_i2c_read(client, 49, &reg);
	reg |= 0x80;
	si5338_reg_i2c_write(client, 49, reg); /**  FCAL_OVRD_EN = 1 **/
	si5338_reg_i2c_write(client, 230, 0x00);                   /**  OEB_ALL = 0 **/

	/** ------------------------------------------------------------**/


#if 0
	int i;
	for ( i= 6; i < 255; i += 7) {
		si5338_reg_i2c_read(client, i, &reg);
		printk( "si5338_reg_i2c_read offset = %d, 0x%02x\n", i, reg);
	}
#endif
	

}

int si5338_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
/**	
	u8 tmp[1], i;
	for(i=0;i<3;i++)
	{
		tmp[0] = 0;
		si5338_reg_i2c_read(client, 27, tmp);
		printk("%d : read reg 27 data %x\n",i,tmp[0]);
	}
**/
	si5338_clk_init(client);
	
	return 0;
}

void si5338_i2c_remove(struct i2c_client *client)
{
	printk("%s : -->\n",__func__);
}

static const struct i2c_device_id si5338_70_id[] = {
	{ "si5338_70", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, si5338_70_id);

static struct i2c_driver si5338_70_i2c_driver = {
	.driver = {
		.name	= "si5338_70",
		.owner	= THIS_MODULE,
	},
	.probe		= si5338_i2c_probe,
	.remove		= __devexit_p(si5338_i2c_remove),
	.id_table	= si5338_70_id,
};

static const struct i2c_device_id si5338_71_id[] = {
	{ "si5338_71", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, si5338_71_id);

static struct i2c_driver si5338_71_i2c_driver = {
	.driver = {
		.name	= "si5338_71",
		.owner	= THIS_MODULE,
	},
	.probe		= si5338_i2c_probe,
	.remove		= __devexit_p(si5338_i2c_remove),
	.id_table	= si5338_71_id,
};

int si5338_init(void)
{
	int ret;
	
	printk("%s : -->\n",__func__);

	ret = i2c_add_driver(&si5338_70_i2c_driver);
	if (ret != 0) {
		printk("Failed to register Si5338-70: %d\n",
				ret);
	}
	printk("%s 1: -->\n",__func__);

	ret |= i2c_add_driver(&si5338_71_i2c_driver);
	if (ret != 0) {
		printk("Failed to register Si5338-71: %d\n",
				ret);
	}
	printk("%s 2: -->\n",__func__);


	return ret;

}

void si5338_exit(void)
{
	printk("si5338 with Golden Gate\n");


	i2c_del_driver(&si5338_70_i2c_driver);
	i2c_del_driver(&si5338_71_i2c_driver);

}

