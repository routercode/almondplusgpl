/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : cs_types.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility general
                 definitions file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef __CS_TYPES_H__
#define __CS_TYPES_H__


typedef	unsigned long	ULONG;
typedef	unsigned long	DWORD;
typedef	unsigned long	uint32;
typedef	unsigned long	u_int32;
#ifdef WIN32
typedef unsigned long	u_int32_t;
#endif
typedef	unsigned long	u32;

typedef	unsigned short	USHORT;
typedef unsigned short  WORD;
typedef unsigned short  USS;
typedef unsigned short  uint16;
typedef unsigned short  u_int16;
#ifdef WIN32
typedef unsigned short  u_int16_t;
#endif
typedef unsigned short  UINT16;
typedef	unsigned short	u16;

typedef	unsigned char	UCHAR;
typedef	unsigned char	BYTE;
typedef	unsigned char	USC;
typedef	unsigned char	uint8;
typedef	unsigned char	u_int8;
#ifdef WIN32
typedef	unsigned char	u_int8_t;
#endif
typedef	unsigned char	UINT8;
typedef	unsigned char	u8;

typedef	char			CHAR;
typedef	short			SHORT;
typedef	int				INT;
typedef long			int32;
typedef short			INT16;
typedef	char			INT8;

typedef unsigned long			IPADDR_T;


#define VOID            void
#define LONG            int
#define ULONGLONG       u64
typedef VOID            *PVOID;
typedef char            *PCHAR;
typedef UCHAR           *PUCHAR;
typedef ULONG           *PULONG;

#define BOOLEAN         u8
#define bool			int

#ifndef FALSE
#define FALSE			0
#define TRUE			1
#endif    

#ifndef false
#define false			0
#define true			1
#endif    

#ifndef NULL
#define NULL    		(void *)0
#endif

#define BIT(x)			(0x1 << x)

#define IPIV(a,b,c,d) 	((a<<24)+(b<<16)+(c<<8)+d)
#define IP1(a)        	((a>>24)&0xff)
#define IP2(a)        	((a>>16)&0xff)
#define IP3(a)        	((a>>8)&0xff)
#define IP4(a)        	((a)&0xff)
#define IPIV_NUM(x)		IP1(x),IP2(x),IP3(x),IP4(x)	



typedef enum
{
    CS_STATUS_SUCCESS,
    CS_STATUS_FAILURE,
    
    CS_STATUS_ERROR_OPEN_FILE,
    CS_STATUS_ERROR_WRITE_FILE,
    CS_STATUS_ERROR_READ_FILE,
    CS_STATUS_ERROR_ZERO_SIZE,
    CS_STATUS_ERROR_NO_FREE_BUFFER,
    
    CS_STATUS_ERROR_MEM_ALLOCATE,
}CS_STATUS;


#endif /* __CS_TYPES_H__ */
