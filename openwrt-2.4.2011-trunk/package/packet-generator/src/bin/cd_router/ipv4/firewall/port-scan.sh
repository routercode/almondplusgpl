WAN_IP="220.168.1.1"
WAN_IFACE="eth2" ## pppoe ## 

LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"

# Localhost Configuration.
# define Loopback IP €Îintercace
LO_IFACE="lo"
LO_IP="127.0.0.1"
# IPTables Configuration.
# IPTABLES="/sbin/iptables"
# support simulttaneous tcp syn through NAT


###########################################################################
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t mangle -P POSTROUTING ACCEPT
iptables -t mangle -P PREROUTING ACCEPT
iptables -t mangle -P FORWARD ACCEPT
iptables -F
iptables -t nat -F
iptables -t mangle -F
iptables -X
iptables -t nat -X
iptables -t mangle -X
iptables -F -t raw

iptables -P INPUT DROP                                               
iptables -P FORWARD DROP                                           
iptables -P OUTPUT DROP

#=========open tcp port :23 53=================
iptables -I INPUT -i eth2 -p tcp --dport 23 -j ACCEPT

#iptables -I INPUT -i eth2 -p tcp --dport 80 -j ACCEPT
#iptables -I INPUT -i eth2 -p tcp --dport 22 -j ACCEPT
#iptables -I INPUT -i eth2 -p tcp --dport 25 -j ACCEPT
iptables -I INPUT -i eth2 -p tcp --dport 53 -j ACCEPT                             
#iptables -I INPUT -i eth2 -p tcp --dport 80 -j ACCEPT
#iptables -I FORWARD -i eth2 -p tcp --dport 23 -j ACCEPT
#iptables -I FORWARD -i eth2 -p tcp --dport 22 -j ACCEPT
#iptables -I FORWARD -i eth2 -p tcp --dport 25 -j ACCEPT
#iptables -I FORWARD -i eth2 -p tcp --dport 53 -j ACCEPT                             
#tables -I FORWARD -i eth2 -p tcp --dport 80 -j ACCEPT  
echo "open tcp port 23  53 "
#=========open udp port =================
iptables -I INPUT -i eth2 -p udp --dport  137 -j ACCEPT 
iptables -I FORWARD -i eth2 -p udp --dport 137 -j ACCEPT
iptables -I INPUT -i eth2 -p udp --dport  138 -j ACCEPT 
iptables -I FORWARD -i eth2 -p udp --dport 138 -j ACCEPT
echo "open udp port 137-138"

iptables -N tcp_allowed
iptables -A tcp_allowed -p tcp --syn -j ACCEPT
iptables -A tcp_allowed -p tcp -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A tcp_allowed -p tcp -j DROP

iptables -N icmp_allowed
iptables -A icmp_allowed -p icmp --icmp-type 11 -j ACCEPT
iptables -A icmp_allowed -p icmp --icmp-type 0 -j ACCEPT 
iptables -A icmp_allowed -p icmp --icmp-type 8 -j ACCEPT
iptables -A icmp_allowed -p icmp -j DROP

# ip spoofing

iptables -t raw -A PREROUTING -i $WAN_IFACE -s 10.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 127.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 172.16.0.0/12 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 192.168.0.0/16 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 224.0.0.0/4 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 240.0.0.0/5 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s $LAN_IP_RANGE -j DROP
iptables -t raw -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP

iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT
# lo 
iptables -A INPUT -p ALL -i $LO_IFACE -s $LO_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $WAN_IP -j ACCEPT
iptables -A INPUT -p ALL -d $WAN_IP -m state --state ESTABLISHED,RELATED -j ACCEPT
##
##iptables -A FORWARD -i eth2 -p tcp --syn  -j DROP
iptables -A FORWARD -i $LAN_IFACE -o $WAN_IFACE -j ACCEPT
iptables -A FORWARD -i $LAN_IFACE -m state --state NEW -j ACCEPT
iptables -A FORWARD -i $WAN_IFACE -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -I FORWARD -i $LAN_IFACE -d 192.168.1.0/24 -j ACCEPT
iptables -I FORWARD -p icmp -d $WAN_IP -j ACCEPT
iptables -A FORWARD -p icmp  -j icmp_allowed
iptables -I INPUT -i eth1 -s $LAN_IP_RANGE -j ACCEPT
iptables -A OUTPUT -p ALL -s $LO_IP -j ACCEPT
iptables -A OUTPUT -p ALL -s $LAN_IP -j ACCEPT
iptables -A OUTPUT -p ALL -s $WAN_IP -j ACCEPT

#################################

# 4.2 nat table /nat type

#################################

#########Full Cone NAT########################

#echo "Full Cone NAT: one to one NAT"
#iptables -t nat -A PREROUTING  -d $WAN_IP -j DNAT --to-destination 192.168.1.10

#iptables -t nat -A POSTROUTING -s 192.168.1.10 -j SNAT --to-source $WAN_IP

#########Port Restricted Cone NAT########################
echo "Port Restricted Cone NAT"
#iptables -t nat -A POSTROUTING -o $WAN_IFACE -j SNAT --to-source $WAN_IP
#iptables -t nat -A POSTROUTING -s 192.168.1.10  -j SNAT --to-source $WAN_IP 
#iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
#########addree Restricted Cone NAT########################
#elif ["NAT_TYPE" = "address Restricted"]; then
#echo "address Restricted Cone NAT"
#iptables -t nat -A POSTROUTING -s 192.168.1.10 -j SNAT --to-source $WAN_IP
#########

#else
#echo "use auto MASQUERADE"
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE 
#iptables -t nat -A POSTROUTING -o $LAN_IFACE -j MASQUERADE
#fi 

/etc/init.d/samba restart
/etc/init.d/telnet restart



