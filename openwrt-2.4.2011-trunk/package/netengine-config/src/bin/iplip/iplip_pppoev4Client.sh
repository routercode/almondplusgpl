#!/bin/sh
#
# Modify /etc/ppp/chap-secrets and /etc/ppp/pppoe.conf for PPPoE Client
echo "cortina * cortina *" > /etc/ppp/chap-secrets
sed 's/ETH=eth1/ETH=eth0/' /etc/ppp/pppoe.conf > /root/pppoe.conf.tmp1
sed 's/USER=bxxxnxnx@sympatico.ca/USER=cortina/' /root/pppoe.conf.tmp1 > /root/pppoe.conf.tmp2
sed 's/CONNECT_TIMEOUT=30/CONNECT_TIMEOUT=0/' /root/pppoe.conf.tmp2 > /root/pppoe.conf.tmp3
sed 's/CLAMPMSS=1412/#CLAMPMSS=1412/' /root/pppoe.conf.tmp3 > /root/pppoe.conf.tmp4
sed 's/#CLAMPMSS=no/CLAMPMSS=no/' /root/pppoe.conf.tmp4 > /etc/ppp/pppoe.conf
rm -f /root/pppoe.conf.tmp1 /root/pppoe.conf.tmp2 /root/pppoe.conf.tmp3 /root/pppoe.conf.tmp4
kill -9 `pidof pppoe-server`

ifconfig eth0 220.168.1.1
ifconfig eth1 192.168.60.1
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

iptables -t nat -A POSTROUTING -o ppp0 -s 192.168.60.0/24 -j MASQUERADE

iptables -A FORWARD -p tcp -o ppp0  --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -A OUTPUT -p tcp -o ppp0 --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -t mangle -A POSTROUTING -p tcp -o ppp0 --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
