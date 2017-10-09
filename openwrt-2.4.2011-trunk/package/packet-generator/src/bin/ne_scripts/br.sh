#!/bin/sh
ifconfig eth1 up
ifconfig eth2 up


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

#iptables -A FORWARD -i eth1 -o eth2 -j ACCEPT
#iptables -A FORWARD -i eth2 -o eth1 -j ACCEPT

#iptables -A POSTROUTING -t nat -o eth2 -s 192.168.2.0/24 -j MASQUERADE
brctl addbr br0
brctl addif br0 eth1
brctl addif br0 eth2
ifconfig br0 192.168.2.1
echo "Bridge OK  ^_^  @_@ " 
