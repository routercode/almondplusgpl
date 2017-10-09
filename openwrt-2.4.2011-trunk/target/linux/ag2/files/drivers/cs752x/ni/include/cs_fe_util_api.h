#ifndef __CS_FE_UTIL_H__
#define __CS_FE_UTIL_H__

#include <linux/types.h>
#include "cs_gc.h"
#include "cs_vtable_hash.h"

/* L2 MAC result table assistence APIs */

/*
 * notes: when inserting both MAC_DA and MAC_SA at the same time,
 * MAC_DA must be the first 6 bytes passed in with *mac_addr.
 */
typedef enum {
	L2_LOOKUP_TYPE_DA,
	L2_LOOKUP_TYPE_SA,
	L2_LOOKUP_TYPE_PAIR,
	L2_LOOKUP_TYPE_MAX
} fe_l2_rslt_lookup_type_e;

int cs_fe_l2_result_alloc(unsigned char *mac_addr, unsigned char type,
	cs_vtable_hash_entry_t *vhash, u16 *return_idx);

int cs_fe_l3_result_alloc(u32 *ip_addr, bool is_v6,
	cs_vtable_hash_entry_t *vhash, u16 *return_idx);

int cs_fe_l2_result_dealloc(u8 da_en, u8 sa_en, u16 index);
int cs_fe_l3_result_dealloc(u16 index);
#if 0
int cs_fe_l2_result_alloc(unsigned char *mac_addr, unsigned char type,
		unsigned int *return_idx);
int cs_fe_l2_result_dealloc(unsigned int l2_idx);

/* L3 IP resource table assistence APIs */

int cs_fe_l3_result_alloc(__u32 *ip_addr, bool is_v6, unsigned int *return_idx);
int cs_fe_l3_result_dealloc(__u16 l3_idx);
#endif

/* FWDRSLT resource table assistence API */
int cs_fe_fwdrslt_del_by_idx(unsigned int idx);

/* helper function of AN BNG MAC table */
int cs_fe_an_bng_mac_get_port_by_mac(unsigned char *mac_addr, __u8 *pspid);

/* helper function of LPB table */
int cs_fe_lpb_get_lspid_by_pspid(__u8 pspid, __u8 *lspid);

/*
 * The purpose of this data structure is keep track of all connections (hash
 * entries) that use this value (L2 MAC, L3 IP, etc). Some are stored in hw,
 * that hw_index points the location.
 */
struct fe_rslt_lookup_entry_s {
//	__u16	crc16;
	__u16	hw_index;
	__u8	valid;
	// this is for entries with no hw index. hw_index can be used for
	// other meaning then.
	__u8	sw_only;

	atomic_t	users;
	union {
		struct {
			__u8 mac_addr[6];
		} mac;
		struct {
			__u8 mac_addr[12];
		} mac_pair;
		struct {
			__u32 addr;
		} ipv4;
		struct {
			__u32 addr[4];
		} ipv6;
		struct {
			__u16 port;
		} l4;
		struct {
			__u32 swid[2];
		} misc;
	};

	/* A gc_entry keeps all hash entries which use this field. When lookup
	 * entry is found, it can points to gc_entry instantantly. When lookup
	 * entry is set invalid or to be freed, gc_entry is set to NULL, while
	 * its memory will be freed in gc_entry garbage collection.
	 */
	struct cs_gc_list_s	*gc_entry;
#if 0
	/*
	 * Gc_entry only keeps mac da or mac sa, not pair. For mac pair lookup,
	 * gc_entry points to mac_da and gc_entry2 points to mac_sa.
	 */
	struct cs_gc_list_s	*gc_entry2;
#endif
	/* next bucket of the entry? */
	struct fe_rslt_lookup_entry_s *next;
};

#if 0
/* internal result lookup entry */
typedef struct fe_rslt_lookup_entry_s {
	__u16 crc16;
	__u16 rslt_index;
	__u16 valid;
	atomic_t users;

	union {
		struct {
			__u8	type;
			__u8	mac_addr[12];
		} l2;

		struct {
			bool	is_v6;
			__u32	ip_addr[4];
		} l3;
	};	/* union */
} fe_rslt_lookup_entry_t;
#endif
#endif /* __CS_FE_UTIL_H__ */
