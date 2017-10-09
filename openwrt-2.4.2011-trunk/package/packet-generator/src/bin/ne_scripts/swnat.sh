#!/bin/sh

### For SW NAT test

if [ ! -z $1 ] && [ $1 == "all" ]; then
	KILLALL=1
else
	KILLALL=0
fi


# Remove bridge interface
ifconfig br-lan down
brctl delbr br-lan

ifconfig eth0 220.168.1.1
ifconfig eth1 192.168.1.1

# Disable GRO
ethtool -K eth0 gro off
ethtool -K eth1 gro off

# NAT
iptables -F
iptables -X
iptables -Z

iptables -F -t mangle
iptables -X -t mangle
iptables -Z -t mangle

iptables -F -t nat
iptables -X -t nat
iptables -Z -t nat

iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT

iptables -A FORWARD -i eth0 -o eth1 -j ACCEPT
iptables -A FORWARD -i eth1 -o eth0 -j ACCEPT

iptables -A POSTROUTING -t nat -o eth0 -s 192.168.1.0/24 -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward

# Remove all IPv6 settings
ifconfig bripv6 down
brctl delif bripv6 eth0

ip6tables -F
ip6tables -X
ip6tables -Z
ip6tables -F -t mangle
ip6tables -X -t mangle
ip6tables -Z -t mangle
ip6tables -F -t raw
ip6tables -X -t raw
ip6tables -Z -t raw

# eth0 IRQ uses CPU core 0
# eth1 IRQ uses CPU core 1
echo 1 > /proc/irq/69/smp_affinity
echo 2 > /proc/irq/70/smp_affinity

echo 0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
echo 0 > /proc/driver/cs752x/ne/ni/ni_fastbridge
echo 0 > /proc/driver/cs752x/ne/ni/ni_use_sendfile

echo 65000 > /proc/sys/net/ipv4/netfilter/ip_conntrack_max
echo 3 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_time_wait

# Remove all processes that influence throughput.
/etc/rc.d/S42d2 stop
/etc/d2/d2.remove
/etc/rc.d/S50mrd6 stop
/etc/rc.d/S60ipsec stop
/etc/init.d/minidlna stop
/etc/init.d/dnsmasq stop
/etc/init.d/samba stop
killall -9 udhcpc


if [ ${KILLALL} -ne 0 ]; then
	killall5
	killall -9 minidlna
	killall -9 dnsmasq
fi

exit 0

