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


cp /sbin/cs75xx/pppoe/ipv6pppoeClient.network /etc/config/network
cp /sbin/cs75xx/pppoe/ipv6pppoeClient.radvd /etc/config/radvd

/etc/init.d/network restart
sleep 10
/etc/init.d/radvd restart

sleep 4
tc qdisc del dev eth0 root hfsc
tc qdisc del dev imq0 root handle 1:hfsc
tc qdisc show
ifconfig imq0 down

echo "cortina * cortina123 *" > /etc/ppp/chap-secrets 
sed 's/ETH=eth1/ETH=eth0/' /etc/ppp/pppoe.conf > /root/pppoe.conf.tmp1
sed 's/USER=bxxxnxnx@sympatico.ca/USER=cortina/' /root/pppoe.conf.tmp1 > /root/pppoe.conf.tmp2
sed 's/PPPD_EXTRA=""/PPPD_EXTRA="ipv6 ,"/' /root/pppoe.conf.tmp2 > /root/pppoe.conf.tmp3 
sed 's/CONNECT_TIMEOUT=30/CONNECT_TIMEOUT=0/' /root/pppoe.conf.tmp3 > /etc/ppp/pppoe.conf
rm -f /root/pppoe.conf.tmp1 /root/pppoe.conf.tmp2 /root/pppoe.conf.tmp3
kill -9 `pidof pppoe-server`
