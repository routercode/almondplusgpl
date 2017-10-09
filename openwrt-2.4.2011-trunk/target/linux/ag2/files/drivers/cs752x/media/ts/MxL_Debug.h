/*******************************************************************************
 *
 * FILE NAME          : MxL_Debug.h
 * 
 * AUTHOR             : David Zhou
 *                      Ron Green - Added MxL_VFC_DEBUG for the MxL Color 
 *                                  Formater filter.
 *
 * DATE CREATED       : 2/7/2008
 *                      9/15/2008
 *
 * DESCRIPTION        : Debug header files 
 *
 *******************************************************************************
 *                Copyright (c) 2008, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MXL_DEBUG_H__
#define __MXL_DEBUG_H__


/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/
#include <linux/device.h>
#include <stdarg.h>
#include <stddef.h>

#include "MaxLinearDataTypes.h"

/******************************************************************************
    Macros
******************************************************************************/

#define MxL_DLL_DEBUG0 printk

#endif /* __MXL_DEBUG_H__ */
