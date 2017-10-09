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
iptables -F -t filter
iptables -X -t filter
iptables -Z -t filter
iptables -F -t raw
iptables -X -t raw
iptables -Z -t raw
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t nat -P OUTPUT ACCEPT

iptables -A FORWARD -i br-lan -o pppoe-wan -j ACCEPT
iptables -A FORWARD -i pppoe-wan -o br-lan -j ACCEPT

iptables -t nat -A POSTROUTING -o pppoe-wan -s 192.168.1.0/24 -j MASQUERADE

iptables -A FORWARD -p tcp -o pppoe-wan --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -A OUTPUT -p tcp -o pppoe-wan --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -t mangle -A POSTROUTING -p tcp -o pppoe-wan --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 

