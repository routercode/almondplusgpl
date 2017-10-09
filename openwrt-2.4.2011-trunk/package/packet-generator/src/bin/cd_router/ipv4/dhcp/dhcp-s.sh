#!/bin/sh
WAN_PORT="static" ### dhcp / static / pppoe
DHCP_S=1 ## 1 or 0 ; 1--> on ; 0--> off 
TEST_Module=basic
echo 65535 > /proc/sys/net/netfilter/nf_conntrack_max
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
uci set network.wan.peerdns=0
uci set network.wan.dns=1.1.1.1
echo " wan port is static " 
uci commit network 
sleep 1
/etc/init.d/network restart
sleep 1
##################################
cp dhcp  /etc/config/dhcp
sleep 1
uci set dhcp.dnsmasq=dnsmasq
uci set dhcp.dnsmasq.domainneeded=1
uci set dhcp.dnsmasq.boguspriv=1
uci set dhcp.dnsmasq.filterwin2k=0
uci set dhcp.dnsmasq.localise_queries=1
uci set dhcp.dnsmasq.local=/lan/
uci set dhcp.dnsmasq.domain=cortina.com
uci set dhcp.dnsmasq.expandhosts=1
uci set dhcp.dnsmasq.nonegcache=0
uci set dhcp.dnsmasq.authoritative=1
uci set dhcp.dnsmasq.readethers=1
uci set dhcp.dnsmasq.leasefile=/tmp/dhcp.leases
uci set dhcp.dnsmasq.resolvfile=/tmp/resolv.conf.auto
uci set dhcp.lan=dhcp
uci set dhcp.lan.interface=lan
uci set dhcp.lan.start=10
uci set dhcp.lan.limit=220
uci set dhcp.lan.leasetime=5m
#uci set dhcp.lan.dhcp_option=51.300
uci set dhcp.lan.dhcp_option=3,192.168.1.1
uci set dhcp.lan.dhcp_option=6,1.1.1.1
uci set dhcp.host=host
uci set dhcp.host.name=qanb
uci set dhcp.host.mac=11:22:33:44:55:66
uci set dhcp.host.ip=192.168.1.66
uci commit dhcp
sleep 2
/etc/init.d/dnsmasq  stop
sleep 2
/etc/init.d/dnsmasq  start
sleep 2
echo "dnsmasq enable"
sleep 2

#################################
./nat-accept-all.sh



                                              









