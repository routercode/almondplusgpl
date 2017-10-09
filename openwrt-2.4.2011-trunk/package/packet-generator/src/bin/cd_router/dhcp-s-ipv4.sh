#!/bin/sh
WAN_PORT="static" ### dhcp / static / pppoe
DHCP_S=1 ## 1 or 0 ; 1--> on ; 0--> off 
TEST_Module=basic
### basic / nat / virtual-server
echo 65535 > /proc/sys/net/netfilter/nf_conntrack_max
##reset ni##
#./reset_ni.sh
chmod +x /etc/hotplug.d/iface/20-ntpclient
echo "nameserver 1.1.1.1" > /etc/resolv.conf
echo "nameserver 1.1.1.2" >> /etc/resolv.conf
##################################
if [ "$WAN_PORT" == "dhcp" ]; then
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
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=0
echo " wan port is dhcp "
uci commit network
sleep 1
#/etc/init.d/network restart
#route add default dev eth2
##################################
elif [ "$WAN_PORT" == "pppoe" ]; then
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
uci set network.wan.proto=pppoe
uci set network.wan.username=cortina
uci set network.wan.password=cortina
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=1
uci set network.wan.mtu=1492
echo " wan port is pppoe " 
#################################
elif [ "$WAN_PORT" == "static" ]; then
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
else
echo " wan port is not pppoe/dhcp/static " 
fi
uci commit network 
sleep 1
/etc/init.d/network restart
sleep 1
##################################

uci set ntpclient.ntpserver=ntpserver
uci set ntpclient.ntpserver.hostname=0.time.nist.gov
uci set ntpclient.ntpserver.port=123
#uci set ntpclient.ntpserver=ntpserver
#uci set ntpclient.ntpserver.hostname=1.time.foo.com
#uci set ntpclient.ntpserver.port=123
uci set ntpclient.ntpdrift=ntpdrift
uci set ntpclient.ntpdrift.freq=0
uci set ntpclient.ntpclient=ntpclient
uci set ntpclient.ntpclient.interval=600
uci set system.system=system

uci set system.system.hostname=g2
uci set system.system.zonename=UTC
uci set system.system.timezone=CST-8
uci set system.rdate=rdate
uci set system.rdate.server=time.nist.gov
uci commit 
#echo "127.0.0.1" > /etc/hosts
#echo "3.3.3.6 time.nist.gov" >>  /etc/hosts
#echo "3.3.3.7 time.foo.gov" >>  /etc/hosts
#/etc/hotplug.d/iface/20-ntpclient

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
uci set dhcp.lan.limit=90
uci set dhcp.lan.leasetime=5m
#uci set dhcp.lan.dhcp_option=51.300
uci set dhcp.lan.dhcp_option=3,192.168.1.1
uci set dhcp.lan.dhcp_option=6,1.1.1.1
#uci set dhcp.host=host
#uci set dhcp.host.name=qanb
#uci set dhcp.host.mac=11:22:33:44:55:66
#uci set dhcp.host.ip=192.168.1.66
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



                                              









