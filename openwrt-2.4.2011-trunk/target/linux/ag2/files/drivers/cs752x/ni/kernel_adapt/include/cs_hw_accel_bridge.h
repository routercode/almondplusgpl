#ifndef __CS_HW_ACCEL_BRIDGE_H__
#define __CS_HW_ACCEL_BRIDGE_H__

#include <linux/skbuff.h>

void k_jt_cs_bridge_hook_before_fwd(struct sk_buff *skb);
void k_jt_cs_bridge_hook_after_fwd(struct sk_buff *skb, cs_uint32 output_dev, char *p_dev_name);
void k_jt_cs_bridge_del_if_hook(cs_uint8 *p_dev_name);
extern void cs_bridge_init(void);
#endif
