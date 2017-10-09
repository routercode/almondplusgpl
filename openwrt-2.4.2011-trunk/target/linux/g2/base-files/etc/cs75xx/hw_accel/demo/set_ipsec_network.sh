#!/bin/sh
IPSEC_HW_ENCP_DECP=$1

if [ "$IPSEC_HW_ENCP_DECP" != "0" ];then
cd /rboot/ipsec
/rboot/rboot ipsec_dec_pe0.bin ipsec_enc_pe1.bin
fi

ifconfig eth0 192.168.60.1

sleep 2
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
iptables -t nat -A POSTROUTING -o eth0 -s 192.168.1.0/24 -d 192.168.2.0/24 -j RETURN
iptables -t nat -A POSTROUTING -o eth0 -s 192.168.1.0/24 -j MASQUERADE

route add -net 192.168.2.0 netmask 255.255.255.0 gw 192.168.60.2
mkdir -p -m 700 /var/run/racoon2
chmod 600  /etc/racoon2/spmd.pwd
chmod 600  -R /etc/racoon2/psk
spmd -f /etc/racoon2/racoon2-dut1-ipv4-psk.conf
sleep 3
iked -f /etc/racoon2/racoon2-dut1-ipv4-psk.conf
