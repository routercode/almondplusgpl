#ifndef CS_KERNEL_NAT_HW_ACC_SUPPORT
#define CS_KERNEL_NAT_HW_ACC_SUPPORT 1

#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_nat.h>
#include <linux/list.h>

#define CS_HW_OK	0x01
#define CS_SW_ONLY	0x02

#define CS_LAN2WAN	1
#define CS_WAN2LAN	2
#define CS_MAX_NAT_SESSION	8192

#define NAT_SIP			0x01
#define NAT_DIP			0x02
#define NAT_SPORT		0x04
#define NAT_DPORT		0x08

#define STAT_UNUSED			0x0
#define STAT_SEMI			0x1
#define STAT_ESTABLISHED	0x2

/* NAT Jump Table Entires
*  Fill/Remove necessary information to/from skb->cb[]
*/
void k_jt_cs_nat_ipv4_add_hook_before_nat(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo);
void k_jt_cs_nat_ipv4_add_hook_after_nat(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo);
void k_jt_cs_nat_ipv4_delete_hook(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo, unsigned int hooknum);
void k_jt_cs_kernel_adapt_nat_del_all(void);

/* del hash entry */
void k_jt_cs_nat_ipv4_del_hash(cs_uint64 guid);

/* Callback function */
void cs_kernel_nat_callback(cs_uint64 guid, int status);


// Initialization
int cs_nat_init(void);

struct cs_ker_adapt_nat_table {
	cs_uint64	guid;					/* GUID */
	cs_uint32	status;					/* valid? */
	cs_uint32	ori_sip,ori_dip;		/* original ip */
	cs_uint16	ori_sport,ori_dport;	/* original port number */
	cs_uint32	final_sip,final_dip;	/* ip after NAT */
	cs_uint16	final_sport,final_dport;/* port number after NAT */
	cs_uint16	proto;					/* TCP or UDP */
	cs_uint32	flag;					/*  */
	cs_uint8	direction;
	struct nf_conn *ct;					/* Conntrack pointer */
	enum ip_conntrack_info ctinfo;		/* conntrack info */
};

struct free_entry_list {
    struct list_head list;
    int index; 		/* free index intable */
};

/* function to pick an empty entry from private table */
struct cs_ker_adapt_nat_table* cs_kernel_nat_get_empty_entry(void);
int cs_kernel_adapt_nat_add_entry(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo);

#endif

