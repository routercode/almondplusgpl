#!/bin/sh
#

iptables -F
iptables -X
iptables -Z
iptables -F -t nat
iptables -X -t nat
iptables -Z -t nat
iptables -F -t mangle
iptables -X -t mangle
iptables -Z -t mangle
iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t mangle -P FORWARD ACCEPT
iptables -t mangle -P PREROUTING ACCEPT
iptables -t mangle -P POSTROUTING ACCEPT

ip6tables -F
ip6tables -Z
ip6tables -X
echo 1 > /proc/sys/net/ipv4/ip_forward


cp /sbin/cs75xx/pppoe/ipv6.network /etc/config/network
cp /sbin/cs75xx/pppoe/ipv6.radvd /etc/config/radvd

/etc/init.d/network restart
sleep 10
/etc/init.d/radvd restart

sleep 4
tc qdisc del dev eth0 root hfsc
tc qdisc del dev imq0 root handle 1:hfsc
tc qdisc show
ifconfig imq0 down
