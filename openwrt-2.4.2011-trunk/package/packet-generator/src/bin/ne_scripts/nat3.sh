#!/bin/sh
ifconfig eth0 192.168.1.1
#ifconfig eth1 192.168.2.1
ifconfig eth2 192.168.2.1


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

iptables -A FORWARD -i eth0 -o eth2 -j ACCEPT
iptables -A FORWARD -i eth2 -o eth0 -j ACCEPT

iptables -A POSTROUTING -t nat -o eth2 -s 192.168.1.0/24 -j MASQUERADE
ifconfig bripv6 down
brctl delif bripv6 eth0
brctl delbr bripv6
echo "Enable IRQ debug message .........."
echo 2 > /proc/driver/cs752x/ne/ni/ni_debug
echo "OK  ^_^" 
