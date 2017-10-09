#include <mach/cs_types.h>
#include <mach/hw_accel.h>
#include "../include/cs_hw_accel_cb.h"
#include "../include/cs_hw_accel_util.h"
#include "../include/cs_hw_accel_core.h"
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in6.h>
#include <net/ip6_fib.h>
#include <net/ipv6.h>
#include "cs_hw_accel_ipv6.h"

#ifdef CONFIG_CS752X_PROC
#include "cs752x_proc.h"

extern u32 cs_adapt_debug;
#endif /* CONFIG_CS752X_PROC */

#define DBG(x)	if(cs_adapt_debug & CS752X_ADAPT_IPV6)	x

extern struct cs_kernel_hw_accel_jt hw_jt;

/* k_jt_cs_nat_ipv4_add_hook_before_nat()
 * This function was designed to be called before NAT.
 * We record original network head here.
*/
void k_jt_cs_ipv6_hw_route_input(struct sk_buff *skb, struct rt6_info *rt)
{
	struct ipv6hdr *hdr;
	CS_KERNEL_ACCEL_CB_T	*cs_cb=(CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	
	if(cs_cb->common.tag != CS_CB_TAG)
		return ;
	
	
	hdr = ipv6_hdr(skb);
	
	
	if((hdr->nexthdr == IPPROTO_HOPOPTS) || (hdr->nexthdr == IPPROTO_ROUTING) || (hdr->nexthdr == IPPROTO_ICMPV6) ||
		(hdr->nexthdr == IPPROTO_NONE) || (hdr->nexthdr == IPPROTO_DSTOPTS) || (hdr->nexthdr == IPPROTO_MH) ){
			DBG(printk("^^^^^^^^^^^^^^^^ skip ^^^^^^^^^^^^^^^^\n"));
			return ;
	}
		
	cs_cb->input.l3_nh.ipv6h.dip[0] = hdr->daddr.s6_addr32[3];
	cs_cb->input.l3_nh.ipv6h.dip[1] = hdr->daddr.s6_addr32[2];
	cs_cb->input.l3_nh.ipv6h.dip[2] = hdr->daddr.s6_addr32[1];
	cs_cb->input.l3_nh.ipv6h.dip[3] = hdr->daddr.s6_addr32[0];
	
	DBG(printk("IPV6 DIP: %x-%x-%x-%x\n",cs_cb->input.l3_nh.ipv6h.dip[0],cs_cb->input.l3_nh.ipv6h.dip[1], \
										cs_cb->input.l3_nh.ipv6h.dip[2],cs_cb->input.l3_nh.ipv6h.dip[3]));

	cs_cb->common.module_mask |= CS_MOD_MASK_IPV6_ROUTING;
	cs_cb->input_mask |= CS_HM_IP_DA_MASK|CS_HM_IPV6_MASK | \
				CS_HM_IPV6_NDP_MASK | CS_HM_IPV6_HBH_MASK | CS_HM_L3_CHKSUM_ERR_MASK |
				CS_HM_IPV6_RH_MASK | CS_HM_IPV6_DOH_MASK | CS_HM_IP_VER_MASK;
						
	return ;
}

void k_jt_cs_ipv6_hw_route_output(struct sk_buff *skb)
{

	struct ipv6hdr *hdr;
	CS_KERNEL_ACCEL_CB_T	*cs_cb=(CS_KERNEL_ACCEL_CB_T *)CS_KERNEL_SKB_CB(skb);
	
	if(cs_cb->common.tag != CS_CB_TAG)
		return ;
	
	
	hdr = ipv6_hdr(skb);
	
	
	if((hdr->nexthdr == IPPROTO_HOPOPTS) || (hdr->nexthdr == IPPROTO_ROUTING) || (hdr->nexthdr == IPPROTO_ICMPV6) ||
		(hdr->nexthdr == IPPROTO_NONE) || (hdr->nexthdr == IPPROTO_DSTOPTS) || (hdr->nexthdr == IPPROTO_MH) ){
			DBG(printk("^^^^^^^^^^^^^^^^ skip ^^^^^^^^^^^^^^^^\n"));
			return ;
	}
		
	if( (cs_cb->common.module_mask | CS_MOD_MASK_IPV6_ROUTING) && \
		(cs_cb->common.sw_only != CS_SWONLY_STATE) ){
		cs_cb->common.sw_only = CS_SWONLY_HW_ACCEL;
		cs_cb->common.dec_ttl = CS_DEC_TTL_ENABLE;
		DBG(printk("IPv6 Forwarding\n"));
	}
		
	return ;
}

int cs_v6_route_init(void)
{
	hw_jt.cs_ipv6_hw_route_input = &k_jt_cs_ipv6_hw_route_input;
	hw_jt.cs_ipv6_hw_route_output = &k_jt_cs_ipv6_hw_route_output;
	return 0;
}





