WAN_IP="220.168.1.1"
WAN_IFACE="eth2" ### pppoe is pppoe-wan ; static/dhcp is eth2
DHCP_S="0" 
LAN_IP="192.168.1.1"
LAN_IP_RANGE="192.168.1.0/24"
LAN_BROADCAST_ADDRESS="192.168.1.255"
LAN_IFACE="eth1"
LO_IFACE="lo"
LO_IP="127.0.0.1"

NAT_TYPE="port_restricted" 
#echo "nat-type : port_restricted"
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

iptables -N tcp_allowed
iptables -A tcp_allowed -p tcp --syn -j ACCEPT
iptables -A tcp_allowed -p tcp -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A tcp_allowed -p tcp -j DROP
# icmp_allowed 
echo "setup icmp_allowed"
iptables -N icmp_allowed
iptables -A icmp_allowed -p icmp --icmp-type 11 -j ACCEPT
iptables -A icmp_allowed -p icmp --icmp-type 0 -j ACCEPT 
iptables -A icmp_allowed -p icmp --icmp-type 8 -j ACCEPT
iptables -A icmp_allowed -p icmp -j DROP

# ip spoofing
echo " setup ip spoofing"
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 10.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 127.0.0.0/8 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 172.16.0.0/12 -j DROP
#iptables -t raw -A PREROUTING -i $WAN_IFACE -s 192.168.0.0/16 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 224.0.0.0/4 -j DROP
iptables -t raw -A PREROUTING -i $WAN_IFACE -s 240.0.0.0/5 -j DROP
if [ $DHCP_S == "0" ]; then
iptables -t raw -A PREROUTING -i $WAN_IFACE -s $LAN_IP_RANGE -j DROP
iptables -t raw -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP
fi
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT
### null-scan ###
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL NONE -j LOG \
#--log-prefix "IPTABLES NULL-SCAN:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL NONE -j DROP
#
### xmas-scan ###
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL ALL -j LOG \
#--log-prefix "IPTABLES XMAS-SCAN:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL ALL -j DROP

#
# synfin-scan
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL SYN,FIN -j LOG \
#--log-prefix "IPTABLES SYNFIN-SCAN:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL SYN,FIN -j DROP

#
# nmap-xmas-scan
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL URG,PSH,FIN -j LOG \
#--log-prefix "IPTABLES NMAP-XMAS-SCAN:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL URG,PSH,FIN -j DROP

#
### fin-scan ###
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL FIN -j LOG --log-prefix "IPTABLES FIN-SCAN:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL FIN -j DROP

#
### nmap-id ###
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL URG,PSH,SYN,FIN -j LOG --log-prefix "IPTABLES NMAP-ID:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags ALL URG,PSH,SYN,FIN -j DROP

#
### syn-rst ###
#
#iptables -t nat -A PREROUTING -p TCP --tcp-flags SYN,RST SYN,RST -j LOG --log-prefix "IPTABLES SYN-RST:"
#iptables -t nat -A PREROUTING -p TCP --tcp-flags SYN,RST SYN,RST -j DROP
#
### new-without-syna ###
#
#iptables -t nat -A PREROUTING -p TCP ! --syn -m state --state NEW -j LOG --log-prefix "IPTABLES NEW-WITHOUT-SYN:"
#iptables -t nat -A PREROUTING -p TCP ! --syn -m state --state NEW -j DROP
#
### syn-flood ###
#
#iptables -t nat -N syn_flood
#iptables -t nat -A syn_flood -m limit --limit 3/m --limit-burst 3 -j RETURN
#iptables -t nat -A syn_flood -j LOG --log-level INFO --log-prefix "IPTABLES SYN-FLOOD:"
#iptables -t nat -A syn_flood -j DROP
#iptables -t nat -A PREROUTING -p TCP --syn -j syn_flood

#
### port-scan ###
#
iptables -t nat -N port_scan
iptables -t nat -A port_scan -m limit --limit 1/s --limit-burst 4 -j RETURN
#iptables -t nat -A port_scan -j \
#LOG --log-level INFO --log-prefix "IPTABLES PORT-SCAN:"
#iptables -t nat -A port_scan -j DROP
#iptables -t nat -A PREROUTING -p TCP --tcp-flags SYN,ACK,FIN,RST RST -j port_scan

#
### ping-death ###
#
iptables -t nat -N ping_death
iptables -t nat -A ping_death -m limit --limit 1/s --limit-burst 4 -j RETURN
iptables -t nat -A ping_death -j LOG --log-level INFO --log-prefix "IPTABLES PING-DEATH:"
#iptables -t nat -A ping_death -j DROP
iptables -t nat -A PREROUTING -p ICMP --icmp-type 8 -j ping_death

#
### ip spoofing ###
#
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 10.0.0.0/8 -j DROP
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 127.0.0.0/8 -j DROP
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 172.16.0.0/12 -j DROP
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 192.168.0.0/16 -j DROP
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 224.0.0.0/4 -j DROP
#iptables -t nat -A PREROUTING -i $WAN_IFACE -s 240.0.0.0/5 -j DROP
#iptables -t nat -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP
#iptables -t nat -A PREROUTING -i $DMZ_IFACE ! -s $DMZ_IP_RANGE -j DROP

#
#INPUT chain
#
#
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT
# iptables -A INPUT -p igmp  -j ACCEPT

# lo 
iptables -A INPUT -p ALL -i $LO_IFACE -s $LO_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $LAN_IP -j ACCEPT
iptables -A INPUT -p ALL -i $LO_IFACE -s $WAN_IP -j ACCEPT

#iptables -A INPUT -p UDP -i $LAN_IFACE --dport $DHCP_SERVER --sport $DHCP_CLIENT -j ACCEPT

iptables -A INPUT -p ALL -d $WAN_IP -m state --state ESTABLISHED,RELATED -j ACCEPT

#iptables -A INPUT -m limit --limit 3/minute --limit-burst 3 -j LOG --log-level INFO --log-prefix "IPT INPUT packet died: "
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
#iptables -A OUTPUT -p ALL -s $LAN_IP -j ACCEPT
#iptables -A OUTPUT -p ALL -s $WAN_IP -j ACCEPT

#################################

#iptables -t raw -A PREROUTING -i $LAN_IFACE ! -s $LAN_IP_RANGE -j DROP                              
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_IP -j ACCEPT                                         
iptables -A INPUT -p ALL -i $LAN_IFACE -d $LAN_BROADCAST_ADDRESS -j ACCEPT 
# lo                                                                                                
iptables -A INPUT -p ALL -i $LO_IFACE -s $LO_IP -j ACCEPT                                           
iptables -A INPUT -p ALL -i $LO_IFACE -s $LAN_IP -j ACCEPT                                          
iptables -A INPUT -p ALL -i $LO_IFACE -s $WAN_IP -j ACCEPT                                          
 #                                                                                                   
                                                                             
iptables -A FORWARD -i $LAN_IFACE -m state --state NEW -j ACCEPT                                    
iptables -A FORWARD -i $WAN_IFACE -m state --state ESTABLISHED,RELATED -j ACCEPT                    
iptables -I FORWARD -d 192.168.1.0/24 -j ACCEPT                                                     
iptables -I FORWARD -p icmp -d $WAN_IP -j ACCEPT                                                    
iptables -A FORWARD -p icmp  -j icmp_allowed                                                        
iptables -I INPUT -s $LAN_IP_RANGE -j ACCEPT                                                        
iptables -A OUTPUT -p ALL -s $LO_IP -j ACCEPT                                                       
iptables -A OUTPUT -p ALL -s $LAN_IP -j ACCEPT                                                      
iptables -A OUTPUT -p ALL -s $WAN_IP -j ACCEPT 
#iptables -I OUTPUT -p udp --dport 67:68 -j ACCEPT
#iptables -I INPUT -p udp --dport 67:68 -j ACCEPT
                                                    
iptables -A FORWARD -p esp -j ACCEPT                                           
iptables -A FORWARD -p ah -j ACCEPT                                            
iptables -A FORWARD -p udp --sport 500 --dport 500 -j ACCEPT
                                                      
#################################
#if [ $WAN_IFACE == "eth2" ]; then                                                   
#WAN_IP=$(cat /tmp/state/network | grep network.wan.ipaddr | awk -F "=" '{print $2}')
#fi      
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
iptables -t nat -A POSTROUTING -o $WAN_IFACE -j SNAT --to-source $WAN_IP
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
                                                                             
echo "firewall setup finish"
                                                                             
