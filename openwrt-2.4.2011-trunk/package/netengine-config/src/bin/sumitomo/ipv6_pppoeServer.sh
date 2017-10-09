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


cp /sbin/cs75xx/pppoe/ipv6pppoeServer.network /etc/config/network
cp /sbin/cs75xx/pppoe/ipv6pppoeServer.radvd /etc/config/radvd

/etc/init.d/network restart
sleep 10
/etc/init.d/radvd restart

sleep 4
tc qdisc del dev eth0 root hfsc
tc qdisc del dev imq0 root handle 1:hfsc
tc qdisc show
ifconfig imq0 down

echo "cortina * cortina123 *" > /etc/ppp/chap-secrets
echo "220.133.199.2-2" > /etc/ppp/allip
sed 's/require-pap/require-chap/' /etc/ppp/pppoe-server-options > /root/pppoe-server-options
echo "ipv6 ::1,::2" >> /root/pppoe-server-options
cp /root/pppoe-server-options /etc/ppp/pppoe-server-options
rm -f /root/pppoe-server-options

kill -9 `pidof pppoe-server`
pppoe-server -C ISP-Cortina -L 220.133.199.1 -p /etc/ppp/allip -I eth0

echo 0x000003ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
