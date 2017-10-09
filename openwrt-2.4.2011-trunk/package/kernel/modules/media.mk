MEDIA_MENU:=Media Support

# dvb-core and cs752x_ts depend on each other
# and cannot therefore be loaded as modules. 
# They need to be built into the kernel as '*'.
# $(LINUX_DIR)/drivers/media/dvb/dvb-core/dvb-core.$(LINUX_KMOD_SUFFIX)
# $(LINUX_DIR)/drivers/media/dvb/cs752x-ts/cs752x_ts.$(LINUX_KMOD_SUFFIX)
# dvb-core
# cs752x_ts
# Also, probably because of the dependency, depmod segfaults if the .ko
# files for these two modules are present in /lib/modules/xxx/
#
define KernelPackage/matrox-gseries
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=Matrox G-series PCIe graphics cards
  FILES:=\
	$(LINUX_DIR)/drivers/video/cfbfillrect.ko \
	$(LINUX_DIR)/drivers/video/cfbcopyarea.ko \
	$(LINUX_DIR)/drivers/video/cfbimgblt.ko \
	$(LINUX_DIR)/drivers/video/matrox/g450_pll.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_DAC1064.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_Ti3026.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_accel.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_base.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_crtc2.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_g450.ko \
	$(LINUX_DIR)/drivers/video/matrox/matroxfb_misc.ko \
	$(LINUX_DIR)/drivers/video/matrox/i2c-matroxfb.ko \
	$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-bit.ko
  KCONFIG:=\
	CONFIG_FB=y \
	CONFIG_FB_CFB_COPYAREA \
	CONFIG_FB_CFB_FILLRECT \
	CONFIG_FB_CFB_IMAGEBLIT \
	CONFIG_FB_MATROX \
	CONFIG_FB_MATROX_MILLENIUM=y \
	CONFIG_FB_MATROX_MYSTIQUE=y \
	CONFIG_FB_MATROX_G=y \
	CONFIG_FB_MATROX_I2C \
	CONFIG_I2C_ALGOBIT \
	CONFIG_FB_MATROX_MAVEN=n \
	CONFIG_FB_TILEBLITTING=y
  AUTOLOAD:=$(call AutoLoad,20,\
	cfbfillrect \
	cfbcopyarea \
	cfbimgblt \
	matroxfb_accel \
	matroxfb_misc \
	g450_pll \
	matroxfb_g450 \
	matroxfb_DAC1064 \
	matroxfb_Ti3026 \
	matroxfb_base \
	i2c-algo-bit \
	i2c-matroxfb)
endef

define KernelPackage/matrox-gseries/install
	perl -i -pe 's/matroxfb_base/matroxfb_base depth=24/g' $(1)/etc/modules.d/20-matrox-gseries
endef

$(eval $(call KernelPackage,matrox-gseries))

define KernelPackage/nvidia
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=Nvidia video cards
  FILES:=\
	$(LINUX_DIR)/drivers/video/cfbfillrect.ko \
	$(LINUX_DIR)/drivers/video/cfbcopyarea.ko \
	$(LINUX_DIR)/drivers/video/cfbimgblt.ko \
	$(LINUX_DIR)/drivers/video/fb_ddc.ko \
	$(LINUX_DIR)/drivers/video/nvidia/nvidiafb.ko \
	$(LINUX_DIR)/drivers/video/riva/rivafb.ko 
  KCONFIG:=\
	CONFIG_FB=y \
	CONFIG_FB_NVIDIA_I2C=y \
	CONFIG_FB_NVIDIA_BACKLIGHT=n \
	CONFIG_FB_CFB_COPYAREA \
	CONFIG_FB_CFB_FILLRECT \
	CONFIG_FB_CFB_IMAGEBLIT \
	CONFIG_FB_DDC \
	CONFIG_FB_NVIDIA_DEBUG=y \
	CONFIG_FB_NVIDIA \
	CONFIG_FB_RIVA \
	CONFIG_FB_RIVA_I2C=y \
	CONFIG_FB_RIVA_DEBUG=y \
	CONFIG_FB_RIVA_BACKLIGHT=n

  AUTOLOAD:=$(call AutoLoad,20,\
	cfbfillrect \
	cfbcopyarea \
	cfbimgblt \
	fb_ddc \
	nvidiafb rivafb)
endef

define KernelPackage/nvidia/install
	perl -i -pe 's/nvidiafb/nvidiafb bpp=24/g' $(1)/etc/modules.d/20-nvidia
endef

$(eval $(call KernelPackage,nvidia))

define KernelPackage/ati
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=ATI video cards
  FILES:=\
	$(LINUX_DIR)/drivers/video/cfbfillrect.ko \
	$(LINUX_DIR)/drivers/video/cfbcopyarea.ko \
	$(LINUX_DIR)/drivers/video/cfbimgblt.ko \
	$(LINUX_DIR)/drivers/video/fb_ddc.ko \
	$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-bit.ko \
	$(LINUX_DIR)/drivers/video/aty/atyfb.ko \
	$(LINUX_DIR)/drivers/video/aty/aty128fb.ko \
	$(LINUX_DIR)/drivers/video/aty/radeonfb.ko
  KCONFIG:=\
	CONFIG_FB=y CONFIG_PCI=y \
	CONFIG_FB_MODE_HELPERS \
	CONFIG_FB_CFB_FILLRECT \
	CONFIG_FB_CFB_COPYAREA \
	CONFIG_FB_CFB_IMAGEBLIT \
	CONFIG_I2C_ALGOBIT \
	CONFIG_FB_MACMODES=n \
	CONFIG_FB_RADEON \
	CONFIG_FB_DDC \
	CONFIG_FB_RADEON_I2C=y \
	CONFIG_FB_RADEON_BACKLIGHT=n \
	CONFIG_FB_RADEON_DEBUG=y \
	CONFIG_FB_ATY128 \
	CONFIG_FB_ATY128_BACKLIGHT=n \
	CONFIG_FB_ATY \
	CONFIG_FB_ATY_CT=y \
	CONFIG_FB_ATY_GENERIC_LCD=n \
	CONFIG_FB_ATY_GX=y \
	CONFIG_FB_ATY_BACKLIGHT=n
  AUTOLOAD:=$(call AutoLoad,20,\
	cfbfillrect \
	cfbcopyarea \
	cfbimgblt \
	fb_ddc \
	i2c-algo-bit \
	atyfb aty128fb radeonfb)
endef

$(eval $(call KernelPackage,ati))

define KernelPackage/hauppauge-hvr850
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=Hauppauge WinTV HVR 850 USB
  FILES:=\
	$(LINUX_DIR)/drivers/media/dvb/frontends/au8522.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/tveeprom.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/au0828/au0828.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/common/tuners/xc5000.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videobuf-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videobuf-vmalloc.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=\
	CONFIG_DVB_CAPTURE_DRIVERS=y \
	CONFIG_DVB_CORE=y \
	CONFIG_DVB_FE_CUSTOMISE=y \
	CONFIG_MEDIA_ATTACH=y \
	CONFIG_MEDIA_SUPPORT=y \
	CONFIG_MEDIA_TUNER=y \
	CONFIG_MEDIA_TUNER_CUSTOMISE=y \
	CONFIG_V4L_USB_DRIVERS=y \
	CONFIG_VIDEO_CAPTURE_DRIVERS=y \
	CONFIG_VIDEO_DEV=y \
	CONFIG_VIDEO_MEDIA=y \
	CONFIG_VIDEO_V4L2=y \
	CONFIG_VIDEO_V4L2_COMMON=y \
	CONFIG_VIDEO_V4L1_COMPAT=y \
	CONFIG_DVB_AU8522 \
	CONFIG_I2C_ALGOBIT \
	CONFIG_MEDIA_TUNER_XC5000 \
	CONFIG_VIDEO_TUNER=y \
	CONFIG_VIDEOBUF_GEN \
	CONFIG_VIDEOBUF_VMALLOC \
	CONFIG_VIDEO_AU0828 \
	CONFIG_VIDEO_TVEEPROM \
	CONFIG_HDMI_ANX9805=n
  AUTOLOAD:=$(call AutoLoad,95, \
	v4l1-compat \
	videodev \
	v4l2-int-device \
	v4l2-common \
	tveeprom \
	au8522 \
	xc5000 \
	tuner \
	videobuf-core \
	videobuf-vmalloc \
	au0828)
endef

define KernelPackage/hauppauge-hvr850/install
	perl -i -pe 's/xc5000/xc5000 no_poweroff=1/g' $(1)/etc/modules.d/95-hauppauge-hvr850
	$(INSTALL_DIR) $(1)/lib/firmware
	cp -f modules/firmware/dvb-fe-xc5000-1.1.fw $(1)/lib/firmware
	cp -f modules/firmware/dvb-fe-xc5000-1.6.114.fw $(1)/lib/firmware
endef

$(eval $(call KernelPackage,hauppauge-hvr850))

define KernelPackage/hauppauge-hvr1250
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=Hauppauge WinTV HVR 1250 PCIe
  DEPENDS:=+kmod-i2c-core +kmod-i2c-algo-pca +kmod-i2c-algo-pcf
  FILES:=\
        $(LINUX_DIR)/drivers/media/IR/ir-common.ko \
        $(LINUX_DIR)/drivers/media/IR/ir-core.ko \
	$(LINUX_DIR)/drivers/media/dvb/dvb-core/dvb-core.ko \
	$(LINUX_DIR)/drivers/media/video/tveeprom.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videobuf-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/media/video/videobuf-vmalloc.$(LINUX_KMOD_SUFFIX) \
        $(LINUX_DIR)/drivers/media/video/btcx-risc.ko \
        $(LINUX_DIR)/drivers/media/video/videobuf-dma-sg.ko \
        $(LINUX_DIR)/drivers/media/video/videobuf-dvb.ko \
        $(LINUX_DIR)/drivers/media/dvb/frontends/s5h1411.ko \
        $(LINUX_DIR)/drivers/media/dvb/frontends/s5h1409.ko \
        $(LINUX_DIR)/drivers/media/common/tuners/tda18271.ko \
	$(LINUX_DIR)/drivers/media/video/cx2341x.ko \
        $(LINUX_DIR)/drivers/media/video/cx23885/cx23885.ko \
	$(LINUX_DIR)/drivers/media/video/v4l2-common.ko \
	$(LINUX_DIR)/drivers/media/video/v4l2-int-device.ko \
	$(LINUX_DIR)/drivers/media/video/videodev.ko \
	$(LINUX_DIR)/drivers/media/video/v4l1-compat.ko
  KCONFIG:=\
	CONFIG_IR_CORE \
	CONFIG_DVB_CAPTURE_DRIVERS=y \
	CONFIG_DVB_CORE \
	CONFIG_DVB_FE_CUSTOMISE=y \
	CONFIG_MEDIA_ATTACH=y \
	CONFIG_MEDIA_SUPPORT \
	CONFIG_MEDIA_TUNER \
	CONFIG_MEDIA_TUNER_CUSTOMISE=y \
	CONFIG_VIDEO_CAPTURE_DRIVERS=y \
	CONFIG_VIDEO_DEV \
	CONFIG_VIDEO_MEDIA \
	CONFIG_VIDEO_V4L2 \
	CONFIG_VIDEO_V4L2_COMMON \
	CONFIG_VIDEO_V4L1_COMPAT \
	CONFIG_VIDEO_TUNER \
	CONFIG_VIDEOBUF_GEN \
	CONFIG_VIDEO_AU0828 \
	CONFIG_VIDEOBUF_VMALLOC \
	CONFIG_VIDEO_TVEEPROM \
        CONFIG_VIDEO_CX23885 \
	CONFIG_DVB_S5H1411 \
	CONFIG_DVB_S5H1409 \
	CONFIG_MEDIA_TUNER_TDA18271 \
	CONFIG_VIDEO_CX18=n \
	CONFIG_DVB_USB=n \
	CONFIG_SMS_SIANO_MDTV=n \
	CONFIG_DVB_DM1105=n \
	CONFIG_MANTIS_CORE=n
  AUTOLOAD:=$(call AutoLoad,95, \
	dvb-core \
	v4l1-compat \
	videodev \
	v4l2-int-device \
	v4l2-common \
	tveeprom \
        videobuf-core \
        videobuf-vmalloc s5h1411 tda18271 \
        cx2341x \
	ir-core ir-common \
        btcx-risc videobuf-dma-sg videobuf-dvb cx2341x cx23885)
endef

$(eval $(call KernelPackage,hauppauge-hvr1250))

define KernelPackage/displaylink-fb
  SUBMENU:=$(MEDIA_MENU)
  TITLE:=Displaylink USB Framebuffer support
  FILES:=$(LINUX_DIR)/drivers/staging/udlfb/udlfb.ko
  DEPENDS:=+kmod-usb-core
  KCONFIG:=\
	CONFIG_STAGING=y \
	STAGING_EXCLUDE_BUILD=n \
	CONFIG_VIDEO_CX25821=n \
	CONFIG_FB_SM7XX=n \
	CONFIG_FB_XGI=n \
	CONFIG_FB \
	CONFIG_FB_UDL
  AUTOLOAD:=$(call AutoLoad,25,udlfb)
endef

define KernelPackage/displaylink-fb/description
  This is a kernel framebuffer driver for DisplayLink USB devices.
  Supports fbdev clients like xf86-video-fbdev, kdrive, fbi, and
  mplayer -vo fbdev. Supports all USB 2.0 era DisplayLink devices.
  To compile as a module, choose M here: the module name is udlfb.
endef

$(eval $(call KernelPackage,displaylink-fb))
