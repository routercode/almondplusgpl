G2_DRIVERS:=\
kmod-g2-watchdog \
kmod-g2-gpio \
kmod-g2-serial \
kmod-g2-spi \
kmod-g2-pcie \
kmod-g2-usb \
kmod-g2-mtd-parallel \
kmod-g2-ahci \
kmod-g2-i2c \
kmod-g2-rtc \
kmod-g2-ni \
kmod-g2-spdif \
kmod-g2-kernel-debug \
kmod-RT5392AP

IPTABLES:=\
iptables-mod-extra \
iptables-mod-conntrack-extra \
iptables-mod-filter \
iptables-mod-ipopt \
iptables-mod-iprange \
iptables-mod-nat-extra \
iptables-mod-ulog \
iptables-mod-ipsec \
kmod-arptables=m \
kmod-ipt-nathelper-extra=m \
kmod-nfnetlink=m \
kmod-nfnetlink-log=m \
kmod-nfnetlink-queue=m

define Profile/default
  NAME:=default
  PACKAGES:=\
ddns-scripts \
dosfsck \
dosfslabel \
e2fsprogs \
ethtool \
fdisk \
hostapd-mini \
hostapd-utils \
hostapd \
igmpproxy \
ip \
$(IPTABLES) \
kmod-ipsec \
kmod-ipsec4 \
kmod-pppoe \
kmod-rtl83xx \
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
luci-app-upnp \
luci-mod-admin-core \
luci-mod-admin-full \
luci-mod-rpc \
luci-sgi-cgi \
luci-theme-base \
luci-theme-openwrt \
mkdosfs \
mount-utils \
ntpclient \
openvpn \
openvpn_complzo \
ppp-mod-pppoe \
pptp \
proftpd \
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
wireless-tools \
zile

endef

define Profile/default/Description
  Default settings for an IPv4 RG.
endef

$(eval $(call Profile,default))
