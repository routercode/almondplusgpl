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
 * $Id: read_write.h,v 1.3 2011/07/14 14:50:25 chsu Exp $
 *
 * Define the Device Control Commands for Regsiter Read/Write Utility.
 *
 * $Log: read_write.h,v $
 * Revision 1.3  2011/07/14 14:50:25  chsu
 * Add ioctl for NI port calendar.
 *
 *
 */
#ifndef _CS_REG_IOCTL_H_
#define _CS_REG_IOCTL_H_	1

#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/*----------------------------------------------------------------------
* Command set
*----------------------------------------------------------------------*/
#define SIOCDEVCS75XX	SIOCDEVPRIVATE	// 0x89F0
#define REGREAD		26
#define REGWRITE	27
#define	GMIIREG		28
#define	SMIIREG		29
#define	NIGETPORTCAL	30
#define	NISETPORTCAL	31


/*----------------------------------------------------------------------
* Command Structure
*----------------------------------------------------------------------*/
// Common Header
struct {
	u_int16_t		cmd;	// command ID
	u_int16_t		len;	// data length, excluding this header
} CS_REGCMD_HDR_T;

//REGREAD
typedef struct {
	u_int16_t		cmd;	// command ID
	u_int16_t		len;	// data length, excluding this header
	u_int32_t		location;
	u_int32_t		length;
	u_int32_t		size;
} CS_REGREAD;

//REGWRITE
typedef struct {
	u_int16_t		cmd;	// command ID
	u_int16_t		len;	// data length, excluding this header
	u_int32_t		location;
	u_int32_t		data;
	u_int32_t		size;
} CS_REGWRITE;

//GMIIREG
typedef	struct{
	u_int16_t		cmd;		// command ID
	u_int16_t		len;		// data length, excluding this header
	u_int16_t		phy_addr;
	u_int16_t		phy_reg;
	u_int16_t		phy_len;
} NAT_GMIIREG_T;

//SMIIREG
typedef	struct{
	u_int16_t		cmd;		// command ID
	u_int16_t		len;		// data length, excluding this header
	u_int16_t		phy_addr;
	u_int16_t		phy_reg;
	u_int32_t		phy_data;
} NAT_SMIIREG_T;

/* NIGETPORTCAL */
typedef	struct{
	u_int16_t		cmd;		// command ID
	u_int16_t		len;		// data length, excluding this header
	u_int16_t 		get_port_cal;
} CS_NIGETPORTCAL_T;

/* NISETPORTCAL */
typedef	struct{
	u_int16_t		cmd;		// command ID
	u_int16_t		len;		// data length, excluding this header
	u_int16_t 		table_address;
	u_int16_t 		pspid_ts;
} CS_NISETPORTCAL_T;


//CLICMD
typedef struct clicmd_t {
    char       *name;
    int       (*action)();
    char       *help_msg;
} CLICMD_T;



#endif // _CS_REG_IOCTL_H_



