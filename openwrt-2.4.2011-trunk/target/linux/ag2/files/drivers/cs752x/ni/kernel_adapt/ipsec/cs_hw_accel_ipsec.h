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
 * cs_hw_accel_ipsec.h
 *
 * $Id: cs_hw_accel_ipsec.h,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This header file defines the data structures and APIs for CS IPsec Offload.
 */

#ifndef __CS_HW_ACCEL_IPSEC_H__
#define __CS_HW_ACCEL_IPSEC_H__

#include <linux/skbuff.h>
#include <net/xfrm.h>
#include <mach/cs_types.h>
#include <mach/cs75xx_fe_core_table.h>
#include <cs_hw_accel_util.h>

#define CS_IPC_ENABLED	1
#ifdef CS_IPC_ENABLED
#include <mach/g2cpu_ipc.h>
#endif

#define CS_IPSEC_TUN_NUM	512
#define CS_IPSEC_RCPU_DEC	0
#define CS_IPSEC_RCPU_ENC	1
#define CS_IPSEC_INBOUND	0
#define CS_IPSEC_OUTBOUND	1
#define CS_IPSEC_IPV4	0
#define CS_IPSEC_IPV6	1

#define CS_IPSEC_RE_SKB_SIZE_MIN	0
#define CS_IPSEC_RE_SKB_SIZE_MAX	1514

#define CS_IPSEC_HASH_TIME_OUT		120

/* guid is a 64-bit ID that's used for hash handling */
#define CS_IPSEC_GID(re_id, sa_id)	(((cs_uint64)CS_KERNEL_IPSEC_ID_TAG << 48) \
		| ((cs_uint64)re_id << 9) | (sa_id))
#define CS_IPSEC_GET_TAG_ID_FROM_GID(guid)	((cs_uint16)((guid >> 48) & 0xffff))
#define CS_IPSEC_GET_SA_ID_FROM_GID(guid)	((cs_uint16)(guid & 0x01ff))
#define CS_IPSEC_GET_RE_ID_FROM_GID(guid)	((cs_uint8)((guid >> 9) & 0x01))
#define CS_IPSEC_SPGID(re_id, sp_id) (((cs_uint64)CS_KERNEL_IPSEC_ID_TAG << 48) \
		| ((cs_uint64)re_id << 17) | ((cs_uint64)sp_id << 10))
#define CS_IPSEC_SPGID_MASK			0x0003fc00
#define CS_IPSEC_GET_SP_ID_FROM_SP_GID(guid)	((cs_uint16)((guid >> 10) & 0x07f))
#define CS_IPSEC_GET_RE_ID_FROM_SP_GID(guid)	((cs_uint8)((guid >> 17) & 0x01))

#define CS_IPSEC_SW_ACTION_ID(func_id, recir_id, sa_id)		\
	(((cs_uint32)(func_id << 16)) | ((cs_uint32)(recir_id << 9)) | (sa_id))
#define CS_IPSEC_GET_RE_ID_FROM_FUNC_ID(func_id)			\
	((func_id >> 1) ^ 0x1)

/* VOQ related definition */
#define CS_IPSEC_RE_VOQ_ID(re_id)	((re_id << 3) + ENCRYPTION_VOQ_BASE)
//#define CS_IPSEC_RE_VOQ_ID(re_id)	((re_id << 3) + 51)

typedef union cs_ipaddr_s {
	cs_uint32 addr;	/* IPv4 address */
//	cs_uint32 addr6[4]; /* IPv6 address */ // FIXME! disable for now
} cs_ipaddr_t;

typedef struct cs_ipsec_skb_queue_s {
	struct cs_ipsec_skb_queue_s *next;
	struct sk_buff *skb;
} cs_ipsec_skb_queue_t;

/* maximum queue size to handle recovery queue packet */
#define CS_IPSEC_CB_QUEUE_MAX	30

typedef struct cs_ipsec_cb_queue_s {
	struct cs_ipsec_cb_queue_s *next;
	cs_uint8 idx;
	struct sk_buff skb;
} cs_ipsec_cb_queue_t;

#define MAX_ENC_KEY_LEN		32
#define MAX_AUTH_KEY_LEN	32

typedef struct cs_ipsec_sadb_s {
	cs_uint16	sa_idx;
	cs_uint8	replay_window;
	cs_uint32	spi;
	cs_uint32	seq_num;
	cs_uint8	ekey[MAX_ENC_KEY_LEN];
	cs_uint8	akey[MAX_AUTH_KEY_LEN];
	cs_uint32	used:1,		/* 0=No, 1=Yes */
				ip_ver:1,	/* 0=IPv4, 1=IPv6 */
				proto:1,	/* 0=ESP; 1=AH */
				mode:1,		/* 0=Tunnel; 1=Transport */
				sa_dir:1,	/* 0=Inbound; 1=Outbound */
				accelerated:1,	/* 0=No, 1=Yes */
				stop:1,		/* 0=No, 1=Yes */
	/* encryption mode based on elliptic */
				ealg:2,
				ealg_mode:3,
				enc_keylen:4,	/* (n * 4) bytes */
				iv_len:4,	/* (n * 4) bytes for CBC */
	/* authentication mode based on elliptic */
				aalg:3,
				auth_keylen:4,	/* (n * 4) bytes */
				icv_trunclen:4,	/* (n * 4) bytes */
				rsvd:1;

	/* if tunnel mode the outer IP header template is for reconstructing */
	cs_ipaddr_t	tunnel_saddr;
	cs_ipaddr_t	tunnel_daddr;
	cs_uint32	lifetime_bytes;
	cs_uint32	bytes_count;
	cs_uint32	lifetime_packets;
	cs_uint32	packets_count;
	cs_uint16	checksum;
	cs_uint8	ekey_context[64];
	cs_uint8	akey_context[64];
	struct xfrm_state *x_state;
} __attribute__((__packed__)) cs_ipsec_sadb_t;

typedef struct cs_ipsec_db_s {
	cs_uint16 p_re0_idx;
	cs_uint16 p_re1_idx;
	cs_uint16 re0_used_count;
	cs_uint16 re1_used_count;
	cs_uint16 re0_spidx_mask;
	cs_uint16 re1_spidx_mask;
	cs_ipsec_cb_queue_t *re0_cb_q[CS_IPSEC_TUN_NUM];
	cs_ipsec_cb_queue_t *re1_cb_q[CS_IPSEC_TUN_NUM];
	cs_ipsec_skb_queue_t *re0_skb_q[CS_IPSEC_TUN_NUM];
	cs_ipsec_skb_queue_t *re1_skb_q[CS_IPSEC_TUN_NUM];
	cs_ipsec_sadb_t re0_sadb_q[CS_IPSEC_TUN_NUM];
	cs_ipsec_sadb_t re1_sadb_q[CS_IPSEC_TUN_NUM];
} cs_ipsec_db_t;

/* special index handling */
#define CS_IPSEC_SP_IDX_START	112
#define CS_IPSEC_SP_IDX_NUM		16

/* CS IPsec Offload supported cipher algorithm */
#define CS_IPSEC_CIPHER_NULL	0
#define CS_IPSEC_DES			1
#define CS_IPSEC_3DES			1
#define CS_IPSEC_AES			2

static cs_int8 cs_ipsec_get_cipher_alg(unsigned char *alg_name)
{
	unsigned char name[10][20] = {"ecb(cipher_null)", "cipher_null", 
		"cbc(des)", "des", "cbc(des3_ede)", "des3_ede", 
		"aes", "cbc(aes)", "rfc4106(gcm(aes))", "rfc4309(ccm(aes))"};
	cs_uint8 alg_num[10] = {CS_IPSEC_CIPHER_NULL, CS_IPSEC_CIPHER_NULL, 
		CS_IPSEC_DES, CS_IPSEC_DES, CS_IPSEC_3DES, CS_IPSEC_3DES, 
		CS_IPSEC_AES, CS_IPSEC_AES, CS_IPSEC_AES, CS_IPSEC_AES};
	cs_uint8 iii;

	for (iii = 0; iii < 10; iii++) {
		if (strncmp(alg_name, &name[iii][0], 20) == 0)
			return alg_num[iii];
	}
	return -1;
} /* cs_ipsec_get_cipher_alg */

/* CS IPsec Offload supported cipher mode */
#define CS_IPSEC_CIPHER_ECB		0
#define CS_IPSEC_CIPHER_CBC		1
#define CS_IPSEC_CIPHER_CTR		2
#define CS_IPSEC_CIPHER_CCM		3
#define CS_IPSEC_CIPHER_GCM		5
#define CS_IPSEC_CIPHER_OFB		7
#define CS_IPSEC_CIPHER_CFB		8

static cs_uint8 cs_ipsec_get_cipher_mode(cs_uint8 alg_mode)
{
	switch (alg_mode) {
		case SADB_EALG_NULL:
			return CS_IPSEC_CIPHER_ECB;
		case SADB_EALG_DESCBC:
		case SADB_EALG_3DESCBC:
		case SADB_X_EALG_AESCBC:
			return CS_IPSEC_CIPHER_CBC;
		case SADB_X_EALG_AESCTR:
			return CS_IPSEC_CIPHER_CTR;
		case SADB_X_EALG_AES_CCM_ICV8:
		case SADB_X_EALG_AES_CCM_ICV12:
		case SADB_X_EALG_AES_CCM_ICV16:
			return CS_IPSEC_CIPHER_CCM;
		case SADB_X_EALG_AES_GCM_ICV8:
		case SADB_X_EALG_AES_GCM_ICV12:
		case SADB_X_EALG_AES_GCM_ICV16:
			return CS_IPSEC_CIPHER_GCM;
		default:
			return -1;
	}
} /* cs_ipsec_get_cipher_mode */

/* CS IPsec Offload supported authentication algorithm */
#define CS_IPSEC_AUTH_NULL	0
#define CS_IPSEC_MD5		1
#define CS_IPSEC_SHA1		2
#define CS_IPSEC_SHA224		3
#define CS_IPSEC_SHA256		4

static cs_int8 cs_ipsec_get_auth_alg(unsigned char *alg_name)
{
	unsigned char name[5][16] = {"md5", "sha1", "sha224", "sha256", 
		"digest_null"};
	unsigned char name_hmac[5][20] = {"hmac(md5)", "hmac(sha1)", 
		"hmac(sha224)", "hmac(sha256)", "hmac(digest_null)"};
	cs_uint8 alg_num[5] = {CS_IPSEC_MD5, CS_IPSEC_SHA1, CS_IPSEC_SHA224, 
		CS_IPSEC_SHA256, CS_IPSEC_AUTH_NULL};
	cs_uint8 iii;

	for (iii = 0; iii < 5; iii++) {
		if (strncmp(alg_name, &name[iii][0], 16) == 0)
			return alg_num[iii];
		if (strncmp(alg_name, &name_hmac[iii][0], 20) == 0)
			return alg_num[iii];
	}
	return -1;
} /* cs_ipsec_get_auth_alg */

#ifdef CS_IPC_ENABLED
#define CS_IPSEC_IPC_TIMER_PERIOD	5	/* second */

/* IPC CPU ID for Kernel, RE0, and RE1 */
#define CS_IPSEC_IPC_ARM_CPU_ID		0x00
#define CS_IPSEC_IPC_RE0_CPU_ID		0x01
#define CS_IPSEC_IPC_RE1_CPU_ID		0x02

/* client ID for RE0 and RE1 */
#define CS_IPSEC_IPC_RE0_CLNT_ID	0x1
#define CS_IPSEC_IPC_RE1_CLNT_ID	0x2

/* msg type */
#define CS_IPSEC_IPC_RE_INIT			0x01
#define CS_IPSEC_IPC_RE_STOP			0x02
#define CS_IPSEC_IPC_RE_UPDATE			0x03
#define CS_IPSEC_IPC_RE_INIT_COMPLETE	0x04
#define CS_IPSEC_IPC_RE_STOP_COMPLETE	0x05
#define CS_IPSEC_IPC_RE_UPDATE_COMPLETE	0x06
#define CS_IPSEC_IPC_RE_STOP_BY_RE		0x07

#define CS_IPSEC_IPC_RE_DEAD			0x00
#define CS_IPSEC_IPC_RE_ACTIVE			0x01
#define CS_IPSEC_IPC_RE_ACTIVE_CHECK1	0x02
#define CS_IPSEC_IPC_RE_ACTIVE_CHECK2	0x02

typedef struct cs_ipsec_ipc_msg_s {
	cs_uint8	re_id;
	cs_uint16	sa_id;
	cs_uint32	data;
} __attribute__((__packed__)) cs_ipsec_ipc_msg_t;
#endif

cs_status k_jt_cs_ipsec_handler(struct sk_buff *skb, struct xfrm_state *x, cs_uint8 ip_ver, cs_uint8 dir);
void k_jt_cs_ipsec_done_handler(struct sk_buff *skb, cs_uint8 ip_ver, cs_uint8 dir);
void k_jt_cs_ipsec_x_state_add(struct xfrm_state *x);
void k_jt_cs_ipsec_x_state_delete(struct xfrm_state *x);
void k_jt_cs_ipsec_x_state_update(struct xfrm_state *x);

cs_status cs_hw_accel_ipsec_handle_rx(struct sk_buff *skb, 
		cs_uint8 src_port, cs_uint16 in_voq_id, cs_uint32 sw_action);
void cs_hw_accel_ipsec_hash_callback(cs_uint64 guid, cs_int32 status);
cs_status cs_hw_accel_ipsec_create_hash(CS_KERNEL_ACCEL_CB_T *cs_cb, 
		cs_uint64 guid);
void cs_hw_accel_ipsec_init(void);
void cs_hw_accel_ipsec_exit(void);

#endif /* __CS_HW_ACCEL_IPSEC_H__ */
