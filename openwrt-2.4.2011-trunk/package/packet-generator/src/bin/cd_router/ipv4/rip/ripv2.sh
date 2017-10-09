#!/bin/sh
cp ripv2.tgz /
tar -zxvf /ripv2.tgz -C /
#################################
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
uci set network.wan.defaultroute=0
uci set network.wan.peerdns=0
uci set network.wan.dns=1.1.1.1
uci commit network 
/etc/init.d/network restart
#################################
sleep 2
./firewall.sh
echo "---------------------------------------------"
echo "iptables accept all packet for this test case"
echo "---------------------------------------------" 
################################
echo "clean route table cache"
ip route flush cache
###############################
cp /etc/quagga/ripd_v2.conf /etc/quagga/ripd.conf
cp /etc/quagga/zebra_v2.conf /etc/quagga/zebra.conf
echo "##########################"
echo "ripv2 daemon restart......"
echo "##########################"

/etc/init.d/quagga stop
sleep 1
/etc/init.d/quagga start
