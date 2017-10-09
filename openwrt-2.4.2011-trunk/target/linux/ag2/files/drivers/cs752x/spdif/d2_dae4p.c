/*
* 	d2_dae4p.c

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


#include "d2_dae4p.h"

#

#define PFX "Cortina: "
#

#define DEBUG
#ifdef DEBUG
# define DPRINTK printk
#else
# define DPRINTK(x,...)
#endif


static int d2_dae4p_reg_i2c_write(struct i2c_client *client, u32 reg, u32 value)
{
	static struct i2c_msg msg;
	static unsigned char buf[6];


	buf[0] = ((reg>>16)&0xff);
	buf[1] = ((reg>>8)&0xff);
	buf[2] = ((reg)&0xff);

	buf[3] = ((value>>16)&0xff);
	buf[4] = ((value>>8)&0xff);
	buf[5] = ((value)&0xff);

	msg.addr = client->addr;
	msg.buf = buf;
	msg.len = 6;

	return i2c_transfer(client->adapter, &msg, 1);
}

static int d2_dae4p_reg_i2c_read(struct i2c_client *client, u32 reg)
{
	static struct i2c_msg msg[2];
	static unsigned char reg_buf[4], data_buf[4];


	reg_buf[0] = ((reg>>16)&0xff);
	reg_buf[1] = ((reg>>8)&0xff);
	reg_buf[2] = ((reg)&0xff);

	memset(data_buf, 0, 4);

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = reg_buf;
	msg[0].len = 3;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = data_buf;
	msg[1].len = 3;

	i2c_transfer(client->adapter, msg, 2);

	return (((unsigned int)data_buf[0]<<16) |((unsigned int)data_buf[1]<<8)| (data_buf[2]) );


}

int d2_default_init(struct i2c_client *client)
{
	static int i;
	int rc;
	//static reg_data curr;
	printk("%s --> \n",__func__);
	udelay(30);
	for(i=0;i<NUM_REGS_MAX;i++)
	{
		//curr = d2_reg[i];
		printk("d2_reg[%d].Reg_Addr(%06x) d2_reg[%d].Reg_Val(%06x)\n",
			i, d2_reg[i].Reg_Addr, i, d2_reg[i].Reg_Val);
		rc = d2_dae4p_reg_i2c_write(client, d2_reg[i].Reg_Addr, d2_reg[i].Reg_Val);
		if (rc < 0) {
			printk("rc = %d\n", rc);
			return -1;
		}
		udelay(30);
	}
	printk("%s <-- \n",__func__);

	mdelay(3000);
#ifdef CONFIG_SOUND_D2_45057_SSP
	d2_dae4p_reg_i2c_write(client, 0x020001, 0xC0000E);
#else
	d2_dae4p_reg_i2c_write(client, 0x020001, 0xC0000F);
#endif
	d2_dae4p_reg_i2c_write(client, 0, 0xFA0000);

	return 0;
}

void d2_dae4p_reset(int slot)
{
        unsigned int gpio_mux;

        /**
         *  reset slot1 -> GPIO2[22]
         *  reset slot1 -> GPIO2[23]
        **/
        mdelay(20);
        /* set gpio */
        gpio_mux = __raw_readl(GLOBAL_GPIO_MUX_2);
	gpio_mux |= BIT(23);
	__raw_writel(gpio_mux, GLOBAL_GPIO_MUX_2);
        mdelay(10);

	/* set gpio out */
	gpio_mux = __raw_readl(PER_GPIO2_CFG);
	gpio_mux &= ~BIT(23);
	__raw_writel(gpio_mux, PER_GPIO2_CFG);
	mdelay(10);

	/* set gpio 0 */
	gpio_mux = __raw_readl(PER_GPIO2_OUT);
	gpio_mux &= ~BIT(23);
	__raw_writel(gpio_mux, PER_GPIO2_OUT);

	mdelay(20);

	/* set gpio 1 */
	gpio_mux = __raw_readl(PER_GPIO2_OUT);
	gpio_mux |= BIT(23);
	__raw_writel(gpio_mux, PER_GPIO2_OUT);

	mdelay(200);




}

int d2_dae4p_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        int j;

        d2_dae4p_reset(2);
        mdelay(2000);
	if (d2_default_init(client))
		return -1;
#if 0
	int i,val;
	udelay(30);
	val = d2_dae4p_reg_i2c_read(client, 0x020001);
	printk("%d : reg 0x020001 data :%x-->\n",i,val);
	udelay(30);
	for(i=0;i<3;i++)
	{
		//d2_dae4p_reg_i2c_write(client,  0x020001,  0xC0000F);
		//udelay(30);
		val = d2_dae4p_reg_i2c_read(client, 0x020001);
		printk("%d : reg 0x020001 data :%x-->\n",i,val);
		udelay(30);
	}

#endif

	return 0;
}

void d2_dae4p_i2c_remove(struct i2c_client *client)
{
	printk("%s : -->\n",__func__);
}

static const struct i2c_device_id d2_dae4p_id[] = {
	{ "d2_dae4p", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, d2_dae4p_id);

static struct i2c_driver d2_dae4p_i2c_driver = {
	.driver = {
		.name	= "d2_dae4p",
		.owner	= THIS_MODULE,
	},
	.probe		= d2_dae4p_i2c_probe,
	.remove		= __devexit_p(d2_dae4p_i2c_remove),
	.id_table	= d2_dae4p_id,
};


int __init d2_dae4p_init(void)
{
	int ret;

	printk("%s : -->\n",__func__);
	ret = i2c_add_driver(&d2_dae4p_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register d2_dae4p-70: %d\n",
				ret);
	}
	return ret;

}

void __exit d2_dae4p_exit(void)
{
	printk("d2_dae4p with Golden Gate SSP\n");


	i2c_del_driver(&d2_dae4p_i2c_driver);

}

module_init(d2_dae4p_init);
module_exit(d2_dae4p_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Middle Huang <middle.huang@cortina-systems.com>");
MODULE_DESCRIPTION("d2_dae4p with Cortina Golden Gate SSP driver");


