WAN_IP="220.168.1.1"
WAN_IFACE="eth2" ### pppoe is pppoe-wan ; static/dhcp is eth2
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

NAT_TYPE="port_restricted" 
#### full_cone / port_restricted /address_restricted


##################################################
iptables -F
iptables -t nat -F
iptables -t mangle -F
iptables -X
iptables -t nat -X
iptables -t mangle -X
iptables -Z          
iptables -t nat -Z
iptables -t mangle -Z
iptables -F -t raw

iptables -P INPUT DROP                                                   
iptables -P FORWARD DROP                                                 
iptables -P OUTPUT DROP 

#iptables -N tcp_allowed
#iptables -A tcp_allowed -p tcp --syn -j ACCEPT
#iptables -A tcp_allowed -p tcp -m state --state ESTABLISHED,RELATED -j ACCEPT
#iptables -A tcp_allowed -p tcp -j DROP

iptables -N icmp_allowed
iptables -A icmp_allowed -p icmp --icmp-type 11 -j ACCEPT
iptables -A icmp_allowed -p icmp --icmp-type 0 -j ACCEPT 
iptables -A icmp_allowed -p icmp --icmp-type 8 -j ACCEPT
iptables -A icmp_allowed -p icmp -j DROP

# ip spoofing

iptables -t raw -A PREROUTING -i $WAN_IFACE -s 10.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 127.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 172.16.0.0/12 -j DROP
#iptables -t raw -A PREROUTING -i $WAN_IFACE -s 192.168.0.0/16 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 224.0.0.0/4 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 240.0.0.0/5 -j DROP
#iptables -t raw -A PREROUTING -i $WAN_IFACE -s $LAN_IP_RANGE -j DROP
#iptables -t raw -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP

iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT

# lo 
iptables -A INPUT -p ALL -i $LO_IFACE -s $LO_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $WAN_IP -j ACCEPT
iptables -A INPUT -p ALL -d $WAN_IP -m state --state ESTABLISHED,RELATED -j ACCEPT
#

iptables -A FORWARD -i $LAN_IFACE -o $WAN_IFACE -j ACCEPT
iptables -A FORWARD -i $LAN_IFACE -m state --state NEW -j ACCEPT
iptables -A FORWARD -i $WAN_IFACE -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -I FORWARD -d 192.168.1.0/24 -j ACCEPT
iptables -I FORWARD -p icmp -d $WAN_IP -j ACCEPT
iptables -A FORWARD -p icmp  -j icmp_allowed
iptables -I INPUT -s $LAN_IP_RANGE -j ACCEPT
iptables -A OUTPUT -p ALL -s $LO_IP -j ACCEPT
iptables -A OUTPUT -p ALL -s $LAN_IP -j ACCEPT
iptables -A OUTPUT -p ALL -s $WAN_IP -j ACCEPT

#################################

iptables -t raw -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP                              
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT                                         
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT                          
#############ALG(Application layer gateway)setup####################
###FTP ALG(TCP)
#APP1_PORT=20
#APP2_PORT=21
###MSN ALG(TCP/UDP)
#APP3_PORT=1863 
###H323 ALG(TCP)                                                                    
#APP4_PORT=1720
##RTSP (TCP/UDP)
#APP5PORT=554
#APP6_PORT=8554
##SIP ALG(TCP/UDP)
#APP7_PORT=5060
#APP8_PORT=5061
#iptables -I FORWARD -p tcp -o eth2   --sport 1024:65535 --dport 1024:65535  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
#iptables -I FORWARD -p tcp -i eth2  --sport 1024:65535 --dport 1024:65535  -m state --state ESTABLISHED,RELATED -j ACCEPT

iptables -I FORWARD -p tcp   --sport 20 --dport 20  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p tcp   --sport 21 --dport 21  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT

iptables -I FORWARD -p tcp   --sport 1863 --dport 1863  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p udp   --sport 1863 --dport 1863  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p tcp   --sport 1720 --dport 1720  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p tcp   --sport 554 --dport 554  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p udp   --sport 8554 --dport 8554  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p tcp   --sport 5060 --dport 5060  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT
iptables -I FORWARD -p udp   --sport 5061 --dport 5061  -m state --state ESTABLISHED,RELATED,NEW -j ACCEPT

#iptables -I FORWARD -p tcp -i eth2 --dport 1024:65535 -j ACCEPT
iptables -t nat -A PREROUTING -i eth2 -p udp --dport 554 -j DNAT --to-destination 192.168.1.10:554


#################################
#if [ $WAN_IFACE == "eth2" ]; then                                                   
#WAN_IP=$(cat /tmp/state/network | grep network.wan.ipaddr | awk -F "=" '{print $2}')
#fi      
#################################

# 4.2 nat table /nat type

#################################

#########Full Cone NAT########################
if [ $NAT_TYPE == "full_cone" ]; then
echo "Full Cone NAT: one to one NAT"
iptables -t nat -A PREROUTING  -d $WAN_IP -j DNAT --to-destination 192.168.1.10
iptables -t nat -A POSTROUTING -s 192.168.1.10 -j SNAT --to-source $WAN_IP
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
#########Port Restricted Cone NAT##############
elif [ $NAT_TYPE  == "port_restricted" ]; then
echo "Port Restricted Cone NAT"
#iptables -t nat -A POSTROUTING -o $WAN_IFACE -j SNAT --to-source $WAN_IP
#iptables -t nat -A POSTROUTING -s 192.168.1.10  -j SNAT --to-source $WAN_IP
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
#########addree Restricted Cone NAT##########ps:need to study setup again##
#elif [ "$NAT_TYPE" = "address_restricted" ]; then
#echo "address Restricted Cone NAT"
#iptables -t nat -A POSTROUTING -s 192.168.1.10 -j SNAT --to-source $WAN_IP
#iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
#############################################

else
echo "use auto MASQUERADE"
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE 
fi

if [ $WAN_IFACE == "pppoe-wan" ]; then
iptables -I FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -I OUTPUT -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
fi   


echo "##################################"
echo "you can start CDRouter ALG testing"
echo "##################################"

