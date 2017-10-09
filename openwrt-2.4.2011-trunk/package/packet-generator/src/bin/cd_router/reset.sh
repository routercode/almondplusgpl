llall pppoe-server
echo "stop pppoe-server"
#/etc/init.d/pppoe-server stop
#echo "stop quagga"
#/etc/init.d/quagga stop
#echo "samba" 
#/etc/init.d/samba stop
HW_NET=1 ## 1--> enable ; 0--> disable 
killall pppd
#killall udhcpc
#cp network /etc/config/network
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth1 up
ifconfig eth1 192.168.1.1
ifconfig eth2 down
ifconfig eth2 up
ifconfig eth2 0.0.0.0
#ifconfig eth1 hw ether 00:50:c2:01:01:01
#ifconfig eth2 hw ether 00:50:c2:02:02:02
#ip route
#ip link

if [ $HW_NET = 0 ] ; then

ne_cfg -m fe -t Classifier -s -c get 0 0 -b 1
ne_cfg -m fe -t Classifier -e
echo "HW NAT disable"
else 

#ne_cfg -m fe -t Classifier -s -c get 0 0 -b 0
#ne_cfg -m fe -t Classifier -e
echo "HW NAT enable"
fi
echo "wait 3 sec"
sleep 3



