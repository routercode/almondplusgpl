G2_DRIVERS:=\
kmod-RT5392AP=m \
kmod-crypto-test=m \
kmod-g2-crypto-spacc=m \
kmod-g2-mtd-tests=m \
kmod-g2-ts=m \
kmod-g2-usb-dwc=m \
kmod-g2-zarlink=m 

define Profile/kernel_release
  NAME:=kernel_release
  PACKAGES:=\
6in4 \
6scripts \
6to4 \
6tunnel \
bridge \
classpath \
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
hwclock \
igmpproxy \
ip \
ip6tables \
iperf-mt \
iperf \
iptables-mod-conntrack-extra \
iptables-mod-filter \
iptables-mod-ipopt \
jamvm \
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
osgi-equinox \
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

define Profile/kernel_release/Description
	This profile is only for kernel team release used 
	throughout development. It's a little bit differnt  
	than default profile.
endef

$(eval $(call Profile,kernel_release))
