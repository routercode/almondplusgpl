WAN_IP="220.168.1.1"
WAN_IFACE="eth2" 
LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"


NAT_TYPE="port_restricted" 
#### full_cone / port_restricted /address_restricted /          

echo "199.199.1.2 www.yahoo.com.tw" >> /etc/hosts
echo "199.199.1.3 www.google.com" >> /etc/hosts


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
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE

#############################
iptables -I FORWARD -m string --algo bm --string "www.yahoo.com.tw" -j DROP
iptables -I FORWARD -m string --algo bm --string "www.google.com" -j ACCEPT
iptables -A FORWARD -d www.yahoo.com.tw -j DROP
iptables -A FORWARD -d 199.199.1.2 -j DROP
iptables -A FORWARD -d www.google.com -j ACCEPT
iptables -A FORWARD -d 199.199.1.3 -j ACCEPT
echo "################################"
echo "start CDRouter urlfilter testing"
echo "################################"
echo "drop www.yahoo.com.tw"
echo "accept www.google.com"



