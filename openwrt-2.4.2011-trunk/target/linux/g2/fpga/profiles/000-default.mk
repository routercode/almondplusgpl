G2_DRIVERS:=\
kmod-g2-watchdog \
kmod-g2-gpio \
kmod-g2-serial \
kmod-g2-spi \
kmod-g2-pcie \
kmod-g2-usb \
kmod-g2-zarlink \
kmod-g2-mtd-parallel \
kmod-g2-reg-rw \
kmod-g2-ahci \
kmod-g2-i2c \
kmod-g2-rtc \
kmod-g2-ni \
kmod-g2-switch-rt \
kmod-g2-debug-proc \
kmod-g2-crypto-spacc \
kmod-g2-kernel-debug \
kmod-RT5392AP=m \
kmod-crypto-test=m \
kmod-g2-crypto-spacc=m \
kmod-g2-mtd-tests=m \
kmod-g2-ts=m \
kmod-g2-usb-dwc=m

define Profile/default
  NAME:=default
  PACKAGES:=\
6in4 \
6scripts \
6to4 \
6tunnel \
bridge \
cortina-ocs-tr069 \
ddns-scripts \
dosfsck \
dosfslabel \
dvdrwtool \
e2fsprogs \
ethtool \
fdisk \
hostapd-mini \
hostapd-utils \
hostapd \
igmpproxy \
ip \
ip6tables \
iperf-mt \
iperf \
iptables-mod-conntrack-extra \
iptables-mod-filter \
iptables-mod-ipopt \
kmod-ip6-tunnel \
kmod-ipsec \
kmod-ipsec4 \
kmod-ipsec6 \
kmod-ipt-nathelper \
kmod-iptunnel6 \
kmod-pppoe \
$(G2_DRIVERS) \
ldconfig \
ldd \
losetup \
lua \
luci \
luci-app-firewall \
luci-app-initmgr \
luci-app-ntpc \
luci-app-openvpn \
luci-app-radvd \
luci-app-samba \
luci-app-tr069 \
luci-app-upnp \
luci-mod-admin-core \
luci-mod-admin-full \
luci-mod-rpc \
luci-sgi-cgi \
luci-theme-base \
luci-theme-openwrt \
mkdosfs \
miniupnpd \
mount-utils \
ntpclient \
openvpn \
openvpn_complzo \
ppp-mod-pppoe \
pptp \
proftpd \
quagga \
quagga-libzebra \
racoon2 \
radvd \
rp-pppoe-client \
rp-pppoe-relay \
rp-pppoe-sniff \
samba3 \
tc \
tcpdump-mini \
tcpdump \
uci-precommit \
ucitrigger \
udev \
uhttpd \
wide-dhcpv6-client \
wide-dhcpv6-control \
wide-dhcpv6-relay \
wide-dhcpv6-server \
wireless-tools \
zile

endef

define Profile/default/Description
	This profile represents the old default.config used 
	throughout development.  Almost all packages and features are
	enabled.  
endef

$(eval $(call Profile,default))
