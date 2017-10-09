/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * cs_gc.c
 *
 * $Id: cs_gc.c,v 1.1 2011/08/10 02:29:20 peebles Exp $
 *
 * It contains the implementation of garbage collector.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include "cs_fe.h"
#include "cs_gc.h"
#include "cs_fe_hash.h"	/* for crc16 computation */
#include "cs_vtable_hash.h"

static struct timer_list cs_gc_timer_obj;
static struct cs_gc_table_s global_gc_table[FE_GC_TYPE_MAX];


int cs_gc_table_cleanup(struct cs_gc_table_s *table)
{
	struct cs_gc_list_s *tmp_gc, *prev_gc, *gc_list;
	unsigned long flags;
	struct fe_rslt_lookup_entry_s *lookup_entry;
	struct cs_gc_entry_s *gc_entry;

	if (table == NULL)
		return FE_TABLE_OK;

	gc_list = table->head_gc;
#if 0
	printk("%s::table %p, head %p, tail %p, gc_list %p\n",
		__func__, table, table->head_gc, table->tail_gc, gc_list);
#endif
	spin_lock_irqsave(&table->lock, flags);
	while (gc_list != NULL) {
		gc_entry = gc_list->data;
		cs_gc_entry_remove_hash(gc_entry, 0);
		tmp_gc = gc_list->next;

		if (atomic_read(&gc_entry->lookup_entry->users) == 0) {
			gc_entry->lookup_entry->valid = 0;
			kfree(gc_entry);
			gc_list->data = NULL;
		
			if (gc_list == table->head_gc) {
				table->head_gc = tmp_gc;
			}
		}
		if (gc_list->data == NULL) {
			kfree(gc_list);
			gc_list = NULL;
		}
		if (tmp_gc == NULL)
			table->tail_gc = gc_list;
		gc_list = tmp_gc;
	}

	if (table->head_gc == NULL)
		table->tail_gc = NULL;

	spin_unlock_irqrestore(&table->lock, flags);
	return 0;
}

int cs_gc_table_cleanup_all(void)
{
	unsigned int i;

	for (i = 0; i < FE_GC_TYPE_MAX; i++)
		cs_gc_table_cleanup(&global_gc_table[i]);
	return 0;
} /* cs_gc_table_cleanup_all */

#if 0
static cs_vtable_rslt_lookup_entry_t *mac_lookup_table[4096];
static cs_vtable_rslt_lookup_entry_t *vid_lookup_table[4096];
static cs_vtable_rslt_lookup_entry_t *ipv4_lookup_table[4096];
static cs_vtable_rslt_lookup_entry_t *ipv6_lookup_table[4096];
static cs_vtable_rslt_lookup_entry_t *port_lookup_table[4096];
static cs_vtable_rslt_lookup_entry_t *swid_lookup_table[4096];

int cs_vtable_rslt_add_entry(unsigned int table_type, void *key, 
		cs_vtable_rslt_lookup_entry_t **entry)
{
	cs_vtable_rslt_lookup_entry_t **p_table, *p_entry, *p_next_entry;
	void *entry_key = NULL;
	unsigned int byte_len, table_index;
	__u16 crc16;

	switch (table_type) {
	case CS_VTABLE_RSLT_TYPE_MAC:
		p_table = mac_lookup_table;
		byte_len = 6;
		break;
	case CS_VTABLE_RSLT_TYPE_VID:
		p_table = vid_lookup_table;
		byte_len = 2;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV4:
		p_table = ipv4_lookup_table;
		byte_len = 4;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV6:
		p_table = ipv6_lookup_table;
		byte_len = 16;
		break;
	case CS_VTABLE_RSLT_TYPE_PORT:
		p_table = port_lookup_table;
		byte_len = 2;
		break;
	case CS_VTABLE_RSLT_TYPE_SWID64:
		p_table = swid_lookup_table;
		byte_len = 8;
		break;
	default:
		/* unsupported table type */
		return -1;
	};

	/* Compute CRC16.  The first 12 bits are used as the index to the 
	 * lookup_table */
	crc16 = cs_fe_hash_keygen_crc16(CRC16_CCITT, (unsigned char*)key, byte_len);

	table_index = crc16 >> 4;

	if (p_table[table_index] != NULL) {
		p_entry = p_table[table_index];
		do {
			p_next_entry = p_entry->next;
			switch (table_type) {
			case CS_VTABLE_RSLT_TYPE_MAC:
				entry_key = (void*)p_entry->l2.mac_addr;
				break;
			case CS_VTABLE_RSLT_TYPE_VID:
				entry_key = (void*)&p_entry->vlan.vid;
				break;
			case CS_VTABLE_RSLT_TYPE_IPV4:
				entry_key = (void*)&p_entry->ipv4.addr;
				break;
			case CS_VTABLE_RSLT_TYPE_IPV6:
				entry_key = (void*)p_entry->ipv6.addr;
				break;
			case CS_VTABLE_RSLT_TYPE_PORT:
				entry_key = (void*)&p_entry->l4.port;
				break;
			case CS_VTABLE_RSLT_TYPE_SWID64:
				entry_key = (void*)p_entry->misc.swid;
			};
			if (memcmp(entry_key, key, byte_len) == 0) {
				/* found an existing matched entry. so we don't need to create 
				 * a new one */
				*entry = p_entry;
				return 0;
			}
			p_entry = p_next_entry;
		} while (p_entry != NULL);
	}

	/* reach here! meaning we need to create a new rslt_lookup entry */
	p_entry = kmalloc(sizeof(cs_vtable_rslt_lookup_entry_t), GFP_ATOMIC);
	if (p_entry == NULL) return -ENOMEM;
	p_entry->table_type = table_type;
	p_entry->table_index = table_index;
	switch (table_type) {
	case CS_VTABLE_RSLT_TYPE_MAC:
		memcpy(p_entry->l2.mac_addr, key, byte_len);
		break;
	case CS_VTABLE_RSLT_TYPE_VID:
		p_entry->vlan.vid = *(__u16*)key;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV4:
		p_entry->ipv4.addr = *(__u32*)key;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV6:
		memcpy(p_entry->ipv6.addr, key, byte_len);
		break;
	case CS_VTABLE_RSLT_TYPE_PORT:
		p_entry->l4.port = *(__u16*)key;
		break;
	case CS_VTABLE_RSLT_TYPE_SWID64:
		memcpy(p_entry->misc.swid, key, byte_len);
		break;
	};
	p_entry->next = NULL;

	if (p_table[table_index] == NULL)
		p_table[table_index] = p_entry;
	else {
		p_entry->next = p_table[table_index];
		p_table[table_index] = p_entry;
	}

	return 0;
} /* cs_vtable_rslt_add_entry */

int cs_vtable_rslt_set_gc_entry(cs_vtable_rslt_lookup_entry_t *lookup_entry, 
		cs_gc_entry_t *gc_entry)
{
	if ((lookup_entry == NULL) || (gc_entry == NULL)) return -1;

	/* gc_entry and lookup_entry should not point to anything for their 
	 * counterparts before they are going to be linked. */
	if (gc_entry->lookup_entry != NULL) return -1;
	if (lookup_entry->gc_entry != NULL) return -1;

	gc_entry->lookup_entry = lookup_entry;
	lookup_entry->gc_entry = gc_entry;

	return 0;
} /* cs_vtable_rslt_set_gc_entry */

static int rslt_find_entry(unsigned int table_type, void *key, 
		cs_vtable_rslt_lookup_entry_t **entry)
{
	cs_vtable_rslt_lookup_entry_t *p_curr, **p_table;
	void *entry_key = NULL;
	unsigned int byte_len, table_index;
	__u16 crc16;

	if ((key == NULL) || (entry == NULL)) return -1;

	switch (table_type) {
	case CS_VTABLE_RSLT_TYPE_MAC:
		p_table = mac_lookup_table;
		byte_len = 6;
		break;
	case CS_VTABLE_RSLT_TYPE_VID:
		p_table = vid_lookup_table;
		byte_len = 2;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV4:
		p_table = ipv4_lookup_table;
		byte_len = 4;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV6:
		p_table = ipv6_lookup_table;
		byte_len = 16;
		break;
	case CS_VTABLE_RSLT_TYPE_PORT:
		p_table = port_lookup_table;
		byte_len = 2;
		break;
	case CS_VTABLE_RSLT_TYPE_SWID64:
		p_table = swid_lookup_table;
		byte_len = 8;
		break;
	default:
		/* unsupported table type */
		return -1;
	};

	crc16 = cs_fe_hash_keygen_crc16(CRC16_CCITT, (unsigned char*)key, byte_len);
	table_index = crc16 >> 4;

	if (p_table[table_index] == NULL) return -1;
	p_curr = p_table[table_index];
	do {
		switch (table_type) {
		case CS_VTABLE_RSLT_TYPE_MAC:
			entry_key = (void*)p_curr->l2.mac_addr;
			break;
		case CS_VTABLE_RSLT_TYPE_VID:
			entry_key = (void*)&p_curr->vlan.vid;
			break;
		case CS_VTABLE_RSLT_TYPE_IPV4:
			entry_key = (void*)&p_curr->ipv4.addr;
			break;
		case CS_VTABLE_RSLT_TYPE_IPV6:
			entry_key = (void*)p_curr->ipv6.addr;
			break;
		case CS_VTABLE_RSLT_TYPE_PORT:
			entry_key = (void*)&p_curr->l4.port;
			break;
		case CS_VTABLE_RSLT_TYPE_SWID64:
			entry_key = (void*)p_curr->misc.swid;
		};
		if (memcmp(entry_key, key, byte_len) == 0) {
			/* found an existing matched entry. */
			*entry = p_curr;
			return 0;
		}
		p_curr = p_curr->next;
	} while (p_curr != NULL);

	/* if gets here, meaning nothing is found */
	return -1;
} /* rslt_find_entry */

static int rslt_remove_from_list(cs_vtable_rslt_lookup_entry_t *p_entry)
{
	cs_vtable_rslt_lookup_entry_t *p_curr, *p_prev, **p_table;

	switch (p_entry->table_type) {
	case CS_VTABLE_RSLT_TYPE_MAC:
		p_table = mac_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_VID:
		p_table = vid_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV4:
		p_table = ipv4_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV6:
		p_table = ipv6_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_PORT:
		p_table = port_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_SWID64:
		p_table = swid_lookup_table;
		break;
	default:
		/* unsupported table type */
		return -1;
	};

	if (p_table[p_entry->table_index] == NULL) return -1;

	p_prev = NULL;
	p_curr = p_table[p_entry->table_index];
	while (p_curr != NULL) {
		if (p_curr == p_entry) {
			if (p_prev == NULL) p_table[p_entry->table_index] = p_curr->next;
			else p_prev->next = p_curr->next;
			return 0;
		}
	};

	return -1;
} /* rslt_remove_from_list */

int cs_vtable_rslt_remove_entry(unsigned int table_type, void *key)
{
	cs_vtable_rslt_lookup_entry_t *p_entry;
	int ret;

	if (key == NULL) return -1;

	ret = rslt_find_entry(table_type, key, &p_entry);
	if (ret != 0) return ret;

	/* remove garbage collector info that links to this rslt_lookup */
	if (p_entry->gc_entry != NULL) cs_gc_table_del_entry(p_entry->gc_entry);
	kfree(p_entry->gc_entry);
	p_entry->gc_entry = NULL;

	ret = rslt_remove_from_list(p_entry);
	if (ret != 0) return ret;

	kfree(p_entry);

	return 0;
} /* cs_vtable_rslt_remove_entry */

int cs_vtable_rslt_release_table(unsigned int table_type)
{
	cs_vtable_rslt_lookup_entry_t *p_curr, *p_next, **p_table;
	unsigned int i;

	switch (table_type) {
	case CS_VTABLE_RSLT_TYPE_MAC:
		p_table = mac_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_VID:
		p_table = vid_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV4:
		p_table = ipv4_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_IPV6:
		p_table = ipv6_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_PORT:
		p_table = port_lookup_table;
		break;
	case CS_VTABLE_RSLT_TYPE_SWID64:
		p_table = swid_lookup_table;
		break;
	default:
		/* unsupported table type */
		return -1;
	};

	for (i = 0; i < 4096; i++) {
		if (p_table[i] != NULL) {
			p_curr = p_table[i];
			while (p_curr != NULL) {
				p_next = p_curr->next;
				if (p_curr->gc_entry != NULL)
					cs_gc_table_del_entry(p_curr->gc_entry);
				kfree(p_curr->gc_entry);
				rslt_remove_from_list(p_curr);
				kfree(p_curr);
				p_curr = p_next;
			};
		}
	}

	return 0;
} /* cs_vtable_rslt_release_table */
#endif

inline void cs_gc_entry_set_rslt_lookup_entry(struct cs_gc_entry_s *gc_entry,
	struct fe_rslt_lookup_entry_s *lookup_entry)
{
	gc_entry->lookup_entry = lookup_entry;
}

int cs_gc_entry_add_hash(struct cs_gc_entry_s *gc_entry,
	void* vhash)
{
	unsigned long flags;
	struct cs_gc_list_s* vhash_list;
	struct cs_vtable_hash_entry_s *hash = vhash;

	vhash_list = kzalloc(sizeof(struct cs_gc_list_s), GFP_ATOMIC);
	if (vhash_list == NULL) {
		return FE_TABLE_ENOMEM;
	}
	vhash_list->data = vhash;

	spin_lock_irqsave(&gc_entry->lock, flags);
	if (gc_entry->head_vhash == NULL) {
		gc_entry->head_vhash = vhash_list;
		gc_entry->tail_vhash = vhash_list;
	} else {
		vhash_list->prev = gc_entry->tail_vhash;
		gc_entry->tail_vhash->next = vhash_list;
		gc_entry->tail_vhash = vhash_list;
	}
	atomic_inc(&hash->gc_users);
	spin_unlock_irqrestore(&gc_entry->lock, flags);
	return FE_TABLE_OK;
}

int cs_gc_entry_remove_hash(struct cs_gc_entry_s *entry, int not_check)
{
	struct cs_gc_list_s *curr_gc, *next_gc;
	cs_vtable_hash_entry_t *hash;
	unsigned long flags;

	if (entry == NULL)
		return FE_TABLE_ENTRYNOTFOUND;

	curr_gc = entry->head_vhash;
#if 0
	if (curr_gc)
		printk("%s::head %p, tail %p, vhash %p\n\tlookup_entry %p, use %x, not_check %d\n",
			__func__, entry->head_vhash, entry->tail_vhash, curr_gc->data,
			entry->lookup_entry, atomic_read(&entry->lookup_entry->users),
			not_check);
#endif	
	spin_lock_irqsave(&entry->lock, flags);

	while (curr_gc != NULL) {
		next_gc = curr_gc->next;
		hash = curr_gc->data;

		if (not_check || (hash->state == CS_VHASH_STATE_INVALID)) {

			if (curr_gc->prev != NULL)
				curr_gc->prev->next = next_gc;
		
			if (curr_gc->next != NULL)
				curr_gc->next->prev = curr_gc->prev;

			if (entry->head_vhash == curr_gc)
				entry->head_vhash = next_gc;

			if (entry->tail_vhash == curr_gc)
				entry->tail_vhash = curr_gc->prev;

			atomic_dec(&hash->gc_users);
			kfree(curr_gc);

			if (atomic_read(&hash->gc_users) == 0) {
				cs_vhash_del_fwd_entry(hash);
				kfree(hash);
			} else {
#if 0
				printk("%s::hash %p, users %x\n",
					__func__, hash, atomic_read(&hash->gc_users));
#endif
			}
			atomic_dec(&entry->lookup_entry->users);
		}
		curr_gc = next_gc;
	}
	spin_unlock_irqrestore(&entry->lock, flags);
	return FE_TABLE_OK;
}

void cs_gc_entry_del_vhash(void)
{

}

/* timer functions */
void cs_gc_timer_func(unsigned long data)
{
	cs_gc_table_cleanup_all();
	mod_timer((struct timer_list*)data, jiffies + (GC_TIMER_PERIOD * HZ));
} /* cs_gc_timer_func */

void cs_gc_timer_init(void)
{
	init_timer(&cs_gc_timer_obj);
	cs_gc_timer_obj.expires = jiffies + (GC_TIMER_PERIOD * HZ);
	cs_gc_timer_obj.data = (unsigned long)&cs_gc_timer_obj;
	cs_gc_timer_obj.function = (void *)&cs_gc_timer_func;
	add_timer(&cs_gc_timer_obj);
} /* cs_gc_timer_init */

int cs_gc_init(void)
{
	unsigned int i;

	for (i = 0; i < FE_GC_TYPE_MAX; i++) {
		cs_gc_table_init(i);
	}

	cs_gc_timer_init();

	return 0;
} /* cs_gc_init */

int cs_gc_table_init(cs_gc_type_e gc_type)
{
	cs_gc_table_t *curr_gc_table = &global_gc_table[gc_type];
	curr_gc_table->gc_type = gc_type;
	curr_gc_table->head_gc = NULL;
	curr_gc_table->tail_gc = NULL;
	spin_lock_init(&curr_gc_table->lock);
	return 0;
}

int cs_gc_entry_release(struct cs_gc_entry_s *entry)
{
	return 0;
}
/*
 * Add a gc entry to corresponding table. Result lookup resolves entry key
 * value uniqueness.
 */
int cs_gc_table_add_entry(cs_gc_type_e type, struct cs_gc_list_s *entry)
{
	cs_gc_table_t *table = &global_gc_table[type];
	unsigned long flags;

	spin_lock_irqsave(&table->lock, flags);
	if (table->head_gc == NULL) {
		table->head_gc = entry;
		table->tail_gc = entry;
	} else {
		entry->prev = table->tail_gc;
		table->tail_gc->next = entry;
		table->tail_gc = entry;
	}
	spin_unlock_irqrestore(&table->lock, flags);
	return FE_TABLE_OK;
}

/* this function will create a new gc_entry and add into gc_table of type */
int cs_gc_table_alloc_entry(cs_gc_type_e type, struct cs_gc_list_s **entry)
{
	struct cs_gc_list_s *gc_list = NULL;
	struct cs_gc_entry_s *gc_entry = NULL;
	cs_gc_table_t *table = &global_gc_table[type];
	unsigned long flags;

	gc_list = kzalloc(sizeof(struct cs_gc_list_s), GFP_ATOMIC);
	if (gc_list == NULL)
		return FE_TABLE_ENOMEM;

	gc_entry = kzalloc(sizeof(struct cs_gc_entry_s), GFP_ATOMIC);
	if (gc_entry == NULL) {
		kfree(gc_list);
		return FE_TABLE_ENOMEM;
	}
	gc_list->data = gc_entry;
	spin_lock_init(&gc_entry->lock);

	spin_lock_irqsave(&table->lock, flags);
	if (table->head_gc == NULL) {
		table->head_gc = gc_list;
		table->tail_gc = gc_list;
	} else {
		gc_list->prev = table->tail_gc;
		table->tail_gc->next = gc_list;
		table->tail_gc = gc_list;
	}
	spin_unlock_irqrestore(&table->lock, flags);

	*entry = gc_list;
	return FE_TABLE_OK;
}

int cs_gc_table_del_entry(cs_gc_type_e type, struct cs_gc_list_s *entry)
{
	cs_gc_table_t *table = &global_gc_table[type];
	unsigned long flags;

	spin_lock_irqsave(&table->lock, flags);
	if (entry->prev == NULL) {
#if 0
		if (table->head_gc != entry)
			printk("%s::table head %p, entry %p\n",
				__func__, table->head_gc, entry);
#endif
		// head of the table
		table->head_gc = entry->next;
	} else
		entry->prev->next = entry->next;

	if (entry->next == NULL) {
#if 0
		if (table->tail_gc != entry)
			printk("%s::table tail %p, entry %p\n",
				__func__, table->tail_gc, entry);
#endif
		// tail of the table
		table->tail_gc = entry->prev;
	} else
		entry->next->prev = entry->prev;

	spin_unlock_irqrestore(&table->lock, flags);

	cs_gc_entry_remove_hash(entry->data, 1);
	kfree(entry);
	return 0;
}

void cs_gc_table_release(cs_gc_type_e type)
{
	cs_gc_table_t *table = &global_gc_table[type];
	unsigned long flags;
	struct cs_gc_list_s *entry = table->head_gc, *tmp;

	spin_lock_irqsave(&table->lock, flags);
	while (entry != NULL) {
		tmp = entry->next;
		cs_gc_entry_release(entry->data);
		kfree(entry);
		entry = tmp;
	}
	table->head_gc = table->tail_gc = NULL;
	spin_unlock_irqrestore(&table->lock, flags);
}

#if 0
int cs_gc_table_init(unsigned int gc_type)
{
	cs_gc_table_t *curr_gc_table = &global_gc_table[gc_type];

	printk("%s::init table type %x\n", __func__, gc_type);
	curr_gc_table->gc_type = gc_type;
	INIT_LIST_HEAD(&curr_gc_table->entry_list);
	spin_lock_init(&curr_gc_table->lock);

	return 0;
} /* cs_gc_table_init */

int cs_gc_table_add_entry(cs_gc_type_e type, void *key, unsigned int key_size, 
		cs_gc_entry_t **entry)
{
	cs_gc_table_t *curr_gc_table = &global_gc_table[type];
	cs_gc_entry_t *curr_gc_entry;
	struct list_head *next, *tmp;

	if (curr_gc_table == NULL) return -1;
	if ((key == NULL) || (entry == NULL)) return -1;
	
	printk("%s::gc table %p, type %x\n",
		__func__, curr_gc_table, curr_gc_table->gc_type);
	
	list_for_each_safe(next, tmp, &curr_gc_table->entry_list) {
	//list_for_each(next, &curr_gc_table->entry_list) {
		printk("%s::next %p\n", __func__, next);

		curr_gc_entry = (cs_gc_entry_t*)list_entry(next, cs_gc_entry_t, 
				gc_entry_list);
		if ((memcmp(key, curr_gc_entry->key, curr_gc_entry->key_size) == 0) && 
				(key_size == curr_gc_entry->key_size)) {
			/* found a matching entry, we can use it!! */
			*entry = curr_gc_entry;
			return 0;
		}
	}

	/* if reach here, no previous entry has been created. we have to create 
	 * a new one */
	curr_gc_entry = kmalloc(sizeof(cs_gc_entry_t), GFP_ATOMIC);
	if (curr_gc_entry == NULL) return -ENOMEM;

	curr_gc_entry->type = type;
	curr_gc_entry->key_size = key_size;
	curr_gc_entry->key = kmalloc(key_size, GFP_ATOMIC);
	memcpy(curr_gc_entry->key, key, key_size);
	curr_gc_entry->fwd_hash_list = NULL;
	spin_lock_init(&curr_gc_entry->lock);
	list_add(&curr_gc_entry->gc_entry_list, &curr_gc_table->entry_list);

	return 0;
} /* cs_gc_table_add_entry */

int cs_gc_table_del_entry(cs_gc_entry_t *entry)
{
	if (entry == NULL) return -1;

	cs_gc_entry_remove_all_hashes(entry);
	list_del(&entry->gc_entry_list);
	return 0;
} /* cs_gc_table_del_entry */

int cs_gc_entry_add_hash(cs_gc_entry_t *entry, cs_vtable_hash_entry_t *hash)
{
	cs_gc_list_t *curr_gc_list, *prev_gc_list;
	unsigned long flags;

	if ((entry == NULL) || (hash == NULL)) return -1;

	spin_lock_irqsave(&entry->lock, flags);
	if (entry->fwd_hash_list == NULL) {
		/* adding a new entry to the list */
		curr_gc_list = kmalloc(sizeof(cs_gc_list_t), GFP_ATOMIC);
		if (curr_gc_list == NULL) {
			spin_unlock_irqrestore(&entry->lock, flags);
			return -ENOMEM;
		}
		curr_gc_list->prev = NULL;
		curr_gc_list->next = NULL;
		curr_gc_list->hash = (void *)hash;
		atomic_inc(&hash->gc_users);
		entry->fwd_hash_list = curr_gc_list;
		spin_unlock_irqrestore(&entry->lock, flags);
		return 0;
	}

	/* there are existing entries in the fwd_hash_list */
	prev_gc_list = NULL;
	curr_gc_list = entry->fwd_hash_list;
	do {
		/* found a matching hash.. don't need to add it to the list */
		if (cs_vhash_compare(curr_gc_list->hash, hash) == true) {
			spin_unlock_irqrestore(&entry->lock, flags);
			return 0;
		}

		prev_gc_list = curr_gc_list;
		curr_gc_list = curr_gc_list->next;
	} while (curr_gc_list != NULL);

	/* if reaches here, meaning there is no existing matched entry */
	curr_gc_list = kmalloc(sizeof(cs_gc_list_t), GFP_ATOMIC);
	if (curr_gc_list == NULL) {
		spin_unlock_irqrestore(&entry->lock, flags);
		return -ENOMEM;
	}
	curr_gc_list->prev = prev_gc_list;
	curr_gc_list->next = NULL;
	curr_gc_list->hash = (void *)hash;
	atomic_inc(&hash->gc_users);
	prev_gc_list->next = curr_gc_list;

	spin_unlock_irqrestore(&entry->lock, flags);
	return 0;
} /* cs_gc_entry_add_hash */

int cs_gc_entry_remove_invalid_hash(cs_gc_entry_t *entry)
{
	cs_gc_list_t *curr_gc, *next_gc, *prev_gc;
	cs_vtable_hash_entry_t *hash;
	unsigned long flags;

	if (entry == NULL) return -1;

	prev_gc = NULL;
	curr_gc = entry->fwd_hash_list;
	spin_lock_irqsave(&entry->lock, flags);
	while (curr_gc != NULL) {
		next_gc = curr_gc->next;
		hash = (cs_vtable_hash_entry_t*)curr_gc->hash;
		if (hash->state == CS_VHASH_STATE_INVALID) {
			if (curr_gc->prev != NULL)
				curr_gc->prev->next = curr_gc->next;
			else
				entry->fwd_hash_list = curr_gc->next;
			if (curr_gc->next != NULL)
				curr_gc->next->prev = curr_gc->prev;
			atomic_dec(&hash->gc_users);
			kfree(curr_gc);
			if (atomic_read(&hash->gc_users) == 0) {
				cs_vhash_del_fwd_entry(hash);
				kfree(hash);
			}

			if (prev_gc == NULL) entry->fwd_hash_list = next_gc;
			else prev_gc->next = next_gc;
		} else prev_gc = curr_gc;
		curr_gc = next_gc;
	};

	spin_unlock_irqrestore(&entry->lock, flags);
	return 0;
} /* cs_gc_entry_remove_invalid_hash */

int cs_gc_entry_remove_all_hashes(cs_gc_entry_t *entry)
{
	cs_gc_list_t *curr_gc, *next_gc;
	cs_vtable_hash_entry_t *hash;
	unsigned long flags;

	if (entry == NULL) return -1;

	curr_gc = entry->fwd_hash_list;
	spin_lock_irqsave(&entry->lock, flags);
	while (curr_gc != NULL) {
		next_gc = curr_gc->next;
		hash = (cs_vtable_hash_entry_t*)curr_gc->hash;
		if (curr_gc->prev != NULL)
			curr_gc->prev->next = curr_gc->next;
		else
			entry->fwd_hash_list = curr_gc->next;
		if (curr_gc->next != NULL)
			curr_gc->next->prev = curr_gc->prev;
		kfree(curr_gc);

		/* in this case, we don't care about the gc_users count of the hash 
		 * entry.  We are just going to delete it no matter what. */
		cs_vhash_del_fwd_entry(hash);
		kfree(hash);

		curr_gc = next_gc;
	};
	entry->fwd_hash_list = NULL;

	spin_unlock_irqrestore(&entry->lock, flags);
	return 0;
} /* cs_gc_entry_remove_all_hashes */

int cs_gc_table_cleanup(unsigned int gc_type)
{
	cs_gc_table_t *curr_gc_table = &global_gc_table[gc_type];
	cs_gc_entry_t *curr_gc_entry;
	struct list_head *next;

	if (curr_gc_table == NULL) return -1;

	list_for_each(next, &curr_gc_table->entry_list) {
		curr_gc_entry = (cs_gc_entry_t*)list_entry(next, cs_gc_entry_t, 
				gc_entry_list);

		if (curr_gc_entry->fwd_hash_list != NULL)
			cs_gc_entry_remove_invalid_hash(curr_gc_entry);
		else {
			cs_gc_table_del_entry(curr_gc_entry);

			/* clean up the rslt_lookup */
			if (curr_gc_entry->lookup_entry != NULL) {
				rslt_remove_from_list(curr_gc_entry->lookup_entry);
				kfree(curr_gc_entry->lookup_entry);
			}
			kfree(curr_gc_entry);
		}
	}

	return 0;
} /* cs_gc_table_cleanup */

int cs_gc_table_cleanup_all(void)
{
	unsigned int i;

	for (i = 0; i < FE_GC_TYPE_MAX; i++)
		cs_gc_table_cleanup(i);
	return 0;
} /* cs_gc_table_cleanup_all */

int cs_gc_entry_update_timer(cs_gc_entry_t *entry, unsigned long jiffies)
{
	unsigned long flags;

	if (entry == NULL) return -1;

	spin_lock_irqsave(&entry->lock, flags);
	entry->last_jiffies = jiffies;
	spin_unlock_irqrestore(&entry->lock, flags);
	return 0;
} /* cs_gc_entry_update_timer */
#endif

#if 0	/* old define, do we still need these? */
int cs_gc_table_add_conn(cs_gc_table_t *table, cs_vtable_hash_entry_t *hash)
{
	return 0;
} /* cs_gc_table_add_conn */

int cs_gc_table_clean_conn(cs_gc_table_t *table)
{
	return 0;
} /* cs_gc_table_clean_conn */
#endif

