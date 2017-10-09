#!/bin/sh
# setup CSME (Cortina-Systems Multicast Enhancement feature)
# This feature will convert MC/UC
# Default: CSME disable
# Command to enable CSME: 
#       echo 1 > /proc/driver/cs752x/wfo/wifi_offload_csme

kill -9 `pidof udhcpc`
ifconfig eth0 192.168.0.1

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
iptables -A POSTROUTING -t nat -o eth0 -j MASQUERADE

echo 0x802c > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
echo 0 > /proc/driver/cs752x/wfo/wifi_offload_csme

/etc/init.d/igmpproxy stop
sed 's/192.168.1.0/192.168.0.0/' /etc/igmpproxy.conf >  /root/igmpproxy.conf
mv /root/igmpproxy.conf /etc/igmpproxy.conf
/usr/sbin/igmpproxy -d -v /etc/igmpproxy.conf&
