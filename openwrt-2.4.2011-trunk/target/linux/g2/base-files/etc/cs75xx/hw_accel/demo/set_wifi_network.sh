#!/bin/sh
WFO_Enable=$1
pci0=`sh /etc/cs75xx/hw_accel/demo/get_pci_dev.sh | awk '{print $1}'`
pci1=`sh /etc/cs75xx/hw_accel/demo/get_pci_dev.sh | awk '{print $2}'`

if [ $pci0 -eq 3 ] || [ $pci1 -eq 3 ]; then

if [ -f /usr/lib/lua/luci/model/cbi/rt3593.lua ]; then
echo " RT3593 WebGUI ready, please configure WiFi via GUI "

else
#Remove RT3593 CGI
rm -rf /lib/wifi/rt3593.sh

ifconfig ra0 up
ifconfig ra1 up
sleep 2
ifconfig ra0 down
ifconfig ra1 down
sleep 2
cd /rboot/wfo_ralink
if [ "$WFO_Enable" == "0" ]; then
	rmmod rt3593ap_wfo
	sleep 2
	insmod /lib/modules/2.6.36/rt3593ap_cs.ko
	echo 0 > /proc/driver/cs752x/wfo/wifi_offload_enable 
else
	/rboot/rboot wfo_pe0.bin wfo_pe1.bin
	rmmod rt3593ap_cs
	sleep 2
	insmod /lib/modules/2.6.36/rt3593ap_wfo.ko
	echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable
fi


cp RT2860AP-5G.dat /etc/Wireless/RT2860AP/RT2860AP.dat
ifconfig ra0 up
cp RT2860AP-2.4G.dat /etc/Wireless/RT2860AP/RT2860AP.dat
ifconfig ra1 up
brctl addif br-lan ra0
brctl addif br-lan ra1
fi
exit 0
fi

if [ \( $pci0 -eq 0 -a $pci1 -eq 1 \) -o \( $pci0 -eq 1 -a $pci1 -eq 0 \) ]; then
     ##Atheros DBDC
     cd /rboot/wfo_atheros_11AC
     if [ "$WFO_Enable" == "1" ]; then
       ./wfo_start_dbdc.sh  
     elif [ "$WFO_Enable" == "14" ] || [ "$WFO_Enable" == "17" ]; then
       /rboot/rboot ar988x_pe0.bin /rboot/ipsec/ipsec_pe1.bin
       #/rboot/rboot ar988x_pe0.bin 
       sleep 2
       echo "enabling 11AC WFO..."
       echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable
       sh ath_dbdc_ap.sh 2
     elif [ "$WFO_Enable" == "16" ]; then
       /rboot/rboot ar988x_pe0.bin
       sleep 2
       echo "enabling 11AC WFO..."
       echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable
       sh ath_dbdc_ap.sh 2
     else
       ./ath_dbdc_ap.sh  
     fi
     exit 0
fi

if [ $pci0 -eq 0 ] || [ $pci1 -eq 0 ]; then
     ##Atheros  11ac
     cd /rboot/wfo_atheros_11AC
     if [ "$WFO_Enable" == "1" ]; then
       ./wfo_start.sh  
     elif [ "$WFO_Enable" == "14" ]; then
       /rboot/rboot ar988x_pe0.bin /rboot/ipsec/ipsec_pe1.bin
       #/rboot/rboot ar988x_pe0.bin

       sleep 2
       echo "enabling 11AC WFO..."
       echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable
       sh ath_11ac_ap.sh 2
     else
       ./ath_11ac_ap.sh
        
     fi     
     exit 0
fi

if [ $pci0 -eq 1 ] || [ $pci1 -eq 1 ]; then
     ##Atheros  11n
     cd /rboot/wfo_atheros_11AC
     if [ "$WFO_Enable" == "1" ]; then
       	/rboot/rboot ar988x_pe0.bin ar9580_pe1.bin 
	sleep 2
	echo "enabling 11n WFO..."
	echo 1 > /proc/driver/cs752x/wfo/wifi_offload_enable 
	./ath_11n_ap.sh 1
     else
       ./ath_11n_ap.sh
     fi     
     exit 0
fi
