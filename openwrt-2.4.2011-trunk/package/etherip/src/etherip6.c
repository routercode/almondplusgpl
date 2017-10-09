/*
 * etherip6.c: Ethernet over IPv6 tunnel driver (according to RFC3378)
 *
 * This driver could be used to tunnel Ethernet packets through IPv4
 * networks. This is especially usefull together with the bridging
 * code in Linux.
 *
 * This code was written with an eye on the IPIP driver in linux from
 * Sam Lantinga. Thanks for the great work.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 (no later version) as published by the
 *      Free Software Foundation.
 *
 */

#include <linux/version.h>
#include <linux/capability.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/in6.h>
#include <linux/if_tunnel.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/netfilter_ipv4.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/protocol.h>
#include <net/route.h>
#include <net/ipip.h>
#include <net/xfrm.h>
#include <net/ip6_fib.h>
#include <net/ip6_route.h>
#include <net/inet_ecn.h>
#include <linux/ip6_tunnel.h>

#ifdef CONFIG_CS752X_HW_ACCEL_ETHERIP
#include <mach/hw_accel.h>      /* for Cortina Acceleration */

extern void k_jt_cs_etherip_handler(struct sk_buff *skb,
				struct ip6_tnl_parm *p,
				cs_uint8 ip_ver,
				cs_uint8 dir);
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cortina Systems");
MODULE_DESCRIPTION("Ethernet over IPv6 tunnel driver");

#ifndef IPPROTO_ETHERIP
#define IPPROTO_ETHERIP 97
#endif

/*
 * These 2 defines are taken from ipip.c - if it's good enough for them
 * it's good enough for me.
 */
#define HASH_SIZE        16
#define HASH(addr)       ((addr.s6_addr32[3]^(addr.s6_addr32[3]>>4))&0xF)

#define ETHERIP_HEADER   ((u16)0x0300)
#define ETHERIP_HLEN     2
#define ETHERIP_MAX_MTU  (65535 - 20 - ETHERIP_HLEN)

#define BANNER1 "etherip: Ethernet over IPv6 tunneling driver\n"

struct etherip6_tunnel {
	struct list_head list;
	struct net_device *dev;
	struct net_device_stats stats;
	struct ip6_tnl_parm parms;
	unsigned int recursion;
};

struct net_device *etherip6_tunnel_dev;
struct list_head tunnels[HASH_SIZE];

DEFINE_RWLOCK(etherip6_lock);

void etherip6_tunnel_setup(struct net_device *dev);

/* add a tunnel to the hash */
void etherip6_tunnel_add(struct etherip6_tunnel *tun)
{
	unsigned h = HASH(tun->parms.raddr);
	list_add_tail(&tun->list, &tunnels[h]);
}

/* delete a tunnel from the hash*/
void etherip6_tunnel_del(struct etherip6_tunnel *tun)
{
	list_del(&tun->list);
}

/* find a tunnel in the hash by parameters from userspace */
struct etherip6_tunnel* etherip6_tunnel_find(struct ip6_tnl_parm *p)
{
	struct etherip6_tunnel *ret;
	unsigned h = HASH(p->raddr);

	list_for_each_entry(ret, &tunnels[h], list)
		if (ipv6_addr_equal(&ret->parms.raddr, &p->raddr))
			return ret;

	return NULL;
}

/* find a tunnel by its destination address */
struct etherip6_tunnel* etherip6_tunnel_locate(struct in6_addr remote)
{
	struct etherip6_tunnel *ret;
	unsigned h = HASH(remote);

	list_for_each_entry(ret, &tunnels[h], list)
		if (ipv6_addr_equal(&ret->parms.raddr, &remote))
			return ret;

	return NULL;
}

int etherip6_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < 68 || new_mtu > ETHERIP_MAX_MTU)
		return -EINVAL;
	dev->mtu = new_mtu;

	return 0;
}

static struct net_device_stats *etherip6_get_stats(struct net_device *dev)
{
	struct etherip6_tunnel *tunnel = netdev_priv(dev);
	return &tunnel->stats;
}

/* netdevice hard_start_xmit function
 * it gets an Ethernet packet in skb and encapsulates it in another IP
 * packet */
int etherip6_tunnel_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct etherip6_tunnel *tunnel = netdev_priv(dev);
	struct ipv6hdr *iph;
	struct flowi fl;
	struct net_device *tdev;
	int max_headroom;
	int err = 0;
	struct net_device_stats *stats = &tunnel->stats;
	struct dst_entry *dst = NULL;

	if (tunnel->recursion++) {
		tunnel->stats.collisions++;
		tunnel->stats.tx_errors++;
		goto tx_error;
	}

	memset(&fl, 0, sizeof(fl));
	fl.oif               = tunnel->parms.link;
	fl.proto             = IPPROTO_ETHERIP;
	fl.fl6_dst  = tunnel->parms.raddr;
	fl.fl6_src  = tunnel->parms.laddr;

	/*
	 * In IPv4 implementation, ip_route_output_key will check xfrm_lookup.
	 * However, in IPv6 implementation, we need to call it by ourself.
	 */
	if ((dst = ip6_route_output(dev_net(dev), NULL, &fl)) == NULL) {
		goto tx_error_icmp;
	} else {
		if (dst->error ||
		    (err = xfrm_lookup(dev_net(dev), &dst, &fl, NULL, 0)) < 0) {
			goto tx_error_icmp;
		}
	}

	tdev = dst->dev;
	if (tdev == dev) {
		if (dst) dst_release(dst);

		tunnel->stats.collisions++;
		tunnel->stats.tx_errors++;
		goto tx_error;
	}


#ifdef CONFIG_CS752X_HW_ACCEL_ETHERIP
	/* Cortina Acceleration */

	k_jt_cs_etherip_handler(skb,&tunnel->parms,1,1);
#endif

	max_headroom = (LL_RESERVED_SPACE(tdev)+sizeof(struct ipv6hdr)
			+ ETHERIP_HLEN);

	if (skb_headroom(skb) < max_headroom || skb_shared(skb) ||
	    (skb_cloned(skb) && !skb_clone_writable(skb, 0))) {
		struct sk_buff *skn = skb_realloc_headroom(skb, max_headroom);
		if (!skn) {
			if (dst) dst_release(dst);
			dev_kfree_skb(skb);
			tunnel->stats.tx_dropped++;
			return 0;
		}
		if (skb->sk)
			skb_set_owner_w(skn, skb->sk);
		dev_kfree_skb(skb);
		skb = skn;
	}

	skb->transport_header = skb->mac_header;
	skb_push(skb, sizeof(struct ipv6hdr)+ETHERIP_HLEN);
	skb_reset_network_header(skb);
	memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
	IPCB(skb)->flags &= ~(IPSKB_XFRM_TUNNEL_SIZE | IPSKB_XFRM_TRANSFORMED |
			IPSKB_REROUTED);

	skb_dst_drop(skb);
	skb_dst_set(skb, dst_clone(dst));

	/* Build the IP header for the outgoing packet
	 *
	 * Note: This driver never sets the DF flag on outgoing packets
	 *       to ensure that the tunnel provides the full Ethernet MTU.
	 *       This behavior guarantees that protocols can be
	 *       encapsulated within the Ethernet packet which do not
	 *       know the concept of a path MTU
	 */
	iph = ipv6_hdr(skb);
	*(__be32*)iph = fl.fl6_flowlabel | htonl(0x60000000);
	iph->nexthdr = IPPROTO_ETHERIP;
	iph->daddr = fl.fl6_dst;
	iph->saddr = fl.fl6_src;
	iph->hop_limit = tunnel->parms.hop_limit;
	if (iph->hop_limit == 0)
		iph->hop_limit = dst_metric(dst, RTAX_HOPLIMIT);

	/* add the 16bit etherip header after the ip header */
	((u16*)(iph+1))[0] = htons(ETHERIP_HEADER);
	nf_reset(skb);

	err = ip6_local_out(skb);
	if (net_xmit_eval(err) == 0) {
		stats->tx_bytes += skb->len;
		stats->tx_packets++;
	} else {
		stats->tx_errors++;
		stats->tx_aborted_errors++;
	}

	tunnel->dev->trans_start = jiffies;
	tunnel->recursion--;

	return 0;

tx_error_icmp:
	dst_link_failure(skb);
	if (dst) dst_release(dst);
tx_error:
	dev_kfree_skb(skb);
	tunnel->recursion--;
	return 0;
}

/* checks parameters the driver gets from userspace */
int etherip6_param_check(struct ip6_tnl_parm *p)
{
	if (p->proto != IPPROTO_ETHERIP ||
	    (ipv6_addr_type(&p->raddr) & IPV6_ADDR_ANY) ||
	    (ipv6_addr_type(&p->raddr) & IPV6_ADDR_MULTICAST))
		return -EINVAL;

	return 0;
}

/* central ioctl function for all netdevices this driver manages
 * it allows to create, delete, modify a tunnel and fetch tunnel
 * information */
int etherip6_tunnel_ioctl(struct net_device *dev, struct ifreq *ifr,
		int cmd)
{
	int err = 0;
	struct ip6_tnl_parm p;
	struct net_device *tmp_dev;
	char *dev_name;
	struct etherip6_tunnel *t;


	switch (cmd) {
	case SIOCGETTUNNEL:
		t = netdev_priv(dev);
		if (copy_to_user(ifr->ifr_ifru.ifru_data, &t->parms,
				sizeof(t->parms)))
			err = -EFAULT;
		break;
	case SIOCADDTUNNEL:
		err = -EINVAL;
		if (dev != etherip6_tunnel_dev)
			goto out;

	case SIOCCHGTUNNEL:
		err = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			goto out;

		err = -EFAULT;
		if (copy_from_user(&p, ifr->ifr_ifru.ifru_data,
					sizeof(p)))
			goto out;

		if ((err = etherip6_param_check(&p)) < 0)
			goto out;

		t = etherip6_tunnel_find(&p);

		err = -EEXIST;
		if (t != NULL && t->dev != dev)
			goto out;

		if (cmd == SIOCADDTUNNEL) {

			p.name[IFNAMSIZ-1] = 0;
			dev_name = p.name;
			if (dev_name[0] == 0)
				dev_name = "ethip%d";

			err = -ENOMEM;
			tmp_dev = alloc_netdev(
					sizeof(struct etherip6_tunnel),
					dev_name,
					etherip6_tunnel_setup);

			if (tmp_dev == NULL)
				goto out;

			if (strchr(tmp_dev->name, '%')) {
				err = dev_alloc_name(tmp_dev, tmp_dev->name);
				if (err < 0)
					goto add_err;
			}

			t = netdev_priv(tmp_dev);
			t->dev = tmp_dev;
			strncpy(p.name, tmp_dev->name, IFNAMSIZ);
			memcpy(&(t->parms), &p, sizeof(p));

			err = -EFAULT;
			if (copy_to_user(ifr->ifr_ifru.ifru_data, &p,
						sizeof(p)))
				goto add_err;

			err = register_netdevice(tmp_dev);
			if (err < 0)
				goto add_err;

			write_lock_bh(&etherip6_lock);
			etherip6_tunnel_add(t);
			write_unlock_bh(&etherip6_lock);

		} else {
			err = -EINVAL;
			if ((t = netdev_priv(dev)) == NULL)
				goto out;
			if (dev == etherip6_tunnel_dev)
				goto out;
			write_lock_bh(&etherip6_lock);
			memcpy(&(t->parms), &p, sizeof(p));
			write_unlock_bh(&etherip6_lock);
		}

		err = 0;
		break;
add_err:
		free_netdev(tmp_dev);
		goto out;

	case SIOCDELTUNNEL:
		err = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			goto out;

		err = -EFAULT;
		if (copy_from_user(&p, ifr->ifr_ifru.ifru_data,
					sizeof(p)))
			goto out;

		err = -EINVAL;
		if (dev == etherip6_tunnel_dev) {
			t = etherip6_tunnel_find(&p);
			if (t == NULL) {
				goto out;
			}
		} else
			t = netdev_priv(dev);

		write_lock_bh(&etherip6_lock);
		etherip6_tunnel_del(t);
		write_unlock_bh(&etherip6_lock);

		unregister_netdevice(t->dev);
		err = 0;

		break;
	default:
		err = -EINVAL;
	}

out:
	return err;
}

const struct net_device_ops etherip6_netdev_ops = {
	.ndo_start_xmit = etherip6_tunnel_xmit,
	.ndo_do_ioctl   = etherip6_tunnel_ioctl,
	.ndo_change_mtu = etherip6_change_mtu,
	.ndo_get_stats = etherip6_get_stats,
};

/* device init function - called via register_netdevice
 * The tunnel is registered as an Ethernet device. This allows
 * the tunnel to be added to a bridge */
void etherip6_tunnel_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->netdev_ops      = &etherip6_netdev_ops;
	dev->destructor      = free_netdev;
	dev->mtu             = ETH_DATA_LEN;
	dev->hard_header_len = LL_MAX_HEADER + sizeof(struct ipv6hdr) + 
			       ETHERIP_HLEN;
	random_ether_addr(dev->dev_addr);
}

/* receive function for EtherIP packets
 * Does some basic checks on the MAC addresses and
 * interface modes */
int etherip6_rcv(struct sk_buff *skb)
{
	struct ipv6hdr *iph;
	struct etherip6_tunnel *tunnel;
	struct net_device *dev;

	iph = ipv6_hdr(skb);

	read_lock_bh(&etherip6_lock);
	tunnel = etherip6_tunnel_locate(iph->saddr);
	if (tunnel == NULL)
		goto drop;

	dev = tunnel->dev;
	secpath_reset(skb);
	skb_pull(skb, (skb_network_header(skb)-skb->data) +
			sizeof(struct ipv6hdr)+ETHERIP_HLEN);


#ifdef CONFIG_CS752X_HW_ACCEL_ETHERIP
	/* Cortina Acceleration */
	k_jt_cs_etherip_handler(skb,&tunnel->parms,1,0);
#endif

	skb->dev = dev;
	skb->pkt_type = PACKET_HOST;
	skb->protocol = eth_type_trans(skb, tunnel->dev);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb_dst_drop(skb);

	/* do some checks */
	if (skb->pkt_type == PACKET_HOST || skb->pkt_type == PACKET_BROADCAST)
		goto accept;

	if (skb->pkt_type == PACKET_MULTICAST &&
			(dev->mc.count > 0 || dev->flags & IFF_ALLMULTI))
		goto accept;

	if (skb->pkt_type == PACKET_OTHERHOST && dev->flags & IFF_PROMISC)
		goto accept;

drop:
	tunnel->stats.rx_dropped++;
	read_unlock_bh(&etherip6_lock);
	kfree_skb(skb);
	return 0;

accept:
	tunnel->dev->last_rx = jiffies;
	tunnel->stats.rx_packets++;
	tunnel->stats.rx_bytes += skb->len;
	nf_reset(skb);
	netif_rx(skb);
	read_unlock_bh(&etherip6_lock);
	return 0;

}

 struct inet6_protocol etherip6_protocol = {
	.handler      = etherip6_rcv,
	.err_handler  = 0,
};

/* module init function
 * initializes the EtherIP protocol (97) and registers the initial
 * device */
int __init etherip6_init(void)
{
	int err, i;
	struct etherip6_tunnel *p;

	printk(KERN_INFO BANNER1);

	for (i = 0; i < HASH_SIZE; ++i)
		INIT_LIST_HEAD(&tunnels[i]);

	if (inet6_add_protocol(&etherip6_protocol, IPPROTO_ETHERIP)) {
		printk(KERN_ERR "etherip6: can't add protocol\n");
		return -EBUSY;
	}

	etherip6_tunnel_dev = alloc_netdev(sizeof(struct etherip6_tunnel),
			"ip6ethip0",
			etherip6_tunnel_setup);

	if (!etherip6_tunnel_dev) {
		err = -ENOMEM;
		goto err2;
	}

	p = netdev_priv(etherip6_tunnel_dev);
	p->dev = etherip6_tunnel_dev;
	/* set some params for iproute2 */
	strcpy(p->parms.name, "ip6ethip0");
	p->parms.proto = IPPROTO_ETHERIP;

	if ((err = register_netdev(etherip6_tunnel_dev)))
		goto err1;

out:
	return err;
err1:
	free_netdev(etherip6_tunnel_dev);
err2:
	inet6_del_protocol(&etherip6_protocol, IPPROTO_ETHERIP);
	goto out;
}

/* destroy all tunnels */
void __exit etherip6_destroy_tunnels(void)
{
	int i;
	struct list_head *ptr;
	struct etherip6_tunnel *tun;

	for (i = 0; i < HASH_SIZE; ++i) {
		list_for_each(ptr, &tunnels[i]) {
			tun = list_entry(ptr, struct etherip6_tunnel, list);
			ptr = ptr->prev;
			etherip6_tunnel_del(tun);
			dev_put(tun->dev);
			unregister_netdevice(tun->dev);
		}
	}
}

/* module cleanup function */
void __exit etherip6_exit(void)
{
	rtnl_lock();
	etherip6_destroy_tunnels();
	unregister_netdevice(etherip6_tunnel_dev);
	rtnl_unlock();
	if (inet6_del_protocol(&etherip6_protocol, IPPROTO_ETHERIP))
		printk(KERN_ERR "etherip6: can't remove protocol\n");
}

module_init(etherip6_init);
module_exit(etherip6_exit);
