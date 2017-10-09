/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_fe_core.c
 *
 * $Id: cs_fe_main.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * FIXME!!! description!
 */
#include "cs_fe.h"

extern int cs_fe_hash_init(void);
extern void cs_fe_hash_exit(void);
extern int fe_l2_rslt_lookup_table_init(void);
extern int fe_l3_rslt_lookup_table_init(void);

int cs_fe_init(void)
{
	int ret = 0;

	ret |= cs_fe_table_init();
	ret |= cs_fe_hash_init();
	ret |= fe_l2_rslt_lookup_table_init();
	ret |= fe_l3_rslt_lookup_table_init();

	return ret;
} /* cs_fe_init */

void cs_fe_exit(void)
{
	cs_fe_hash_exit();
} /* cs_fe_exit */
