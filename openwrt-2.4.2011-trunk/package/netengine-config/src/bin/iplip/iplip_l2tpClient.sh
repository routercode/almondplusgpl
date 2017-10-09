#!/bin/sh
# Create /etc/xl2tpd/xl2tpd.conf

echo "[lac iplip]" > /etc/xl2tpd/xl2tpd.conf
echo "lns = 220.133.199.1" >> /etc/xl2tpd/xl2tpd.conf
echo "ppp debug = yes" >> /etc/xl2tpd/xl2tpd.conf
echo "pppoptfile = /etc/ppp/options.l2tpd.client" >> /etc/xl2tpd/xl2tpd.conf
echo "length bit = yes" >> /etc/xl2tpd/xl2tpd.conf

# Setup /etc/ppp/options.l2tpd.client
echo "ipcp-accept-local" > /etc/ppp/options.l2tpd.client
echo "ipcp-accept-remote" >> /etc/ppp/options.l2tpd.client
echo "refuse-eap" >> /etc/ppp/options.l2tpd.client
echo "require-mschap-v2" >> /etc/ppp/options.l2tpd.client
echo "noccp" >> /etc/ppp/options.l2tpd.client
echo "noauth" >> /etc/ppp/options.l2tpd.client
echo "idle 1800" >> /etc/ppp/options.l2tpd.client
echo "mtu 1410" >> /etc/ppp/options.l2tpd.client
echo "mru 1410" >> /etc/ppp/options.l2tpd.client
echo "defaultroute" >> /etc/ppp/options.l2tpd.client
echo "replacedefaultroute" >> /etc/ppp/options.l2tpd.client
echo "usepeerdns" >> /etc/ppp/options.l2tpd.client
echo "debug" >> /etc/ppp/options.l2tpd.client
echo "lock" >> /etc/ppp/options.l2tpd.client
echo "connect-delay 5000" >> /etc/ppp/options.l2tpd.client
echo "name cortina" >> /etc/ppp/options.l2tpd.client
echo "password cortina" >> /etc/ppp/options.l2tpd.client

# Invoke xl2tpd
chmod 777 /etc/xl2tpd/start_xl2tpd.sh
killall xl2tpd

/etc/xl2tpd/start_xl2tpd.sh
