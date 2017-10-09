G2_DRIVERS:=\
kmod-atheros-11AC-10.1.389 \
kmod-atheros-11AC-10.1.389-wfo \
kmod-atheros-11AC-10.1.389-wfo-11ac \
kmod-atheros-11AC-10.1.389-wfo-11n \
kmod-broadcom-11AC \
kmod-g2-ahci \
kmod-g2-gpio \
kmod-g2-hw-accel-ni \
kmod-g2-i2c \
kmod-g2-ipc \
kmod-g2-ir=m \
kmod-g2-mtd-tests=m \
kmod-g2-ni \
kmod-g2-pcie \
kmod-g2-reg-rw \
kmod-g2-rtc \
kmod-g2-sd \
kmod-g2-serial \
kmod-g2-smb-tuning \
kmod-g2-snd-evm \
kmod-g2-spi \
kmod-g2-ts=m \
kmod-g2-usb \
kmod-g2-usb-dwc=m \
kmod-g2-vfs-fastpath \
kmod-g2-watchdog \
kmod-g2-wfo-core \
kmod-g2-wfo-rt3593 \
kmod-input-gpio-buttons \
kmod-input-gpio-keys \
kmod-mxl241sf-source \
kmod-nls-utf8 \
kmod-ntfs \
kmod-powermanage \
kmod-RT3593AP \
kmod-RT3593AP_CS \
kmod-RT3593AP_WFO \
kmod-RT5392AP \
kmod-raid-support \
kmod-rtl8192ce \
kmod-rtl83xx \
kmod-usb2 \
kmod-usb3 \
kmod-usb-gadget=m \
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
luci-app-ar988x \
luci-app-transmission \
luci-app-vpn \
luci-app-webdav \
luci-proto-pptp \
luci-theme-cortina 


define Profile/unified
  NAME:=unified
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
linuxtv-dvb-apps \
losetup \
mdadm \
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
pciutils \
pe_ralink \
pe_ipsec_ath_wifi \
pe_ipsec_d \
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
vlc2 \
wfo-ipc-test \
wide-dhcpv6-client \
wide-dhcpv6-control \
wide-dhcpv6-relay \
wide-dhcpv6-server \
wireless-tools \
xl2tpd

endef

define Profile/unified/Description
	This profile is used for GA
endef

$(eval $(call Profile,unified))
