#!/bin/sh
ifconfig eth0 0.0.0.0
ifconfig eth1 up
sleep 2
ifconfig eth2 up
sleep 2
cp dhcp6s.conf /etc/
cp dhcp6c /etc/config/
cp dhcp6s /etc/config/
cp dhcp_radvd /etc/config/radvd
cp dhcp_network /etc/config/network
/etc/init.d/quagga stop
sleep 2
echo "Wait 60 second ^_^"
/etc/init.d/network restart
sleep 60
/etc/init.d/radvd restart
sleep 2
echo "IPv6 cdrouter dhcp-client setup OK ^_^"