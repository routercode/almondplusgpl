#!/bin/sh
DHCP_S=0 ## 1 or 0
chmod +x /etc/hotplug.d/iface/20-ntpclient
#killall dynamic_dns_updater.sh
sleep 1
/etc/init.d/dnsmasq stop
echo "1.1.1.1" > /etc/resolv.con
echo "127.0.0.1" > /etc/hosts
echo "3.3.3.6 time.nist.gov" >>  /etc/hosts
#echo "3.3.3.7 time.foo.gov" >>  /etc/hosts
/etc/hotplug.d/iface/20-ntpclient
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
uci commit network         
echo " wan port is dhcp " 

#################################

if [ "$DHCP_S" == "1" ]; then
uci set dhcp.dnsmasq=dnsmasq
uci set dhcp.dnsmasq.domainneeded=1
uci set dhcp.dnsmasq.boguspriv=1
uci set dhcp.dnsmasq.filterwin2k=0
uci set dhcp.dnsmasq.localise_queries=1
uci set dhcp.dnsmasq.local=/lan/
uci set dhcp.dnsmasq.domain=qa.contina.com
uci set dhcp.dnsmasq.expandhosts=1
uci set dhcp.dnsmasq.nonegcache=0
uci set dhcp.dnsmasq.authoritative=1
uci set dhcp.dnsmasq.readethers=1
uci set dhcp.dnsmasq.leasefile=/tmp/dhcp.leases
uci set dhcp.dnsmasq.resolvfile=/tmp/resolv.conf.auto

uci set dhcp.dhcp=dhcp
uci set dhcp.dhcp.interface=lan
uci set dhcp.dhcp.start=192.168.1.10
uci set dhcp.dhcp.limit=192.168.1.14
uci set dhcp.dhcp.leasetime=12
uci commit dhcp
echo "dhcp server enable"
/etc/init.d/dnsmasq  restart
fi
#################################
uci commit network                           
sleep 1                                   
/etc/init.d/network restart                             
sleep 1 

#uci set ddns.myddns.enable=1
#uci set ddns.myddns=service
uci set ddns.myddns.service_name=dyndns.org
uci set ddns.myddns.ip_source=network
uci set ddns.myddns.ip_network=wan
uci set ddns.myddns.force_interval=72
uci set ddns.myddns.force_unit=hours
uci set ddns.myddns.check_interval=3
uci set ddns.myddns.check_unit=minutes
uci set ddns.myddns.domain=cortina.dyndns.org
uci set ddns.myddns.username=cortina
uci set ddns.myddns.password=cortina
uci set ddns.myddns.enabled=1
uci commit ddns
sleep 2
echo "run ddns"
ACTION=ifup INTERFACE=wan /sbin/hotplug-call inface
#/usr/lib/ddns/dynamic_dns_functions.sh

./nat-accept-all.sh

