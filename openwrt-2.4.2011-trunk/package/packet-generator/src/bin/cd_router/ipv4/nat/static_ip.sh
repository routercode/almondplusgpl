#!/bin/sh
WAN_PORT="static" ### dhcp / static / pppoe
#echo 65500 > /proc/sys/net/ipv4/netfilter/ip_conntrack_max
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
uci set network.wan.proto=static
uci set network.wan.ipaddr=220.168.1.1
uci set network.wan.netmask=255.255.255.0
uci set network.wan.gateway=220.168.1.254
uci set network.wan.mtu=1500
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=1
uci set network.wan.dns=1.1.1.1
echo " wan port is static " 
/etc/init.d/network stop
sleep 2
/etc/init.d/network start

#uci set system.@system[0]=system
#uci set system.@system[0].hostname=g2
#uci set system.@system[0].zonename=UTC
#uci set system.@system[0].timezone=CST-8
#uci set system.@rdate[0]=rdate
#uci set system.@rdate[0].server=time.nist.gov




