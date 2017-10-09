#!/bin/sh
ifconfig bripv6 down
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth1 up
ifconfig eth1 192.168.1.1
ifconfig eth2 down
ifconfig eth2 up
ifconfig eth2 0.0.0.0

ne_cfg -m fe -t Classifier -s -c get 0 0 -b 1
ne_cfg -m fe -t Classifier -e
echo "##############"
echo "HW NAT disable"
echo "##############"

echo "###############"
echo "openWRT setting"
echo "###############"
sleep 3
./static_ip.sh

echo "###############"
echo "iptable setting"
echo "###############"
sleep 3

./firewall.sh


echo "###############"
echo "static setting"
echo "###############"
sleep 2
./static-route.sh

echo "##########################################"
echo "you can start CDRouer static_route testing"
echo "##########################################"
