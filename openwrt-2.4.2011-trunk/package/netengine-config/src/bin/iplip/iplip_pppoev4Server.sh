#!/bin/sh
#
# Modify /etc/ppp/chap-secrets and /etc/ppp/pppoe-server-options for PPPoE Server
echo "cortina * cortina *" > /etc/ppp/chap-secrets
echo "220.133.199.2-2" > /etc/ppp/allip
sed 's/require-pap/require-chap/' /etc/ppp/pppoe-server-options > /root/pppoe-server-options
cp /root/pppoe-server-options /etc/ppp/pppoe-server-options
rm -f /root/pppoe-server-options

ifconfig eth0 220.133.199.1
ifconfig eth1 10.1.1.1
echo "1" >/proc/sys/net/ipv4/ip_forward

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

iptables -A POSTROUTING -t nat -o eth0 -s 10.1.1.0/24 -j MASQUERADE

kill -9 `pidof pppoe-server`
pppoe-server -C ISP-Cortina -L 220.133.199.1 -p /etc/ppp/allip -I eth0

# SW Path
echo 0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
