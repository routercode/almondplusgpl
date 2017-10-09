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
kmod-rtl83xx \
kmod-g2-spi \
kmod-g2-ssp \
kmod-rtl8192ce \
kmod-RT3593AP_CS \
kmod-RT3593AP \
kmod-RT5392AP \
kmod-rtl83xx \
kmod-g2-mtd-tests=m \
kmod-g2-vfs-fastpath \
kmod-g2-smb-tuning \
kmod-g2-watchdog 

CRYPTO:=\
kmod-crypto-aes \
kmod-crypto-authenc \
kmod-crypto-core \
kmod-crypto-des \
kmod-crypto-hmac \
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
kmod-crypto-loop \
kmod-soft-crypto


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
kmod-ipt-nathelper-extra=m \
kmod-nfnetlink=m \
kmod-nfnetlink-log=m \
kmod-nfnetlink-queue=m

LUCI=:\
luci-light

define Profile/nas
  NAME:=nas
  PACKAGES:=\
$(CRYPTO) \
$(G2_DRIVERS) \
$(LUCI) \
ntfs-3g \
ntfsprogs \
minidlna \
bridge \
dosfsck \
dosfslabel \
e2fsprogs \
ethtool \
fdisk \
hostapd-mini \
hostapd-utils \
hostapd \
ip \
kmod-usb-ohci \
ldconfig \
ldd \
losetup \
mkdosfs \
mount-utils \
mtd-utils \
ocf-crypto-headers \
openssl-util \
proftpd \
tgt \
samba35 \
switch_config \
rp-pppoe-client \
igmpproxy \
tcpdump-mini \
tcpdump \
uci-precommit \
ucitrigger \
udev \
uhttpd \
libevent \
MPlayer_New \
wireless-tools

endef

define Profile/nas/Description
	This profile is a nas configuration
endef

$(eval $(call Profile,nas))
