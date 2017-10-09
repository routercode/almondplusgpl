/*******************************************************************************
 *
 * FILE NAME          : MaxLinearDataTypes.h
 * 
 * AUTHOR             : Brenndon Lee
 * DATE CREATED       : Jul/31, 2006
 *
 * DESCRIPTION        : This file contains MaxLinear-defined data types.
 *                      Instead of using ANSI C data type directly in source code
 *                      All module should include this header file.
 *                      And conditional compilation switch is also declared
 *                      here.
 *
 *******************************************************************************
 *                Copyright (c) 2006, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MAXLINEAR_DATA_TYPES_H__
#define __MAXLINEAR_DATA_TYPES_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include <linux/types.h>

/******************************************************************************
    Macros
******************************************************************************/

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/
typedef uint8_t              UINT8;
typedef uint16_t             UINT16;
typedef uint32_t             UINT32;
typedef uint64_t             UINT64;
typedef int8_t               SINT8;
typedef int16_t              SINT16;
typedef int32_t              SINT32;
typedef int64_t              SINT64;
typedef UINT32               REAL32;
typedef UINT64               REAL64;

typedef enum 
{
  MXL_TRUE = 0,
  MXL_FALSE = 1,  
} MXL_STATUS;

typedef enum
{
  MXL_DISABLE = 0,
  MXL_ENABLE,

  MXL_NO_FREEZE = 0,
  MXL_FREEZE,

  MXL_UNLOCKED = 0,
  MXL_LOCKED
} MXL_BOOL;

typedef struct
{
  void * Priv;
  UINT8 SlaveAddr;
} MXL_I2C_CONTROLLER;

/******************************************************************************
    Global Variable Declarations
******************************************************************************/

/******************************************************************************
    Prototypes
******************************************************************************/

#endif /* __MAXLINEAR_DATA_TYPES_H__ */

