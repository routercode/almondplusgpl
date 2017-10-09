WAN_IP="220.168.1.1"
WAN_IFACE="eth2" 
DMZ_IFACE="eth1"
DMZ_HOST_IP="192.168.1.250"
#LAN_IP="192.168.1.1"
#LAN_IP_RANGE="192.168.1.0/24"
#LAN_BROADCAST_ADDRESS="192.168.1.255"
#LAN_IFACE="eth1"

#LO_IFACE="lo"
#LO_IP="127.0.0.1"

#NAT_TYPE="port_restricted" 
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


################
# chain policies
################

iptables -P INPUT DROP
iptables -P OUTPUT DROP
iptables -P FORWARD DROP

#################################

iptables -P FORWARD DROP

iptables -t nat -A PREROUTING -i $WAN_IFACE -j DNAT \
--to-destination $DMZ_HOST_IP

#iptables -t nat -I PREROUTING -i eth2 -p tcp -d  220.168.1.1 --dport 80 -j DNAT --to-destination 192.168.1.250

iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT

iptables -A FORWARD -i $WAN_IFACE -o $DMZ_IFACE -d $DMZ_HOST_IP \
-m state --state NEW -j ACCEPT

iptables -A FORWARD -i $DMZ_IFACE -o $WAN_IFACE -s $DMZ_HOST_IP \
-m state --state NEW -j ACCEPT



iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE

#j=0
#while [ "$j" != "255" ]
#do
#iptables -I FORWARD -p $j -i $WAN_IFACE -o $DMZ_IFACE -j ACCEPT
#iptables -t nat -I PREROUTING -p $j -i eth2  -j DNAT --to-destination $DMZ_HOST_IP
#
#j=$(($j+1))
#done


