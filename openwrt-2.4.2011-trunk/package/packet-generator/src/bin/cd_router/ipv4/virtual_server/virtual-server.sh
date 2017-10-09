WAN_IP="220.168.1.1"
WAN_IFACE="eth2"
LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"

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

iptables -N tcp_allowed
iptables -A tcp_allowed -p tcp --syn -j ACCEPT
iptables -A tcp_allowed -p tcp -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A tcp_allowed -p tcp -j DROP

iptables -N icmp_allowed
iptables -A icmp_allowed -p icmp --icmp-type 11 -j ACCEPT
iptables -A icmp_allowed -p icmp --icmp-type 0 -j ACCEPT 
iptables -A icmp_allowed -p icmp --icmp-type 8 -j ACCEPT
iptables -A icmp_allowed -p icmp -j DROP


iptables -A FORWARD -i $LAN_IFACE -o $WAN_IFACE -j ACCEPT
iptables -A FORWARD -i $LAN_IFACE -m state --state NEW -j ACCEPT
iptables -A FORWARD -i $WAN_IFACE -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -I FORWARD -d 192.168.1.0/24 -j ACCEPT
iptables -I FORWARD -p icmp -d $WAN_IP -j ACCEPT
iptables -A FORWARD -p icmp  -j icmp_allowed
iptables -I INPUT -s $LAN_IP_RANGE -j ACCEPT

###TCP Virtual server
VSER1_IP=192.168.1.18
VSER1_PORT=8080
VSER2_IP=192.168.1.28                                                           
VSER2_PORT=25
VSER3_IP=192.168.1.38                                                              
VSER3_PORT=110

###UDP Virtual server
VSER4_IP=192.168.1.8                                                                
VSER4_PORT=10000 

##
iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER1_PORT -j DNAT --to-destination $VSER1_IP:$VSER1_PORT
iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER2_PORT -j DNAT --to-destination $VSER2_IP:$VSER2_PORT
iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER3_PORT -j DNAT --to-destination $VSER3_IP:$VSER3_PORT
iptables -t nat -I PREROUTING -i $WAN_IFACE -p udp -d $WAN_IP --dport $VSER4_PORT -j DNAT --to-destination $VSER4_IP:$VSER4_PORT

iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER1_PORT -j DNAT --to-destination $VSER1_IP:$VSER1_PORT
iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER2_PORT -j DNAT --to-destination $VSER2_IP:$VSER2_PORT
iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER3_PORT -j DNAT --to-destination $VSER3_IP:$VSER3_PORT
iptables -t nat -I PREROUTING -i $LAN_IFACE -p udp -d $WAN_IP --dport $VSER4_PORT -j DNAT --to-destination $VSER4_IP:$VSER4_PORT

echo "virtual server:$VSER1_IP:$VSER1_PORT"
echo "virtual server:$VSER2_IP:$VSER2_PORT"
echo "virtual server:$VSER3_IP:$VSER3_PORT"
echo "virtual server:$VSER4_IP:$VSER4_PORT"


#########Port Restricted Cone NAT##############
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j SNAT --to-source $WAN_IP
iptables -t nat -A POSTROUTING -s 192.168.1.10  -j SNAT --to-source $WAN_IP
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE

echo "****************************************"
echo "start cdrouter virtual services  testing"
echo "****************************************" 

