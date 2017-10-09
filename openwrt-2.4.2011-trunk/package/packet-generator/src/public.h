/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *				CH Hsu <ch.hsu@cortina-systems.com>
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
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "cs_types.h"

#define PKT_L3_IPV4			4
#define PKT_L3_IPV6			6


uint16 ip_csum(uint16 *w, int len, uint32 init_sum);
uint32 pseudo_sum(int type, uint16 *sip, uint16 *dip, uint16 len, uint8 protocol);
unsigned long string2hex(unsigned char *str_p);
unsigned long string2value(unsigned char *str_p);
unsigned int get_random_number(unsigned int min, unsigned int max);
int stricmp(unsigned char *x, char *y);
int memicmp(char *x, char *y, int size);

#endif //__PUBLIC_H__
