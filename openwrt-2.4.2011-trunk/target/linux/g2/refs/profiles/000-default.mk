G2_DRIVERS:=\
kmod-g2-watchdog \
kmod-g2-gpio \
kmod-input-gpio-buttons \
kmod-input-gpio-keys \
kmod-g2-serial \
kmod-d2 \
kmod-g2-spi \
kmod-g2-ssp \
kmod-g2-pcie \
kmod-g2-reg-rw \
kmod-g2-ahci \
kmod-g2-i2c \
kmod-i2c-core \
kmod-g2-rtc \
kmod-g2-ni \
kmod-rtl8192cd \
kmod-rtl83xx \
kmod-g2-spdif \
kmod-g2-debug-proc \
kmod-g2-kernel-debug \
kmod-RT3593AP_CS \
kmod-g2-mtd-tests=m \
kmod-g2-ts=m \
kmod-g2-usb \
kmod-usb-ohci \
kmod-usb-hid \
kmod-usb-uvc \
kmod-usb-printer \
kmod-g2-vfs-fastpath \
kmod-usb-power-management

IPTABLES:=\
iptables \
ip6tables \
ip6tables-utils \
iptables-mod-extra \
iptables-mod-conntrack-extra \
iptables-mod-filter \
iptables-mod-ipopt \
iptables-mod-hashlimit \
iptables-mod-iprange \
kmod-ipt-nathelper \
iptables-mod-nat-extra \
iptables-mod-ulog \
iptables-mod-ipsec \
kmod-arptables=m \
kmod-ipt-nathelper-extra=m \
kmod-nfnetlink=m \
kmod-nfnetlink-log=m \
kmod-nfnetlink-queue=m

CRYPTO:=\
kmod-g2-crypto-spacc \
kmod-crypto-test=m \
kmod-crypto-core \
kmod-crypto-aes \
kmod-crypto-authenc \
kmod-crypto-des \
kmod-crypto-hmac \
kmod-crypto-md5 \
kmod-crypto-null \
kmod-crypto-sha1 \
kmod-crypto-test \
kmod-ipsec \
kmod-ipsec4 \
kmod-ipsec6

LUCI:=\
luci-full

define Profile/default
  NAME:=default
  PACKAGES:=\
$(CRYPTO) \
$(IPTABLES) \
$(G2_DRIVERS) \
$(LUCI) \
ppp \
firewall \
6in4 \
6to4 \
bridge \
tr069 \
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
ip \
rboot \
iperf-mt \
iperf \
kmod-ip6-tunnel \
kmod-iptunnel6 \
kmod-ledtrig-usbdev \
kernel_test \
ldconfig \
ldd \
losetup \
mkdosfs \
miniupnpd \
mount-utils \
mtd-utils \
ntpclient \
openvpn \
openvpn_complzo \
pptp \
proftpd \
psplash \
quagga \
quagga-libzebra \
racoon2 \
ipsec-tools \
radvd \
samba35 \
switch_config \
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
libevent \
libffmpeg \
ffmpeg \
libpostproc \
libswscale \
live555 \
internet-video-demo \
zile

endef

define Profile/default/Description
	This profile represents the old default.config used 
	throughout development.  Almost all packages and features are
	enabled.  
endef

$(eval $(call Profile,default))
