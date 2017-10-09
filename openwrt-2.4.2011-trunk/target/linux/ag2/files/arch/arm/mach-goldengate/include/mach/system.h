/*
 *  arch/arm/mach-goldengate/include/mach/hardware.h
 *
 *  This file contains the hardware definitions of the GoldenGate platform.
 *
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                Jason Li <jason.li@cortina-systems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/platform.h>

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
#ifndef CONFIG_CORTINA_FPGA
	/*
	 * To reset, use watchdog to reset whole system
	 */
	unsigned int reg_v;
	reg_v = readl(IO_ADDRESS(GLOBAL_GLOBAL_CONFIG));
	/* enable axi & L2 reset */
	reg_v &= ~0x00000300;

	/* wd*_enable are exclusive with wd0_reset_subsys_enable */
	reg_v &= ~0x0000000E;

	/* reset remap, all block & subsystem */
	reg_v |= 0x000000F0;
	writel(reg_v, IO_ADDRESS(GLOBAL_GLOBAL_CONFIG));

	/* Stall RCPU0/1, stall and clocken */
	writel(0x129, IO_ADDRESS(GLOBAL_RECIRC_CPU_CTL));
	
	/* Fire */
	writel(0, IO_ADDRESS(GOLDENGATE_TWD_BASE + 0x28)); /* Disable WD */
	writel(10, IO_ADDRESS(GOLDENGATE_TWD_BASE + 0x20)); /* LOAD */
	/* Enable watchdog - prescale=256, watchdog mode=1, enable=1 */
	writel(0x0000FF09, IO_ADDRESS(GOLDENGATE_TWD_BASE + 0x28)); /* Enable WD */
#endif
}

#endif
