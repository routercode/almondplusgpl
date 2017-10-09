#!/bin/sh
ifconfig eth0 0.0.0.0
ifconfig eth1 up
sleep 3
ifconfig eth2 up
sleep 3
cp /sbin/cs75xx/cd_router/ipv6/network /etc/config/
cp /sbin/cs75xx/cd_router/ipv6/radvd /etc/config/
sleep 1
/etc/init.d/network restart
echo "Wait 30 second ^_^"
sleep 30
/etc/init.d/radvd restart
echo "IPv6 cdrouter basic setup OK ^_^"
