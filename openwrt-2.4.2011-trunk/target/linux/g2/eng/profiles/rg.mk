G2_DRIVERS:=\
kmod-g2-watchdog \
kmod-g2-gpio \
kmod-input-gpio-buttons \
kmod-input-gpio-keys \
kmod-g2-serial \
kmod-g2-spi \
kmod-g2-pcie \
kmod-g2-usb \
kmod-g2-ahci \
kmod-g2-i2c \
kmod-g2-rtc \
kmod-g2-ni \
kmod-g2-spdif \
kmod-g2-debug-proc \
kmod-g2-crypto-spacc \
kmod-g2-kernel-debug \
kmod-RT5392AP

LUCI_WIFI_GUI:=\
kmod-mac80211 \
libiwinfo \
libnl-tiny

G2_DRIVERS += $(LUCI_WIFI_GUI)

IPTABLES:=\
iptables-mod-extra \
iptables-mod-conntrack-extra \
iptables-mod-filter \
iptables-mod-ipopt \
iptables-mod-hashlimit \
iptables-mod-iprange \
iptables-mod-nat-extra \
iptables-mod-ulog \
iptables-mod-ipsec \
kmod-arptables=m \
kmod-ipt-nathelper \
kmod-ipt-nathelper-extra=m \
kmod-nfnetlink=m \
kmod-nfnetlink-log=m \
kmod-nfnetlink-queue=m

LUCI:=\
luci-full

define Profile/rg
  NAME:=router gateway
  PACKAGES:=\
$(LUCI) \
iptables \
ppp \
firewall \
bridge \
ddns-scripts \
dosfsck \
dosfslabel \
e2fsprogs \
ethtool \
fdisk \
hostapd-mini \
hostapd-utils \
hostapd \
ip \
$(IPTABLES) \
iperf-mt \
iperf \
kmod-ipsec \
kmod-ipsec4 \
$(G2_DRIVERS) \
kernel_test \
ldconfig \
ldd \
losetup \
mkdosfs \
mount-utils \
mtd-utils \
ntpclient \
openvpn \
openvpn_complzo \
psplash \
tc \
tcpdump-mini \
tcpdump \
uci-precommit \
ucitrigger \
udev \
uhttpd \
wireless-tools \
zile \
libevent \
strace

endef

define Profile/rg/Description
  This is a fairly minimal IPv4 r/g profile.
endef

$(eval $(call Profile,rg))
