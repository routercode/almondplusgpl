#!/bin/sh

if [ -f /usr/lib/lua/luci/model/cbi/rt3593.lua ]; then
echo " RT3593 WebGUI ready, please configure WiFi via GUI "

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/driver/cs752x/ne/ni/ni_fastbridge

# Apply OpenWrt Firewall rules
/etc/init.d/firewall restart
sleep 2
else

ifconfig eth0 0.0.0.0 up
ifconfig eth1 0.0.0.0 up
brctl addbr br-lan
brctl addif br-lan eth1
ifconfig br-lan 192.168.1.1 up

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/driver/cs752x/ne/ni/ni_fastbridge

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

iptables -A FORWARD -i br-lan -o eth0 -j ACCEPT
iptables -A FORWARD -i eth0 -o br-lan -j ACCEPT

iptables -t nat -A POSTROUTING -o eth0 -s 192.168.1.0/24 -j MASQUERADE
fi
