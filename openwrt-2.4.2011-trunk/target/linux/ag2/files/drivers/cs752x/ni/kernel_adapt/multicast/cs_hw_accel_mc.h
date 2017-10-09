/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                CH Hsu <ch.hsu@cortina-systems.com>
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
#ifndef __CS_HW_ACCEL_MC_H__
#define __CS_HW_ACCEL_MC_H__

#include <linux/skbuff.h>
#include <mach/cs_types.h>
#include <linux/mroute.h>
#include <linux/mroute6.h>
#include <mach/cs75xx_fe_core_table.h>
#include <cs_hw_accel_util.h>
#include "../include/uthash.h"

#define CS_MC_V4_HASH_FIELDS    10000100001b // fake number
// #define CS_MC_V4_HASH_FIELDS	( HASH_MASK_IPV4_SRC_IP | HASH_MASK_IPV4_DST_IP)
#define CS_MC_V6_HASH_FIELDS    11111100001b // fake number
// #define CS_MC_V6_HASH_FIELDS	( HASH_MASK_IPV6_SRC_IP | HASH_MASK_IPV6_DST_IP | \
//				HASH_MASK_IPV6_NDP | HASH_MASK_IPV6_HBH)


#define CS_MC_OK		0x01
#define CS_MC_SW_ONLY	0x02
#define CS_MC_HASH_TIME_OUT 300

#define CS_READ		0
#define CS_WRITE	1

struct cs_mc_hash 
{
	cs_uint64 guid;
	void *mfc_ptr;
	int type;
	int vifi;
	UT_hash_handle hh;
};

typedef struct cs_mcgid_table_s {
	cs_uint8	mcgid;			
	int			mcindex;
	spinlock_t	lock;
} cs_mc_table_s;
cs_status cs_mcgid_init(void);
cs_status cs_mcgid_allocate(cs_uint8 mcgid);
int cs_find_mcindex_by_mcgid(cs_uint8 mcgid); 
cs_status cs_mcgid_dallocate(cs_uint8 mcgid);
int get_m_index(void* mc_ptr, int type); 
int set_m_index( struct mfc_cache *mc_ptr, int idx);
int add_cs_mc_hash(cs_uint64 guid, void *ptr, int type, int vifi); 
struct cs_mc_hash *find_mc_hash(cs_uint64 guid);
void delete_cs_mc_hash(struct cs_mc_hash *s);
void delete_cs_mc_hash_by_guid(cs_uint64 guid);
void cs_mc_shared_delete(cs_uint64 *cs_guid);
void cs_mc_add_hash_after_forwarding(struct sk_buff *skb);
int cs_kernel_mc_core_logic_extension(struct sk_buff * skb);
void cs_mc_set_skb_cb_info(struct sk_buff *skb, cs_uint8 vif);
void cs_mc_shared_forwarding( int *cs_stat, cs_uint64 *cs_guid, 
	struct sk_buff *skb, int vifi, int flag, struct mfc_cache *c, cs_uint16 mask);
void cs_kernel_mc_cb(cs_uint64 guid, int status);
cs_status cs_mc_set_2nd_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint8 mcindex, 
		cs_uint8 o_lspid, cs_uint8 d_voq);
cs_uint8 cs_mc_get_mcgid(void) ;
cs_status cs_hw_accel_mc_create_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, cs_uint64 guid);	
void cs_mc_init(void); 
void k_jt_cs_mc_ipv4_forwarding_hook(struct sk_buff *skb, struct mfc_cache *c, int vifi);	
extern void set_mc_table(cs_uint8 mc_index, cs_uint8 read_write, cs_uint16 mc_vec);									
#endif /* __CS_HW_ACCEL_MC_H__ */

