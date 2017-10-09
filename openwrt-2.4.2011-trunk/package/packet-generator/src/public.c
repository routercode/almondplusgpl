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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cs_types.h"
#include "public.h"

//=================================================================
//                        Local Functions
//=================================================================

/*----------------------------------------------------------------------
* ascii2hex
* Description:	
*		To convert a character into hexdecimal
* Parameters :	
*		unsigned char c	: 
* Return:
*		unsigned long	: result
*----------------------------------------------------------------------*/
unsigned long ascii2hex(unsigned char c)
{
        if (c >= '0' && c <= '9')
                return (c - '0');
        else if (c >= 'a' && c <= 'f')
                return (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
                return (c - 'A' + 10);
        else
                return (0xffffffff);
}

/*----------------------------------------------------------------------
* ascii2decimal
* Description:	
*		To convert a character into decimal value
* Parameters :	
*		unsigned char c	: 
* Return:
*		unsigned long	: result
*----------------------------------------------------------------------*/
unsigned long ascii2decimal(unsigned char c)
{
        if (c >= '0' && c <= '9')
                return (c - '0');
        else
                return (0xffffffff);
}

/*----------------------------------------------------------------------
* string2decimal
* Description:	
*		To convert a string int a decimal value
* Parameters :	
*		unsigned char *str_p	: points a source string
* Return:
*		unsigned long	: result
*----------------------------------------------------------------------*/
unsigned long string2decimal(unsigned char *str_p)
{
    unsigned long i, result;

    result=0;
    while ((i=ascii2decimal(*str_p))!=0xffffffff){
          result=(result)*10+i;
          str_p++;
    }
    while (*str_p==' '|| *str_p==',') str_p++; /* skip space */

    return(result);

}



//=================================================================
//                       Global Functions
//=================================================================

/*----------------------------------------------------------------------
 * ip_csum
*----------------------------------------------------------------------*/
uint16 ip_csum(uint16 *w, int len, uint32 init_sum)
{
	uint32 sum = init_sum;

    union {
		volatile uint8 c[2];
		volatile uint16 s;
	} su;

    union {
		volatile uint16 s[2];
		volatile int i;
    } iu;

    while ((len -= 2) >= 0)
		sum += *w++;

    if (len == -1)
    {
		su.c[0] = *(char *)w;
		su.c[1] = 0;
		sum += su.s;
    }

    iu.i = sum;
    sum = iu.s[0] + iu.s[1];
    if (sum > 65535)
	sum -= 65535;

    su.s = (uint16)(~sum);

    return (su.c[0] << 8) | su.c[1];
}

/*----------------------------------------------------------------------
* pseudo_sum
*----------------------------------------------------------------------*/
uint32 pseudo_sum(int type, uint16 *sip, uint16 *dip, uint16 len, uint8 protocol)
{
    u32		sum;
    u32		num;

    union {
		volatile unsigned char c[2];
		volatile unsigned short s;
    } su;
    
    if (type == PKT_L3_IPV4)
    	num = 4 / 2;
    else if (type == PKT_L3_IPV6)
    	num = 16 /2;
    else
    	return 0;
    	
    sum = 0;
    while (num--)
    {
    	sum += *sip++;
    	sum += *dip++;
    }
    
    su.c[0] = 0;
    su.c[1] = protocol;
    sum += su.s;

	len = ((len >> 8) & 0xff) + ((len << 8) & 0xff00);
    sum += len;
    
    return sum;
}

/*----------------------------------------------------------------------
* string2hex
* Description:	
*		To convert a string into a hexdecimal value
* Parameters :	
*		unsigned char *str_p	: points a source string
* Return:
*		unsigned long	: result
*----------------------------------------------------------------------*/
unsigned long string2hex(unsigned char *str_p)
{
    unsigned long i, result;

    result=0;
    if (*str_p=='0' && toupper(*(str_p+1))=='X') str_p+=2;
   // printf("*str_p= %x\n",*str_p);
    while ((i=ascii2hex(*str_p))!=0xffffffff)
    {
          //printf("*str_p= %x, i =%x\n",*str_p,i);
          result=(result)*16+i;
          str_p++;
    }
    while (*str_p==' '|| *str_p==',') str_p++; /* skip space */

    return(result);

}

/*----------------------------------------------------------------------
* string2value
* Description:	
*		To convert a string into a decimal or hex value
* Parameters :	
*		unsigned char *str_p	: points a source string
* Return:
*		unsigned long	: result
*----------------------------------------------------------------------*/
unsigned long string2value(unsigned char *str_p)
{

	if (str_p[0]=='0' && (str_p[1]=='x' || str_p[1]=='X'))
	{
		return(string2hex(str_p+2));
	}
	else
	{
		return(string2decimal(str_p));
	}
}

/*----------------------------------------------------------------------
* get_random_number
*----------------------------------------------------------------------*/
unsigned int get_random_number(unsigned int min, unsigned int max)
{
	if (max != 0xffffffff)
		return (unsigned int)(rand() % (max + 1 - min)) + min;
	else
		return (unsigned int)rand();
}

#ifndef WIN32
/*----------------------------------------------------------------------
* stricmp
*----------------------------------------------------------------------*/
int stricmp(unsigned char *x, char *y)
{
	int         size;
	
	if (strlen((char *)x) != strlen(y))
		return -1;
	
	size = strlen((char *)x);
	
	while (size--)
	{
		if (toupper(*y) != toupper(*x))
			return -1;
		y++;
		x++;
	}
	return 0;
	
}

/*----------------------------------------------------------------------
* memicmp
*----------------------------------------------------------------------*/
int memicmp(char *x, char *y, int size)
{
	while (size--)
	{
		if (toupper(*y) != toupper(*x))
			return -1;
		y++;
		x++;
	}
	return 0;
}
#endif //WIN32
