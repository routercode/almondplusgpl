/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * read_write.h
 *
 * $Id: pkt_fwd_ctl.h,v 1.2 2012/06/21 07:55:17 ewang Exp $
 *
 * Define the Device Control Commands for Regsiter Read/Write Utility.
 *
 * $Log: pkt_fwd_ctl.h,v $
 * Revision 1.2  2012/06/21 07:55:17  ewang
 * BUG#31959: porting pkt_fwd_filter from R0.1 to R1.x trunk
 *
 *
 */
#ifndef _PKT_FWD_CTL_H_
#define _PKT_FWD_CTL_H_	1

#include <net/br_pkt_fwd_filter.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>



/*----------------------------------------------------------------------
* Command Structure
*----------------------------------------------------------------------*/

/* CLICMD */
typedef struct clicmd_t {
    char       *name;
    int       (*action)();
    char       *help_msg;
} CLICMD_T;



#endif /* _CS_REG_IOCTL_H_ */



