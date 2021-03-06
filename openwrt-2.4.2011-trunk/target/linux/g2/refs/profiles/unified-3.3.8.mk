G2_DRIVERS:=\
kmod-g2-ahci \
kmod-g2-gpio \
kmod-g2-hw-accel-ni \
kmod-g2-i2c \
kmod-g2-ipc \
kmod-g2-mtd-tests=m \
kmod-g2-ni \
kmod-g2-pcie \
kmod-g2-reg-rw \
kmod-g2-rtc \
kmod-g2-serial \
kmod-g2-spi \
kmod-g2-ssp \
kmod-g2-usb \
kmod-g2-watchdog \
kmod-input-gpio-buttons \
kmod-input-gpio-keys \
kmod-nls-utf8 \
kmod-ntfs \
kmod-raid-support \
kmod-usb-printer

CRYPTO:=\
kmod-crypto-aes \
kmod-crypto-authenc \
kmod-crypto-core \
kmod-crypto-des \
kmod-crypto-hmac \
kmod-crypto-loop \
kmod-crypto-md5 \
kmod-crypto-misc \
kmod-crypto-null \
kmod-crypto-ocf \
kmod-crypto-sha1 \
kmod-crypto-test \
kmod-crypto-test=m \
kmod-g2-crypto-spacc \
kmod-ipsec \
kmod-ipsec4 \
kmod-ipsec6 \
kmod-loop \
kmod-soft-crypto

IPTABLES:=\
ip6tables \
ip6tables-utils \
iptables \
iptables-mod-conntrack-extra \
iptables-mod-extra \
iptables-mod-filter \
iptables-mod-hashlimit \
iptables-mod-ipopt \
iptables-mod-iprange \
iptables-mod-ipsec \
iptables-mod-nat-extra \
iptables-mod-ulog \
kmod-arptables=m \
kmod-ipt-nathelper \
kmod-ipt-nathelper-extra=m \
kmod-nfnetlink-log=m \
kmod-nfnetlink=m \
kmod-nfnetlink-queue=m

LUCI:=\
luci-full \
luci-app-algapp \
luci-app-davfs2 \
luci-app-ddns-simple \
luci-app-dhcp-simple \
luci-app-firewall-simple \
luci-app-ftpsamba \
luci-app-g2qos \
luci-app-igmpproxy \
luci-app-minidlna \
luci-app-miniupnpd \
luci-app-ntpc \
luci-app-radvd \
luci-app-transmission \
luci-app-vpn \
luci-app-webdav \
luci-proto-pptp \
luci-theme-cortina 


define Profile/unified-3.3.8
  NAME:=unified-3.3.8
  PACKAGES:=\
$(CRYPTO) \
$(IPTABLES) \
$(G2_DRIVERS) \
$(LUCI) \
6in4 \
6rd \
6to4 \
blkid \
block-hotplug \
bridge \
cdrominfo \
coremark \
cortina-customizations \
cs-tr069-be \
ddns-scripts \
dosfsck \
dosfslabel \
dump_sadb \
dvdrwtool \
e2fsprogs \
ebtables \
ethtool \
ex-ipupdate \
fdisk \
firewall \
fuse-exfat \
g2_diags \
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
kmod-ip6-tunnel \
kmod-iptunnel6 \
kmod-ledtrig-usbdev \
kmod-openswan \
kmod-pppoe \
kmod-usb-ohci \
ldconfig \
ldd \
libevent \
losetup \
mdadm \
media-server-cgi \
minidlna \
miniupnpd \
mkdosfs \
mount-utils \
MPlayer_New \
mtd-utils \
NeoRaider-mrd6 \
netengine_config \
ntpclient \
ocf-crypto-headers \
openssl-util \
openswan \
p910nd \
pe_ralink \
ppp \
ppp-mod-pppoe \
pptp \
proftpd \
racoon2 \
radvd \
rboot \
rp-pppoe-client \
rp-pppoe-server \
samba35 \
switch_config \
sysstat \
tc \
tcpdump \
tcpdump-mini \
uboot-envtools \
uci-precommit \
ucitrigger \
udev \
uhttpd \
wide-dhcpv6-client \
wide-dhcpv6-control \
wide-dhcpv6-relay \
wide-dhcpv6-server \
wireless-tools \
xl2tpd

endef

define Profile/unified-3.3.8/Description
	This profile is used for GA in kernel 3.3.8
endef

$(eval $(call Profile,unified-3.3.8))
