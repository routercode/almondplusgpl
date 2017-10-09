#!/bin/sh
#
# Scripts for test case :
# Firmware Version: G2_v0_2_2_20121114162000_101
# Function:
#    1. create bridge, add interfaces: eth1, eth2, ra0, ra1 
#    2. enable wi-fi rate control
#    3. clean up iptables rules
#    4. set DNAT
#    5. enable MoCA WAN and MoCA LAN

fw_setenv QM_INT_BUFF 0
sleep 10
echo 0x8024 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
sleep 1
rmmod rt3593ap_cs
sleep 1
cd /rboot/wfo_ralink
/rboot/rboot wfo_pe0.bin wfo_pe1.bin
sleep 2
insmod /lib/modules/2.6.36/rt3593ap_wfo.ko
echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable

brctl delif br0 eth1
brctl delif br0 eth2
brctl delif br0 ra0
brctl delif br0 ra1
ifconfig br0 down
brctl delbr br0

sleep 5
ifconfig eth0 192.168.2.1
ifconfig eth1 0.0.0.0

brctl addbr br-lan
brctl addif br-lan eth1
ifconfig eth2 up
brctl addif br-lan eth2
cp RT2860AP-5G.dat /etc/Wireless/RT2860AP/RT2860AP.dat
ifconfig ra0 up
cp RT2860AP-2.4G.dat /etc/Wireless/RT2860AP/RT2860AP.dat
ifconfig ra1 up
sleep 2
brctl addif br-lan ra0
brctl addif br-lan ra1
ifconfig br-lan 192.168.1.1 up

#Rate Control Enable, Please turn on it if test case is over the air
iwpriv ra0 set WfoRateAdj=1
iwpriv ra1 set WfoRateAdj=1
echo 1 > /proc/driver/cs752x/wfo/wifi_offload_rate_adjust

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/driver/cs752x/ne/ni/ni_fastbridge

iptables -F
iptables -X
iptables -Z
iptables -F -t nat
iptables -X -t nat
iptables -Z -t nat
iptables -F -t mangle
iptables -X -t mangle
iptables -Z -t mangle
iptables -F -t filter
iptables -X -t filter
iptables -Z -t filter
iptables -F -t raw
iptables -X -t raw
iptables -Z -t raw
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t nat -P OUTPUT ACCEPT

iptables -A FORWARD -i br-lan -o eth0 -j ACCEPT
iptables -A FORWARD -i eth0 -o br-lan -j ACCEPT

iptables -t nat -A POSTROUTING -o eth0 -s 192.168.1.0/24 -j MASQUERADE
iptables -t nat -A PREROUTING -i eth0 -j DNAT --to-destination 192.168.1.150

clinkd -Dvvtf /etc -s 2 --firmware /etc/ccpu.elf --mac-addr 00:13:25:A0:34:08 -i /dev/jaws2 &
clinkd -Dvvtf /etc -s 3 --firmware /etc/ccpu.elf --mac-addr 00:13:25:C0:34:08 -i /dev/jaws3 &

