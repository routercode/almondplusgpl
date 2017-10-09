WAN_IP="220.168.1.1"
WAN_IFACE="eth2" 
LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"

LO_IFACE="lo"
LO_IP="127.0.0.1"


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

#################################

# nat type

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

echo "#############################"
echo "you can start CDRouer testing"
echo "#############################"
