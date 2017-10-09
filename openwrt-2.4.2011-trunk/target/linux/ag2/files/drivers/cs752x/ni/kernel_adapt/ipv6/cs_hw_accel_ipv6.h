#ifndef CS_KERNEL_V6_RT_HW_ACC_SUPPORT
#define CS_KERNEL_V6_RT_HW_ACC_SUPPORT 1

#include <linux/skbuff.h>

void k_jt_cs_ipv6_hw_route_input(struct sk_buff *skb, struct rt6_info *rt);
void k_jt_cs_ipv6_hw_route_output(struct sk_buff *skb);
int cs_v6_route_init();

#endif