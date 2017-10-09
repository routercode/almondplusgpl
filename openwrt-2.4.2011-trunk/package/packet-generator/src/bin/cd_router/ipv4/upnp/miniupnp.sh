#!/bin/sh

WAN_IFACE=eth2 ####pppoe-wan or eth2
#/etc/init.d/miniupnpd stop
killall miniupnpd
sleep 3

uci show upnpd
uci set upnpd.config=upnpd
uci set upnpd.config.enabled=1
uci set upnpd.config.secure_mode=1
uci set upnpd.config.log_output=1
uci set upnpd.config.download=1024
uci set upnpd.config.upload=512
uci set upnpd.config.external_iface=wan
uci set upnpd.config.internal_iface=lan

uci commit upnpd

cd /lib
ln -sf libuClibc-0.9.30.1.so libc.so.6
/etc/init.d/miniupnpd start

if [ $WAN_IFACE == "eth2" ]; then

echo "wan mode is static"
iptables -t nat -N MINIUPNPD
iptables -t nat -A PREROUTING -i eth2 -j MINIUPNPD
iptables -N MINIUPNPD      
iptables -A FORWARD -i eth2 -j MINIUPNPD   
else
echo "wan mode is pppoe"
iptables -t nat -N MINIUPNPD     
iptables -t nat -A PREROUTING -i eth2 -j MINIUPNPD
iptables -N MINIUPNPD                             
iptables -A FORWARD -i pppoe-wan -j MINIUPNPD 

fi

                                              
