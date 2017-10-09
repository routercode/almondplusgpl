#!/bin/sh
iptables -P INPUT ACCEPT  
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t mangle -P POSTROUTING ACCEPT
iptables -t mangle -P PREROUTING ACCEPT
iptables -t mangle -P FORWARD ACCEPT
iptables -F
iptables -t nat -F
iptables -t mangle -F
iptables -X
iptables -t nat -X
iptables -t mangle -X
iptables -Z          
iptables -t nat -Z
iptables -t mangle -Z
iptables -F -t raw
iptables -I FORWARD -i eth2 -p udp -d 224.0.0.0/4 -j ACCEPT
iptables -I FORWARD -i eth1 -p udp -d 224.0.0.0/4 -j ACCEPT
iptables -I FORWARD -p igmp -j ACCEPT

iptables -P INPUT ACCEPT                                          
iptables -P FORWARD ACCEPT                                           
iptables -P OUTPUT ACCEPT
iptables -t nat -A POSTROUTING -o eth2 -j MASQUERADE
cp igmpproxy.conf /etc/igmpproxy.conf
igmpproxy /etc/igmpproxy.conf -d -vv &

