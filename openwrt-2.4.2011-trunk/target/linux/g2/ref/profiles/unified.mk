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
kmod-atheros-source \
kmod-RT3592AP \
kmod-RT3593AP \
kmod-RT3593AP_CS \
kmod-RT5392AP \
kmod-rtl8192ce \
kmod-rtl83xx \
kmod-g2-mtd-tests=m \
kmod-g2-vfs-fastpath \
kmod-g2-smb-tuning \
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

define Profile/unified
  NAME:=unified
  PACKAGES:=\
$(CRYPTO) \
$(IPTABLES) \
$(G2_DRIVERS) \
$(LUCI) \
6in4 \
6to4 \
bridge \
coremark \
ddns-scripts \
dosfsck \
dosfslabel \
dump_sadb \
dvdrwtool \
e2fsprogs \
ethtool \
ex-ipupdate \
fdisk \
firewall \
hostapd \
hostapd-mini \
hostapd-utils \
igmpproxy \
ip \
iperf \
iperf-mt \
ipsec-tools \
iwinfo \
kernel_test \
kmod-ledtrig-usbdev \
kmod-ip6-tunnel \
kmod-iptunnel6 \
kmod-pppoe \
kmod-usb-ohci \
ldconfig \
ldd \
libevent \
losetup \
lprng \
media-server-cgi \
minidlna \
miniupnpd \
mkdosfs \
mount-utils \
MPlayer_New \
mtd-utils \
netengine_config \
ntfs-3g \
ntfsprogs \
ntpclient \
ppp \
ppp-mod-pppoe \
pptp \
proftpd \
racoon2 \
radvd \
rboot \
rp-pppoe-client \
samba35 \
switch_config \
sysstat \
tc \
tcpdump-mini \
tgt \
tr069 \
uci-precommit \
ucitrigger \
udev \
uhttpd \
wide-dhcpv6-client \
wide-dhcpv6-control \
wide-dhcpv6-relay \
wide-dhcpv6-server \
wireless-tools

endef

define Profile/unified/Description
	This profile supports hardware forwarding configuration
endef

$(eval $(call Profile,unified))
