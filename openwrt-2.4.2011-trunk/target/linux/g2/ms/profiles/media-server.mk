G2_DRIVERS:=\
kmod-g2-watchdog \
kmod-g2-gpio \
kmod-g2-serial \
kmod-g2-spi \
kmod-g2-pcie \
kmod-g2-usb \
kmod-g2-mtd-parallel \
kmod-g2-ahci \
kmod-g2-lcd \
kmod-g2-i2c \
kmod-g2-rtc \
kmod-g2-ni \
kmod-rtl83xx \
kmod-g2-spdif \
kmod-g2-debug-proc \
kmod-g2-kernel-debug \
kmod-RT5392AP

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


MYTHTV:=\
qt4 \
qt4-gui \
mythtv \
kmod-hauppauge-hvr1250=m \
kmod-hauppauge-hvr850=m 

define Profile/media-server
  NAME:=media-server
  PACKAGES:=\
iptables \
ppp \
firewall \
bridge \
ddns-scripts \
hostapd-mini \
hostapd-utils \
hostapd \
psplash \
wireless-tools \
e2fsprogs \
fdisk \
$(IPTABLES) \
$(G2_DRIVERS) \
$(MYTHTV) \
ldconfig \
ldd \
mount-utils \
ntpclient \
ucitrigger \
udev \
strace \
zile \
kmod-i2c-core \
kmod-i2c-algo-pca \
kmod-i2c-algo-pcf

endef

define Profile/media-server/Description
	Media Server, based on MythTV.
endef

$(eval $(call Profile,media-server))
