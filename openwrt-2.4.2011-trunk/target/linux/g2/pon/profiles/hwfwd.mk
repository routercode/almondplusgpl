G2_DRIVERS:=\
kmod-g2-gpio \
kmod-input-gpio-buttons \
kmod-input-gpio-keys \
kmod-g2-serial \
kmod-g2-pcie \
kmod-g2-usb \
kmod-g2-reg-rw \
kmod-g2-ahci \
kmod-g2-i2c \
kmod-g2-rtc \
kmod-g2-ni \
kmod-g2-hw-accel-ni \
kmod-g2-ipc \
kmod-g2-spi \
kmod-g2-ssp \
kmod-d2 \
kmod-RT3593AP \
kmod-rtl8192ce \
kmod-rtl83xx \
kmod-g2-mtd-tests=m \
kmod-g2-vfs-fastpath \
kmod-g2-watchdog

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

define Profile/hwfwd
  NAME:=hwfwd
  PACKAGES:=\
$(CRYPTO) \
$(IPTABLES) \
$(G2_DRIVERS) \
$(LUCI) \
lprng \
iwinfo \
coremark \
sysstat \
dump_sadb \
racoon2 \
ipsec-tools \
rboot \
ppp \
firewall \
6in4 \
6to4 \
bridge \
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
iperf-mt \
iperf \
kmod-pppoe \
kmod-ip6-tunnel \
kmod-iptunnel6 \
kmod-usb-ohci \
kmod-ledtrig-usbdev \
tr069 \
kernel_test \
ldconfig \
ldd \
losetup \
mkdosfs \
miniupnpd \
mount-utils \
mtd-utils \
ntpclient \
ppp-mod-pppoe \
pptp \
proftpd \
radvd \
rp-pppoe-client \
samba35 \
switch_config \
tc \
tcpdump-mini \
tcpdump \
uci-precommit \
ucitrigger \
udev \
libevent \
netengine_config \
wide-dhcpv6-client \
wide-dhcpv6-control \
wide-dhcpv6-relay \
wide-dhcpv6-server \
wireless-tools

endef

define Profile/hwfwd/Description
	This profile is a Hardware forwarding ipsec/nat configuration
endef

$(eval $(call Profile,hwfwd))
