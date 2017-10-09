#!/bin/sh
WAN_PORT="dhcp" ### dhcp / static / pppoe
DHCP_S=0 ## 1 or 0 ; 1--> on ; 0--> off 
echo "nameserver 1.1.1.1" > /etc/resolv.conf
echo "nameserver 1.1.1.2" >> /etc/resolv.conf
killall dynamic_dns_updater.sh
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
uci commit ddnsa
sleep 1
/etc/init.d/dnsmasq  stop
sleep 1
/etc/init.d/dnsmasq  start
sleep 2
echo "run ddns"
#ACTION=ifup INTERFACE=wan /sbin/hotplug-call inface
INTERFACE=wan ACTION=ifup sh /etc/hotplug.d/iface/25-ddns
chmod +x /usr/lib/ddns/dynamic_dns_functions.sh
/usr/lib/ddns/dynamic_dns_functions.sh
./nat-accept-all.sh

                                              









