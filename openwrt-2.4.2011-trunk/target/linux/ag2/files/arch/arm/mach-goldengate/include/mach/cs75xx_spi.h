/*
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _CS75XX_SPI_H_
#define _CS75XX_SPI_H_

#define CS75XX_SPI_NUM_CHIPSELECT	5
#define CS75XX_SPI_CLOCK_DIV		10

#define CS75XX_SPI_CTLR_NAME	"cs75xx-spi"

struct cs75xx_spi_info {
	u32	tclk;		/* no <linux/clk.h> support yet */
	u32	divider;
	u32	timeout;
};


#endif /*_CS75XX_SPI_H_*/
