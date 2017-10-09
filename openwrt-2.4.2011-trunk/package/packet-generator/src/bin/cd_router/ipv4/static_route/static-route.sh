#!/bin/sh
WAN_PORT=static ## dhcp / static /pppoe
DHCP_S=0 ## 1 or 0

STATIC_LAN_NETWORK=192.168.2.0                                   
STATIC_LAN_NETMASK=255.255.255.0                                 
STATIC_LAN_NEXTHOP=192.168.1.10                                               
##################################
if [ "$WAN_PORT" == "dhcp" ]; then
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
uci set network.loopback=interface
uci set network.loopback.ifname=lo
uci set network.loopback.proto=static
uci set network.loopback.ipaddr=127.0.0.1
uci set network.loopback.netmask=255.0.0.0
uci set network.lan=interface
uci set network.lan.ifname=eth1
uci set network.lan.proto=static
uci set network.lan.ipaddr=192.168.1.1
uci set network.lan.netmask=255.255.255.0
uci set network.lan.defaultroute=0
uci set network.lan.peerdns=0

uci set network.wan=interface
uci set network.wan.ifname=eth2
uci set network.wan.dns=1.1.1.1
uci set network.wan.proto=dhcp
uci set network.wan.mtu=1500
uci set network.wan.defaultroute=0
uci set network.wan.peerdns=0

uci set network.route=route
uci set network.route.interface=lan
uci set network.route.target=192.168.2.0
uci set network.route.netmask=255.255.255.0
uci set network.route.gateway=192.168.1.10

echo " wan port is dhcp "

##################################
elif [ "$WAN_PORT" = "pppoe" ]; then
uci set network.loopback=interface
uci set network.loopback.ifname=lo
uci set network.loopback.proto=static
uci set network.loopback.ipaddr=127.0.0.1
uci set network.loopback.netmask=255.0.0.0
uci set network.lan=interface
uci set network.lan.ifname=eth1
uci set network.lan.proto=static
uci set network.lan.ipaddr=192.168.1.1
uci set network.lan.netmask=255.255.255.0
uci set network.lan.defaultroute=0
uci set network.lan.peerdns=0
uci set network.wan=interface
uci set network.wan.ifname=eth2
uci set network.wan.proto=pppoe
uci set network.wan.username=cortina
uci set network.wan.password=cortina
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=1
uci set network.wan.mtu=1492

uci set network.@route[0]=route                
uci set network.@route[0].interface=lan        
uci set network.@route[0].target=192.168.2.0   
uci set network.@route[0].netmask=255.255.255.0
uci set network.@route[0].gateway=192.168.1.10
echo " wan port is pppoe " 
#################################
elif [ "$WAN_PORT" == "static" ]; then
uci set network.loopback=interface
uci set network.loopback.ifname=lo
uci set network.loopback.proto=static
uci set network.loopback.ipaddr=127.0.0.1
uci set network.loopback.netmask=255.0.0.0
uci set network.lan=interface
uci set network.lan.ifname=eth1
uci set network.lan.proto=static
uci set network.lan.ipaddr=192.168.1.1
uci set network.lan.netmask=255.255.255.0
uci set network.lan.defaultroute=0
uci set network.lan.peerdns=0
uci set network.wan=interface
uci set network.wan.ifname=eth2
uci set network.wan.proto=static
uci set network.wan.ipaddr=220.168.1.1
uci set network.wan.netmask=255.255.255.0
uci set network.wan.gateway=220.168.1.254
uci set network.wan.mtu=1500
uci set network.wan.defaultroute=1
uci set network.wan.peerdns=0
uci set network.wan.dns=1.1.1.1
uci set network.route=route                
uci set network.route.interface=lan        
uci set network.route.target=192.168.2.0   
uci set network.route.netmask=255.255.255.0
uci set network.route.gateway=192.168.1.10
echo " wan port is static " 
else
echo " wan port is not pppoe/dhcp/static " 
fi
uci commit network 
sleep 1
/etc/init.d/network restart
sleep 1
##################################
uci set ntpclient.@ntpserver[0]=ntpserver               
uci set ntpclient.@ntpserver[0].hostname=0.time.nist.gov
uci set ntpclient.@ntpserver[0].port=123               
uci set ntpclient.@ntpserver[1]=ntpserver              
uci set ntpclient.@ntpserver[1].hostname=1.time.foo.com
uci set ntpclient.@ntpserver[1].port=123    
uci set ntpclient.@ntpdrift[0]=ntpdrift     
uci set ntpclient.@ntpdrift[0].freq=0       
uci set ntpclient.@ntpclient[0]=ntpclient   
uci set ntpclient.@ntpclient[0].interval=600 
uci commit ntpclient                       
echo "127.0.0.1" > /etc/hosts              
echo "3.3.3.6 time.nist.gov" >>  /etc/hosts
echo "3.3.3.7 time.foo.gov" >>  /etc/hosts 
/etc/hotplug.d/iface/20-ntpclient 
##################################
echo "test"                                                       
if [ "$DHCP_S" = "1" ]; then                            
                                                                                                                
uci set dhcp.@dnsmasq[0]=dnsmasq                        
uci set dhcp.@dnsmasq[0].domainneeded=1                 
uci set dhcp.@dnsmasq[0].boguspriv=1                    
uci set dhcp.@dnsmasq[0].filterwin2k=0                  
uci set dhcp.@dnsmasq[0].localise_queries=1             
uci set dhcp.@dnsmasq[0].local=/lan/                    
uci set dhcp.@dnsmasq[0].domain=lan                     
uci set dhcp.@dnsmasq[0].expandhosts=1                  
uci set dhcp.@dnsmasq[0].nonegcache=0                   
uci set dhcp.@dnsmasq[0].authoritative=1                
uci set dhcp.@dnsmasq[0].readethers=1                   
uci set dhcp.@dnsmasq[0].leasefile=/tmp/dhcp.leases     
uci set dhcp.@dnsmasq[0].resolvfile=/tmp/resolv.conf.auto
uci set dhcp.lan=dhcp                               
uci set dhcp.lan.interface=lan                      
uci set dhcp.lan.start=192.168.1.10                 
uci set dhcp.lan.limit=192.168.1.200                
uci set dhcp.lan.leasetime=12                       
uci set dhcp.@host[0]=host                               
uci set dhcp.@host[0].name=qanb                          
uci set dhcp.@host[0].mac=00:11:22:33:33:44              
uci set dhcp.@host[0].ip=192.168.1.88                    
uci commit                                               
echo "dnsmasq enable"                                    
sleep 1                                                  
/etc/init.d/dnsmasq  restart                             
else                                                     
/etc/init.d/dnsmasq  stop                                
fi                                                       
sleep 1 
#################################
route add -net $STATIC_LAN_NETWORK netmask  $STATIC_LAN_NETMASK gw $STATIC_LAN_NEXTHOP
#route add -net $STATIC_WAN_NETWORK netmask  $STATIC_WAN_NETMASK gw $STATIC_WAN_NEXTHOP
ip route 
#################################
#./nat-accept-all.sh
sleep 5
iptables -t nat -A POSTROUTING -o eth2 -j MASQUERADE
echo "---------------------------------------------"
echo "iptables accept all packet for this test case"
echo "---------------------------------------------" 

#sleep 3
#if [ "$WAN_PORT" == "dhcp" ]; then
#route add default gw 220.168.1.1 dev eth2
#fi
