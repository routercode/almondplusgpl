#!/bin/sh
ip6tables -F
ip6tables -X
ip6tables -t mangle -F
ip6tables -t mangle -X
# ipv6_firewall_100  (WAN to LAN TCP port scan)
ip6tables -I FORWARD -i eth1 -p tcp --syn -j DROP
# ipv6_firewall_101 (WAN to LAN UDP port scan)
ip6tables -I FORWARD -i eth1 -p udp --dport 0:2048 -j DROP
# ipv6_firewall_505 (Fobidden address LAN to WAN)
ip6tables -I FORWARD -i eth0 -p udp -s fec0::1  -j DROP
ip6tables -I FORWARD -i eth0 -p udp -s ::FFFF:192.168.1.2  -j DROP
ip6tables -I FORWARD -i eth0 -p udp -s ::192.168.1.2 -j DROP
ip6tables -I FORWARD -i eth0 -p udp -s 2001:db8::1  -j DROP
ip6tables -I FORWARD -i eth0 -p udp -s 2001:10::1  -j DROP

# ipv6_firewall_506 (Fobidden address WAN to LAN)
ip6tables -I FORWARD -i eth1 -p udp -s fec0::1  -j DROP
ip6tables -I FORWARD -i eth1 -p udp -s 2001:db8::1  -j DROP
ip6tables -I FORWARD -i eth1 -p udp -s 2001:10::1  -j DROP

#  ipv6_firewall_508 (Verify outbound packets are not forwarded if the source address is not a prefix 
# of the interior network)
ip6tables -I FORWARD -i eth0 -p udp -s 2002:c0c:c01::1  -j DROP
echo "OK!  ^_^"
