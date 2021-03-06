#!/bin/sh
ifconfig bripv6 down
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth1 up
ifconfig eth1 192.168.1.1
ifconfig eth2 down
ifconfig eth2 up
ifconfig eth2 0.0.0.0

echo "###############"
echo "openWRT setting"
echo "###############"
sleep 2
./static_ip.sh

echo "###############"
echo "iptable setting"
echo "###############"
sleep 2
./firewall.sh

echo "###################"
echo "nat-timeout setting"
echo "###################"
sleep 2
./nat-timeout.sh

echo "###################################"
echo "you can start CDRouer NAT testing  "
echo "ARP/NAT/NAT-TIMEOUT/ICMP/FORWARDING"
echo "###################################"
