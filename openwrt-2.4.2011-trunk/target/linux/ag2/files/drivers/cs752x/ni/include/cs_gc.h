/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                Wen Hsu <wen.hsu@cortina-systems.com>
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

#ifndef __CS_GC_H__
#define __CS_GC_H__

#include <linux/list.h>

typedef struct cs_gc_entry_s cs_gc_entry_t;
typedef struct cs_vtable_hash_entry_s cs_vtable_hash_entry_t;
typedef struct fe_rslt_lookup_entry_s fe_rslt_lookup_entry_t;

/*
 * Result lookup tables
 * Table of pointers, 4k hash table with 16 buckets.
 * CRC16 value of the key is calculated first. Then check corresponding 
 * result lookup table. use hw_index to get hardware result table index. 
 * On removal, the caller should calc crc16 again and check this table. And 
 * update gc_list, hardware result entry and release this entry.
 */
#if 0
typedef enum {
	CS_VTABLE_RSLT_TYPE_MAC,
	CS_VTABLE_RSLT_TYPE_VID,
	CS_VTABLE_RSLT_TYPE_IPV4,
	CS_VTABLE_RSLT_TYPE_IPV6,
	CS_VTABLE_RSLT_TYPE_PORT,
	CS_VTABLE_RSLT_TYPE_SWID64,
	CS_VTABLE_RSLT_TYPE_MAX
} cs_vtable_rslt_lookup_table_type_e;
#endif

#if 0
typedef struct cs_vtable_rslt_lookup_entry_s {
	/* next bucket of the same index value */
	struct cs_vtable_rslt_lookup_entry_s	*next;

	unsigned int	table_type;
	__u16			table_index;

	/* This rslt_index is the entry index of L2/L3 result table. */
	__u16		hw_index;
	__u16		state;

	/* pointer to garbage collection list */
	struct cs_gc_entry_s	*gc_entry;

	/* value of the key */
	union {
		struct {
			__u8	mac_addr[6];
		} l2;
		struct {
			__u16	vid;
		} vlan;
		struct {
			__u32	addr;
		} ipv4;
		struct {
			__u32	addr[4];
		} ipv6;
		struct {
			__u16	port;
		} l4;
		struct {
			__u32	swid[2];
		} misc;
	};
} cs_vtable_rslt_lookup_entry_t;
#endif

#if 0
/*
 * This function will try to add a result to hardware table, with given key.
 * The function will calculate CRC16 first to get lookup table index. Then
 * check if hardware result entry is available or not. hw_index will be set
 * if success. Lookup entry state will be set reserved. The allocated result
 * entry will be returned which may be used to set gc_entry. The state will
 * be set valid after gc_entry is set.
 * If look up entry exists, do binary compare. If same, the allocated one
 * will be returned. Otherwise a new entry will be allocated.
 */
int cs_vtable_rslt_add_entry(unsigned int table_type, void *key, 
		cs_vtable_rslt_lookup_entry_t **entry);
/*
 * This function will set gc_entry to result lookup entry, and change entry
 * state to valid.
 */
int cs_vtable_rslt_set_gc_entry(cs_vtable_rslt_lookup_entry_t *lookup_entry, 
		struct cs_gc_entry_s *gc_entry);

/*
 * This function will free hw entry, set state to invalid. While point to
 * next bucket is still valid.
 */
int cs_vtable_rslt_remove_entry(unsigned int table_type, void *key);

/*
 * This function will free hw entries, and release allocated entry memories.
 */
int cs_vtable_rslt_release_table(unsigned int table_type);
#endif

/* =============================================================================
 * Garbage Collection and APIs
 * =============================================================================
 */
/*
 * A garbage collection table.
 *
 */
#define GC_TIMER_PERIOD		20	/* seconds */

typedef enum cs_gc_type {
	FE_GC_TYPE_NONE,
	FE_GC_TYPE_ETHPORT,
	FE_GC_TYPE_MAC,
	FE_GC_TYPE_VID,
	FE_GC_TYPE_IP,
	FE_GC_TYPE_PORT,
	FE_GC_TYPE_SWID64,

	FE_GC_TYPE_MAX
} cs_gc_type_e;

/*
 * In NAT case, for LAN => WAN traffic
 * 	ingress ETH port	egress ETH port
 *	ingress src MAC(LAN)	egress src MAC (host)
 *	ingress dst MAC(host)	egress dst MAC (next hop)
 *	ingress VID		egress VID
 *
 *	ingress src IP(LAN)	egress src IP (host)
 *	ingress dst IP(inet)	egress dst IP (no change)
 *
 *	ingress src port(LAN)	egress src port (host)
 *	ingress dst port(inet)	egress dst port (no change)
 *
 * WAN => LAN traffic
 *	ingress ETH port	egress ETH port
 *	ingress src MAC(inet)	egress src MAC (host)
 *	ingress dst MAC(host)	egress dst MAC (LAN)
 * 	ingress VID		egress VID
 *
 * 	ingress src IP(inet)	egress src IP (inet)
 *	ingress dst IP(host)	egress dst IP (LAN)
 *
 * 	ingress src port(inet)	egress src port (inet)
 * 	ingress dst port(host)	egress dst port (LAN)
 *
 * So garbage collection list should have:
 *  	eth, MAC, VID, IP, port
 */

struct cs_gc_list_s;

struct cs_gc_list_s {
	struct cs_gc_list_s	*prev, *next;
	void	*data;
};

/*
 * Each gc table has unique gc_type and key. Key is a network parameter like
 * MAC addr. When parameter is changed, associated hash entries of this
 * parameter should be cleaned up.
 */
struct cs_gc_entry_s {
	cs_gc_type_e	type;
	/* Key is the key value of this entry, i.e., MAC addr, or IPv4 addr. */
	void*		key;
	unsigned int	key_size;
	unsigned long	last_jiffies;

	/* list of fwd hash entries whose key value matches */
	struct cs_gc_list_s	*head_vhash, *tail_vhash;
	//__u16		crc16;
	spinlock_t	lock;

//	cs_vtable_rslt_lookup_entry_t *lookup_entry;

	/* kernel entry index points to corresponding kernel connection table */
	void*		kernel_entry_index;
	/* when all vhash entries of this gc_entry are invalid, the result
	 * lookup entry could be used to cleanup hw result index though. */
	struct fe_rslt_lookup_entry_s	*lookup_entry;
	/* do we need user count? */
	//atomic_t			users;
	// struct list_head	gc_entry_list;
};

/*
 * Each gc table has one of gc_type, device, MAC, VID, IP, or L4 port.
 * entry_list is a linked list of all used resources.
 */
typedef struct cs_gc_table_s {
	cs_gc_type_e	gc_type;
	/* entry list of different key values */
	struct cs_gc_list_s	*head_gc, *tail_gc;
//	struct list_head	entry_list;
	spinlock_t	lock;
} cs_gc_table_t;


int cs_gc_init(void);

/* Five default gc tables. Initialize one gc table */
int cs_gc_table_init(unsigned int gc_type);

int cs_gc_table_alloc_entry(cs_gc_type_e type, struct cs_gc_list_s **entry);
int cs_gc_table_del_entry(cs_gc_type_e type, struct cs_gc_list_s *entry);

#if 0
/* Add an entry to gc table with "key" value.  Allocated entry address is 
 * returned. All hash entries with this "key" value will be chained in 
 * entry->hash_list. And this entry is referenced in result_lookup_entry, 
 * as result_lookup_entry will guarantee uniqueness. */
int cs_gc_table_add_entry(cs_gc_type_e type, void *key, unsigned int key_size, 
		cs_gc_entry_t **entry);

/* it deletes all the linkage of the garbage collector entry and the hashes 
 * entries that link to this entry. However, it does not free the memory */
int cs_gc_table_del_entry(cs_gc_entry_t *entry);
#endif


/* Add an hash entry to this entry's hash_list. */
int cs_gc_entry_add_hash(struct cs_gc_entry_s *entry, void *hash);
//int cs_gc_entry_add_hash(cs_gc_entry_t *entry, cs_vtable_hash_entry_t *hash);

/* Walk through all linked hash entries and remove/free invalid ones. 
 * This function will be called when timeout or forced garbage collection. */
// int cs_gc_entry_remove_invalid_hash(cs_gc_entry_t *entry);
int cs_gc_entry_remove_hash(struct cs_gc_entry_s *entry, int not_check);
/* Remove all linked hash entries, regarless valid or not. */
int cs_gc_entry_remove_all_hashes(cs_gc_entry_t *entry);

/* clean up all the not-in-used GC entry in a specific table */
int cs_gc_table_cleanup(struct cs_gc_table_s *table);

/* go through all the active tables and clean them up */
int cs_gc_table_cleanup_all(void);

/* Update the latest jiffies of the entry, typically called by hw timeout */
int cs_gc_entry_update_timer(cs_gc_entry_t *entry, unsigned long jiffies);

#if 0
int cs_gc_table_new(cs_uint32 gc_type, cs_gc_table_t *table);
int cs_gc_table_free(cs_gc_table_t *table);

int cs_gc_table_set_key(cs_gc_table_t *table, void* key);
#endif

#if 0	/* old definition, do we still need these */
/* 
 * Add fwd hash entry to a garbage collection table list.
 * As fwd hash entry has link of associated qos hash entry, qos hash entry will
 * expire when fwd hash entry expires. QoS hash entry does not expire alone.
 *
 */
int cs_gc_table_add_conn(cs_gc_table_t *table, cs_vtable_hash_entry_t *hash);

/*
 * Remove invalidate hash entries, usually called when timer is up or forced
 * garbage collection.
 */
int cs_gc_table_clean_conn(cs_gc_table_t *table);
#endif

void cs_gc_entry_set_rslt_lookup_entry(struct cs_gc_entry_s *gc_entry,
	struct fe_rslt_lookup_entry_s *lookup_entry);

#endif	/* __CS_GC_H__ */
