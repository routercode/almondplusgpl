WAN_IP="220.168.1.1"
WAN_IFACE="eth2" ### pppoe is pppoe-wan ; static/dhcp is eth2
LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"

# Localhost Configuration.
# define Loopback IP ?..ntercace
LO_IFACE="lo"
LO_IP="127.0.0.1"

# IPTables Configuration.
# IPTABLES="/sbin/iptables"

NAT_TYPE="port_restricted" 
#### full_cone / port_restricted /address_restricted /          

##################################################

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
iptables -Z          
iptables -t nat -Z
iptables -t mangle -Z
iptables -F -t raw
iptables -P INPUT ACCEPT                                          
iptables -P FORWARD ACCEPT                                           
iptables -P OUTPUT ACCEPT
iptables -I FORWARD -p 50 -j ACCEPT
iptables -I FORWARD -p 51 -j ACCEPT
iptables -I FORWARD -p 47 -j ACCEPT
iptables -I FORWARD -p udp --dport 500 -j ACCEPT
iptables -I FORWARD -p udp --dport 1701 -j ACCEPT
iptables -I FORWARD -p tcp --dport 1723 -j ACCEPT                               

#############virtual server setup####################

###TCP Virtual server
#VSER1_IP=192.168.1.18
#VSER1_PORT=8080
#VSER2_IP=192.168.1.28
#VSER2_PORT=25
#VSER3_IP=192.168.1.38
#VSER3_PORT=110

###UDP Virtual server
#VSER4_IP=192.168.1.8
#VSER4_PORT=10000

##
#iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER1_PORT -j DNAT --to-destination $VSER1_IP:$VSER1_PORT
#iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER2_PORT -j DNAT --to-destination $VSER2_IP:$VSER2_PORT
#iptables -t nat -I PREROUTING -i $WAN_IFACE -p tcp -d $WAN_IP --dport $VSER3_PORT -j DNAT --to-destination $VSER3_IP:$VSER3_PORT
#iptables -t nat -I PREROUTING -i $WAN_IFACE -p udp -d $WAN_IP --dport $VSER4_PORT -j DNAT --to-destination $VSER4_IP:$VSER4_PORT

#iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER1_PORT -j DNAT --to-destination $VSER1_IP:$VSER1_PORT
#iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER2_PORT -j DNAT --to-destination $VSER2_IP:$VSER2_PORT
#iptables -t nat -I PREROUTING -i $LAN_IFACE -p tcp -d $WAN_IP --dport $VSER3_PORT -j DNAT --to-destination $VSER3_IP:$VSER3_PORT
#iptables -t nat -I PREROUTING -i $LAN_IFACE -p udp -d $WAN_IP --dport $VSER4_PORT -j DNAT --to-destination $VSER4_IP:$VSER4_PORT

#echo "virtual server:$VSER1_IP:$VSER1_PORT"
#echo "virtual server:$VSER2_IP:$VSER2_PORT"
#echo "virtual server:$VSER3_IP:$VSER3_PORT"
#echo "virtual server:$VSER4_IP:$VSER4_PORT"

#################################


#################################

# 4.2 nat table /nat type

#################################

#if [ $WAN_IFACE == "eth2" ]; then
#WAN_IP=$(cat /tmp/state/network | grep network.wan.ipaddr | awk -F "=" '{print $2}')
#fi
#########Full Cone NAT########################
if [ $NAT_TYPE == "full_cone"  ]; then

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
#elif [ $NAT_TYPE == "address_restricted" ]; then
#echo "address Restricted Cone NAT"
#iptables -t nat -A POSTROUTING -s 192.168.1.10 -j SNAT --to-source $WAN_IP
#iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
#############################################

else
echo "use auto MASQUERADE"
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE 
fi

if [ $WAN_IFACE == "pppoe-wan" ]; then
iptables -A FORWARD -p tcp -o $WAN_IFACE  --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
iptables -A OUTPUT -p tcp -o $WAN_IFACE --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 
fi
