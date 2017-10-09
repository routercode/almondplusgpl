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
/*
 * cs_hw_accel_ipsec.c
 *
 * $Id: cs_hw_accel_ipsec.c,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This file contains the implementation for CS IPsec Offload Kernel Module.
 */

#include <mach/hw_accel.h>
#include <linux/if_ether.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/esp.h>
#include <net/ah.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/timer.h>
#include <cs_hw_accel_cb.h>
#include <cs_hw_accel_core.h>
#include <cs_hw_accel_base.h>
#include "cs_hw_accel_ipsec.h"
#include <cs752x_eth.h>
#include <linux/etherdevice.h>

#undef CS_IPSEC_DEBUG_MODE
#ifdef CS_IPSEC_DEBUG_MODE
#include <net/inet_ecn.h>
#endif

/* general data base */
static cs_ipsec_db_t ipsec_main_db;
static cs_boolean f_ipsec_enbl = FALSE;

#ifdef CS_IPC_ENABLED
static struct ipc_context *ipc_ctxt0, *ipc_ctxt1;
static struct timer_list cs_ipc_timer;
static unsigned char f_re_status[2] = {CS_IPSEC_IPC_RE_DEAD, CS_IPSEC_IPC_RE_DEAD};
#endif

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"

extern u32 cs_adapt_debug;
#endif /* CONFIG_CS752X_PROC */

#define DBG(x)	if (cs_adapt_debug & CS752X_ADAPT_IPSEC) (x)

extern struct cs_kernel_hw_accel_jt hw_jt;
extern void ni_dm_byte(unsigned int location, int length);

#define MIN(a,b) (((a) <= (b)) ? (a):(b))

#if 1	/* in first release, dir and table id are 1 to 1 mapping */
#define IPSEC_TABLE_ID_BY_DIR(dir) (dir)
#endif


#define CS_FE_LSPID_ARM	CPU_PORT	// Even though we only have 1 ARM core in 
									// FPGA, how do we differ the source port?

#ifdef CS_IPC_ENABLED
#if 0
extern int g2_ipc_send( struct ipc_context *context, unsigned char cpu_id, 
		unsigned char client_id, unsigned char priority, unsigned short msg_no, 
		const void *msg_data, unsigned short msg_size);
extern int g2_ipc_register(unsigned char client_id, 
		const struct g2_ipc_msg *msg_procs, unsigned short msg_count, 
		unsigned short invoke_count, void *private_data, 
		struct ipc_context **context);
extern int g2_ipc_deregister(struct ipc_context *context);
#endif
static cs_status ipsec_ipc_send_stop(unsigned char re_id, unsigned short sa_id);
#endif
static cs_status ipsec_flush_cb_queue(unsigned char table_id, unsigned long long sa_id);

#if 1
/* FIXME! need to define the real function for this in header */
#define CS_IPSEC_CPU_VOQ_ID_FOR_RE(re_id)	(51)
#endif

#if 1
/* FIXME!! TEMPORARY DEFINE.. need to find a real place.. or real function 
 * to retrieve those info */
#define CS_FE_INVALID_SW_ACTION 0x000fffff
#endif

/************************ internal APIs ****************************/
static unsigned char ipsec_get_spidx(unsigned char re_id)
{
	unsigned short curr_mask;
	unsigned char offset = 0;

	if (re_id == 0) curr_mask = ipsec_main_db.re0_spidx_mask;
	else if (re_id == 1) curr_mask = ipsec_main_db.re1_spidx_mask;
	else return 0xff;		/* sp_idx value should be 
							   between 112 to 127 */

	do {
		if (0 == (curr_mask & (0x1 << offset))) {
			curr_mask |= (0x1 << offset);
			if (re_id == 0) ipsec_main_db.re0_spidx_mask = curr_mask;
			else ipsec_main_db.re1_spidx_mask = curr_mask;
			return (CS_IPSEC_SP_IDX_START + offset);
		}
		offset++;
	} while (offset < CS_IPSEC_SP_IDX_NUM);

	return 0xff;
} /* ipsec_get_spidx */

static void ipsec_free_spidx(unsigned char re_id, unsigned char spidx)
{
	if ((spidx < CS_IPSEC_SP_IDX_START) || 
			(spidx >= (CS_IPSEC_SP_IDX_START + CS_IPSEC_SP_IDX_NUM)))
		return;
	if (0 == re_id)
		ipsec_main_db.re0_spidx_mask &= 
			~(0x1 << (spidx - CS_IPSEC_SP_IDX_START));
	else if (1 == re_id)
		ipsec_main_db.re1_spidx_mask &= 
			~(0x1 << (spidx - CS_IPSEC_SP_IDX_START));

	return;
} /* ipsec_free_spidx */

/* CS IPsec SADB handler */
static cs_ipsec_sadb_t *ipsec_get_sadb(unsigned char table_id, unsigned short sa_id)
{
	if (0 == table_id) return &ipsec_main_db.re0_sadb_q[sa_id];
	if (1 == table_id) return &ipsec_main_db.re1_sadb_q[sa_id];
	return NULL;
} /* ipsec_get_sadb */

#if 0	/* not used... */
static cs_status ipsec_find_idx_by_spi(unsigned int spi, unsigned char *p_table, 
		unsigned short *p_idx)
{
	unsigned short iii;

	if ((NULL == p_table) || (NULL == p_idx)) return CS_ERROR;

	for (iii = 0; iii < CS_IPSEC_TUN_NUM; iii++) {
		if (spi == ipsec_main_db.re0_sadb_q[iii].spi) {
			*p_table = 0;
			*p_idx = iii;
			return CS_OK;
		}
			
		if (spi == ipsec_main_db.re1_sadb_q[iii].spi) {
			*p_table = 1;
			*p_idx = iii;
			return CS_OK;
		}
	}
	return CS_ERR_ENTRY_NOT_FOUND;
} /* ipsec_find_idx_by_spi */
#endif

static int ipsec_find_idx_by_xfrm_state(struct xfrm_state *p_x_state, 
		unsigned char *p_table, unsigned short *p_idx)
{
	unsigned short iii;

	if ((NULL == p_x_state) || (NULL == p_table) || (NULL == p_idx))
		return CS_ERROR;

	for (iii = 0; iii < CS_IPSEC_TUN_NUM; iii++) {
		if (p_x_state == ipsec_main_db.re0_sadb_q[iii].x_state) {
			*p_table = 0;
			*p_idx = iii;
			return CS_OK;
		}
			
		if (p_x_state == ipsec_main_db.re1_sadb_q[iii].x_state) {
			*p_table = 1;
			*p_idx = iii;
			return CS_OK;
		}
	}
	return CS_ERR_ENTRY_NOT_FOUND;
} /* ipsec_find_idx_by_xfrm_state */

static cs_status ipsec_get_avail_sa_id(unsigned char table_id, 
		unsigned short *p_sa_id)
{
	unsigned short start_idx, used_count, curr_idx;
	cs_ipsec_sadb_t *p_sadb_table, *p_curr_sadb;

	if (0 == table_id) {
		start_idx = ipsec_main_db.p_re0_idx;
		used_count = ipsec_main_db.re0_used_count;
		p_sadb_table = &ipsec_main_db.re0_sadb_q[0];
	} else if ( 1== table_id) {
		start_idx = ipsec_main_db.p_re1_idx;
		used_count = ipsec_main_db.re1_used_count;
		p_sadb_table = &ipsec_main_db.re1_sadb_q[0];
	}

	if (used_count >= CS_IPSEC_TUN_NUM) return CS_ERROR;

	curr_idx = start_idx;
	p_curr_sadb = p_sadb_table + curr_idx;
	do {
		if (CS_IPSEC_TUN_NUM == curr_idx) {
			curr_idx = 0;
			p_curr_sadb = p_sadb_table;
		}

		if (0 == p_curr_sadb->used) {
			/* found an available sadb to use */
			*p_sa_id = curr_idx;
			return CS_OK;
		}

		curr_idx++;
		p_curr_sadb++;
	} while (curr_idx != start_idx);

	return CS_ERROR;
} /* ipsec_get_avail_sa_id */

static cs_status ipsec_consume_sadb(unsigned char table_id, unsigned short sa_id)
{
	cs_ipsec_sadb_t *p_sadb;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return CS_ERROR;

	if (0 == p_sadb->used) {
		p_sadb->used = 1;
		if (0 == table_id) {
			ipsec_main_db.p_re0_idx = sa_id + 1;
			ipsec_main_db.re0_used_count--;
		}
		if (1 == table_id) {
			ipsec_main_db.p_re1_idx = sa_id + 1;
			ipsec_main_db.re1_used_count--;
		}
	}

	return CS_OK;
} /* ipsec_consume_sadb */

static cs_status ipsec_free_sadb(unsigned char table_id, unsigned short sa_id)
{
	cs_ipsec_sadb_t *p_sadb;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return CS_ERROR;

	if (1 == p_sadb->used) {
		p_sadb->used = 0;
		p_sadb->accelerated = 0;
		p_sadb->stop = 0;
		if (0 == table_id) {
			ipsec_main_db.re0_used_count++;
		}
		if (1 == table_id) {
			ipsec_main_db.re1_used_count++;
		}
	}

	return CS_OK;
} /* ipsec_free_sadb */

static cs_status ipsec_check_xfrm_accelerate(struct xfrm_state *p_x_state)
{
	if ((XFRM_MODE_TUNNEL != p_x_state->props.mode) && 
			(XFRM_MODE_TRANSPORT != p_x_state->props.mode))
	return CS_ERROR;

	/* don't support other encapsulation for now.. such as ESPINUDP. 
	 * ESPINUDP_NON_IKE, and for AH mode, there is no encap */
	if (p_x_state->encap) return CS_ERROR;

	if (IPPROTO_ESP == p_x_state->id.proto) {
		if (p_x_state->aead) {
			if ((0 > cs_ipsec_get_cipher_alg(p_x_state->aead->alg_name)) || 
						(0 > cs_ipsec_get_cipher_mode(p_x_state->props.ealgo)))
				return CS_ERROR;
		} else {
			if ((0 > cs_ipsec_get_cipher_alg(p_x_state->ealg->alg_name)) || 
					(0 > cs_ipsec_get_cipher_mode(p_x_state->props.ealgo)))
			   	return CS_ERROR;
			if (p_x_state->aalg) {
				if (0 >cs_ipsec_get_auth_alg(p_x_state->aalg->alg_name))
					return CS_ERROR;
				if (NULL == xfrm_aalg_get_byname(p_x_state->aalg->alg_name, 0))
					return CS_ERROR;
			}
		}
	} else if (IPPROTO_AH == p_x_state->id.proto) {
		if (0 > cs_ipsec_get_auth_alg(p_x_state->aalg->alg_name))
			return CS_ERROR;
	} else return CS_ERROR;

	return CS_OK;
} /* ipsec_check_xfrm_accelerate */

static cs_status ipsec_update_sadb(unsigned char table_id, unsigned short sa_id, 
		unsigned char dir, struct xfrm_state *p_x_state, unsigned char ip_ver)
{
	cs_ipsec_sadb_t *p_sadb;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return CS_ERROR;

	/* general info */
	p_sadb->spi = p_x_state->id.spi;
	/* IPPROTO_ESP = 50 => p_sadb->proto = 0
	 * IPPOROT_AH = 51  => p_sadb->proto = 1 */
	p_sadb->proto = p_x_state->id.proto - IPPROTO_ESP;
	p_sadb->seq_num = p_x_state->replay.oseq + 1;
	/* advance the sequence number by 1, because this current packet has yet 
	 * updated the real sequence number */
	p_sadb->ip_ver = ip_ver;
	p_sadb->sa_dir = dir;
	p_sadb->accelerated = 0;
	p_sadb->stop = 0;
	p_sadb->x_state = p_x_state;
	p_sadb->lifetime_bytes = MIN(p_x_state->lft.hard_byte_limit, 
			p_x_state->lft.soft_byte_limit);
	p_sadb->bytes_count = p_x_state->curlft.bytes;
	p_sadb->lifetime_packets = MIN(p_x_state->lft.hard_packet_limit, 
			p_x_state->lft.soft_packet_limit);
	p_sadb->packets_count = p_x_state->curlft.packets;	/* add_time and use_time? */

	// FIXME! Debug message
	DBG(printk("%s:%d:spi = 0x%x, proto = %d, seq_num = %d, ", __func__, 
				__LINE__, p_sadb->spi, p_sadb->proto, p_sadb->seq_num));
	DBG(printk("ip_ver = %d, sa_dir = %d\n", p_sadb->ip_ver, p_sadb->sa_dir));
	DBG(printk("\t\tlifetime bytes = %ld, packet = %ld\n", 
				p_sadb->lifetime_bytes, p_sadb->lifetime_packets));
	if (XFRM_MODE_TUNNEL == p_x_state->props.mode)
		p_sadb->mode = 0;	/* 0 for tunnel */
	else if (XFRM_MODE_TRANSPORT == p_x_state->props.mode)
		p_sadb->mode = 1;
	else return CS_ERROR;

	if (p_x_state->id.proto == IPPROTO_ESP) {
		struct esp_data *esp = (struct esp_data *)p_x_state->data;
		struct crypto_aead *aead = esp->aead;

		p_sadb->iv_len = crypto_aead_ivsize(aead) >> 2;

		if (p_x_state->aead) {
			p_sadb->ealg = cs_ipsec_get_cipher_alg(p_x_state->aead->alg_name);
			p_sadb->ealg_mode = cs_ipsec_get_cipher_mode(p_x_state->props.ealgo);
			if ((0 > p_sadb->ealg) || (0 > p_sadb->ealg_mode)) return CS_ERROR;

			p_sadb->enc_keylen = (p_x_state->aead->alg_key_len +7) >> 5;
			memcpy(p_sadb->ekey, p_x_state->aead->alg_key, 
					(p_sadb->enc_keylen << 2));

			/* in AEAD mode, there is no authentication algorithm */
			p_sadb->aalg = CS_IPSEC_AUTH_NULL;
			memset(p_sadb->akey, 0x00, sizeof(unsigned int) * MAX_AUTH_KEY_LEN);
			p_sadb->auth_keylen = 0;

			/* but still need to define ICV length */
#if 0
			p_sadb->icv_trunclen = p_x_state->aead->alg_icv_len >> 5;
#endif
			p_sadb->icv_trunclen = crypto_aead_authsize(aead);
			DBG(printk("%s:%d:p_x_state->aead->alg_icv_len %d vs aead_authsize %d\n", 
					__func__, __LINE__, p_x_state->aead->alg_icv_len >> 5, 
					crypto_aead_authsize(aead)));
		} else {
			struct xfrm_algo_desc *ealg_desc;

			p_sadb->ealg = cs_ipsec_get_cipher_alg(p_x_state->ealg->alg_name);
			p_sadb->ealg_mode = cs_ipsec_get_cipher_mode(p_x_state->props.ealgo);
			if ((0 > p_sadb->ealg) || (0 > p_sadb->ealg_mode)) return CS_ERROR;

			p_sadb->enc_keylen = (p_x_state->ealg->alg_key_len + 7) >> 5;
			memcpy(p_sadb->ekey, p_x_state->ealg->alg_key, 
					p_sadb->enc_keylen << 2);

			/* get IV length */
			ealg_desc = xfrm_ealg_get_byname(p_x_state->ealg->alg_name, 0);
			if (p_x_state->aalg) {
				struct xfrm_algo_desc *aalg_desc;

				p_sadb->aalg = cs_ipsec_get_auth_alg(p_x_state->aalg->alg_name);
				if (0 > p_sadb->aalg) return CS_ERROR;

				p_sadb->auth_keylen = (p_x_state->aalg->alg_key_len + 7) >> 5;
				memcpy(p_sadb->akey, p_x_state->aalg->alg_key, 
						p_sadb->auth_keylen << 2);

				/* get ICV (integrity check value) truncated length */
				aalg_desc = xfrm_aalg_get_byname(p_x_state->aalg->alg_name, 0);
				if (NULL == aalg_desc) return CS_ERROR;

				p_sadb->icv_trunclen = aalg_desc->uinfo.auth.icv_truncbits >> 5;
			} else {
				p_sadb->aalg = CS_IPSEC_AUTH_NULL;
				memset(p_sadb->akey, 0x00, sizeof(unsigned int) * MAX_AUTH_KEY_LEN);
				p_sadb->auth_keylen = 0;
				p_sadb->icv_trunclen = 0;
			}
		}
		// FIXME! debug message
		DBG(printk("%s:%d:ealg %x, ealg_mode %x, enc_keylen %x, iv_len %x", 
					__func__, __LINE__, p_sadb->ealg, p_sadb->ealg_mode, 
					p_sadb->enc_keylen, p_sadb->iv_len));
		DBG(printk("aalg %x, auth_keylen %x, icv_trunclen %x\n", p_sadb->aalg, 
					p_sadb->auth_keylen, p_sadb->icv_trunclen));
	} else if (IPPROTO_AH == p_x_state->id.proto) {
		struct ah_data *ahp = (struct ah_data *)p_x_state->data;

		/* set all the unused field to 0 */
		p_sadb->enc_keylen = 0;
		p_sadb->ealg = 0;
		p_sadb->ealg_mode = 0;
		memset(p_sadb->ekey, 0x00, MAX_ENC_KEY_LEN);

		/* set the real authentication-required value */
		p_sadb->aalg = cs_ipsec_get_auth_alg(p_x_state->aalg->alg_name);
		if (0 > p_sadb->aalg) return CS_ERROR;

		p_sadb->auth_keylen = (p_x_state->aalg->alg_key_len + 7) >> 5;
		memcpy(p_sadb->akey, p_x_state->aalg->alg_key, p_sadb->auth_keylen << 2);
		p_sadb->icv_trunclen = ahp->icv_trunc_len >> 2;
	} else return CS_ERROR;

	if (CS_IPSEC_IPV4 == ip_ver) {
		p_sadb->tunnel_saddr.addr = p_x_state->props.saddr.a4;
		p_sadb->tunnel_daddr.addr = p_x_state->id.daddr.a4;
	} else if (CS_IPSEC_IPV6 == ip_ver) {
#if 0	// FIXME. disable IPv6 for now
		ipv6_addr_copy((struct in6_addr *)&p_sadb->tunnel_saddr.addr6, 
				(struct in6_addr *)&p_x_state->props.saddr.a6);
		ipv6_addr_copy((struct in6_addr *)&p_sadb->tunnel_daddr.addr6, 
				(struct in6_addr *)&p_x_state->id.daddr.a6);
#endif
	} else return CS_ERROR;

	/* need to create a ip header and calculate a checksum for outbound case */
	if (CS_IPSEC_OUTBOUND == dir) {
		if (CS_IPSEC_IPV4 == ip_ver) {
			struct iphdr iph;

			iph.ihl = 5;
			iph.version = IPVERSION;
			iph.tos = 0;
			iph.tot_len = 0;
			iph.id = 0;
			iph.frag_off = 0;
			iph.ttl = 64;
			iph.protocol = p_x_state->id.proto;
			iph.saddr = p_sadb->tunnel_saddr.addr;
			iph.daddr = p_sadb->tunnel_daddr.addr;
			ip_send_check(&iph);
			p_sadb->checksum = iph.check;
		}
	}

	return CS_OK;
} /* ipsec_update_sadb */

static cs_status ipsec_check_and_create_sadb(unsigned char dir, 
		struct xfrm_state *p_x_state, unsigned char ip_ver, unsigned char *p_table_id, 
		unsigned short *p_sa_id)
{
	cs_status status;
	unsigned short sa_id;
	unsigned char table_id;

	if ((NULL == p_x_state) || (NULL == p_table_id) || (NULL == p_sa_id))
		return CS_ERROR;

	table_id = IPSEC_TABLE_ID_BY_DIR(dir);

	if (CS_OK != ipsec_check_xfrm_accelerate(p_x_state))
		return CS_ERROR;

	status = ipsec_get_avail_sa_id(table_id, &sa_id);
	if (status != CS_OK) return status;

	/* consume the sadb first, in case if there are multiple access, by doing so 
	 * other processes won't allocate this resource */
	ipsec_consume_sadb(table_id, sa_id);

	status = ipsec_update_sadb(table_id, sa_id, dir, p_x_state, ip_ver);
	if (status != CS_OK)
		ipsec_free_sadb(table_id, sa_id);

	*p_table_id = table_id;
	*p_sa_id = sa_id;

	return status;
} /* ipsec_check_and_create_sadb */

static cs_status ipsec_stop_sadb(unsigned char table_id, unsigned short sa_id)
{
	cs_ipsec_sadb_t *p_sadb;

	/* 1) set it in Kernel sadb data structure that the acceleration applied 
	 * on this SADB is going to stop. */
	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return CS_ERROR;
	p_sadb->stop = 1;

	/* 2) disable all the hash entries that are related to the 
	 * gu_id */
	cs_kernel_core_del_hash_by_guid(CS_IPSEC_GID(table_id, sa_id));
	/* ignoring all the possible errors from deleting hash. even though 
	 * they are somehow critical */

	/* 3) also need to check it's cb queue to delete the entry */
	ipsec_flush_cb_queue(table_id, sa_id);
	
#ifdef CS_IPC_ENABLED
	/* 4) send ipc message to RE to notify this tunnel is no longer 
	 * accelerated and handled by RE */
	ipsec_ipc_send_stop(table_id, sa_id);
	/* ignoring all the possible erros here too. */
#endif

	/* 5) all the rest of packets will be enqueued, until the module receive 
	 * a "stop complete" from IPsec Offload engine. */
	return CS_OK;
} /* ipsec_stop_sadb */

static void ipsec_stop_all_sadb(unsigned char table_id)
{
	unsigned short sa_id;

	for (sa_id = 0; sa_id < CS_IPSEC_TUN_NUM; sa_id++) {
		ipsec_stop_sadb(table_id, sa_id);
	}
	return;
} /* ipsec_stop_all_sadb */

static cs_status ipsec_get_table_sa_id_by_sp_guid(unsigned long long guid, 
		unsigned char *p_table_id, unsigned short *p_sa_id)
{
	unsigned char table_id;
	unsigned short sp_id, count;
	cs_ipsec_cb_queue_t *p_cb_queue;

	table_id = CS_IPSEC_GET_RE_ID_FROM_SP_GID(guid);
	sp_id = CS_IPSEC_GET_SP_ID_FROM_SP_GID(guid);

	count = 0;
	do {
		if (0 == table_id) p_cb_queue = ipsec_main_db.re0_cb_q[count];
		else p_cb_queue = ipsec_main_db.re1_cb_q[count];
		if (NULL != p_cb_queue) {
			do {
				if (sp_id == p_cb_queue->idx) {
					*p_table_id = table_id;
					*p_sa_id = count;
					return CS_OK;
				}
				p_cb_queue = p_cb_queue->next;
			} while (p_cb_queue != NULL);
		}
		count++;
	} while (count < CS_IPSEC_TUN_NUM);
	return CS_ERR_ENTRY_NOT_FOUND;
} /* ipsec_get_table_sa_id_by_sp_guid */

static cs_status ipsec_get_table_sa_id_by_guid(unsigned long long guid, 
		unsigned char *p_table_id, unsigned short *p_sa_id)
{
	unsigned short tag_id;

	tag_id = CS_IPSEC_GET_TAG_ID_FROM_GID(guid);

	/* the guid is not for IPsec Offload */
	if (CS_KERNEL_IPSEC_ID_TAG != tag_id)
		return CS_ERROR;

	if (guid & CS_IPSEC_SPGID_MASK) {
		/* Special guid ID case. need to look for the table that's currently 
		 * using the SP guid */
		return ipsec_get_table_sa_id_by_sp_guid(guid, p_table_id, p_sa_id);
	} else {
		*p_sa_id = CS_IPSEC_GET_SA_ID_FROM_GID(guid);
		*p_table_id = CS_IPSEC_GET_RE_ID_FROM_GID(guid);
		return CS_OK;
	}
} /* ipsec_get_table_sa_id_by_guid */

static cs_status ipsec_stop_sadb_by_guid(unsigned long long guid)
{
	cs_status status;
	unsigned char table_id;
	unsigned short sa_id;
	cs_ipsec_sadb_t *p_sadb;

	status = ipsec_get_table_sa_id_by_guid(guid, &table_id, &sa_id);
	if (status != CS_OK) return CS_ERROR;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return CS_ERROR;

	status = ipsec_stop_sadb(table_id, sa_id);

	return status;
} /* ipsec_stop_sadb_by_guid */

/* skb handling */
/* the following API is used for calculate the original skb size of
 * a skb that's already in ESP/AH process */
static unsigned short ipsec_calc_orig_skb_size(struct sk_buff *skb)
{
	unsigned short curr_size;

	curr_size = skb->len;

	/* now skb->data is pointing to either AH or ESP header. 
	 * Current skb->len does not include ethernet header and IP header */

	/* add in the IP header length */
	curr_size += ip_hdrlen(skb);

	/* add in ethernet header length */
	curr_size += ETH_HLEN;

	return curr_size;
} /* ipsec_calc_orig_skb_size */

/* Recovery mechanism */
static cs_status ipsec_enqueue_skb_to_sadb(unsigned char table_id, 
		unsigned short sa_id, struct sk_buff *skb)
{
	cs_ipsec_skb_queue_t *p_skb_queue, *p_curr_skb_queue;
	unsigned char pkt_cnt = 0;

	if ((0 != table_id) && (1 != table_id)) return CS_ERROR;
	if (sa_id >= CS_IPSEC_TUN_NUM) return CS_ERROR;
	if (NULL == skb) return CS_ERROR;

	p_skb_queue = kmalloc(sizeof(cs_ipsec_skb_queue_t), GFP_ATOMIC);
	if (NULL == p_skb_queue) return CS_ERROR;
	p_skb_queue->skb = skb;
	p_skb_queue->next = NULL;

	p_curr_skb_queue = NULL;
	if (0 == table_id) {
		if (NULL == ipsec_main_db.re0_skb_q[sa_id])
			ipsec_main_db.re0_skb_q[sa_id] = p_skb_queue;
		else
			p_curr_skb_queue = ipsec_main_db.re0_skb_q[sa_id];
	} else { /* if (1 == table_id) */
		if (NULL == ipsec_main_db.re1_skb_q[sa_id])
			ipsec_main_db.re1_skb_q[sa_id] = p_skb_queue;
		else
			p_curr_skb_queue = ipsec_main_db.re1_skb_q[sa_id];
	}
	/* the original list is not empty, so we have to enqueue this skb to 
	 * the end of the list */
	if (NULL != p_curr_skb_queue) {
		while (NULL != p_curr_skb_queue->next) {
			p_curr_skb_queue = p_curr_skb_queue->next;
			pkt_cnt++;
		};
		// FIXME! debug message..
		DBG(printk("%s:%d:current queue count = %d\n", __func__, __LINE__, 
					pkt_cnt));
		if (pkt_cnt >= CS_IPSEC_CB_QUEUE_MAX) {
			/* we are queuing too many packets.. going to drop this one..
			 * in case of infinite loop */
			kfree(p_skb_queue);
			kfree_skb(skb);
		} else p_curr_skb_queue->next = p_skb_queue;
	}

	return CS_OK;
} /* ipsec_enqueue_skb_to_sadb */

static struct sk_buff *ipsec_dequeue_skb_from_sadb(unsigned char table_id, 
		unsigned short sa_id)
{
	cs_ipsec_skb_queue_t *p_skb_queue;
	struct sk_buff *rslt_skb = NULL;

	if ((0 != table_id) && (1 != table_id)) return NULL;
	if (sa_id >= CS_IPSEC_TUN_NUM) return NULL;

	if ((0 == table_id) && (NULL != ipsec_main_db.re0_skb_q[sa_id])) {
		/* if there are skb queued in the list of table#0+sa_id, dequeue it */
		p_skb_queue = ipsec_main_db.re0_skb_q[sa_id];
		ipsec_main_db.re0_skb_q[sa_id] = p_skb_queue->next;
		rslt_skb = p_skb_queue->skb;
		kfree(p_skb_queue);
	} else if (NULL != ipsec_main_db.re1_skb_q[sa_id]) {
		/* if there are skb queued in the list of table#1+sa_id, dequeue it */
		p_skb_queue = ipsec_main_db.re1_skb_q[sa_id];
		ipsec_main_db.re1_skb_q[sa_id] = p_skb_queue->next;
		rslt_skb = p_skb_queue->skb;
		kfree(p_skb_queue);
	}

	return rslt_skb;
} /* ipsec_dequeue_skb_from_sadb */

static cs_status ipsec_peek_skb_queue_of_sadb(unsigned char table_id, 
		unsigned short sa_id)
{
	if ((0 != table_id) && (1 != table_id)) return CS_ERROR;
	if (sa_id >= CS_IPSEC_TUN_NUM) return CS_ERROR;

	if ((0 == table_id) && (NULL != ipsec_main_db.re0_skb_q[sa_id]))
		return CS_OK;
	return CS_ERROR;
} /* ipsec_peek_skb_queue_of_sadb */

static cs_status ipsec_create_internal_hash(unsigned char table_id, 
		unsigned short sa_id, struct sk_buff *skb, unsigned char sp_idx)
{
	cs_ipsec_sadb_t *p_sadb;
	unsigned long long sp_guid = CS_IPSEC_SPGID(table_id, sp_idx);
	CS_HASH_ENTRY_S hash_entry;
	unsigned char func_id = 0;
	unsigned int rc;
	struct iphdr *iph = ip_hdr(skb);
	cs_status status;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (p_sadb == NULL) return CS_ERROR;

	memset(&hash_entry, 0x0, sizeof(CS_HASH_ENTRY_S));

	if (p_sadb->sa_dir == CS_IPSEC_INBOUND)
		func_id += 2;
	if (p_sadb->proto == IPPROTO_AH)
		func_id += 1;

	/* 1st hash entry
	 * mask/field:
	 * 1. port cpu port id
	 * 2. source mac address (kernel modifies it to the recirc_idx (112~127)
	 * 3. L3 source IP, dst IP, IP protocol, and IP frag (don't care)
	 * 4. L4 (if exists): source port and dst port
	 * action:
	 * 1. sw_action value
	 * 2. destination voq = RE VOQ */

	hash_entry.mask = CS_HM_LSPID_MASK | CS_HM_MAC_SA_MASK;
	hash_entry.mask |= CS_HM_RECIRC_IDX_MASK;
	hash_entry.key.lspid = CS_FE_LSPID_ARM;
	hash_entry.key.mac_sa[0] = sp_idx;
	hash_entry.key.recirc_idx = sp_idx;

#if 1	/* set it to 0, if attempts to take out range match vector */
	hash_entry.mask |= CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK;
	status = cs_kernel_get_pktlen_rng_match_vector(CS_IPSEC_RE_SKB_SIZE_MAX, 
			CS_IPSEC_RE_SKB_SIZE_MIN, &hash_entry.key.pktlen_rng_match_vector);
	DBG(printk("%s:%d:lspid = %d, recirc_idx = %d, rng_match_vector = %x\n", 
				__func__, __LINE__, hash_entry.key.lspid, 
				hash_entry.key.recirc_idx, 
				hash_entry.key.pktlen_rng_match_vector));
	if (status != CS_OK) return status;
#endif

	if (p_sadb->ip_ver == CS_IPSEC_IPV4) {
		hash_entry.mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
		hash_entry.mask |= CS_HM_IP_PROT_MASK;
		hash_entry.key.sa[0] = ntohl(iph->saddr);
		hash_entry.key.da[0] = ntohl(iph->daddr);
		hash_entry.key.ip_prot = iph->protocol;
		// FIXME!! debug for now
		hash_entry.mask |= CS_HM_SPI_MASK;// | CS_HM_IP_FRAGMENT_MASK;
		hash_entry.mask |= CS_HM_SPI_VLD_MASK;
		hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		DBG(printk("%s:%d:sip %x, dip %x, proto %d\n", __func__, __LINE__, 
					iph->saddr, iph->daddr, iph->protocol));
		if (iph->protocol == IPPROTO_ESP) {
			struct ip_esp_hdr *esph;
			esph = (struct ip_esp_hdr*)((void*)iph + (iph->ihl << 2));
			hash_entry.key.spi_vld = 1;
			hash_entry.key.spi_idx = ntohl(esph->spi);
			hash_entry.mask |= CS_HM_SPI_MASK | CS_HM_SPI_VLD_MASK;
		} else if (iph->protocol == IPPROTO_AH) {
			struct ip_auth_hdr *ahh;
			ahh = (struct ip_auth_hdr*)((void*)iph + (iph->ihl << 2));
			hash_entry.key.spi_vld = 1;
			hash_entry.key.spi_idx = ntohl(ahh->spi);
			hash_entry.mask |= CS_HM_SPI_MASK | CS_HM_SPI_VLD_MASK;
		} else if (iph->protocol == IPPROTO_TCP) {
			struct tcphdr *tcph;
			/* check if L4 SPORT/DPORT is needed */
			tcph = (struct tcphdr*)((void*)iph + (iph->ihl << 2));
			hash_entry.key.l4_sp = ntohs(tcph->source);
			hash_entry.key.l4_dp = ntohs(tcph->dest);
			hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		} else if (iph->protocol == IPPROTO_UDP) {
			struct udphdr *udph;
			/* check if L4 SPORT/DPORT is needed */
			udph = (struct udphdr*)((void*)iph + (iph->ihl << 2));
			hash_entry.key.l4_sp = ntohs(udph->source);
			hash_entry.key.l4_dp = ntohs(udph->dest);
			hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		}
	} else {	/* if (CS_IPSEC_IPV6 == p_sadb->ip_ver) */
		/* FIXME! do it later */
	}

	hash_entry.result.action.sw_action = 1;
	hash_entry.result.action.d_voq_id = 1;
	hash_entry.result.action.mac_sa_replace_en = 1;
	hash_entry.result.param.sw_action_id = CS_IPSEC_SW_ACTION_ID(func_id, 
			sp_idx, sa_id);
	hash_entry.result.param.d_voq = CS_IPSEC_RE_VOQ_ID(table_id);
	hash_entry.result.param.mac_sa[0] = sp_idx;

	/* when creating hash, put in the guid */
	/* if unable to create hash entry, return CS_ERROR */
	rc = cs_kernel_core_add_hash(&hash_entry, CS_VTBL_ID_L3_FLOW, 
			&sp_guid, 1, CS_IPSEC_HASH_TIME_OUT);
	if (rc == CS_HASH_ADD_ERR_OUT_OF_RESOURCE)
		return CS_ERROR;

	/* this 2nd hash entry requirement has been shrinked down. It's because 
	 * of the following reasons:
	 * 1) In inbound traffic, at this point, we don't know the content inside 
	 * of the encrypted data.
	 * 2) In outbound traffic, we can estimate the resulted data after the 
	 * packet has been processed by IPsec Offload Engine */

	/* 2nd hash entry..
	 * mask/field:
	 * 1. port: re port id
	 * 2. recirc_idx
	 * 3. L2 source mac
	 * 4. L3 source IP, dst IP, ip protocol, and IP frag (don't care)
	 * 				(only for outbound traffic)
	 * 5. SPI		(only for outbound traffic)
	 * Action:
	 * 1. sw_action value
	 * 2. destination voq = cpu voq for packet from RE */
	memset(&hash_entry, 0x0, sizeof(CS_HASH_ENTRY_S));

	hash_entry.mask = CS_HM_LSPID_MASK;
	hash_entry.mask |= CS_HM_RECIRC_IDX_MASK | CS_HM_MAC_SA_MASK;

	if (CS_IPSEC_RCPU_DEC == table_id)
		hash_entry.key.lspid = ENCRYPTION_PORT;
	else
		hash_entry.key.lspid = ENCAPSULATION_PORT;
	hash_entry.key.mac_sa[0] = sp_idx;
	hash_entry.key.recirc_idx = sp_idx;

#if 0	/* taking out range match vector for now */
	hash_entry.mask |= CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK;
	status = cs_kernel_get_pktlen_rng_match_vector(CS_IPSEC_RE_SKB_SIZE_MAX, 
			CS_IPSEC_RE_SKB_SIZE_MIN, &hash_entry.key.pktlen_rng_match_vector);
	DBG(printk("%s:%d:lspid = %d, recirc_idx = %d, rng_match_vector = %x\n", 
				__func__, __LINE__, hash_entry.key.lspid, 
				hash_entry.key.recirc_idx, 
				hash_entry.key.pktlen_rng_match_vector));
	if (status != CS_OK) return status;
#endif

	/* only in outbound case we include the following header info */
	if (p_sadb->sa_dir == CS_IPSEC_OUTBOUND) {
		hash_entry.mask |= CS_HM_IP_PROT_MASK | CS_HM_SPI_MASK;
		hash_entry.mask |= CS_HM_SPI_VLD_MASK;
		hash_entry.key.ip_prot = p_sadb->proto + IPPROTO_ESP;
		hash_entry.key.spi_vld = 1;
		hash_entry.key.spi_idx = ntohl(p_sadb->spi);
		// FIXME!! debug for now
		hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		if (p_sadb->ip_ver == CS_IPSEC_IPV4) {
			hash_entry.mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
			hash_entry.key.sa[0] = ntohl(p_sadb->tunnel_saddr.addr);
			hash_entry.key.da[0] = ntohl(p_sadb->tunnel_daddr.addr);
		} else {	/* if (CS_IPSEC_IPV6 == p_sadb->ip_ver) */
			hash_entry.mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
#if 0	// FIXME! disable IPv6 for now
			ipv6_addr_copy((struct in6_addr *)hash_entry.key.sa, 
					(struct in6_addr *)&p_sadb->tunnel_saddr.addr6);
			ipv6_addr_copy((struct in6_addr *)hash_entry.key.da, 
					(struct in6_addr *)&p_sadb->tunnel_daddr.addr6);
#endif
		}
	}

	hash_entry.result.action.sw_action = 1;
	hash_entry.result.action.d_voq_id = 1;
	hash_entry.result.param.sw_action_id = CS_IPSEC_SW_ACTION_ID(func_id, 
			sp_idx, sa_id);
	hash_entry.result.param.d_voq = CS_IPSEC_CPU_VOQ_ID_FOR_RE(table_id);
	/* when creating hash, put in the guid */
	/* if unable to create hash entry, return CS_ERROR */
	// FIXME.. the following should be CS_VTBL_ID_IPSEC.. but we don't have it 
	// at the first release... fix it later.
	//rc = cs_kernel_core_add_hash(&hash_entry, CS_VTBL_ID_IPSEC, 
	rc = cs_kernel_core_add_hash(&hash_entry, CS_VTBL_ID_L3_FLOW, 
			&sp_guid, 1, CS_IPSEC_HASH_TIME_OUT);
	if (rc == CS_HASH_ADD_ERR_OUT_OF_RESOURCE)
		return CS_ERROR;

	return CS_OK;
} /* ipsec_create_internal_hash */

static void ipsec_set_skb_cb_info(unsigned char table_id, unsigned short sa_id, 
		struct sk_buff *skb)
{
	int status;
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	cs_ipsec_sadb_t *p_sadb;
	struct iphdr *iph = ip_hdr(skb);	/* NOTE! assuming ip_hdr loc is given 
										   before entering this function */
		
	status = cs_kernel_add_guid(CS_IPSEC_GID(table_id, sa_id), skb);
	if (status != CS_OK) return;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (p_sadb == NULL) return;

	/* Set the hash mask here, but cannot insert / complete action.  Hash 
	 * entries will be created when cs_hw_accel_ipsec_create_hash() is called 
	 * by core logic at the packet transmission stage. */

	cs_cb->input_mask |= CS_HM_ORIG_LSPID_MASK | CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK;
	cs_cb->common.module_mask |= CS_MOD_MASK_IPSEC;

	cs_cb->key_misc.pkt_len_low = CS_IPSEC_RE_SKB_SIZE_MIN;
	cs_cb->key_misc.pkt_len_high = CS_IPSEC_RE_SKB_SIZE_MAX;

	if (p_sadb->ip_ver == CS_IPSEC_IPV4) {
		cs_cb->input_mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK | 
			CS_HM_IP_PROT_MASK;
		if ((p_sadb->sa_dir == CS_IPSEC_OUTBOUND) && 
				(p_sadb->mode == 0) &&	/* Make sure it's tunnel mode */
				(iph->protocol == IPPROTO_IPIP)){
			/* need to locate the inner_iph */
			iph = (struct iphdr *)skb->data;
		}
		cs_cb->input.l3_nh.iph.sip = iph->saddr;
		cs_cb->input.l3_nh.iph.dip = iph->daddr;
		cs_cb->input.l3_nh.iph.protocol = iph->protocol;

		if (IPPROTO_ESP == iph->protocol) {
			struct ip_esp_hdr *esph;
			esph = (struct ip_esp_hdr*)((void*)iph + (iph->ihl << 2));
			cs_cb->input.l4_h.ah_esp.spi = esph->spi;
			cs_cb->input_mask |= CS_HM_SPI_VLD_MASK | CS_HM_SPI_MASK;
			cs_cb->input_mask |= CS_HM_IP_FRAGMENT_MASK;
			cs_cb->input.l3_nh.iph.frag = 0x00;
		} else if (IPPROTO_AH == iph->protocol) {
			struct ip_auth_hdr *ahh;
			ahh = (struct ip_auth_hdr*)((void*)iph + (iph->ihl << 2));
			cs_cb->input.l4_h.ah_esp.spi = ahh->spi;
			cs_cb->input_mask |= CS_HM_SPI_VLD_MASK | CS_HM_SPI_MASK;
			cs_cb->input_mask |= CS_HM_IP_FRAGMENT_MASK;
			cs_cb->input.l3_nh.iph.frag = 0x00;
		} else if (IPPROTO_TCP == iph->protocol) {
			struct tcphdr *tcph;
			/* check if L4 SPORT/DPORT is needed */
			tcph = (struct tcphdr*)((void*)iph + (iph->ihl << 2));
			cs_cb->input.l4_h.th.sport = tcph->source;
			cs_cb->input.l4_h.th.dport = tcph->dest;
			cs_cb->input_mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		} else if (IPPROTO_UDP == iph->protocol) {
			struct udphdr *udph;
			/* check if L4 SPORT/DPORT is needed */
			udph = (struct udphdr*)((void*)iph + (iph->ihl << 2));
			cs_cb->input.l4_h.uh.sport = udph->source;
			cs_cb->input.l4_h.uh.dport = udph->dest;
			cs_cb->input_mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		}
	} else {	/* if (CS_IPSEC_IPV6 == p_sadb->ip_ver) */
		cs_cb->input_mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
		/* FIXME!! more for IPv6 stuff..? */
	}

	/* mark the packet to acceleration capable */
	cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;

	// FIXME!! Wen 2
	DBG(printk("%s:%d:successfully insert skb cb for Ipsec\n", __func__, 
				__LINE__));
	return;
} /* ipsec_set_skb_cb_info */

static cs_status ipsec_store_skb_cb_info(unsigned char table_id, unsigned short sa_id, 
		struct sk_buff *skb, unsigned char sp_idx)
{
	cs_ipsec_cb_queue_t *p_cb_queue, *p_curr_cb_queue = NULL;

	p_cb_queue = kmalloc(sizeof(cs_ipsec_cb_queue_t), GFP_ATOMIC);
	if (NULL == p_cb_queue) return CS_ERROR;

	memcpy(&p_cb_queue->skb, skb, sizeof(struct sk_buff));
	p_cb_queue->idx = sp_idx;
	p_cb_queue->next = NULL;

	if (table_id == 0) {
		if (ipsec_main_db.re0_cb_q[sa_id] == NULL)
			ipsec_main_db.re0_cb_q[sa_id] = p_cb_queue;
		else
			p_curr_cb_queue = ipsec_main_db.re0_cb_q[sa_id];
	} else if (table_id == 1) {
		if (ipsec_main_db.re1_cb_q[sa_id] == NULL)
			ipsec_main_db.re1_cb_q[sa_id] = p_cb_queue;
		else
			p_curr_cb_queue = ipsec_main_db.re1_cb_q[sa_id];
	} else {
		kfree(p_cb_queue);
		return CS_ERROR;
	}
	if (p_curr_cb_queue != NULL) {
		while (p_curr_cb_queue->next != NULL) {
			p_curr_cb_queue = p_curr_cb_queue->next;
		};
		p_curr_cb_queue->next = p_cb_queue;
	}
	return CS_OK;
} /* ipsec_store_skb_cb_info */

static cs_status ipsec_flush_cb_queue(unsigned char table_id, unsigned long long sa_id)
{
	cs_ipsec_cb_queue_t *p_cb_queue;

	if ((0 == table_id) && (NULL != ipsec_main_db.re0_cb_q[sa_id])) {
		do {
			p_cb_queue = ipsec_main_db.re0_cb_q[sa_id];
			ipsec_main_db.re0_cb_q[sa_id] = p_cb_queue->next;
			cs_kernel_core_del_hash_by_guid(CS_IPSEC_SPGID(table_id, 
						p_cb_queue->idx));
			ipsec_free_spidx(table_id, p_cb_queue->idx);
			kfree(p_cb_queue);
		} while (NULL != ipsec_main_db.re0_cb_q[sa_id]);
	} else if ((1 == table_id) && (NULL != ipsec_main_db.re1_cb_q[sa_id])) {
		do {
			p_cb_queue = ipsec_main_db.re1_cb_q[sa_id];
			ipsec_main_db.re1_cb_q[sa_id] = p_cb_queue->next;
			cs_kernel_core_del_hash_by_guid(CS_IPSEC_SPGID(table_id, 
						p_cb_queue->idx));
			ipsec_free_spidx(table_id, p_cb_queue->idx);
			kfree(p_cb_queue);
		} while (NULL != ipsec_main_db.re1_cb_q[sa_id]);
	}
	return CS_OK;
} /* ipsec_flush_cb_queue */

static void ipsec_copy_useful_skb_info(struct sk_buff *dst_skb, 
		struct sk_buff *old_skb)
{
	dst_skb->sk = old_skb->sk;
	memcpy(&dst_skb->tstamp, &old_skb->tstamp, sizeof(ktime_t));
	dst_skb->dev = old_skb->dev;
	dst_skb->_skb_dst = old_skb->_skb_dst;
#ifdef CONFIG_XFRM
	if (old_skb->sp != NULL)
		memcpy(dst_skb->sp, old_skb->sp, sizeof(struct sec_path));
#endif
	memcpy(dst_skb->cb, old_skb->cb, 48 + sizeof(CS_KERNEL_ACCEL_CB_T));
	dst_skb->priority = old_skb->priority;
	dst_skb->sk = old_skb->sk;
	dst_skb->local_df = old_skb->local_df;
	dst_skb->cloned = old_skb->cloned;
	dst_skb->ip_summed = old_skb->ip_summed;
	dst_skb->nohdr = old_skb->nohdr;
	dst_skb->nfctinfo = old_skb->nfctinfo;
	dst_skb->pkt_type = old_skb->pkt_type;
	dst_skb->fclone = old_skb->fclone;
	dst_skb->ipvs_property = old_skb->ipvs_property;
	dst_skb->peeked = old_skb->peeked;
	dst_skb->nf_trace = old_skb->nf_trace;
	dst_skb->protocol = old_skb->protocol;
	dst_skb->destructor = old_skb->destructor;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	dst_skb->nfct = old_skb->nfct;
	dst_skb->nfct_reasm = old_skb->nfct_reasm;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	dst_skb->nf_bridge = old_skb->nf_bridge;
#endif
	dst_skb->iif = old_skb->iif;
#ifdef CONFIG_NET_SCHED
	dst_skb->tc_index = old_skb->tc_index;
#ifdef CONFIG_NET_CLS_ACT
	dst_skb->tc_verd = old_skb->tc_verd;
#endif
#endif
	dst_skb->queue_mapping = old_skb->queue_mapping;
#ifdef CONFIG_IPV6_NDISC_NODETYPE
	dst_skb->ndisc_nodetype = old_skb->ndisc_nodetype;
#endif
#ifdef CONFIG_NETWORK_SECMARK
	dst_skb->secmark = old_skb->secmark;
#endif
	dst_skb->mark = old_skb->mark;
	dst_skb->vlan_tci = old_skb->vlan_tci;
	dst_skb->users = old_skb->users;
} /* ipsec_copy_useful_skb_info */

static cs_status ipsec_replace_skb_cb_info(unsigned char table_id, 
		unsigned short sa_id, unsigned char sp_idx, struct sk_buff *skb)
{
	cs_ipsec_cb_queue_t *p_cb_queue, *p_prev_cb_queue = NULL;

	if (table_id == 0)
		p_cb_queue = ipsec_main_db.re0_cb_q[sa_id];
	else if (table_id == 1)
		p_cb_queue = ipsec_main_db.re1_cb_q[sa_id];
	else return CS_ERROR;

	if (p_cb_queue == NULL) {
		DBG(printk("%s:%d:can't find pre-existing CB queue\n", __func__, 
					__LINE__));
		return CS_ERROR;
	}

	do {
		if (p_cb_queue->idx == sp_idx) {
			/* remove this item off the list */
			if (p_prev_cb_queue != NULL) {
				p_prev_cb_queue->next = p_cb_queue->next;
			} else {
				if (table_id == 0)
					ipsec_main_db.re0_cb_q[sa_id] = p_cb_queue->next;
				else	/* if (1 == table_id) */
					ipsec_main_db.re1_cb_q[sa_id] = p_cb_queue->next;
			}

			/* now copy the skb info and then delete p_cb_queue */
			ipsec_copy_useful_skb_info(skb, &p_cb_queue->skb);

			kfree(p_cb_queue);
			ipsec_free_spidx(table_id, sp_idx);
			return CS_OK;
		}
		p_prev_cb_queue = p_cb_queue;
		p_cb_queue = p_cb_queue->next;
	} while (p_cb_queue != NULL);

	/* found nothing */
	return CS_ERROR;
} /* ipsec_replace_skb_cb_info */

static void ipsec_inc_skb_use_count(struct sk_buff *skb)
{
	if (skb_dst(skb) != NULL) dst_hold(skb_dst(skb));

#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_get(skb->nfct);
	nf_conntrack_get_reasm(skb->nfct_reasm);
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_get(skb->nf_bridge);
#endif
} /* ipsec_inc_skb_use_count */

static cs_status ipsec_skb_convert_send_to_re(unsigned char table_id, 
		struct sk_buff *skb, unsigned char sp_idx)
{
	struct ethhdr *p_eth;
	struct iphdr *p_ip = (struct iphdr*)skb->data;
	unsigned char i;

	/* current skb is an IP packet. without proper L2 info. to be able to 
	 * send it to IPsec Offload Engine, we need to construct a L2 ETH header. */
	skb_push(skb, ETH_HLEN);
	p_eth = (struct ethhdr*)skb->data;
	if (p_ip->version == 4)
		p_eth->h_proto = __constant_htons(ETH_P_IP);
	else if (p_ip->version == 6)
		p_eth->h_proto = __constant_htons(ETH_P_IPV6);

	memset(p_eth->h_source, 0x0, ETH_ALEN);
	/* because of the network byte swap.. write the sp_idx to SA MAC[5] */
	p_eth->h_source[5] = sp_idx;
	if (skb->dev != NULL) {
		for (i = 0; i < ETH_ALEN; i++)
			p_eth->h_dest[i] = skb->dev->dev_addr[i];
	}

	/* send the packet. by using ni_special_start_xmit */
	ni_special_start_xmit(skb, skb->dev, CS_FE_LSPID_ARM, sp_idx, 
			CS_IPSEC_RE_VOQ_ID(table_id));

	// FIXME!! Wen Debug message 2
	DBG(printk("%s:%d:sent the packet out via ni_special_start_xmit\n", 
				__func__, __LINE__));
	return CS_OK;
} /* ipsec_skb_convert_send_to_re */

static void ipsec_revert_ipip_on_skb(struct sk_buff *skb)
{
	struct iphdr *p_ip = ip_hdr(skb);

	/* at this point ip_hdr will point to out_ip, and we only deal 
	 * with IPv4 */
	if (p_ip->version != 4)
		return;

	/* if outer IP header is not IPIP, then we don't have to worry about 
	 * reverting it back to an IP packet */
	if (p_ip->protocol != IPPROTO_IPIP)
		return;

	/* however skb->data still points to the inner IP, we will just have to 
	 * reset the network header to the inner IP */
	skb_reset_network_header(skb);

	return;
} /* ipsec_revert_ipip_on_skb */

static void ipsec_send_sadb_skb_to_re(unsigned char table_id, unsigned short sa_id,  
		struct sk_buff *skb)
{
	cs_status status;
	unsigned char sp_idx;

	sp_idx = ipsec_get_spidx(table_id);
	if (0xff == sp_idx) goto get_sp_idx_fail;

	/* if this skb is hijacked from kernel, the new ip header has been 
	 * inserted, so we will have to remove it! */
	if (table_id == CS_IPSEC_RCPU_ENC)
		ipsec_revert_ipip_on_skb(skb);

	/* 1) create 2 hash entries for this specific flow */
	status = ipsec_create_internal_hash(table_id, sa_id, skb, sp_idx);
	if (status != CS_OK) goto create_internal_hash_fail;

	/* 2) set up and store the cb info */
	ipsec_set_skb_cb_info(table_id, sa_id, skb);
	status = ipsec_store_skb_cb_info(table_id, sa_id, skb, sp_idx);
	if (status != CS_OK) goto store_skb_cb_info_fail;

	/* 3) increment dst and nf_conntrack use_count */
	ipsec_inc_skb_use_count(skb);

	/* 4) convert the packet to an ethernet packet and send it to RE */
	status = ipsec_skb_convert_send_to_re(table_id, skb, sp_idx);
	if (status != CS_OK) goto store_skb_cb_info_fail;

	return;

store_skb_cb_info_fail:
	/* remove the hash that's created */
	cs_kernel_core_del_hash_by_guid(CS_IPSEC_SPGID(table_id, sp_idx));

create_internal_hash_fail:
	ipsec_free_spidx(table_id, sp_idx);

get_sp_idx_fail:
	kfree_skb(skb);
	ipsec_stop_sadb(table_id, sa_id);

	return;
} /* ipsec_send_sadb_skb_to_re */

static void ipsec_dequeue_skb_and_send_to_re(unsigned char table_id, unsigned short sa_id)
{
	cs_boolean f_stop = FALSE;
	unsigned short new_size;
	cs_ipsec_sadb_t *p_sadb;
	struct sk_buff *skb;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	p_sadb->accelerated = 1;
	while ((skb = ipsec_dequeue_skb_from_sadb(table_id, sa_id)) != NULL) {
		/* Estimate the packet size from current size, make sure it does not go 
		 * over the accepted packet size in IPsec Offload engine. */
		new_size = ipsec_calc_orig_skb_size(skb);
		if ((FALSE == f_stop) && (new_size <= CS_IPSEC_RE_SKB_SIZE_MAX)) {
			ipsec_send_sadb_skb_to_re(table_id, sa_id, skb);
		} else {
			/* stop the acceleration of this sadb */
			ipsec_stop_sadb(table_id, sa_id);
			f_stop = TRUE;
		}
	};
} /* ipsec_dequeue_skb_and_send_to_re */

static unsigned char ipsec_ipv4_selector_cmp(struct iphdr *iph, 
		struct xfrm_selector *p_sel)
{
	if (!(addr_match(&(iph->saddr), &p_sel->saddr.a4, p_sel->prefixlen_s) && 
			addr_match(&(iph->daddr), &p_sel->daddr.a4, p_sel->prefixlen_d)))
		return 0;	/* IP addresses do not match */

	/* IPv4 addresses matches.. need to check other */
	if (!(!p_sel->proto || (iph->protocol == p_sel->proto)))
		return 0;	/* protocol doesn't match */

	if ((IPPROTO_TCP == iph->protocol) || (IPPROTO_UDP == iph->protocol)) {
		struct tcphdr *p_tcp;
		unsigned short sport, dport;

		p_tcp = (struct tcphdr*)((void*)iph + (iph->ihl << 2));
		sport = p_tcp->source;
		dport = p_tcp->dest;

		if (!(((sport ^ p_sel->sport) & p_sel->sport_mask) && 
				((dport ^ p_sel->dport) & p_sel->dport_mask)))
			return 0;	/* Port does not match */
	}

	/* if reaches here, everything matches */
	return 1;
} /* ipsec_ipv4_selector_cmp */

static unsigned char ipsec_ipv6_selector_cmp(struct ipv6hdr *iph, 
		struct xfrm_selector *p_sel)
{
	if (!(addr_match(iph->saddr.s6_addr32, p_sel->saddr.a6, 
					p_sel->prefixlen_s) && 
			addr_match(iph->daddr.s6_addr32, p_sel->daddr.a6, 
				p_sel->prefixlen_d)))
		return 0;	/* IP addresses do not match */

	/* IPv6 addresses matches.. need to check other */

	/* FIXME! Implement later when in need to support IPv6 */
#if 0
	/* how do you locate the L4 header in IPv6 if there are multiple 
	 * extension header */
	if !(!p_sel->proto || (iph->nexthdr == p_sel->proto))
		return 0;	/* protocol doesn't match */

	if ((IPPROTO_TCP == iph->nexthdr) || (IPPROTO_UDP == iph->nexthdr)) {
		struct tcphdr *p_tcp;
		unsigned short sport, dport;

		p_tcp = (struct tcphdr*)((void*)iph + sizeof(struct ipv6hdr));
		sport = p_tcp->source;
		dport = p_tcp->dest;

		if !(((sport ^ p_sel->sport) & p_sel->sport_mask) && 
				((dport ^ p_sel->dport) & p_sel->dport_mask))
			return 0;	/* Port does not match */
	}
#endif
	/* if reaches here, everything matches */
	return 1;
} /* ipsec_ipv6_selector_cmp */

static unsigned char ipsec_ip_selector_cmp(void *p_header, 
		struct xfrm_selector *p_sel, unsigned char ip_ver)
{
	if (CS_IPSEC_IPV4 == ip_ver) /* IPv4 */
		return ipsec_ipv4_selector_cmp((struct iphdr*)p_header, p_sel);
	else if (CS_IPSEC_IPV6 == ip_ver)	/* IPv6 */
		return ipsec_ipv6_selector_cmp((struct ipv6hdr*)p_header, p_sel);
	else return 0;
} /* ipsec_ip_selector_cmp */

static cs_status ipsec_find_sadb_by_ip_selector(unsigned char table_id, 
		struct sk_buff *skb, unsigned short *p_sa_id)
{
	cs_ipsec_sadb_t *p_sadb;
	unsigned short curr_id;
	struct ethhdr *eth_hdr = (struct ethhdr*)(skb->data - ETH_HLEN);
	unsigned char ip_ver;

	if (0 == table_id) p_sadb = &ipsec_main_db.re0_sadb_q[0];
	else if (1 == table_id) p_sadb = &ipsec_main_db.re1_sadb_q[0];
	else return CS_ERROR;

	/* packet should be either an Ethernet packet w/o VLAN nor any other L2 
	 * headers sitting in front of IPv4 or IPv6 */
	if (__constant_htons(ETH_P_IP) == eth_hdr->h_proto)
		ip_ver = CS_IPSEC_IPV4;
	else if (__constant_htons(ETH_P_IPV6) == eth_hdr->h_proto)
		ip_ver = CS_IPSEC_IPV6;
	else return CS_ERROR;

	curr_id = 0;
	do {
		if ((1 == p_sadb->used) && (ip_ver == p_sadb->ip_ver)) {
			if ((0 == p_sadb->mode) && (CS_IPSEC_OUTBOUND == p_sadb->sa_dir)) {
				/* in Outbound Tunnel mode, perform the following comparison */
				if (CS_IPSEC_IPV4 == ip_ver) {
					struct iphdr *p_ip;
					p_ip = (struct iphdr *)(skb->data + ETH_HLEN);
					if ((p_ip->saddr == p_sadb->tunnel_saddr.addr) && 
							(p_ip->daddr == p_sadb->tunnel_daddr.addr)) {
						/* found it */
						*p_sa_id = curr_id;
						return CS_OK;
					}
				} else {	/* IPv6 */
#if 0	// FIXME! disable IPv6 for now
					struct ipv6hdr *p_ipv6;
					p_ipv6 = (struct ipv6hdr *)(skb->data + ETH_HLEN);

					if ((0 == ipv6_addr_cmp((const struct in6_addr *)
									p_sadb->tunnel_saddr.addr6, 
									&p_ipv6->saddr)) && 
							(0 == ipv6_addr_cmp((const struct in6_addr *)
												p_sadb->tunnel_daddr.addr6, 
												&p_ipv6->daddr))) {
						/* found it */
						*p_sa_id = curr_id;
						return CS_OK;
					}
#endif
				}
			} else {
				/* all other cases, find the sadb by the IP addresses in 
				 * selector */
				if (NULL != p_sadb->x_state && 
						ipsec_ip_selector_cmp((void*)(skb->data), 
						&(p_sadb->x_state->sel), ip_ver)) {
					/* found it */
					*p_sa_id = curr_id;
					return CS_OK;
				}
			}
		}
		curr_id++;
		p_sadb++;
	} while (curr_id < CS_IPSEC_TUN_NUM);

	/* not found */
	return CS_ERROR;
} /* ipsec_find_sadb_by_ip_selector */

#ifdef CS_IPC_ENABLED
/* IPC related */
static cs_status ipsec_ipc_send_message(unsigned char re_id, unsigned short sa_id, 
		unsigned char msg_type, unsigned int data)
{
	int rc;
	cs_ipsec_ipc_msg_t msg;

	msg.re_id = re_id;
	msg.sa_id = sa_id;
	msg.data = data;

	if (CS_IPSEC_RCPU_DEC == re_id) {
		rc = g2_ipc_send(ipc_ctxt0, CS_IPSEC_IPC_RE0_CPU_ID, 
				CS_IPSEC_IPC_RE0_CLNT_ID, G2_IPC_HPRIO, msg_type, 
				&msg, sizeof(cs_ipsec_ipc_msg_t));
		if (G2_IPC_OK != rc) return CS_ERROR;
		else return CS_OK;
	}
	if (CS_IPSEC_RCPU_ENC == re_id) {
		rc = g2_ipc_send(ipc_ctxt1, CS_IPSEC_IPC_RE1_CPU_ID, 
				CS_IPSEC_IPC_RE1_CLNT_ID, G2_IPC_HPRIO, msg_type, 
				&msg, sizeof(cs_ipsec_ipc_msg_t));
		if (G2_IPC_OK != rc) return CS_ERROR;
		else return CS_OK;
	}

	return CS_ERROR;
} /* ipsec_ipc_send_message */

static cs_status ipsec_ipc_send_init(unsigned char re_id, unsigned short sa_id, 
		unsigned int start_loc)
{
	return ipsec_ipc_send_message(re_id, sa_id, CS_IPSEC_IPC_RE_INIT, 
			start_loc);
} /* ipsec_ipc_start */

static cs_status ipsec_ipc_send_stop(unsigned char re_id, unsigned short sa_id)
{
	return ipsec_ipc_send_message(re_id, sa_id, CS_IPSEC_IPC_RE_STOP, 0);
} /* ipsec_ipc_stop */

static cs_status ipsec_ipc_send_update(unsigned char re_id, unsigned short sa_id)
{
	return ipsec_ipc_send_message(re_id, sa_id, CS_IPSEC_IPC_RE_UPDATE, 0);
} /* ipsec_ipc_update */

static cs_status ipsec_ipc_init_complete_callback(struct ipc_addr peer, 
		unsigned short msg_no, const void *msg_data, unsigned short msg_size, 
		struct ipc_context *context)
{
	unsigned char re_id, table_id;
	unsigned short sa_id;
	cs_ipsec_ipc_msg_t *p_msg = (cs_ipsec_ipc_msg_t*)msg_data;

	if (msg_size != sizeof(cs_ipsec_ipc_msg_t)) return CS_ERROR;
	re_id = p_msg->re_id;
	sa_id = p_msg->sa_id;

	if (re_id == CS_IPSEC_IPC_RE0_CPU_ID) table_id = CS_IPSEC_RCPU_DEC;
	else if (re_id == CS_IPSEC_IPC_RE1_CPU_ID) table_id = CS_IPSEC_RCPU_ENC;
	else table_id = 0xff;

	DBG(printk("%s:%d:got here! re_id = %d, table_id = %d, sa_id = %d\n", 
				__func__, __LINE__, re_id, table_id, sa_id));

	if ((CS_IPSEC_RCPU_DEC == table_id) || (CS_IPSEC_RCPU_ENC == table_id))
		f_re_status[table_id] = CS_IPSEC_IPC_RE_ACTIVE;

	return CS_OK;
} /* ipsec_ipc_start_complete_callback */

static void ipsec_ipc_stop_complete_callback(struct ipc_addr peer, 
		unsigned short msg_no, const void *msg_data, unsigned short msg_size, 
		struct ipc_context *context)
{
	unsigned char table_id, re_id;
	unsigned short sa_id;
	int err;
	cs_ipsec_sadb_t *p_sadb;
	struct sk_buff *skb;
	cs_ipsec_ipc_msg_t *p_msg = (cs_ipsec_ipc_msg_t*)msg_data;

	if (msg_size != sizeof(cs_ipsec_ipc_msg_t)) return;
	re_id = p_msg->re_id;
	sa_id = p_msg->sa_id;

	if (re_id == CS_IPSEC_IPC_RE0_CPU_ID) table_id = CS_IPSEC_RCPU_DEC;
	else if (re_id == CS_IPSEC_IPC_RE1_CPU_ID) table_id = CS_IPSEC_RCPU_ENC;
	else table_id = 0xff;

	// FIXME! debug message 4
	DBG(printk("%s:%d:got here! re_id = %d, table_id = %d, sa_id = %d\n", 
				__func__, __LINE__, re_id, table_id, sa_id));

	if (table_id == 0xff) return;
	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return;
	if (0 == p_sadb->stop) return;

	/* Update the real xfrm information based on the current number 
	 * stored in DDR */
	p_sadb->x_state->replay.oseq = p_sadb->seq_num;
	p_sadb->x_state->curlft.bytes = p_sadb->bytes_count;
	p_sadb->x_state->curlft.packets = p_sadb->packets_count;

	p_sadb->accelerated = 0;
	while ((skb = ipsec_dequeue_skb_from_sadb(table_id, sa_id)) != NULL) {
		/* resume the packet to its proper handler in Kernel */
		if (CS_IPSEC_INBOUND == p_sadb->sa_dir) {
			err = p_sadb->x_state->type->input(p_sadb->x_state, skb);
			if (-EINPROGRESS != err) {
				xfrm_input_resume(skb, err);
			}
		} else { /* if (CS_IPSEC_OUTBOUND == p_sadb->sa_dir) */
			err = p_sadb->x_state->type->output(p_sadb->x_state, skb);
			if (-EINPROGRESS != err) {
				xfrm_output_resume(skb, err);
			}
		}
	};

	ipsec_free_sadb(table_id, sa_id);
	p_sadb->stop = 0;

	return;
} /* ipsec_ipc_stop_complete_callback */

static cs_status ipsec_ipc_update_complete_callback(struct ipc_addr peer, 
		unsigned short msg_no, const void *msg_data, unsigned short msg_size, 
		struct ipc_context *context)
{
	/* FIXME! implementation 
	 * What do we do here? do we want to stop the running hash and 
	 * change the status of sa? then at update complete, change the 
	 * status back to active? */

	return CS_OK;
} /* ipsec_ipc_update_complete_callback */

static cs_status ipsec_ipc_rcv_stop(struct ipc_addr peer, 
		unsigned short msg_no, const void *msg_data, unsigned short msg_size, 
		struct ipc_context *context)
{
	unsigned char table_id;
	unsigned short sa_id;
	cs_ipsec_ipc_msg_t *p_msg = (cs_ipsec_ipc_msg_t*)msg_data;

	if (msg_size != sizeof(cs_ipsec_ipc_msg_t)) return CS_ERROR;
	table_id = p_msg->re_id;
	sa_id = p_msg->sa_id;

	/* mark the tunnel is no longer accelerated */
	return ipsec_stop_sadb(table_id, sa_id);
} /* ipsec_ipc_rcv_stop */

struct g2_ipc_msg invoke_procs[] = 
{
	{CS_IPSEC_IPC_RE_INIT_COMPLETE, 
		(unsigned long)ipsec_ipc_init_complete_callback},
	{CS_IPSEC_IPC_RE_STOP_COMPLETE, 
		(unsigned long)ipsec_ipc_stop_complete_callback},
	{CS_IPSEC_IPC_RE_UPDATE_COMPLETE, 
		(unsigned long)ipsec_ipc_update_complete_callback},
	{CS_IPSEC_IPC_RE_STOP_BY_RE, (unsigned long)ipsec_ipc_rcv_stop},
};

/* for every 5 second period, call this timer func to check the status of RE */
static void ipsec_ipc_timer_func(unsigned long data)
{
	struct timer_list *p_cs_ipc_timer;
	unsigned int tbl_start_loc;

	p_cs_ipc_timer = (struct timer_list *)data;

	/* RE0 */
	if (CS_IPSEC_IPC_RE_ACTIVE_CHECK1 == f_re_status[CS_IPSEC_RCPU_DEC])
		f_re_status[CS_IPSEC_RCPU_DEC] = CS_IPSEC_IPC_RE_ACTIVE_CHECK2;
	else if (CS_IPSEC_IPC_RE_ACTIVE_CHECK2 == f_re_status[CS_IPSEC_RCPU_DEC]) {
		f_re_status[CS_IPSEC_RCPU_DEC] = CS_IPSEC_IPC_RE_DEAD;
		ipsec_stop_all_sadb(CS_IPSEC_RCPU_DEC);
	} else if (CS_IPSEC_IPC_RE_ACTIVE == f_re_status[CS_IPSEC_RCPU_DEC])
		f_re_status[CS_IPSEC_RCPU_DEC] = CS_IPSEC_IPC_RE_ACTIVE_CHECK1;

	/* need to send IPC when the acceleration is first started */
	tbl_start_loc = virt_to_phys((void *)ipsec_get_sadb(CS_IPSEC_RCPU_DEC, 0));
	/* tell IPsec Offload Engine to starts this acceleration */
	ipsec_ipc_send_init(CS_IPSEC_RCPU_DEC, 0, tbl_start_loc);

	/* RE1 */
	if (CS_IPSEC_IPC_RE_ACTIVE_CHECK1 == f_re_status[CS_IPSEC_RCPU_ENC])
		f_re_status[CS_IPSEC_RCPU_ENC] = CS_IPSEC_IPC_RE_ACTIVE_CHECK2;
	else if (CS_IPSEC_IPC_RE_ACTIVE_CHECK2 == f_re_status[CS_IPSEC_RCPU_ENC]) {
		f_re_status[CS_IPSEC_RCPU_ENC] = CS_IPSEC_IPC_RE_DEAD;
		ipsec_stop_all_sadb(CS_IPSEC_RCPU_ENC);
	} else if (CS_IPSEC_IPC_RE_ACTIVE == f_re_status[CS_IPSEC_RCPU_ENC])
		f_re_status[CS_IPSEC_RCPU_ENC] = CS_IPSEC_IPC_RE_ACTIVE_CHECK1;

	/* need to send IPC when the acceleration is first started */
	tbl_start_loc = virt_to_phys((void *)ipsec_get_sadb(CS_IPSEC_RCPU_ENC, 0));
	/* tell IPsec Offload Engine to starts this acceleration */
	ipsec_ipc_send_init(CS_IPSEC_RCPU_ENC, 0, tbl_start_loc);

	p_cs_ipc_timer->expires = round_jiffies(jiffies + 
			(CS_IPSEC_IPC_TIMER_PERIOD * HZ));
	add_timer(p_cs_ipc_timer);
	return;
} /* ipsec_ipc_timer_func */

static cs_status ipsec_register_ipc(void)
{
	short status;

	status = g2_ipc_register(CS_IPSEC_IPC_RE0_CLNT_ID, invoke_procs, 
			4, 0, NULL, &ipc_ctxt0);
	if (status != G2_IPC_OK) {
		printk("%s::Failed to register IPC for IPsec Offload RE0\n", __func__);
		return CS_ERROR;
	} else printk("%s::successfully register IPC for RE0\n", __func__);

	status = g2_ipc_register(CS_IPSEC_IPC_RE1_CLNT_ID, invoke_procs, 
			4, 0, NULL, &ipc_ctxt1);
	if (status != G2_IPC_OK) {
		printk("%s::Failed to register IPC for IPsec Offload RE1\n", __func__);
		return CS_ERROR;
	} else printk("%s::successfully register IPC for RE1\n", __func__);

	return CS_OK;
} /* ipsec_register_ipc */

static void ipsec_deregister_ipc(void)
{
	g2_ipc_deregister(ipc_ctxt0);
	g2_ipc_deregister(ipc_ctxt1);
	printk("%s::Done deregister IPC for IPsec Offload\n", __func__);
} /* ipsec_unregister_ipc */
#endif

static void cs_ipsec_db_init(void)
{
	ipsec_main_db.p_re0_idx = 0;
	ipsec_main_db.p_re1_idx = 0;
	ipsec_main_db.re0_used_count = 0;
	ipsec_main_db.re1_used_count = 0;
	ipsec_main_db.re0_spidx_mask = 0;
	ipsec_main_db.re1_spidx_mask = 0;
	memset(ipsec_main_db.re0_cb_q, 0x0, 
			sizeof(cs_ipsec_cb_queue_t *) * CS_IPSEC_TUN_NUM);
	memset(ipsec_main_db.re1_cb_q, 0x0, 
			sizeof(cs_ipsec_cb_queue_t *) * CS_IPSEC_TUN_NUM);
	memset(ipsec_main_db.re0_skb_q, 0x0, 
			sizeof(cs_ipsec_skb_queue_t *) * CS_IPSEC_TUN_NUM);
	memset(ipsec_main_db.re1_skb_q, 0x0, 
			sizeof(cs_ipsec_skb_queue_t *) * CS_IPSEC_TUN_NUM);
	memset(ipsec_main_db.re0_sadb_q, 0x0, 
			sizeof(cs_ipsec_sadb_t) * CS_IPSEC_TUN_NUM);
	memset(ipsec_main_db.re1_sadb_q, 0x0, 
			sizeof(cs_ipsec_sadb_t) * CS_IPSEC_TUN_NUM);
} /* cs_ipsec_db_init */

/************************ external APIs ****************************/

/************************ main handlers *********************/
/* this function returns CS_OK when it is ok for Kernel to continue
 * its original task.  CS_DONE means Kernel does not have to 
 * handle anymore. */
cs_status k_jt_cs_ipsec_handler(struct sk_buff *skb, struct xfrm_state *x, 
		unsigned char ip_ver, unsigned char dir)
{
	cs_status status;
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	unsigned char table_id = 0;
	unsigned short sa_id = 0;
	cs_ipsec_sadb_t *p_sadb;

	if (f_ipsec_enbl == FALSE) return CS_ERROR;

	if (cs_cb == NULL) return CS_ERROR;
	if (cs_cb->common.tag != CS_CB_TAG) return CS_ERROR;

	/* check skb->cb.. 
	 * if IPsec module has already make hw_accel decision on this packet, then 
	 * there is something wrong.. packet shouldn't come into this function again. 
	 * overwrite the decision with sw_only. 
	 * If it is already sw_only... just skip.
	 * if it is hw_accel by someone else... then there is something wrong too..
	 * Only take care when it is "don't care" from all other CS752 modules */
	if (cs_cb->common.sw_only & (CS_SWONLY_HW_ACCEL | CS_SWONLY_STATE)) {
		cs_cb->common.sw_only = CS_SWONLY_STATE;
		return CS_ERROR;
	}

	status = ipsec_find_idx_by_xfrm_state(x, &table_id, &sa_id);
	if (status == CS_ERROR) return CS_ERROR;

	/* Make sure the table id matched with the direction. */
	if ((status == CS_OK) && (IPSEC_TABLE_ID_BY_DIR(dir) != table_id)) {
		/* shouldn't happen */
		return CS_ERROR;
	}

	p_sadb = ipsec_get_sadb(table_id, sa_id);

	/* check if this tunnel has been accelerated */
	if ((status == CS_OK) && (NULL != p_sadb) && (1 == p_sadb->used)) {
		if ((1 == p_sadb->accelerated) && (1 == p_sadb->stop)) {
			/* this tunnel was accelerated but has been stopped because some 
			 * errors occured */
			/* enqueue the packet into tunnel queue */
			ipsec_enqueue_skb_to_sadb(table_id, sa_id, skb);
		} else if ((1 == p_sadb->accelerated) && (0 == p_sadb->stop)) {
			unsigned short new_size = 0;
			/* this tunnel is accelerated, and now there is a newly introduced 
			 * traffic. */
			/* estimate the packet size that will be sent to IPsec Offload 
			 * engine, and make sure it does not violate the packet size 
			 * limitation. */
			new_size = ipsec_calc_orig_skb_size(skb);
			if ((CS_IPSEC_IPV4 == ip_ver) && (CS_IPSEC_INBOUND == dir)) {
				struct iphdr *iph = ip_hdr(skb);
				if (iph->frag_off & htons(IP_MF|IP_OFFSET)) {
					ipsec_enqueue_skb_to_sadb(table_id, sa_id, skb);
					ipsec_stop_sadb(table_id, sa_id);
				}
			}
			if (new_size <= CS_IPSEC_RE_SKB_SIZE_MAX) {
				ipsec_send_sadb_skb_to_re(table_id, sa_id, skb);
			} else {
				/* stop the acceleration of this sadb */
				ipsec_enqueue_skb_to_sadb(table_id, sa_id, skb);
				ipsec_stop_sadb(table_id, sa_id);
			}
		} else if ((0 == p_sadb->accelerated) && (0 == p_sadb->stop)) {
			/* this tunnel is in the process of being accelerated, but the task 
			 * of negotiation to IPsec Offload engine is not yet complete. We 
			 * have to queue this packet */
			ipsec_enqueue_skb_to_sadb(table_id, sa_id, skb);
		} else { /* if ((0 == p_sadb->accelerated) && (1 == p_sadb->stop)) */
			/* this tunnel was no longer accelerated, but we need to let 
			 * the kernel process the rest. */
			return CS_OK;	/* Kernel please continues the rest */
		}

		return CS_DONE;	/* Kernel please skips */
	} else {

		/* check if the tunnel can be accelerated by hardware */
		if (CS_OK == ipsec_check_and_create_sadb(dir, x, ip_ver, 
					&table_id, &sa_id)) {
			ipsec_set_skb_cb_info(table_id, sa_id, skb);
		} else {
			/* not accelerable. */
			cs_cb->common.sw_only = CS_SWONLY_STATE;
		}
	}

	return CS_OK;
} /* k_jt_cs_ipsec_handler */

static void ipsec_set_skb_cb_output_info(struct sk_buff *skb, 
		unsigned char ip_ver, unsigned char dir)
{
	struct iphdr *iph = ip_hdr(skb);
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);

	/* does not support IPv6 for now.. FIXME */
	if (ip_ver == CS_IPSEC_IPV6) return;

	if (cs_cb == NULL) return;
	if (cs_cb->common.tag != CS_CB_TAG) return;
	if (!(cs_cb->common.module_mask & CS_MOD_MASK_IPSEC)) return;

	cs_cb->output.l3_nh.iph.sip = iph->saddr;
	cs_cb->output.l3_nh.iph.dip = iph->daddr;
	cs_cb->output.l3_nh.iph.protocol = iph->protocol;

	DBG(printk("%s:%d:dir %d, sip %x, dip %x, proto %d\n", __func__, __LINE__, 
				dir, iph->saddr, iph->daddr, iph->protocol));
	if (iph->protocol == IPPROTO_ESP) {
		struct ip_esp_hdr *esph;
		esph = (struct ip_esp_hdr*)((void*)iph + (iph->ihl << 2));
		cs_cb->output.l4_h.ah_esp.spi = esph->spi;
		DBG(printk("%s:%d:esp spi %x\n", __func__, __LINE__, esph->spi));
	} else if (iph->protocol == IPPROTO_AH) {
		struct ip_auth_hdr *ahh;
		ahh = (struct ip_auth_hdr*)((void*)iph + (iph->ihl << 2));
		cs_cb->output.l4_h.ah_esp.spi = ahh->spi;
		DBG(printk("%s:%d:ah spi %x\n", __func__, __LINE__, ahh->spi));
	} else if (iph->protocol == IPPROTO_TCP) {
		struct tcphdr *tcph;
		/* check if L4 SPORT/DPORT is needed */
		tcph = (struct tcphdr*)((void*)iph + (iph->ihl << 2));
		cs_cb->output.l4_h.th.sport = tcph->source;
		cs_cb->output.l4_h.th.dport = tcph->dest;
		DBG(printk("%s:%d:tcp sp %d, dp %d\n", __func__, __LINE__, 
					tcph->source, tcph->dest));
	} else if (iph->protocol == IPPROTO_UDP) {
		struct udphdr *udph;
		/* check if L4 SPORT/DPORT is needed */
		udph = (struct udphdr*)((void*)iph + (iph->ihl << 2));
		cs_cb->output.l4_h.uh.sport = udph->source;
		cs_cb->output.l4_h.uh.dport = udph->dest;
		DBG(printk("%s:%d:udp sp %d, dp %d\n", __func__, __LINE__, 
					udph->source, udph->dest));
	}
} /* ipsec_set_skb_cb_output_info */

void k_jt_cs_ipsec_done_handler(struct sk_buff *skb, unsigned char ip_ver, 
		unsigned char dir)
{
	ipsec_set_skb_cb_output_info(skb, ip_ver, dir);
	return;
} /* k_jt_cs_ipsec_done_handler */

#ifdef CS_IPSEC_DEBUG_MODE
/* the following is a simplified xfrm_input */
static cs_status cs_ipsec_handle_input(struct sk_buff *skb, 
		cs_ipsec_sadb_t *p_sadb)
{
	struct xfrm_state *x = p_sadb->x_state;
	int nexthdr, err;
	__be32 seq, spi;
	const unsigned char *old_mac;
	struct iphdr *iph;
	cs_boolean f_use_dummy = FALSE;

	iph = ip_hdr(skb);
	skb->data += (iph->ihl << 2);
	skb->len -= (iph->ihl << 2);
	skb_reset_transport_header(skb);
	//__skb_pull(skb, ip_hdrlen(skb));
	if (!skb->sp || atomic_read(&skb->sp->refcnt) != 1) {
		struct sec_path *sp;
		
		sp = secpath_dup(skb->sp);
		if (!sp) {
			//XFRM_INC_STATS(net, LINUX_MIB_XFRMINERROR);
			goto drop;
		}
		if (skb->sp)
			secpath_put(skb->sp);
		skb->sp = sp;
	} else {
		static struct sec_path dummy;
		dummy.len = 0;
		skb->sp = &dummy;
		f_use_dummy = TRUE;
	}
	skb->sp->len++;
	skb->sp->xvec[skb->sp->len - 1] = x;

	xfrm_parse_spi(skb, iph->protocol, &spi, &seq);
	DBG(printk("%s:%d:spi = %x, seq = %x\n", __func__, __LINE__, spi, seq));
	nexthdr = x->type->input(x, skb);
	if (nexthdr == -EINPROGRESS) {
		DBG(printk("%s:%d:packet is still in IPsec editing progress\n", 
					__func__, __LINE__));
		return CS_ERROR;
	}

	spin_lock(&x->lock);
	if (x->props.replay_window)
		xfrm_replay_advance(x, seq);
	x->curlft.bytes += skb->len;
	x->curlft.packets++;
	spin_unlock(&x->lock);

	XFRM_MODE_SKB_CB(skb)->protocol = nexthdr;
	DBG(printk("%s:%d:nexthdr = %d\n", __func__, __LINE__, nexthdr));

#if 0 // what for?
	err = x->outer_mode->afinfo->extract_input(x, skb);
#endif
	skb->protocol = x->inner_mode->afinfo->eth_proto;

	/* the following is from xfrm4_mode_tunnel_input */
	if (XFRM_MODE_SKB_CB(skb)->protocol != IPPROTO_IPIP) {
		DBG(printk("%s:%d:not ipip protocol\n", __func__, __LINE__));
		goto drop;
	}

	if (!pskb_may_pull(skb, sizeof(struct iphdr))) {
		DBG(printk("%s:%d:not able to pskb_may_pull\n", __func__, __LINE__));
		goto drop;
	}

	if (skb_cloned(skb) && 
			(err = pskb_expand_head(skb, 0, 0, GFP_ATOMIC))) {
		DBG(printk("%s:%d:error here!\n", __func__, __LINE__));
		goto drop;
	}

	if (x->props.flags & XFRM_STATE_DECAP_DSCP)
		ipv4_copy_dscp(XFRM_MODE_SKB_CB(skb)->tos, ipip_hdr(skb));
#if 0
	if (!(x->props.flags & XFRM_STATE_NOECN))
		ipip_ecn_decapsulate(skb);
#endif

	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	old_mac = skb_mac_header(skb);
	skb_push(skb, ETH_HLEN);
	skb_reset_mac_header(skb);
	memmove(skb_mac_header(skb), old_mac, skb->mac_len);

	DBG(printk("%s:%d:network header = %x, mac_header = %x, skb->len %d, ", 
				__func__, __LINE__, skb_network_header(skb), 
				skb_mac_header(skb), skb->len));
	DBG(printk("iph->tot_len %d, old_mac = %x\n", iph->tot_len, old_mac));
	skb->sp->xvec[skb->sp->len - 1] = NULL;
	skb->sp->len--;

	return CS_OK;
drop:
	if (f_use_dummy == TRUE)
		skb->sp = NULL;
	dev_kfree_skb(skb);
	return CS_ERROR;
} /* cs_ipsec_handle_input */

static cs_status cs_ipsec_handle_output(struct sk_buff *skb, 
		cs_ipsec_sadb_t *p_sadb)
{
	struct xfrm_state *x = p_sadb->x_state;
	int err;
	struct iphdr *top_iph;
	static u32 ipsec_fallback_id;
	u32 salt;

	skb->protocol = htons(ETH_P_IP);
	err = xfrm_inner_extract_output(x, skb);
	if (err) {
		DBG(printk("%s:%d:error at xfrm_inner_extract_output\n", __func__, 
					__LINE__));
		goto drop;
	}
	memset(IPCB(skb), 0, sizeof(*IPCB(skb)));
	IPCB(skb)->flags |= IPSKB_XFRM_TUNNEL_SIZE | IPSKB_XFRM_TRANSFORMED;

	DBG(printk("%s:%d:original iph->tot_len = %d\n", __func__, __LINE__, 
				ntohs(ip_hdr(skb)->tot_len)));
	skb_set_network_header(skb, -x->props.header_len);
	skb->mac_header = skb->network_header + offsetof(struct iphdr, protocol);
	skb->transport_header = skb->network_header + sizeof(*top_iph);

	top_iph = ip_hdr(skb);

	top_iph->ihl = 5;
	top_iph->version = 4;
	top_iph->protocol = IPPROTO_IPIP;
	top_iph->tos = 0xC0;
	if (x->props.flags & XFRM_STATE_NOECN)
		IP_ECN_clear(top_iph);
	top_iph->frag_off = 0;
	top_iph->ttl = 64;
	top_iph->saddr = x->props.saddr.a4;
	top_iph->daddr = x->id.daddr.a4;

	DBG(printk("%s:%d:done inserting 2 headers!\n", __func__, __LINE__));

	spin_lock_bh(&x->lock);
	XFRM_SKB_CB(skb)->seq.output = ++x->replay.oseq;
	err = xfrm_state_check_expire(x);
	if (err) {
		DBG(printk("%s:%d:xfrm_state_check_expire returns err\n", __func__, 
					__LINE__));
		spin_unlock_bh(&x->lock);
		goto drop;
	}
	x->curlft.bytes += skb->len;
	x->curlft.packets++;
	spin_unlock_bh(&x->lock);

	err = x->type->output(x, skb);
	if (err == -EINPROGRESS) {
		DBG(printk("%s:%d:IPsec packet editing still in progress\n", __func__, 
					__LINE__));
		return CS_ERROR;
	}

	skb_reset_network_header(skb);
	top_iph->tot_len = htons(skb->len);
	salt = secure_ip_id((__force __be32)ipsec_fallback_id ^ top_iph->daddr);
	top_iph->id = htons(salt & 0xFFFF);
	ipsec_fallback_id = salt;
	ip_send_check(top_iph);
	DBG(printk("%s:%d:resulted iphinfo:\n", __func__, __LINE__));
	DBG(printk("new_tot_len = %d, checksum = %x, id = %d\n", 
				ntohs(ip_hdr(skb)->tot_len), ip_hdr(skb)->check, 
				ip_hdr(skb)->id));
	skb_push(skb, ETH_HLEN);
	skb_reset_mac_header(skb);

	return CS_OK;
drop:
	dev_kfree_skb(skb);
	return CS_ERROR;
} /* cs_ipsec_handle_output */
#endif

static void ipsec_insert_sec_path(struct sk_buff *skb, unsigned char table_id, 
		unsigned short sa_id)
{
	cs_ipsec_sadb_t *p_sadb;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (p_sadb == NULL) return;

	if (!skb->sp || atomic_read(&skb->sp->refcnt) != 1) {
		struct sec_path *sp;
		
		sp = secpath_dup(skb->sp);
		if (!sp) {
			//XFRM_INC_STATS(net, LINUX_MIB_XFRMINERROR);
			return;
		}
		if (skb->sp)
			secpath_put(skb->sp);
		skb->sp = sp;
	}

	if (skb->sp->len == XFRM_MAX_DEPTH) return;

	xfrm_state_hold(p_sadb->x_state);
	skb->sp->xvec[skb->sp->len++] = p_sadb->x_state;

	return;
} /* ipsec_insert_sec_path */

static void ipsec_fill_input_info_to_skb_cb(struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct iphdr *iph = ip_hdr(skb);
	unsigned int hdroff = iph->ihl << 2;
	struct tcphdr *tcphdr;
	struct udphdr *udphdr;

	cs_cb->output.l3_nh.iph.sip = iph->saddr;
	cs_cb->output.l3_nh.iph.dip = iph->daddr;
	cs_cb->output.l3_nh.iph.tos = iph->tos;
	cs_cb->output.l3_nh.iph.protocol = iph->protocol;

	if (iph->protocol == IPPROTO_TCP) {
		tcphdr = (struct tcphdr*)(skb->data + hdroff);
		cs_cb->output.l4_h.th.sport = tcphdr->source;
		cs_cb->output.l4_h.th.dport = tcphdr->dest;
	} else if (iph->protocol == IPPROTO_UDP) {
		udphdr = (struct udphdr*)(skb->data + hdroff);
		cs_cb->output.l4_h.uh.sport = udphdr->source;
		cs_cb->output.l4_h.uh.dport = udphdr->dest;
	}
	return;
} /* ipsec_fill_input_info_to_skb_cb */

cs_status cs_hw_accel_ipsec_handle_rx(struct sk_buff *skb, unsigned char src_port, 
		unsigned short in_voq_id, unsigned int sw_action)
{
	unsigned char re_id = 0, sp_idx, func_id;
	cs_status status;
	unsigned short sa_id;
	int err;
#ifdef CS_IPSEC_DEBUG_MODE
	unsigned short recirc_id;
	unsigned char func_id, pspid, voq_id;
	cs_ipsec_sadb_t *p_sadb;
	struct iphdr *iph;
	struct ethhdr *ethh;
	CS_KERNEL_ACCEL_CB_T *cs_cb = CS_KERNEL_SKB_CB(skb);
	struct net_device *dev;

	DBG(printk("%s:%d:Got here!! src_port = %d, ", __func__, __LINE__, 
				src_port));
	DBG(printk("in_voq_id = %d, sw_action = %x\n", in_voq_id, sw_action));

	sa_id = (sw_action & 0x01ff);
	recirc_id = ((sw_action >> 9) & 0x007f);
	func_id = ((sw_action >> 16) & 0x0003);

	DBG(printk("%s:%dsa_id %d, recirc_id %d, func_id %d\n", __func__, __LINE__, 
				sa_id, recirc_id, func_id));
	ni_dm_byte(skb->data, skb->len);
	if (in_voq_id == CS_IPSEC_RE_VOQ_ID(0))
		p_sadb = ipsec_get_sadb(0, sa_id);
	else if (in_voq_id == CS_IPSEC_RE_VOQ_ID(1))
		p_sadb = ipsec_get_sadb(1, sa_id);
	else {
		DBG(printk("%s:%d:not a supported voq id\n", __func__, __LINE__));
		return CS_ERROR;
	}

	if (p_sadb == NULL) {
		DBG(printk("%s:%d:sadb not found!\n", __func__, __LINE__));
		return CS_ERROR;
	}

	if (p_sadb->used != 1) {
		DBG(printk("%s:%d:sadb is not used?\n", __func__, __LINE__));
		return CS_ERROR;
	}

	if (cs_cb == NULL) {
		DBG(printk("%s:%d:cs_cb is NULL?\n", __func__, __LINE__));
		return CS_ERROR;
	}
	/* untag the cs_cb */
	cs_cb->common.tag = 0;

	/* move the pointer to the start of IP Header */
//	skb->protocol = eth_type_trans(skb, skb->dev);
	skb_reset_network_header(skb);

	if (p_sadb->sa_dir == CS_IPSEC_INBOUND) {
		status = cs_ipsec_handle_input(skb, p_sadb);
		pspid = ENCRYPTION_PORT;
		voq_id = 11;
		dev = ni_get_device(1);
	} else {
		status = cs_ipsec_handle_output(skb, p_sadb);
		pspid = ENCAPSULATION_PORT;
		voq_id = 3;
		dev = ni_get_device(0);
	}

	if (status != CS_OK) return CS_OK;

	/* packet has been going through IPsec editing, some more editings 
	 * are required before transmit it out. */
	ethh = (struct ethhdr *)skb_mac_header(skb);
	memset(ethh->h_source, 0x0, ETH_ALEN);
	memset(ethh->h_dest, 0x0, ETH_ALEN);
	ethh->h_source[5] = (unsigned char)recirc_id;
	ethh->h_proto = htons(ETH_P_IP);

	DBG(ni_dm_byte(skb->data, skb->len));

	/* transmit the resulted packet */
	if (ni_special_start_xmit(skb, dev, pspid, recirc_id, voq_id))
		printk("%s:%d:packet tx fails!\n", __func__, __LINE__);
	else
		printk("%s:%d:packet tx succeeds\n", __func__, __LINE__);

	return CS_OK;
#else
	/* 1) check if the src port is one of the recirculation engine ports, 
	 * and make re_id equals to recirculation cpu id */
	/* FIXME!! what needs to be implemented here is that.. based on different 
	 * receiving VOQ, we can assigned the right re_id */

	DBG(printk("%s:%d:Got here!! src_port = %d, ", __func__, __LINE__, 
				src_port));
	DBG(printk("in_voq_id = %d, sw_action = %x\n", in_voq_id, sw_action));

	/* FIXME. We need to insert some original info here.. well. we can do it 
	 * in ipsec_set_skb_cb_info(). We still need to set the offset to L3 header 
	 * here. Or before at the usual RX handler in NI */

	/* check the sw_action value from header E */
	if (CS_FE_INVALID_SW_ACTION == sw_action) {
		/* if there is no valid sw_action, that means this packet is a hash miss 
		 * from RE. */
		/* 1) fill up the control block info */
		/* notes here.. packet will be from recirculation engine. we will 
		 * have to mark this packet with guid.. how do we find its original 
		 * selector / SADB? */
		status = ipsec_find_sadb_by_ip_selector(re_id, skb, &sa_id);
		if (status != CS_OK) {
			DBG(printk("%s:%d:going to free the packet\n", __func__, __LINE__));
			/* well. i'd suggest just drop this packet for now. 
			 * Any better solution? */
			kfree_skb(skb);
			return CS_OK;
		}
		skb_reset_network_header(skb);
		ipsec_set_skb_cb_info(re_id, sa_id, skb);
		ipsec_fill_input_info_to_skb_cb(skb);

		/* 2) transmit the packet via the normal Kernel, then 
		 * it will be handled and proper hash entry will be create. */

		/* need to fill sec_path in skb, such that the Kernel could realize 
		 * this packet when we do netif_rx */
		ipsec_insert_sec_path(skb, re_id, sa_id);
		netif_rx(skb);
	} else {
		/* if there is valid sw_action, that means this packet is one of the 
		 * special packet we offload encryption/decryption to RE, we need to 
		 * resume the Kernel task before sending it out */
		/* note! in this case, voq may be different if we assign a special 
		 * ingress voq for it. */

		sa_id = sw_action & 0x01ff;
		sp_idx = (unsigned char)((sw_action >> 9) & 0x7f);
		func_id = (unsigned char)((sw_action >> 16) & 0x3);

		re_id = CS_IPSEC_GET_RE_ID_FROM_FUNC_ID(func_id);
		DBG(printk("%s:%d:sa_id = %d, sp_idx = %d, func_id = %d, re_id = %d\n", 
					__func__, __LINE__, sa_id, sp_idx, func_id, re_id));

		status = ipsec_replace_skb_cb_info(re_id, sa_id, sp_idx, skb);
		if (status != CS_OK) {
			kfree_skb(skb);
			return CS_OK;
		}

		skb_reset_network_header(skb);
		/* delete the hash by guid */
		cs_kernel_core_del_hash_by_guid(CS_IPSEC_SPGID(re_id, sp_idx));

		/* if it's inbound */
		if (re_id == CS_IPSEC_RCPU_DEC) {
			ipsec_set_skb_cb_output_info(skb, CS_IPSEC_IPV4, CS_IPSEC_INBOUND);
			DBG(printk("%s:%d:a decryption path packet!!\n", __func__, 
						__LINE__));
			netif_rx(skb);
		} else {	/* if it's outbound */
			ipsec_set_skb_cb_output_info(skb, CS_IPSEC_IPV4, CS_IPSEC_OUTBOUND);
			DBG(printk("%s:%d:an encryption path packet!!\n", __func__, 
						__LINE__));
			xfrm_output_resume(skb, 0);
		}
	}
#endif
	return CS_OK;
} /* cs_hw_accel_ipsec_handle_rx */

/* hooks inserted at xfrm_state_add() in net/xfrm/xfrm_state.c */
void k_jt_cs_ipsec_x_state_add(struct xfrm_state *x)
{
	unsigned char table_id;
	unsigned short sa_id;
	cs_status status;

	status = ipsec_find_idx_by_xfrm_state(x, &table_id, &sa_id);
	if (status != CS_OK) return;

	/* FIXME! */
	/* Possible Implenetation, but most likely it is not need:  
	 * We need to make sure the packet doesn't get hash hit by 
	 * other hash entries... one way to ensure that is to make a hash 
	 * based on selector to ensure the traffic all goes to CPU. */

	return;
} /* k_jt_cs_ipsec_x_state_add */

/* hooks inserted at __xfrm_state_delete() in net/xfrm/xfrm_state.c */
void k_jt_cs_ipsec_x_state_delete(struct xfrm_state *x)
{
	unsigned char table_id;
	unsigned short sa_id;
	cs_status status;

	status = ipsec_find_idx_by_xfrm_state(x, &table_id, &sa_id);
	if (status != CS_OK) return;

	/* if a xfrm state is deleted, just stop the sadb */
	ipsec_stop_sadb(table_id, sa_id);

	return;
} /* k_jt_cs_ipsec_x_state_delete */

/* hooks inserted at xfrm_state_update() in net/xfrm/xfrm_state.c */
void k_jt_cs_ipsec_x_state_update(struct xfrm_state *x)
{
	unsigned char table_id;
	unsigned short sa_id;
	cs_status status;
	cs_ipsec_sadb_t *p_sadb;

	status = ipsec_find_idx_by_xfrm_state(x, &table_id, &sa_id);
	if (status != CS_OK) return;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (NULL == p_sadb) return;

	/* assuming p_sadb ip_ver and dir are not going to change */
	status = ipsec_update_sadb(table_id, sa_id, p_sadb->sa_dir, x, 
			p_sadb->ip_ver);
	if (status != CS_OK) return;

#ifdef CS_IPC_ENABLED
	ipsec_ipc_send_update(table_id, sa_id);
#endif

	/* FIXME! Do we need to stop all the hash? well at least for now 
	 * this is not implemented? */

	return;
} /* k_jt_cs_ipsec_x_state_update */

void cs_hw_accel_ipsec_hash_callback(unsigned long long guid, int status)
{
	switch (status) {
		case CS_HASH_CB_SUCCESSFUL:
			break;
		case CS_HASH_CB_FAILED_SW_REQ:
		case CS_HASH_CB_FAILED_RESOURCE:
			/* fail to create the hash */
			ipsec_stop_sadb_by_guid(guid);
			break;
		case CS_HASH_CB_DELETE_BY_PEER:
			break;
		default:
			printk("%s::unknown callback status code %x\n", __func__, status);
			break;
	};
} /* cs_hw_accel_ipsec_hash_callback */

/* supposedly. when core logic call the following API, it should have already 
 * find a guid that belongs to IPsec Offload flow. and some of the decisions 
 * are already made in core logic.. such as sw_only state.. */
cs_status cs_hw_accel_ipsec_create_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, 
		unsigned long long guid)
{
	unsigned char table_id, func_id = 0;
	unsigned short tag_id, sa_id;
	unsigned int rc;
	cs_ipsec_sadb_t *p_sadb;
	CS_HASH_ENTRY_S hash_entry;
	cs_boolean f_vlan_remove = FALSE, f_pppoe_remove = FALSE;
	cs_status status;

	/* this should not happen! */
	if (cs_cb == NULL) return CS_ERROR;

	memset(&hash_entry, 0x0, sizeof(CS_HASH_ENTRY_S));

	/* find the guid for IPsec */
	tag_id = (guid >> 48) & 0xffff;
	sa_id = guid & 0x01ff;
	table_id = (guid >> 9) & 0x01;

	p_sadb = ipsec_get_sadb(table_id, sa_id);
	if (p_sadb == NULL) return CS_ERROR;
	if (p_sadb->used == 0) return CS_ERROR;

	DBG(printk("%s:%d:going to create hash for table_id %d, sa_id %d\n", 
				__func__, __LINE__, table_id, sa_id));
	DBG(printk("\tguid %ld, tag_id %d\n", guid, tag_id));
#ifdef CS_IP_ENABLED
	/* if init for a given RCPU is not complete, just skip */
	if (f_re_status[table_id] == FALSE) return CS_ERROR;
#endif

	if (cs_cb->common.sw_only & (CS_SWONLY_DONTCARE | CS_SWONLY_STATE)) {
		/* this hash was marked with HW_ACCEL earlier, but someone 
		 * modifies the decision.. now we need to get rid of some stuff */
		ipsec_stop_sadb(table_id, sa_id);
		return CS_ERROR;
	}

	/* When incoming LSPID is any of the GE port, we need to create 
	 * the 1st hash entry that redirect incoming packet from GE#0, GE#1, 
	 * and GE#2 to RE */
	if ((cs_cb->key_misc.orig_lspid == GE_PORT0) || 
			(cs_cb->key_misc.orig_lspid == GE_PORT1) || 
			(cs_cb->key_misc.orig_lspid == GE_PORT2)) {

		/* 1st entry for (CS_IPSEC_INBOUND == p_sadb->sa_dir)
		 * mask requirement: (on top of the original mask)
		 * 1. Port (original source port, could be GE or RE)
		 * 2. Packet Size
		 * 3. L3 Source IP, destination IP, IP protocol and no IP frag
		 * 4. SPI.
		 * Action: (on top of the original action)
		 * 1. L2 source MAC to voq_id, remove vlan and pppoe if they exist
		 * 2. sw_action value
		 * 3. destination voq = RE VOQ */
		/* 1st entry for (CS_IPSEC_OUTBOUND == p_sadb->sa_dir)
		 * mask requirement: (on top of the original mask)
		 * 1. port (original source port, could be GE or RE)
		 * 2. packet size
		 * 3. L3 source IP, dst IP, IP protocol, (mask off ip frag; don't care)
		 * 4. L4 (if needed), src port, dst port
		 * Action:
		 * 1) L2 source mac to voq_id, remove VLAN/PPPoE
		 * 2) L3: decrement TTL
		 * 3) sw_action value
		 * 4) destination voq = RE VOQ */

		DBG(printk("%s:%d:going to create 1st entry\n", __func__, __LINE__));
		/* we can share the hash mask from cb for the first hash field */
		status = cs_kernel_get_hash_mask_from_cs_cb(cs_cb, &(hash_entry.mask));
		if (status != CS_OK) return status;

		/* always enable those masks to save tuple usage */
		hash_entry.mask |= CS_HM_SPI_MASK;// | CS_HM_IP_FRAGMENT_MASK;
		hash_entry.mask |= CS_HM_SPI_VLD_MASK;
		hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;

		/* we can also share the hash key from cb for the first hash */
		status = cs_kernel_get_hash_key_from_cs_cb(cs_cb, &(hash_entry.key));
		if (status != CS_OK) return status;

		if (CS_IPSEC_INBOUND == p_sadb->sa_dir)
			func_id += 2;
		if (IPPROTO_AH == p_sadb->proto)
			func_id += 1;

		hash_entry.result.action.sw_action = 1;
		hash_entry.result.action.d_voq_id = 1;
		hash_entry.result.action.mac_sa_replace_en = 1;
		hash_entry.result.param.d_voq = CS_IPSEC_RE_VOQ_ID(table_id);
		hash_entry.result.param.sw_action_id = CS_IPSEC_SW_ACTION_ID(func_id, 
				cs_cb->action.voq_pol.d_voq_id, sa_id);
		hash_entry.result.param.mac_sa[0] = cs_cb->action.voq_pol.d_voq_id;

		/* check if VLAN exists, remove it. */
		if ((cs_cb->action.l2.flow_vlan_op_en == CS_VLAN_OP_INSERT) || 
				(cs_cb->action.l2.flow_vlan_op_en == CS_VLAN_OP_REPLACE)) {
			hash_entry.result.action.flow_vlan_op_en = CS_VLAN_OP_REMOVE;
			f_vlan_remove = TRUE;
		}
#if 0	// FIXME... the following..
		/* check if PPPoE exists, remove it. */
		if (CS_HASH_MASK_PPPOE & hash_entry.mask) {
			hash_entry.result.action.pppoe_decap_en = 1;
			f_pppoe_remove = TRUE;
		}
#endif

		if (CS_IPSEC_OUTBOUND == p_sadb->sa_dir)
			hash_entry.result.action.decr_ttl_hoplimit = 1;

		/* when creating hash, put in the guid */
		/* if the same hash entry has been created.. then it's fine.. 
		 * if the hash entry is created, then it's fine, too
		 * however, if the hash entry is not able to be created, then 
		 * it is NOT fine. */

		rc = cs_kernel_core_add_hash(&hash_entry, 
				CS_VTBL_ID_L3_FLOW, cs_cb->common.guid, 
				cs_cb->common.guid_cnt, CS_IPSEC_HASH_TIME_OUT);
		if (rc == CS_HASH_ADD_ERR_OUT_OF_RESOURCE)
			return CS_ERROR;
		DBG(printk("%s:%d:done creating 1st entry\n", __func__, __LINE__));
	}

	/* 2nd entry for (CS_IPSEC_INBOUND == p_sadb->sa_dir)
	 * mask requirement:
	 * 1. port: re port id
	 * 2. recirc_idx = org_voq_idx 
	 * 3. packet size
	 * 4. l2 source mac
	 * 5. l3 source ip, dst ip, and ip protocol
	 * 6. l4 (if needed), source port, dst port
	 * Action: 
	 * 1. L2 source mac, dst mac, vlan, pppoe
	 * 2. dst voq = original dst voq */

	/* 2nd entry for (CS_IPSEC_OUTBOUND == p_sadb->sa_dir)
	 * mask requirement:
	 * 1. port: re port id
	 * 2. recirc_idx = org_voq_idx
	 * 3. packet size
	 * 4. L2 source mac
	 * 5. L3 source ip, dest ip, ip protocol, and no ip frag 
	 * 6. SPI 
	 * Action:
	 * 1) L2 source mac, dst mac, vlan, pppoe
	 * 2) dst voq = org dst voq*/

	DBG(printk("%s:%d:Creating 2nd entry\n", __func__, __LINE__));
	memset(&hash_entry, 0x0, sizeof(CS_HASH_ENTRY_S));

	/* FIXME!.. 
	 * some thinking might need to be done here if we need to consider 
	 * QoS within the IPsec Tunnel.  What could happen here is that 
	 * two different traffic might reach here within the same tunnel, but 
	 * the user puts a throttle on one of the traffic; therefore, we need to 
	 * apply QoS on it.  In this case, there will be conflict with recirc_idx 
	 * and mac_sa matching.   However, in another direction which the user 
	 * configures QoS on top of two outgoing traffic that go out in the same 
	 * tunnel, then we definitely need recirc_idx and mac_sa to provide 
	 * granutuity */
	hash_entry.mask = CS_HM_LSPID_MASK | CS_HM_PKTLEN_RNG_MATCH_VECTOR_MASK;
	hash_entry.mask |= CS_HM_RECIRC_IDX_MASK | CS_HM_MAC_SA_MASK;
	hash_entry.key.mac_sa[0] = (unsigned char)cs_cb->action.voq_pol.d_voq_id;
	hash_entry.key.recirc_idx = cs_cb->action.voq_pol.d_voq_id;

	if (CS_IPSEC_RCPU_DEC == table_id)
		hash_entry.key.lspid = ENCRYPTION_PORT;
	else
		hash_entry.key.lspid = ENCAPSULATION_PORT;

	status = cs_kernel_get_pktlen_rng_match_vector(CS_IPSEC_RE_SKB_SIZE_MAX, 
			CS_IPSEC_RE_SKB_SIZE_MIN, &hash_entry.key.pktlen_rng_match_vector);
	DBG(printk("%s:%d:lspid = %d, recirc_idx = %d, rng_match_vector = %x\n", 
				__func__, __LINE__, hash_entry.key.lspid, 
				hash_entry.key.recirc_idx, 
				hash_entry.key.pktlen_rng_match_vector));
	if (status != CS_OK) return status;

	if (p_sadb->ip_ver == CS_IPSEC_IPV4) {
		hash_entry.mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
		hash_entry.mask |= CS_HM_IP_PROT_MASK;
		hash_entry.key.sa[0] = ntohl(cs_cb->output.l3_nh.iph.sip);
		hash_entry.key.da[0] = ntohl(cs_cb->output.l3_nh.iph.dip);
		hash_entry.key.ip_prot = cs_cb->output.l3_nh.iph.protocol;
		DBG(printk("%s:%d:sip %x, dip %x, proto %d\n", __func__, __LINE__, 
					cs_cb->output.l3_nh.iph.sip, cs_cb->output.l3_nh.iph.dip, 
					cs_cb->output.l3_nh.iph.protocol));
		/* Enable all these hash masks to save tuple usage */
		hash_entry.mask |= CS_HM_SPI_MASK;// | CS_HM_IP_FRAGMENT_MASK;
		hash_entry.mask |= CS_HM_SPI_VLD_MASK;
		hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
		if ((cs_cb->output.l3_nh.iph.protocol == IPPROTO_ESP) || 
			   (cs_cb->output.l3_nh.iph.protocol == IPPROTO_AH))	{
			hash_entry.mask |= CS_HM_SPI_MASK;// | CS_HM_IP_FRAGMENT_MASK;
			hash_entry.mask |= CS_HM_SPI_VLD_MASK;
			hash_entry.key.spi_vld = 1;
			hash_entry.key.spi_idx = ntohl(cs_cb->output.l4_h.ah_esp.spi);
			DBG(printk("%s:%d:spi = %x\n", __func__, __LINE__, 
						cs_cb->output.l4_h.ah_esp.spi));
		//	hash_entry.key.ip_frag = 0x00;
		} else if (cs_cb->output.l3_nh.iph.protocol == IPPROTO_TCP) {
			/* check if L4 SPORT/DPORT is needed */
			hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
			hash_entry.key.l4_sp = ntohs(cs_cb->output.l4_h.th.sport);
			hash_entry.key.l4_dp = ntohs(cs_cb->output.l4_h.th.dport);
		} else if (cs_cb->output.l3_nh.iph.protocol == IPPROTO_UDP) {
			/* check if L4 SPORT/DPORT is needed */
			hash_entry.mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK;
			hash_entry.key.l4_sp = ntohs(cs_cb->output.l4_h.uh.sport);
			hash_entry.key.l4_dp = ntohs(cs_cb->output.l4_h.uh.dport);
		}
	} else {	/* if (CS_IPSEC_IPV6 == p_sadb->ip_ver) */
		hash_entry.mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK;
		/* FIXME! more for IPv6 stuff */
	}

	/* Action:
	 * 1) L2 source mac, dst mac, vlan, pppoe
	 * 2) dst voq = org dst voq*/

	/* hash action mask, field */
	cs_cb->output_mask |= CS_HM_MAC_DA_MASK | CS_HM_MAC_SA_MASK;
	status = cs_kernel_get_hash_action_from_cs_cb(cs_cb, &(hash_entry.result));
	if (status != CS_OK) return status;
	if (p_sadb->sa_dir == CS_IPSEC_INBOUND)
		hash_entry.result.action.decr_ttl_hoplimit = 1;
	/* assuming, the above API call will return the following info and set up 
	 * 		action/result info accordingly:
	 * 1) VLAN/PPPoE operation
	 * 2) dst_voq */
	/* however, we still need to worry about 3 cases that could happen to 
	 * VLAN/PPPoE operation because of the previous hash.
	 * 1) when replacing VLAN/PPPoE, but earlier we have already removed it.
	 * 2) when removing VLAN/PPPoe, but earlier we have already removed it.
	 * 3) when not doing any modification to VLAN/PPPoE, but earlier we have 
	 * 		already remove it. */
	if (f_pppoe_remove == TRUE) {
		if ((0 == hash_entry.result.action.pppoe_decap_en) && 
			   (0 == hash_entry.result.action.pppoe_encap_en)) {
			/* need to change the action to insert */
			hash_entry.result.action.pppoe_encap_en = 1;
			hash_entry.result.param.pppoe_session_id = 
				cs_cb->output.raw.pppoe_frame;
		} else if (hash_entry.result.action.pppoe_decap_en) {
			/* don't need to do anything, it's been done already */
			hash_entry.result.action.pppoe_decap_en = 0;
		}
	}

	if (f_vlan_remove == TRUE) {
		if (CS_VLAN_OP_REPLACE == hash_entry.result.action.flow_vlan_op_en) {
			/* need to change the action to insert */
			hash_entry.result.action.flow_vlan_op_en = CS_VLAN_OP_INSERT;
		} else if (CS_VLAN_OP_REMOVE == 
				hash_entry.result.action.flow_vlan_op_en) {
			/* don't need to do anything, it's been done already */
			hash_entry.result.action.flow_vlan_op_en = CS_VLAN_OP_NO_ENABLE;
		} else if (CS_VLAN_OP_KEEP == 
				hash_entry.result.action.flow_vlan_op_en) {
			if (CS_VLAN_OP_KEEP == cs_cb->action.l2.flow_vlan_op_en) {
				/* we need to insert the VLAN back */
				hash_entry.result.action.flow_vlan_op_en = CS_VLAN_OP_INSERT;
				hash_entry.result.param.first_vid = cs_cb->output.raw.vlan_id;
			}
		}
	}

	/* when creating hash, put in the guid */
	/* if the same hash entry has been created.. then it's fine.. 
	 * if the hash entry is created, then it's fine, too
	 * however, if the hash entry is not able to be created, then 
	 * it is NOT fine. */

	// FIXME.. the following should be CS_VTBL_ID_IPSEC.. but we don't have it 
	// at the first release... fix it later.
	//rc = cs_kernel_core_add_hash(&hash_entry, CS_VTBL_ID_IPSEC, 
	rc = cs_kernel_core_add_hash(&hash_entry, CS_VTBL_ID_L3_FLOW, 
			cs_cb->common.guid, cs_cb->common.guid_cnt, 
			CS_IPSEC_HASH_TIME_OUT);
	if (rc == CS_HASH_ADD_ERR_OUT_OF_RESOURCE)
		return CS_ERROR;
	DBG(printk("%s:%d:Done creating 2nd entry\n", __func__, __LINE__));

	/* at this point, at least one pair of hash entries have been created for 
	 * this given direction of tunnel, we should mark the tunnel as accelerated 
	 * in the software data structure */
	p_sadb->accelerated = 1;

	if (CS_OK == ipsec_peek_skb_queue_of_sadb(table_id, sa_id))
		ipsec_dequeue_skb_and_send_to_re(table_id, sa_id);

	return CS_OK;
} /* cs_hw_accel_ipsec_create_hash */

void cs_hw_accel_ipsec_init(void)
{
	unsigned int status;
#ifdef CS_IPC_ENABLED
	cs_status status_c;
#endif

	/* initialize the data base */
	cs_ipsec_db_init();

	/* FIXME! allocate classifier and SDB for traffic coming from RE */

	status = cs_kernel_reg_hash_cb(CS_KERNEL_IPSEC_ID_TAG, 
			cs_hw_accel_ipsec_hash_callback);
	if (status == CS_REG_FAILURE_TAG_OUT_OF_RANGE) {
		printk("%s::unable to register CS IPsec module callback function\n", 
				__func__);
		return;
	}
	if (status != CS_REG_SUCCESSFUL)
		printk("%s:warning:register callback return err code %x\n", __func__, 
				status);

	/* register the hook function to core */
	hw_jt.cs_ipsec_handler = k_jt_cs_ipsec_handler;
	hw_jt.cs_ipsec_done_handler = k_jt_cs_ipsec_done_handler;
	hw_jt.cs_ipsec_x_state_add = k_jt_cs_ipsec_x_state_add;
	hw_jt.cs_ipsec_x_state_delete = k_jt_cs_ipsec_x_state_delete;
	hw_jt.cs_ipsec_x_state_update = k_jt_cs_ipsec_x_state_update;

#ifdef CS_IPC_ENABLED
	/* FIXME! make sure IPC is insmod'ed */
	status_c = ipsec_register_ipc();
//	if (status_c != CS_OK) return;

	init_timer(&cs_ipc_timer);
	cs_ipc_timer.expires = round_jiffies(jiffies + 
			(CS_IPSEC_IPC_TIMER_PERIOD * HZ));
	cs_ipc_timer.data = (unsigned long)(&cs_ipc_timer);
	cs_ipc_timer.function = ipsec_ipc_timer_func;
	add_timer(&cs_ipc_timer);
#endif

	/* FIXME!! more? */

	f_ipsec_enbl = TRUE;

	return;
} /* cs_hw_accel_ipsec_init */

void cs_hw_accel_ipsec_exit(void)
{
	/* FIXME! implementation */
	hw_jt.cs_ipsec_handler = (void *)cs_hw_accel_nop;
	hw_jt.cs_ipsec_done_handler = (void *)cs_hw_accel_nop;
	hw_jt.cs_ipsec_x_state_add = (void *)cs_hw_accel_nop;
	hw_jt.cs_ipsec_x_state_delete = (void *)cs_hw_accel_nop;
	hw_jt.cs_ipsec_x_state_update = (void *)cs_hw_accel_nop;

	f_ipsec_enbl = FALSE;

	/* stop all the accelerated tunnel */
	
	/* deregister the hook function from core */

#ifdef CS_IPC_ENABLED
	del_timer_sync(&cs_ipc_timer);

	ipsec_deregister_ipc();
#endif

} /* cs_hw_accel_ipsec_exit */


