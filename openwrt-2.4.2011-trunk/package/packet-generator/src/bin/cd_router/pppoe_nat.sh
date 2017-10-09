#!/bin/sh
ifconfig eth0 192.168.0.1
ifconfig eth1 192.168.2.1
ifconfig eth2 192.168.3.1

#ifconfig eth0 hw ether 00:50:c2:00:00:30
#ifconfig eth1 hw ether 00:50:c2:00:00:38
#ifconfig eth2 hw ether 00:50:c2:00:00:40

iptables -F
iptables -X
iptables -Z

iptables -F -t mangle
iptables -X -t mangle
iptables -Z -t mangle

iptables -F -t nat
iptables -X -t nat
iptables -Z -t nat

iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT

iptables -A POSTROUTING -t nat -o ppp0 -s 192.168.3.0/24 -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward
echo "OK  ^_^" 
