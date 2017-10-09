#!/bin/sh

ifconfig eth0 192.168.60.1

echo "#USERNAME  PROVIDER  PASSWORD  IPADDRESS" > /etc/ppp/chap-secrets
echo "user1 * 1234 *" >> /etc/ppp/chap-secrets

echo "[lns default] " > /etc/xl2tpd/xl2tpd.conf
echo "ip range = 192.168.2.128-192.168.2.254 " >> /etc/xl2tpd/xl2tpd.conf
echo "local ip = 192.168.2.99 " >> /etc/xl2tpd/xl2tpd.conf
echo "require chap = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "refuse pap = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "require authentication = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "name = CortinaVPNserver" >> /etc/xl2tpd/xl2tpd.conf
echo "ppp debug = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "pppoptfile = /etc/ppp/options.xl2tpd" >> /etc/xl2tpd/xl2tpd.conf
echo "length bit = yes" >> /etc/xl2tpd/xl2tpd.conf


echo "# do not change the indenting of that \"}\" " > /etc/ipsec.secrets
echo "%any %any: PSK \"1234567890\"" >> /etc/ipsec.secrets


echo "config setup " > /etc/ipsec.conf
echo "  nat_traversal=no " >> /etc/ipsec.conf
echo "  oe=off        " >> /etc/ipsec.conf
echo "  protostack=auto" >> /etc/ipsec.conf
echo "  dumpdir=/var/run/pluto/" >> /etc/ipsec.conf
echo "conn L2TP-PSK-NAT" >> /etc/ipsec.conf
echo "  rightsubnet=vhost:%no,%priv" >> /etc/ipsec.conf
echo "  also=L2TP-PSK-noNAT" >> /etc/ipsec.conf

echo "conn L2TP-PSK-noNAT" >> /etc/ipsec.conf
echo "    authby=secret" >> /etc/ipsec.conf
echo "    pfs=no" >> /etc/ipsec.conf
echo "    auto=add" >> /etc/ipsec.conf
echo "    rekey=no" >> /etc/ipsec.conf
echo "    keyingtries=3" >> /etc/ipsec.conf
echo "    ikelifetime=8h" >> /etc/ipsec.conf
echo "    keylife=1h" >> /etc/ipsec.conf
echo "    type=transport" >> /etc/ipsec.conf
echo "    left=%defaultroute" >> /etc/ipsec.conf
echo "    leftprotoport=17/1701" >> /etc/ipsec.conf
echo "    right=%any" >> /etc/ipsec.conf
echo "    rightprotoport=17/%any" >> /etc/ipsec.conf
echo "    ike=3des-md5;modp1024,aes-sha1;modp1536" >> /etc/ipsec.conf

chmod 777 /etc/xl2tpd/start_xl2tpd.sh
route add default gw 192.168.60.2
killall xl2tpd

/etc/xl2tpd/start_xl2tpd.sh
/etc/openswan/ipsec restart
