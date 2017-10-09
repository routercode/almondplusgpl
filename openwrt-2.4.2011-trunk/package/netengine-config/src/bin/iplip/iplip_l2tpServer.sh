#!/bin/sh
# Setup xl2tpd config file
echo "[global]" > /etc/xl2tpd/xl2tpd.conf
echo "ipsec saref = no" >> /etc/xl2tpd/xl2tpd.conf
echo "[lns default]" >> /etc/xl2tpd/xl2tpd.conf
echo "ip range = 100.0.0.100-100.0.0.200" >> /etc/xl2tpd/xl2tpd.conf
echo "local ip = 100.0.0.10" >> /etc/xl2tpd/xl2tpd.conf
echo "require chap = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "require authentication = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "ppp debug = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "pppoptfile =/etc/ppp/options.xl2tpd" >> /etc/xl2tpd/xl2tpd.conf
echo "length bit = yes" >> /etc/xl2tpd/xl2tpd.conf

# Create /etc/ppp/options.xl2tpd
echo "require-chap" > /etc/ppp/options.xl2tpd
echo "ms-dns 172.17.92.188" >> /etc/ppp/options.xl2tpd
echo "lcp-echo-interval 10" >> /etc/ppp/options.xl2tpd
echo "lcp-echo-failure 3" >> /etc/ppp/options.xl2tpd
echo "auth" >> /etc/ppp/options.xl2tpd
echo "idle 1800" >> /etc/ppp/options.xl2tpd
echo "mtu 1410" >> /etc/ppp/options.xl2tpd
echo "mru 1410" >> /etc/ppp/options.xl2tpd
echo "lock" >> /etc/ppp/options.xl2tpd
echo "connect-delay 5000" >> /etc/ppp/options.xl2tpd
echo "+ipv6" >> /etc/ppp/options.xl2tpd
echo "ipv6 ::1,::2" >> /etc/ppp/options.xl2tpd
echo "ipv6cp-accept-local" >> /etc/ppp/options.xl2tpd


# Restart xl2tpd
chmod 777 /etc/xl2tpd/start_xl2tpd.sh
killall xl2tpd

/etc/xl2tpd/start_xl2tpd.sh
