#include <mach/cs_types.h>
#include <mach/hw_accel.h>
#include "cs_hw_accel_cb.h"
#include "cs_hw_accel_util.h"
#include "cs_hw_accel_core.h"
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include "cs_hw_accel_nat.h"

#define errork(x)
//#define CS_MOD_MASK_NAT	0x0010	/* FIXME: use definition in cs_hw_accel_cb.h */

struct cs_ker_adapt_nat_table cs_ker_adapt_nat_table[CS_MAX_NAT_SESSION];

extern struct cs_kernel_hw_accel_jt hw_jt;

/* TCP BYPASS PORT */
#define FTP_CTRL_PORT 			(21)
#define H323_PORT				(1720)
#define T120_PORT				(1503)
#define PPTP_PORT				(1723)
#define UPNP_PORT				(5000)

/* UDP BYPASS PORT */
#define TFTP_PORT 				(69)
#define DNS_PORT 				(53)
#define NTP_PORT				(123)
#define RAS_PORT				(1719)
#define BOOTP67_PORT			(67)
#define BOOTP68_PORT			(68)
#define ISAKMP_PORT				(500)
#define UPNP_DISC_PORT			(1900)
#define L2TP_CTRL_PORT			(1701)


static unsigned short	bypass_tcp_port_list[]={FTP_CTRL_PORT,
		   				H323_PORT,
		   				T120_PORT,
		   				PPTP_PORT,
		   				UPNP_PORT,
						0};
static unsigned short	bypass_udp_port_list[]={DNS_PORT,
				  		NTP_PORT,
				  		TFTP_PORT,
						RAS_PORT,
				  		BOOTP67_PORT,
				  		BOOTP68_PORT,
				  		ISAKMP_PORT,
				  		UPNP_DISC_PORT,
				  		L2TP_CTRL_PORT,
				   		0};


LIST_HEAD(free_list);

struct cs_ker_adapt_nat_table* cs_kernel_nat_get_empty_entry(void)
{
	int idx=0;
	struct list_head *ptr;
	struct free_entry_list *entry;

	rcu_read_lock();

	/* Return if no free entry */
	if(list_empty(&free_list))
		return NULL;

	/* Get one from free list */
	ptr = free_list.next;
	entry = list_entry(ptr,struct free_entry_list, list);
	idx = entry->index;
	list_del(ptr);
	kfree(entry);

	rcu_read_unlock();

	return  &cs_ker_adapt_nat_table[idx];

/*	idx = 0;
	while(idx<CS_MAX_NAT_SESSION && (cs_ker_adapt_nat_table[idx].status!=STAT_UNUSED))
		idx++;

	if(idx==CS_MAX_NAT_SESSION)
		return NULL;

	return &cs_ker_adapt_nat_table[idx];
*/
}

int cs_kernel_adapt_nat_add_entry(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct iphdr 		*iph = ip_hdr(skb);
	CS_KERNEL_ACCEL_CB_T	*cs_cb=(CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	struct cs_ker_adapt_nat_table *nat_info;
#if 0
	nat_info = cs_kernel_nat_get_empty_entry();
	if(nat_info == NULL){
		errork("NAT table full\n");
		return -EBUSY;
	}

	/* FIXME: When cs_hw_accel_util.c ready */
	//nat_info->guid = cs_kernel_adapt_get_guid(CS_KERNEL_NAT_ID_TAG);
	nat_info->status = STAT_SEMI;
	nat_info->ori_sip = cs_cb->input.l3_nh.iph.sip;
	nat_info->ori_dip = cs_cb->input.l3_nh.iph.dip;
	nat_info->proto = iph->protocol;
	nat_info->final_sip = iph->saddr;
	nat_info->final_dip = iph->daddr;
	nat_info->ct = ct;
	nat_info->ctinfo = ctinfo;
	if( iph->protocol== IPPROTO_TCP){
		nat_info->ori_sport = cs_cb->input.l4_h.th.sport;
		nat_info->ori_dport = cs_cb->input.l4_h.th.dport;
		nat_info->final_sport = cs_cb->output.l4_h.th.sport;
		nat_info->final_dport = cs_cb->output.l4_h.th.dport;

		/* forward only if special control flag not set */
		//FIXME: we need enable these flags after delete fuction work.
		cs_cb->input.l4_h.th.urg = 0;
		cs_cb->input.l4_h.th.syn = 0;
		cs_cb->input.l4_h.th.fin = 0;
		cs_cb->input.l4_h.th.rst = 0;
		cs_cb->input_mask |= CS_HM_TCP_CTRL_MASK;
		cs_cb->input_tcp_flag_mask = CS_HASH_MASK_TCP_RST_FLAG | CS_HASH_MASK_TCP_SYN_FLAG
			| CS_HASH_MASK_TCP_FIN_FLAG | CS_HASH_MASK_TCP_URG_FLAG;
	}
	else if( iph->protocol== IPPROTO_UDP){
		nat_info->ori_sport = cs_cb->input.l4_h.uh.sport;
		nat_info->ori_dport = cs_cb->input.l4_h.uh.dport;
		nat_info->final_sport = cs_cb->output.l4_h.uh.sport;
		nat_info->final_dport = cs_cb->output.l4_h.uh.dport;
	}
#endif
#if 0
	/* Always use SIP/DIP/PROTOCOL/SPORT/DPORT as hash key */
	//FIXME: wget will got problem. if enable CS_HM_TCP_CTRL_MASK
	if( iph->protocol== IPPROTO_TCP) {
		cs_cb->input.l4_h.th.urg = 0;
		cs_cb->input.l4_h.th.syn = 0;
		cs_cb->input.l4_h.th.fin = 0;
		cs_cb->input.l4_h.th.rst = 0;
		cs_cb->input_mask |= CS_HM_TCP_CTRL_MASK;
		cs_cb->input_tcp_flag_mask = CS_HASH_MASK_TCP_RST_FLAG | CS_HASH_MASK_TCP_SYN_FLAG
			| CS_HASH_MASK_TCP_FIN_FLAG | CS_HASH_MASK_TCP_URG_FLAG;
	}
#endif
	
	cs_cb->input_mask |= CS_HM_IP_SA_MASK|CS_HM_IP_DA_MASK|
			CS_HM_IP_PROT_MASK|CS_HM_L4_DP_MASK|CS_HM_L4_SP_MASK |
			CS_HM_IP_VLD_MASK | CS_HM_IP_VER_MASK | CS_HM_L4_VLD_MASK |
			CS_HM_L3_CHKSUM_ERR_MASK ;

	/* timer out period */
	cs_cb->tmo.interval = 120;

	//printk("%s: cs_cb->input_mask = 0x%X\n",__func__, cs_cb->input_mask);
	return 0;
}

/* k_jt_cs_nat_ipv4_add_hook_before_nat()
 * This function was designed to be called before NAT.
 * We record original network head here.
*/
void k_jt_cs_nat_ipv4_add_hook_before_nat(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct iphdr 	*iph = ip_hdr(skb);
	unsigned int	hdroff = iph->ihl*4;
	struct tcphdr 	*tcphdr;
	struct udphdr 	*udphdr;
	int i;
	cs_uint16	sport, dport, *port_ptr;
	CS_KERNEL_ACCEL_CB_T	*cs_cb=(CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);

//	printk("%s::tag %x, sw only %x, protocol %x\n",
//		__func__, cs_cb->common.tag, cs_cb->common.sw_only, iph->protocol);

	if(cs_cb->common.tag != CS_CB_TAG)
		return ;

	if(cs_cb->common.sw_only == CS_SWONLY_STATE)
		return;



	/* PREROUTING, record original field for hash key */
	if(iph->protocol == IPPROTO_TCP) {					/* TCP */
		tcphdr = (struct tcphdr*)(skb->data + hdroff);

		if (tcp_flag_word(tcphdr) & (TCP_FLAG_SYN | TCP_FLAG_FIN | TCP_FLAG_RST)) {
			cs_cb->common.sw_only = CS_SWONLY_STATE;
			return;
		}
		cs_cb->input.l3_nh.iph.protocol = IPPROTO_TCP;

		cs_cb->input.l4_h.th.sport = tcphdr->source;
		cs_cb->input.l4_h.th.dport = tcphdr->dest;
		cs_cb->input_mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK ;
		/* FIXME, no hard code: FTP control packet */
		sport = ntohs(tcphdr->source);
		dport = ntohs(tcphdr->dest);
		port_ptr = bypass_tcp_port_list;
		for (i=0; *port_ptr; i++, port_ptr++) {
			if (sport == *port_ptr || dport == *port_ptr) {
				cs_cb->common.sw_only = CS_SWONLY_STATE;
				//printk("SW only: TCP, sport or dport = %d\n", *port_ptr);
				return;
			}
		}
	}
	else if(iph->protocol == IPPROTO_UDP){				/* UDP */
		udphdr = (struct udphdr*)(skb->data + hdroff);
		cs_cb->input.l3_nh.iph.protocol = IPPROTO_UDP;
		cs_cb->input.l4_h.uh.sport = udphdr->source;
		cs_cb->input.l4_h.uh.dport = udphdr->dest;
		cs_cb->input_mask |= CS_HM_L4_SP_MASK | CS_HM_L4_DP_MASK ;
		sport = ntohs(udphdr->source);
		dport = ntohs(udphdr->dest);
		port_ptr = bypass_udp_port_list;
		for (i=0; *port_ptr; i++, port_ptr++) {
			if (sport == *port_ptr || dport == *port_ptr) {
				cs_cb->common.sw_only = CS_SWONLY_STATE;
				//printk("SW only: UDP, sport or dport = %d\n", *port_ptr);
				return;
			}
		}
	}
	else{			/* Only TCP/UDP be offloaded to FE */
		return ;
	}

	cs_cb->input.l3_nh.iph.sip = iph->saddr;
	cs_cb->input.l3_nh.iph.dip = iph->daddr;
	cs_cb->input_mask |= CS_HM_IP_SA_MASK | CS_HM_IP_DA_MASK ;

	cs_cb->input.l3_nh.iph.protocol = iph->protocol;
	cs_cb->input_mask |= CS_HM_IP_PROT_MASK ;

	cs_cb->common.module_mask |= CS_MOD_MASK_NAT_PREROUTE;
//	printk("%s: cs_cb = 0x%X, cs_cb->common.module_mask = 0x%x\n",
//		__func__,cs_cb, cs_cb->common.module_mask);
}

/* k_jt_cs_nat_ipv4_add_hook_after_nat()
 * This function was designed to be called after NAT.
 * We record modified network head here. And add information to skb->cb[]
*/
void k_jt_cs_nat_ipv4_add_hook_after_nat(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct iphdr 		*iph = ip_hdr(skb);
	unsigned int hdroff = iph->ihl*4;
	struct tcphdr 		*tcphdr;
	struct udphdr 		*udphdr;
	CS_KERNEL_ACCEL_CB_T *cs_cb= (CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	int rc;

	if(cs_cb->common.tag != CS_CB_TAG)
		return ;

	if(cs_cb->common.sw_only == CS_SWONLY_STATE)
		return;

	/* POSTROUTING, record modified info */
	/* Check if packet be processed by NAT-PREROUTING ? */
	if(cs_cb->common.module_mask & CS_MOD_MASK_NAT_PREROUTE){
		if((cs_cb->input.l3_nh.iph.sip ^ iph->saddr)){		/* SIP changed */
			//cs_cb->action.l3.ip_sa_replace_en = 1;
			cs_cb->output.l3_nh.iph.sip = iph->saddr;
			cs_cb->output_mask |= CS_HM_IP_SA_MASK ;
			//printk("%s:cs_cb->output_mask=0x%X, SIP changed \n",__func__,cs_cb->output_mask);
		}
		if((cs_cb->input.l3_nh.iph.dip ^ iph->daddr)){		/* DIP changed */
			//cs_cb->action.l3.ip_da_replace_en = 1;
			cs_cb->output.l3_nh.iph.dip = iph->daddr;
			cs_cb->output_mask |= CS_HM_IP_DA_MASK ;
			//printk("%s:cs_cb->output_mask=0x%X, DIP changed \n",__func__,cs_cb->output_mask);
		}
		if(iph->protocol == IPPROTO_TCP){

			if (cs_cb->common.state != TCP_CONNTRACK_ESTABLISHED) {
				//printk("NOT ESTABLISHED: cs_cb->common.state = %d\n", cs_cb->common.state);
				return;
			}
			tcphdr = (struct tcphdr*)(skb->data + hdroff);

			//if (tcphdr->rst && tcphdr->ack)
				//printk("************* tcphdr RST + ACK ***************\n");
			if((cs_cb->input.l4_h.th.sport ^ tcphdr->source)){/* tcp sport changed */
				//cs_cb->action.l4.sp_replace_en = 1;
				cs_cb->output.l4_h.th.sport = tcphdr->source;
				cs_cb->output_mask |= CS_HM_L4_SP_MASK ;
				//printk("%s: tcp sport changed \n",__func__);
			}
			if((cs_cb->input.l4_h.th.dport ^ tcphdr->dest)){	/* tcp dport changed */
				//cs_cb->action.l4.dp_replace_en = 1;
				cs_cb->output.l4_h.th.sport = tcphdr->dest;
				cs_cb->output_mask |= CS_HM_L4_DP_MASK ;
				//printk("%s: tcp dport changed \n",__func__);
			}

		}
		else if(iph->protocol == IPPROTO_UDP){
			udphdr = (struct udphdr*)(skb->data + hdroff);
			if((cs_cb->input.l4_h.uh.sport ^ udphdr->source)){/* udp sport changed */
				//cs_cb->action.l4.sp_replace_en = 1;
				cs_cb->output.l4_h.uh.sport = udphdr->source;
				cs_cb->output_mask |= CS_HM_L4_SP_MASK ;
				//printk("%s: IPPROTO_UDP \n",__func__);
			}
			if((cs_cb->input.l4_h.uh.dport ^ udphdr->dest)){	/* udp dport changed */
				//cs_cb->action.l4.dp_replace_en = 1;
				cs_cb->output.l4_h.uh.sport = udphdr->dest;
				cs_cb->output_mask |= CS_HM_L4_DP_MASK ;
			}
		}
		cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
		cs_cb->common.module_mask |= CS_MOD_MASK_NAT;
		//printk("%s: cs_cb->common.sw_only = %d\n",__func__, cs_cb->common.sw_only);
	}
	else{			/* This packet is not come from prerouting? don't touch it! */
		cs_cb->common.module_mask &= ~CS_MOD_MASK_NAT;
	}
//	printk(" ESTABLISHED: cs_cb->common.state = %d\n", cs_cb->common.state);
//	printk("%s: cs_cb->output_mask = 0x%X\n",__func__, cs_cb->output_mask);
	/* record extra information */
	rc = cs_kernel_adapt_nat_add_entry(skb, ct, ctinfo);
}

void k_jt_cs_nat_ipv4_delete_hook(struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo, unsigned int hooknum)
{

}

void k_jt_cs_nat_ipv4_del_hash(cs_uint64 guid)
{
	/* FIXME: when cs_hw_accel_core.c ready */
	//cs_kernel_core_del_hash_by_guid(guid);
}

/* Delete all entry and hash */
void k_jt_cs_kernel_adapt_nat_del_all(void)
{
	int i;
	struct list_head *ptr;
	struct free_entry_list *e;
	struct free_entry_list *entry;


	for(i=0;i<CS_MAX_NAT_SESSION;i++){
		if(cs_ker_adapt_nat_table[i].status == STAT_ESTABLISHED)
			k_jt_cs_nat_ipv4_del_hash(cs_ker_adapt_nat_table[i].guid);
		memset(&cs_ker_adapt_nat_table[i],0,sizeof(struct cs_ker_adapt_nat_table));
	}


	/* Clear free list first */
	while(!list_empty(&free_list)){
		ptr = free_list.next;
		entry = list_entry(ptr,struct free_entry_list, list);
		i = entry->index;
		list_del(ptr);
		kfree(entry);
	}

	/* Re-init free-list than */
	for(i=0;i<CS_MAX_NAT_SESSION;i++){
		e = kmalloc(sizeof(struct free_entry_list), GFP_KERNEL);
		if(e==NULL){
			errork("mem alloc fail!\n");
		}
		e->index = i;
		list_add_tail(&e->list, &free_list);
	}

}

void cs_kernel_nat_callback(cs_uint64 guid, int status )
{
	int i;
	struct cs_ker_adapt_nat_table *nat_info;

	for(i=0;i<CS_MAX_NAT_SESSION;i++){
		if(cs_ker_adapt_nat_table[i].guid == guid){
			nat_info = &cs_ker_adapt_nat_table[i];
			break;
		}
	}

	if(i==CS_MAX_NAT_SESSION){
		//printk("Invalid GUID for NAT Callback!\n");
		return ;
	}

	switch(status){
		case CS_HASH_CB_FAILED_RESOURCE:
		case CS_HASH_CB_DELETE_BY_PEER:
			/* Clear private table entry */
			memset(nat_info,0,sizeof(struct cs_ker_adapt_nat_table));
			break;
		case CS_HASH_CB_SUCCESSFUL:
		default:
			nat_info->status = STAT_ESTABLISHED;
			break;
	}
}

int cs_nat_init(void )
{
	int i;
	struct list_head *ptr;
	struct free_entry_list *e;
	struct free_entry_list *entry;

	/* Init private table */
	memset(&cs_ker_adapt_nat_table,0,sizeof(cs_ker_adapt_nat_table)*CS_MAX_NAT_SESSION);


	/* init free list */
	for(i=0;i<CS_MAX_NAT_SESSION;i++){
		e = kmalloc(sizeof(struct free_entry_list), GFP_KERNEL);
		if(e==NULL){
			errork("mem alloc fail!\n");
			goto Abort;
		}
		e->index = i;
		list_add_tail(&e->list, &free_list);
	}

	hw_jt.cs_nat_ipv4_add_hook_before_nat = &k_jt_cs_nat_ipv4_add_hook_before_nat;
	hw_jt.cs_nat_ipv4_add_hook_after_nat = &k_jt_cs_nat_ipv4_add_hook_after_nat;

	/* register callback function */
	/* FIXME: wait when cs_hw_accel_util.c ready */
	//cs_kernel_reg_hash_cb( CS_KERNEL_NAT_ID_TAG, &cs_kernel_nat_callback );
	return 0;

Abort:
	/* free list */
	while(!list_empty(&free_list)){
		ptr = free_list.next;
		entry = list_entry(ptr,struct free_entry_list, list);
		list_del(ptr);
		kfree(entry);
	}
	return 1;
}





