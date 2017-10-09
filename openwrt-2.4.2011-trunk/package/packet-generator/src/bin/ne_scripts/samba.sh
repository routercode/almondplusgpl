#!/bin/sh

### For SW NAT test

if [ ! -z $1 ] && [ $1 == "all" ]; then
	KILLALL=1
else
	KILLALL=0
fi


# Remove bridge interface
ifconfig br-lan down
brctl delbr br-lan

ifconfig eth0 192.168.10.1	# WAN
ifconfig eth1 192.168.1.1	# LAN

echo 0 > /proc/driver/cs752x/ne/ni/ni_use_sendfile

# Remove all processes that influence throughput.
/etc/rc.d/S42d2 stop
/etc/d2/d2.remove
/etc/rc.d/S50mrd6 stop
/etc/rc.d/S60ipsec stop
/etc/init.d/samba stop

if [ ${KILLALL} -ne 0 ]; then
	killall5
	killall -9 minidlna
	killall -9 dnsmasq
fi

# These IRQs use CPU core 0
echo 1 > /proc/irq/64/smp_affinity	# AHCI
echo 1 > /proc/irq/70/smp_affinity	# eth1
echo 1 > /proc/irq/76/smp_affinity	# ARP


# Let Samba use CPU core 1
/etc/init.d/samba start
ID=$(cat /var/log/samba/smbd.pid)
taskset -p 2 $ID
ID=$(cat /var/log/samba/nmbd.pid)
taskset -p 2 $ID

echo "*** Make sure the test user 'samba' is in /etc/passwd & /etc/samba/smbpasswd ***"
echo "Commands to add user:"
echo "1. adduser samba"
echo "2. smbpasswd -a samba"
echo ""
echo "*** Don't forat to mount HDD to /mnt ***"
echo "Command to mount:"
echo "- mount /dev/sda1 /mnt"
echo ""

exit 0

