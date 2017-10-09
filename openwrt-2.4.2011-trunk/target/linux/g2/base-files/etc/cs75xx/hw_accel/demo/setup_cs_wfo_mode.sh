#!/bin/sh
SCRIPT_DIR=/etc/cs75xx/hw_accel/demo

WFO_MODE_ID=$1

echo "WFO_MODE_ID = $WFO_MODE_ID"
#if  [ "$WFO_MODE_ID" == "0" ]; then
#	echo "	WFO(Bridge Mode):	Disable"
#		echo "		load rt3593ap_wfo.ko / wfo_rl_pe0.bin / wfo_rl_pe1.bin"
#	echo "	IPSEC(HW ENCP/DECP): 	N/A"
#	echo "	L2TP: 			N/A"
#	echo "	NAT(HW): 		Disable"
#	echo "	hw_accel_enable: 	0xFFFF"
if  [ "$WFO_MODE_ID" == "1" ]; then
	echo "	WFO(Bridge Mode):	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Enable"
	echo "	hw_accel_enable: 	0xf0ff"
	sleep 1
	echo 0xf0ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
elif  [ "$WFO_MODE_ID" == "2" ]; then
	echo "	WFO(Bridge Mode):	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			Enable"
	echo "		load openswan / cs75xx_spacc.ko"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x0000"
	sleep 1
	echo 0x0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        if [ "$(uname -r)" == "2.6.36" ]; then
        insmod /lib/modules/2.6.36/cs75xx_spacc.ko
        elif [ "$(uname -r)" == "3.4.11" ]; then
        insmod /lib/modules/3.4.11/cs75xx_spacc.ko
        fi
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
	$SCRIPT_DIR/set_openswan_application.sh
elif  [ "$WFO_MODE_ID" == "3" ]; then
	echo "	WFO(Bridge Mode):	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x0000"
	sleep 1
	echo 0x0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
elif  [ "$WFO_MODE_ID" == "4" ]; then
	echo "	WFO(Bridge Mode):	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	Enable"
	echo "		load racoon2 / cs75xx_spacc.ko"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0xf0ff"
	sleep 1
	echo 0x71ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        if [ "$(uname -r)" == "2.6.36" ]; then
        insmod /lib/modules/2.6.36/cs75xx_spacc.ko
        elif [ "$(uname -r)" == "3.4.11" ]; then
        insmod /lib/modules/3.4.11/cs75xx_spacc.ko
        fi
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	echo 0 > /proc/driver/cs752x/rt3593/noncacheable_buffer
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
	$SCRIPT_DIR/set_ipsec_network.sh 1	# IPSEC HW Acceleration: 0
elif [ "$WFO_MODE_ID" == "5" ]; then
	echo "	WFO(Bridge Mode): 	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	Disable"
	echo "		load racoon2 / cs75xx_spacc.ko"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x0000"
	sleep 1
	echo 0x0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        if [ "$(uname -r)" == "2.6.36" ]; then
        insmod /lib/modules/2.6.36/cs75xx_spacc.ko
        elif [ "$(uname -r)" == "3.4.11" ]; then
        insmod /lib/modules/3.4.11/cs75xx_spacc.ko
        fi
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	echo 0 > /proc/driver/cs752x/rt3593/noncacheable_buffer
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
	$SCRIPT_DIR/set_ipsec_network.sh 0	# IPSEC HW Acceleration: 0
elif [ "$WFO_MODE_ID" == "6" ]; then
	echo "	WFO(Bridge Mode): 	Disable"
	echo "		load original wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x0000"
	sleep 1
	echo 0x0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 0		# WiFi-Offload: 0
elif [ "$WFO_MODE_ID" == "8" ]; then
	echo "	WFO(Bridge Mode): 	Enable"
	echo "		load wfo wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x8004"
	sleep 1
	echo 0x8004 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 1		# WiFi-Offload: 1
elif [ "$WFO_MODE_ID" == "9" ]; then
	echo "	WFO(Bridge Mode): 	Enable"
	echo "		load wfo wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Enable"
	echo "	hw_accel_enable: 	0x8024"
	sleep 1
	echo 0x8024 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 1		# WiFi-Offload: 1
elif [ "$WFO_MODE_ID" == "10" ]; then
	echo "	WFO(Bridge Mode): 	Enable"
		echo "		load wfo wifi driver"
	echo "	IPSEC(HW ENCP/DECP):	N/A"
	echo "	L2TP: 			Enable"
		echo "		load openswan / cs75xx_spacc.ko"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x8004"
	sleep 1
	echo 0x8004 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        if [ "$(uname -r)" == "2.6.36" ]; then
        insmod /lib/modules/2.6.36/cs75xx_spacc.ko
        elif [ "$(uname -r)" == "3.4.11" ]; then
        insmod /lib/modules/3.4.11/cs75xx_spacc.ko
        fi
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 1		# WiFi-Offload: 1
	$SCRIPT_DIR/set_openswan_application.sh
elif [ "$WFO_MODE_ID" == "12" ]; then
	echo "	WFO(Bridge Mode): 	Enable"
	echo "		load wfo wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	Disable"
	echo "		load racoon2 / cs75xx_spacc.ko"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x8004"
	sleep 1
	echo 0x8004 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        if [ "$(uname -r)" == "2.6.36" ]; then
        insmod /lib/modules/2.6.36/cs75xx_spacc.ko
        elif [ "$(uname -r)" == "3.4.11" ]; then
        insmod /lib/modules/3.4.11/cs75xx_spacc.ko
        fi
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 1		# WiFi-Offload: 1
	$SCRIPT_DIR/set_ipsec_network.sh 0	# IPSEC HW Acceleration: 0
elif [ "$WFO_MODE_ID" == "13" ]; then
	echo "	WFO(Bridge Mode): 	Enable"
	echo "		load wfo wifi driver"
	echo "	IPSEC(HW ENCP/DECP): 	N/A"
	echo "	L2TP: 			N/A"
	echo "	NAT(HW): 		Disable"
	echo "	hw_accel_enable: 	0x9024"
	sleep 1
	echo 0x9024 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
	sleep 1
	$SCRIPT_DIR/set_default_network.sh
	$SCRIPT_DIR/set_wifi_network.sh 1		# WiFi-Offload: 1
	$SCRIPT_DIR/set_pppoe_client.sh
elif [ "$WFO_MODE_ID" == "14" ]; then
        echo "  WFO(Bridge Mode):       Enable"
        echo "          load 11ac wfo and 11n A9 wifi"
        echo "  IPSEC(HW ENCP/DECP):    Enable at PE#1"
        echo "  L2TP:                   N/A"
        echo "  NAT(HW):                Enable"
        echo "  hw_accel_enable:        0xf1ff"
	echo "  ipsec_offload_mode:     3"
        sleep 1
        echo 3 > /proc/driver/cs752x/ne/accel_manager/ipsec_offload_mode
        echo 0xf1ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        sleep 1
        $SCRIPT_DIR/set_default_network.sh
        $SCRIPT_DIR/set_wifi_network.sh 14               # WiFi-Offload: 1
        $SCRIPT_DIR/set_ipsec_network.sh 0      # IPSEC HW Acceleration: 1 but PE image is loaded in wifi script
elif [ "$WFO_MODE_ID" == "16" ]; then
        echo "  WFO(Bridge Mode):       Enable"
        echo "          load 11ac wfo and 11n A9 wifi"
        echo "  IPSEC(HW ENCP/DECP):    N/A"
        echo "  L2TP:                   N/A"
        echo "  NAT(HW):                Enable"
        echo "  hw_accel_enable:        0xf0ff"
	echo "  ipsec_offload_mode:     3"
        sleep 1
        echo 0xf0ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        sleep 1
        $SCRIPT_DIR/set_default_network.sh
        $SCRIPT_DIR/set_wifi_network.sh 16               # WiFi-Offload: 1
elif [ "$WFO_MODE_ID" == "17" ]; then
        echo "  WFO(Bridge Mode):       Enable"
        echo "          load 11ac wfo and 11n A9 wifi"
        echo "  IPSEC(HW ENCP/DECP):    Enable at PE#1"
        echo "  L2TP:                   N/A"
        echo "  NAT(HW):                Enable"
        echo "  hw_accel_enable:        0xf1ff"
	echo "  ipsec_offload_mode:     3"
        sleep 1
        echo 3 > /proc/driver/cs752x/ne/accel_manager/ipsec_offload_mode
        echo 0xf1ff > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable
        sleep 1
        $SCRIPT_DIR/set_default_network.sh
        $SCRIPT_DIR/set_wifi_network.sh 17               # WiFi-Offload: 1
        $SCRIPT_DIR/set_ipsec_network.sh 0      # IPSEC HW Acceleration: 1 but PE image is loaded in wifi script

elif [ "$WFO_MODE_ID" == "-1" ] || ["$WFO_MODE_ID" == "-2" ] ; then
	echo "	Reserved mode."
else
	echo "Feature not supported($WFO_MODE_ID)"
fi
