#!/bin/sh
# Create /etc/radvd.conf
echo "interface ppp1" > /etc/radvd.conf
echo "" >> /etc/radvd.conf
echo "{" >> /etc/radvd.conf
echo "AdvSendAdvert on;" >> /etc/radvd.conf
echo "MinRtrAdvInterval 5;" >> /etc/radvd.conf
echo "MaxRtrAdvInterval 100;" >> /etc/radvd.conf
echo "AdvManagedFlag off;" >> /etc/radvd.conf
echo "AdvOtherConfigFlag off;" >> /etc/radvd.conf
echo "prefix 2013::/64" >> /etc/radvd.conf
echo "{" >> /etc/radvd.conf
echo "AdvOnLink on;" >> /etc/radvd.conf
echo "AdvAutonomous on;" >> /etc/radvd.conf
echo "};" >> /etc/radvd.conf
echo "};" >> /etc/radvd.conf

# Configure L2TP interface (ppp1)
ifconfig ppp1 add 2013::1/64

killall radvd
sleep 2 
radvd -C /etc/radvd.conf -m stderr_syslog -p /var/run/radvd.pid

ip -6 route add ::/0 dev ppp1
ip6tables -P FORWARD ACCEPT
