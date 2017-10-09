#!/bin/sh
WAN_PORT="dhcp" ### dhcp / static / pppoe
DHCP_S=0 ## 1 or 0 ; 1--> on ; 0--> off 
echo "nameserver 1.1.1.1" > /etc/resolv.conf
echo "nameserver 1.1.1.2" >> /etc/resolv.conf
##################################
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
uci set network.wan.dns=1.1.1.1
uci set network.wan.proto=dhcp
uci set network.wan.mtu=1500
uci set network.wan.defaultroute=0
uci set network.wan.peerdns=0
uci commit network
/etc/init.d/network restart
#route add default dev eth2
sleep 1
##################################
./nat-accept-all.sh

                                              









