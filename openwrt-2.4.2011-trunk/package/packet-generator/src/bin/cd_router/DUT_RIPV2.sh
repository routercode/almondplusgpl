#!/bin/sh
cp rip.tgz.sh /rip.tgz
tar -zxvf /rip.tgz -C /
chmod +x /etc/hotplug.d/iface/20-ntpclient
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
uci set network.wan.defaultroute=0
uci set network.wan.peerdns=0
uci set network.wan.dns=1.1.1.1
uci commit network 
sleep 1
/etc/init.d/network restart
sleep 1                                              
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

cp /etc/quagga/ripd_v2.conf /etc/quagga/ripd.conf   
cp /etc/quagga/zebra_v2.conf /etc/quagga/zebra.conf
rm /etc/quagga/zebra.log
/etc/init.d/quagga stop
/etc/init.d/quagga start
