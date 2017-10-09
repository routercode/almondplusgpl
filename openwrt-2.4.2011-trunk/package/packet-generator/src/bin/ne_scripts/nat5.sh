#!/bin/sh
ifconfig eth0 192.168.1.1
ifconfig eth1_0 192.168.2.1
#ifconfig eth2 192.168.3.1
ifconfig eth1_1 down

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

iptables -A FORWARD -i eth0 -o eth1_0 -j ACCEPT
iptables -A FORWARD -i eth1_0 -o eth0 -j ACCEPT

iptables -A POSTROUTING -t nat -o eth1_0 -s 192.168.1.0/24 -j MASQUERADE
ifconfig bripv6 down
brctl delif bripv6 eth0
echo "OK  ^_^" 
