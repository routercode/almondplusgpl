#!/bin/sh
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth1 192.168.1.1
ifconfig eth2 down
ifconfig eth2 0.0.0.0
ip route
ip link

chmod +x /etc/hotplug.d/iface/20-ntpclient
echo "nameserver 1.1.1.1" > /etc/resolv.conf
echo "nameserver 1.1.1.2" >> /etc/resolv.conf

uci set network.loopback=interface
uci set network.loopback.ifname=lo
uci set network.loopback.proto=static
uci set network.loopback.ipaddr=127.0.0.1
uci set network.loopback.netmask=255.0.0.0
uci set network.lan=interface
uci set network.lan.ifname=eth1
uci set network.lan.proto=static
uci set network.lan.ipaddr=192.168.1.1
uci set network.lan.netmask=255.255.255.0
uci set network.lan.defaultroute=0
uci set network.lan.peerdns=0
uci set network.wan=interface
uci set network.wan.ifname=eth2
uci set network.wan.proto=static
uci set network.wan.ipaddr=220.168.1.1
uci set network.wan.netmask=255.255.255.0
uci set network.wan.gateway=220.168.1.254
uci set network.wan.mtu=1500
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=0
uci set network.wan.dns=1.1.1.1
uci commit network 
sleep 1
/etc/init.d/network restart
sleep 1
uci set ntpclient.@ntpserver[0]=ntpserver
uci set ntpclient.@ntpserver[0].hostname=0.time.nist.gov
uci set ntpclient.@ntpserver[0].port=123
uci set ntpclient.@ntpserver[1]=ntpserver
uci set ntpclient.@ntpserver[1].hostname=1.time.foo.com
uci set ntpclient.@ntpserver[1].port=123
uci set ntpclient.@ntpdrift[0]=ntpdrift
uci set ntpclient.@ntpdrift[0].freq=0
uci set ntpclient.@ntpclient[0]=ntpclient
uci set ntpclient.@ntpclient[0].interval=600
uci set system.@system[0]=system

uci set system.@system[0].hostname=g2
uci set system.@system[0].zonename=UTC
uci set system.@system[0].timezone=CST-8
uci set system.@rdate[0]=rdate
uci set system.@rdate[0].server=time.nist.gov

uci commit 
echo "127.0.0.1" > /etc/hosts
echo "3.3.3.6 time.nist.gov" >>  /etc/hosts
echo "3.3.3.7 time.foo.gov" >>  /etc/hosts
/etc/hotplug.d/iface/20-ntpclient
sleep 2
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

iptables -P INPUT ACCEPT                                          
iptables -P FORWARD ACCEPT                                           
iptables -P OUTPUT ACCEPT

iptables -t nat -A POSTROUTING -o eth2 -j MASQUERADE 
echo " ^_^ OK!"




