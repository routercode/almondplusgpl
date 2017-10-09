/*
 * Cortina Systems 
 * 
 * This is auto generated. Please do not make modification to this file
 * For any changes, please update it to ,  ./bin/genCortinaJt.sh
 */

#ifndef CS_KERNEL_HW_ACCEL_JT
#define CS_KERNEL_HW_ACCEL_JT 1

#include <linux/skbuff.h>
#include <linux/mroute.h>
#include <linux/mroute6.h>
#ifdef CONFIG_NETFILTER
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <net/netfilter/nf_conntrack.h>
#endif

void cs_hw_accel_nop(void);

struct cs_kernel_hw_accel_jt {
	cs_status (*cs_hw_accel_ipsec_handler) (struct sk_buff *skb, struct xfrm_state *x, cs_uint8 ip_ver, cs_uint8 dir);
	void (*cs_hw_accel_ipsec_x_state_add) (struct xfrm_state *x);
	void (*cs_hw_accel_ipsec_x_state_delete) (struct xfrm_state *x);
	void (*cs_hw_accel_ipsec_x_state_update) (struct xfrm_state *x);
	void (*cs_qos_set_skb_sw_only) (struct sk_buff *skb);
	void (*cs_qos_set_skb_pol_id) (struct sk_buff *skb, u8 pol_id);
	void (*cs_qos_enbl_filter_policer) (u8 *p_pol_id, u32 cir, u32 cbs, u32 pir, u32 pbs);
	void (*cs_qos_dsbl_filter_policer) (u8 pol_id);
	int (*cs_qos_check_and_steal_multiq_skb) (struct Qdisc *qdisc, struct sk_buff *skb);
	void (*cs_qos_set_multiq_attribute) (struct Qdisc *qdisc, cs_uint16 burst_size, cs_uint32 rate_bps);
	int (*cs_qos_set_multisubq_attribute) (struct Qdisc *qdisc, cs_uint8 band_id, cs_uint8 priority, cs_uint32 weight, cs_uint32 rsrv_size, cs_uint32 max_size, cs_uint32 rate_bps, void *p_wred);
	void (*cs_qos_reset_multiq) (struct Qdisc *qdisc);
	void (*cs_qos_reset_multisubq) (struct Qdisc *qdisc, cs_uint8 band_id);
	void (*cs_qos_set_pol_cfg_ingress_qdisc) (struct Qdisc *qdisc, cs_uint8 enbl, cs_uint8 bypass_yellow, cs_uint8 bypass_red, cs_uint32 rate_bps, cs_uint32 cbs, cs_uint32 pbs);
	void (*cs_bridge_hook_before_fwd) (struct sk_buff *skb);
	void (*cs_bridge_hook_after_fwd) (struct sk_buff *skb, cs_uint32 output_dev, char *p_dev_name);
	void (*cs_bridge_del_if_hook) (cs_uint8 *p_dev_name);
	void (*cs_vlan_skb_recv_hook) (struct sk_buff *skb, struct net_device *dev, cs_uint16 vlan_tci); 
	void (*cs_vlan_dev_hard_start_xmit_hook) (struct sk_buff *skb, cs_uint16 vlan_tci); 
	void (*cs_vlan_get_port_by_dev_hook) (struct net_device *dev, cs_uint8 *p_port);
	//void (*cs_vlan_get_port_by_dev_hook) (cs_uint8 *p_dev_name, cs_uint8 *p_port);
	void (*cs_vlan_add_vlan_table_hook) (cs_uint8 port, cs_uint16 vlan_id);
	void (*cs_vlan_del_vlan_table_hook) (cs_uint8 port, cs_uint16 vlan_id);
	void (*cs_nat_ipv4_add_hook_before_nat) (struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo);
	void (*cs_nat_ipv4_add_hook_after_nat) (struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo);
	void (*cs_nat_ipv4_delete_hook) (struct sk_buff *skb, struct nf_conn *ct, enum ip_conntrack_info ctinfo, unsigned int hooknum);
	void (*cs_kernel_adapt_nat_del_all) (void);
	void (*cs_nat_ipv4_del_hash) (cs_uint64 guid);
	void (*cs_mc_ipv4_forwarding_hook) (struct sk_buff *skb, struct mfc_cache *c, int vifi); 
	void (*cs_mc_ipv4_delete_hook) ( struct mfc_cache *c); 
	void (*cs_mc_ipv4_add_hook) ( struct mfc_cache *c); 
	void (*cs_mc_ipv4_init ) ( struct mfc_cache *c);
	void (*cs_mc_ipv6_forwarding_hook) (struct sk_buff *skb, struct mfc6_cache *c, int vifi); 
	void (*cs_mc_ipv6_delete_hook) ( struct mfc6_cache *c); 
	void (*cs_mc_ipv6_add_hook) ( struct mfc6_cache *c, int vifi); 
	void (*cs_mc_ipv6_init ) ( struct mfc6_cache *c);
};
#endif
