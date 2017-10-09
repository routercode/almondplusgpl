#!/bin/sh

/etc/init.d/samba stop
# These IRQs use CPU core 0
echo 1 > /proc/irq/64/smp_affinity
echo 1 > /proc/irq/70/smp_affinity
echo 1 > /proc/irq/75/smp_affinity
echo 1 > /proc/irq/76/smp_affinity

# Let Samba use CPU core 1
/etc/init.d/samba start
ID=$(cat /var/log/samba/smbd.pid)
taskset -p 2 $ID
ID=$(cat /var/log/samba/nmbd.pid)
taskset -p 2 $ID

# Remove bridge interface
brctl delif br-lan eth1
ifconfig br-lan 0.0.0.0
ifconfig br-lan down
brctl delbr br-lan 
ifconfig eth1 192.168.1.1

# Disable Netfilter & NE H/W acceleration
echo 0 > /proc/sys/net/netfilter/nf_conntrack_generic_enabled
echo 0 > /proc/driver/cs752x/ne/accel_manager/hw_accel_enable

# Disable WFO
cd /etc/cs75xx/hw_accel/demo
cs_dp_mode_save -1
echo 0 >  /proc/driver/cs752x/wfo/wifi_offload_enable

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
