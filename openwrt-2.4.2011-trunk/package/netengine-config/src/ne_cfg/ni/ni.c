/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ni.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility ni module
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/ 
    
    
#include <stdio.h>
    
#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>
    
/*----------------------------------------------------------------------
* ROUTINE NAME - necfg_ni_mod
*-----------------------------------------------------------------------
* DESCRIPTION:
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED
*----------------------------------------------------------------------*/ 
int necfg_ni_mod(int argc, char **argv) 
{
	int ret = -1;
	
	printf(" --- NOT Implement yet --- \n");
	return ret;
} /*end necfg_ni_mod() */


