# Kernel modules specific to Goldengate
#
G2_MENU:=G2
G2_KMOD_DIR:=$(LINUX_DIR)/drivers

# PCIe is a feature who's code resides in
# arch/arm/mach-goldengate, and appears to be
# a compiled in feature of the G2 kernel.
#
define KernelPackage/g2-pcie
  SUBMENU:=$(G2_MENU) PCIe Support
  TITLE:=G2 PCIe support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CORTINA_G2_PCIE=y
endef
define KernelPackage/g2-pcie/config
  source "$(SOURCE)/modules/Config-pcie.in"
endef
$(eval $(call KernelPackage,g2-pcie))

# Sound Port
define KernelPackage/g2-sport
  SUBMENU:=$(G2_MENU) Sport Port Support
  TITLE:=Cortina CS75XX Sport Port
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_SOUND=y CONFIG_SND=y CONFIG_SND_SOC=y \
	CONFIG_SND_CS75XX_SOC
  FILES:=$(LINUX_DIR)/sound/soc/cs75xx/snd-soc-cs75xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,g2-sport)
endef

define KernelPackage/g2-sport/description
  SSP/SPDIF on the GoldenGate Soc.
endef
$(eval $(call KernelPackage,g2-sport))

# Sound EVM
define KernelPackage/g2-snd-evm
  SUBMENU:=$(G2_MENU) Sound Evaluation Module
  TITLE:=Cortina CS75XX Evaluation Module
  DEPENDS:=@TARGET_g2||TARGET_ag2 +kmod-g2-sport
  KCONFIG:=CONFIG_SOUND=y CONFIG_SND=y CONFIG_SND_SOC=y \
	CONFIG_SND_CS75XX_SOC_EVM
  FILES:=$(LINUX_DIR)/sound/soc/cs75xx/snd-soc-evm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,g2-snd-evm)
endef

define KernelPackage/g2-snd-evm/description
  Sound Support on EVM
endef

define KernelPackage/g2-snd-evm/config
	menu "Configuration"
		depends on PACKAGE_kmod-g2-snd-evm

	choice
		prompt "Codec Selection"
		default KERNEL_SND_CS75XX_SOC_EVM_SPDIF

	config KERNEL_SND_CS75XX_SOC_EVM_SPDIF
		bool "SPDIF"
	        default y

	config KERNEL_SND_CS75XX_SOC_EVM_DAE4P
		bool "DAE4P"
	        default n

	endchoice

	endmenu
endef

$(eval $(call KernelPackage,g2-snd-evm))

# USB
define KernelPackage/g2-usb
  SUBMENU:=$(G2_MENU) USB Support
  TITLE:=G2 USB support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_USB_SUPPORT=y CONFIG_USB=y \
	CONFIG_CORTINA_G2_USB_HOST=y
endef
$(eval $(call KernelPackage,g2-usb))

define KernelPackage/g2-usb-dwc
  SUBMENU:=$(G2_MENU) USB Support
  TITLE:=G2 USB gadget (synopsys dwc) support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_USB_SUPPORT=y \
	CONFIG_USB_GADGET \
	CONFIG_USB_FILE_STORAGE \
        CONFIG_USB_GADGET_DUALSPEED=y CONFIG_USB_GADGET_SELECTED=y \
        CONFIG_USB_GADGET_SNPS_DWC_OTG=y CONFIG_USB_GADGET_VBUS_DRAW=2 \
        CONFIG_USB_SNPS_DWC CONFIG_USE_GENERIC_SMP_HELPERS=y
  FILES:=$(LINUX_DIR)/drivers/usb/gadget/g_file_storage.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/usb/gadget/dwc_otg.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,dwc_otg)
endef
$(eval $(call KernelPackage,g2-usb-dwc))

# Flash
define KernelPackage/g2-mtd-parallel
  SUBMENU:=$(G2_MENU) Flash Support
  TITLE:=Parallel NOR flash support
  DEPENDS:=@!(TARGET_g2_engs||TARGET_g2_refs)&&TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_MTD CONFIG_MTD_PARTITIONS \
	CONFIG_MTD_CFI \
	CONFIG_MTD_CORTINA_CS752X_CFI 

ifeq ($(LINUX_VERSION),3.4.11)
  KCONFIG += CONFIG_MTD_CFI_ADV_OPTIONS CONFIG_MTD_CFI_AMDSTD 
endif
  FILES:=$(G2_KMOD_DIR)/mtd/maps/cs752x_cfi.ko
  AUTOLOAD:=$(call AutoLoad,50,cs752x_cfi)
endef

define KernelPackage/g2-mtd-parallel/config
	menu "Configuration"
		depends on PACKAGE_kmod-g2-mtd-parallel

	config KERNEL_MTD_CORTINA_CS752X_PFLASH_SIZE
		hex "Parallel flash size on board."
		default "0x8000000"
		help
	  	  Set flash size on board. Default size is 128MB.

	endmenu
endef

$(eval $(call KernelPackage,g2-mtd-parallel))


define KernelPackage/g2-mtd-serial
  SUBMENU:=$(G2_MENU) Flash Support
  TITLE:=Spansion Serial Flash device
  KCONFIG:=CONFIG_MTD CONFIG_MTD_PARTITIONS \
	 CONFIG_MTD_CORTINA_CS752X_SERIAL
  FILES:=$(G2_KMOD_DIR)/mtd/maps/cs752x_serial.ko
  AUTOLOAD:=$(call AutoLoad,50,cs752x)
endef
$(eval $(call KernelPackage,g2-mtd-serial))

define KernelPackage/g2-mtd-shared
  SUBMENU:=$(G2_MENU) Flash Support
  TITLE:=Parallel Flash share pin
  DEPENDS:=@!(TARGET_g2_engs||TARGET_g2_refs)&&TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CORTINA_CS752X_SHARE_PIN=y
endef
$(eval $(call KernelPackage,g2-mtd-shared))

define KernelPackage/g2-mtd-nand
  SUBMENU:=$(G2_MENU) Flash Support
  TITLE:=NAND Flash device on Cortina board
  KCONFIG:=CONFIG_MTD_NAND CONFIG_MTD_PARTITIONS \
	CONFIG_MTD_ROOTFS_BADBLOCK=y CONFIG_MTD_CORTINA_CS752X_NAND
  FILES:=$(G2_KMOD_DIR)/mtd/nand/cs752x_nand.ko
  AUTOLOAD:=$(call AutoLoad,50,cs752x_nand)
endef
define KernelPackage/g2-mtd-nand/config
  source "$(SOURCE)/modules/Config-nand.in"
endef
$(eval $(call KernelPackage,g2-mtd-nand))

define KernelPackage/multi-flash-support
  SUBMENU:=G2 Flash Support
  TITLE:=Enable mutliple different flash work together
  KCONFIG:=CONFIG_MTD_CS752X_MULTIFLASH
endef

define KernelPackage/multi-flash-support/description
  Enable mutliple different flash work together.
  For example, NAND flash and serial flash are on target boards.
  They works together. It needs a flags to protected.
endef
$(eval $(call KernelPackage,multi-flash-support))

define KernelPackage/g2-mtd-tests
  SUBMENU:=$(G2_MENU) Flash Support
  TITLE:=drivers/mtd/tests
  KCONFIG:=CONFIG_MTD CONFIG_MTD_TESTS
  FILES:=$(LINUX_DIR)/drivers/mtd/tests/mtd*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,g2-mtd-tests)
endef
$(eval $(call KernelPackage,g2-mtd-tests))

# Allow the user one easy entry point to common
# kernel debugging options.
#
define KernelPackage/g2-kernel-debug
  SUBMENU:=$(G2_MENU) Kernel Debugging
  TITLE:=Useful Kernel Debugging Options
  KCONFIG:=\
    CONFIG_DEBUG_BUGVERBOSE=y \
    CONFIG_DEBUG_ERRORS=y \
    CONFIG_DEBUG_INFO=y \
    CONFIG_DEBUG_KERNEL=y \
    CONFIG_DEBUG_LL=y \
    CONFIG_DEBUG_SPINLOCK=y \
    CONFIG_DEBUG_SPINLOCK_SLEEP=y \
    CONFIG_DEBUG_USER=y \
    CONFIG_CS752X_PROC=y
endef
$(eval $(call KernelPackage,g2-kernel-debug))

define KernelPackage/g2-kernel-kgdb
  SUBMENU:=$(G2_MENU) KGDB
  TITLE:=Enable debugging with kgdb
  KCONFIG:=\
	CONFIG_DEBUG_ERRORS=y \
	CONFIG_DEBUG_FS=y \
	CONFIG_DEBUG_KERNEL=y \
	CONFIG_DEBUG_KMEMLEAK=y \
	CONFIG_DEBUG_KMEMLEAK_EARLY_LOG_SIZE=400 \
	CONFIG_GENERIC_LOCKBREAK=y \
	CONFIG_DEBUG_INFO=y \
	CONFIG_FRAME_POINTER=y \
	CONFIG_KDB_KEYBOARD=y \
	CONFIG_KGDB=y \
	CONFIG_KGDB_KDB=n \
	CONFIG_KGDB_TESTS=n \
	CONFIG_KGDB_SERIAL_CONSOLE=y \
	CONFIG_MAGIC_SYSRQ=y \
	CONFIG_STACKTRACE=y
endef
$(eval $(call KernelPackage,g2-kernel-kgdb))

g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/util/cs_util.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/fe/table/cs_fe_table.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/fe/core/cs_fe_core.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/ni/cs_ni.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/qm/cs_qm_main.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/sch/cs752x_sch_main.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/tm/cs_tm.ko
g2-ni-files-$(CONFIG_CS752X) += $(G2_KMOD_DIR)/net/cs752x/src/core/cs752x_core.ko
g2-ni-files-$(CONFIG_CS752X_PROC) += $(G2_KMOD_DIR)/net/cs752x/src/diagnostic/cs752x_proc_main.ko

# COMMENTED LINES CORRESPOND TO COMMENTED OUT "obj-m +=" in the module's Makefile

g2-ni-modules-m := $(foreach m,$(g2-ni-files-m),$(shell basename $(m) .ko))

define KernelPackage/g2-ni
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X NI module support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_NETDEV_1000=y CONFIG_CS752X_PROC=y CONFIG_CS752X
  FILES:=$(g2-ni-files-m)
  AUTOLOAD:=$(call AutoLoad,50,$(g2-ni-modules-m))
endef

define KernelPackage/g2-ni/description
  This driver supports Cortina CS752x gigabit ethernet family of
  adapters.  For more information on how to identify your adapter, go
  to the Adapter and Driver ID Guide at:

  <http://www.cortina-systems.com/>
endef

define KernelPackage/g2-ni/config
  source "$(SOURCE)/modules/Config-ni.in"
endef

$(eval $(call KernelPackage,g2-ni))

define KernelPackage/g2-ni-fe-test
  SUBMENU:=G2 Networking
  TITLE:=Unit Test of FE Table Management support
  DEPENDS:=+kmod-g2-ni
  KCONFIG:=CONFIG_CS75XX_FE_TBL_MGMT_UT
  FILES:=$(LINUX_DIR)/drivers/net/cs752x/src/fe/table/cs_fe_table_ut.ko
#  AUTOLOAD:=$(call AutoLoad,50,cs_fe_table_ut)
endef

define KernelPackage/g2-ni-fe-test/description
     This module tests table management funtions in the
     forwarding engine. It covers basic test, boundary test, mixed
     operations, and combination of multiple tables.
endef

$(eval $(call KernelPackage,g2-ni-fe-test))

define KernelPackage/g2-virtual-ni
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X NI virtual support
  DEPENDS:=@TARGET_g2||TARGET_ag2
endef

define KernelPackage/g2-virtual-ni/description
  Supports Cortina CS752x gigabit virtual ethernet
endef

define KernelPackage/g2-virtual-ni/config
	menu Configuration
	        depends on PACKAGE_kmod-g2-virtual-ni

	config KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE
	        bool "Cortina CS752X Virtual Network Interface"
	        default y
	        ---help---
	          This driver supports Cortina CS752x chip family of built-in virtual
	          network interfaces. For more information on how to identify your
	          adapter, go to the Adapter & Driver ID Guide at:

	          <http://www.cortina-systems.com/>

	choice
	        prompt "Attribute that Virtual Interfaces based on"
	        default KERNEL_CS752X_VIRTUAL_NI_DBLTAG

	config KERNEL_CS752X_VIRTUAL_NI_CPUTAG
	        bool "Realtek CPU TAG at the end of packet"
	        default n
	        help
	          This will create the virtual interfaces that based on the port_mask
	          in Realtek CPU tag that is appended at the end of each packet.

	config KERNEL_CS752X_VIRTUAL_NI_DBLTAG
	        bool "SVLAN tag that is always at the outer VLAN tag"
	        default y
	        help
	          This will create the virtual interfaces that based on the VLAN ID
	          of outer VLAN (SVLAN) header in each packet.
	endchoice

	config KERNEL_CS752X_VIRTUAL_ETH0
	        bool "Cortina CS752X Virtual Network Interface on eth0 (MAC#0)"
	        default n
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE
	        ---help---
	          This driver supports Cortina CS752x chip family of built-in virtual
	          network interfaces on eth0.

	config KERNEL_CS752X_NR_VIRTUAL_ETH0
	        int "Number of Virtual Interfaces created on eth0 (1-80)"
	        range 1 80
	        default "4"
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH0

	config KERNEL_CS752X_VID_START_ETH0
	        int "The starting VLAN ID for the virtual interfaces eth0 (1 - 255)"
	        range 1 255
	        default "100"
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH0 && KERNEL_CS752X_VIRTUAL_NI_DBLTAG

	config KERNEL_CS752X_VIRTUAL_ETH1
	        bool "Cortina CS752X Virtual Network Interface on eth1 (MAC#1)"
	        default y
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE
	        ---help---
	          This driver supports Cortina CS752x chip family of built-in virtual
	          network interfaces on eth1.

	config KERNEL_CS752X_NR_VIRTUAL_ETH1
	        int "Number of Virtual Interfaces created on eth1 (1-80)"
	        range 1 80
	        default "4"
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH1

	config KERNEL_CS752X_VID_START_ETH1
	        int "The starting VLAN ID for the virtual interfaces eth1 (1 - 255)"
	        range 1 255
	        default "100"
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH1 && KERNEL_CS752X_VIRTUAL_NI_DBLTAG

	config KERNEL_CS752X_VIRTUAL_ETH2
	        bool "Cortina CS752X Virtual Network Interface on eth2 (MAC#2)"
	        default n
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE
	        ---help---
	          This driver supports Cortina CS752x chip family of built-in virtual
	          network interfaces on eth2.

	config KERNEL_CS752X_NR_VIRTUAL_ETH2
	        int "Number of Virtual Interfaces created on eth2 (1-80)"
	        range 1 80
	        default "4"
	        depends on KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH2

	config KERNEL_CS752X_VID_START_ETH2
	        int "The starting VLAN ID for the virtual interfaces eth2 (1 - 255)"
	        range 1 255
	        default "100"
	        depends on  KERNEL_CS752X_VIRTUAL_NETWORK_INTERFACE && \
	                KERNEL_CS752X_VIRTUAL_ETH2 && KERNEL_CS752X_VIRTUAL_NI_DBLTAG
	endmenu
endef

$(eval $(call KernelPackage,g2-virtual-ni))

# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/core/cs_hw_accel_core.ko
g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/bridge/cs_bridge.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/core/cs_core.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/ipv6/cs_ipv6.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/pppoe/cs_pppoe.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/multicast/cs_mc.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/qos/cs_qos.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/common/cs_common.ko
g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/netfilter/cs_netfilter.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/8021q/cs_vlan.ko
# g2-ni-hw-files-$(CONFIG_CS752X_HW_ACCELERATION_IPSEC) += $(G2_KMOD_DIR)/net/cs752x/src/kernel_adapt/ipsec/cs_ipsec.ko
g2-ni-hw-modules-m := $(foreach m,$(g2-ni-hw-files-m),$(shell basename $(m) .ko))

define KernelPackage/g2-hw-accel-ni
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X NI hardware acceleration support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CS752X_ACCEL_KERNEL
#  FILES:=$(g2-ni-hw-files-m)
#  AUTOLOAD:=$(call AutoLoad,50,$(g2-ni-hw-modules-m))
endef

define KernelPackage/g2-hw-accel-ni/description
  Supports Cortina CS752x gigabit hardware acceleration
endef

define KernelPackage/g2-hw-accel-ni/config
	menu Configuration
	        depends on PACKAGE_kmod-g2-hw-accel-ni
	config KERNEL_CS752X_HW_ACCELERATION
	        bool "Cortina CS752X Hardware Acceleration support"
	        default y
	        help
	          This driver supports Cortina CS752x chip family of hardware acceleration
	          adapters.  For more information on how to identify your adapter, go
	          to the Adapter & Driver ID Guide at:

	          <http://www.cortina-systems.com/>

	config KERNEL_CS752X_HW_ACCELERATION_IPSEC
	        bool "Cortina CS752X Hardware IPsec Acceleration support"
	        default y
	        depends on KERNEL_CS752X_HW_ACCELERATION
	        help
	          This driver supports Cortina CS752x chip family of hardware acceleration
	          adapters.  For more information on how to identify your adapter, go
	          to the Adapter & Driver ID Guide at:

	          <http://www.cortina-systems.com/>

	config KERNEL_CS752X_HW_ACCEL_ETHERIP
		bool "Cortina CS752X Hardware Acceleration for EtherIP encapsulation"
		default n
		depends on KERNEL_CS752X_HW_ACCELERATION_IPSEC
		help
		  When set ether IP encapsulation will be accelerated using packet
		  engine. This option is only available when IPsec HW acceleration
		  is enabled. EtherIP acceleration without IPsec is not yet supported.

		  <http://www.cortina-systems.com/>
          
	config KERNEL_CS75XX_HW_ACCEL_IPLIP
		bool "Cortina CS75XX Hardware Acceleration for IPLIP encapsulation"
		default y
		depends on KERNEL_CS752X_HW_ACCELERATION
		help
		  This function supports IPv6 over PPP over L2TP over IPv4
		  over PPPoE encapsulation by using G2 network engine and
		  packet engine.

	config KERNEL_CS75XX_DOUBLE_CHECK
	        bool "Cortina CS75XX Double Check Support"
	        default n
	        depends on KERNEL_CS752X_HW_ACCELERATION
	        help
		  This function supports double check mechanism of CS75XX
		  hardware acceleration.
		  
	config KERNEL_CS75XX_MTU_CHECK
		bool "Cortina CS75XX MTU check"
		default y
		depends on KERNEL_CS752X_HW_ACCELERATION
		help
		  This function supports packet length range check for IPoE, PPPoE and IPLIP.
		  
	endmenu
endef

$(eval $(call KernelPackage,g2-hw-accel-ni))

define AddDepends/ni
  SUBMENU:=G2 Networking
  DEPENDS+=kmod-g2-hw-accel-ni $(1) @!TARGET_uml
endef

define KernelPackage/g2-wfo-core
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X WFO support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CS75XX_WFO
  $(call AddDepends/ni)
endef

define KernelPackage/g2-wfo-core/description
  Supports Cortina WFO core support
endef

$(eval $(call KernelPackage,g2-wfo-core))

define AddDepends/wfo
  SUBMENU:=G2 Networking
  DEPENDS+=kmod-g2-wfo-core $(1) @!TARGET_uml
endef

define KernelPackage/g2-wfo-rt3593
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X WFO RT3593 support
  KCONFIG:=CONFIG_CS75XX_WFO_RT3593
  $(call AddDepends/wfo)
endef

define KernelPackage/g2-wfo-rt3593/description
  Supports Cortina WFO RT3593 support
endef

$(eval $(call KernelPackage,g2-wfo-rt3593))
#FastNET
#
#define KernelPackage/g2-accel-core-ni
#  SUBMENU:=G2 Networking
#  TITLE:=Cortina CS752X NI ACCEL Core support
#  DEPENDS:=@TARGET_g2||TARGET_ag2 
#  KCONFIG:=CONFIG_CS752X_ACCEL_KERNEL
#endef

#define KernelPackage/g2-accel-core-ni/description
#  Supports Cortina CS752x CS752X NI ACCEL Core 
#endef


define KernelPackage/g2-fastnet-ni
  SUBMENU:=G2 Networking
  TITLE:=Cortina CS752X NI FastNet support
  DEPENDS:=@TARGET_g2||TARGET_ag2 
  #DEPENDS:=+kmod-i2c-core
  KCONFIG:=CONFIG_CS752X_FASTNET CONFIG_CS752X_ACCEL_KERNEL
endef

define KernelPackage/g2-fastnet-ni/description
  Supports Cortina CS752x fastnet acceleration
endef

$(eval $(call KernelPackage,g2-fastnet-ni))

# Serial ATA

define KernelPackage/g2-ahci
  SUBMENU:=G2 AHCI Support
  TITLE:=Golden-Gate AHCI Serial ATA
  DEPENDS:=+kmod-i2c-core
  KCONFIG:=CONFIG_ATA=y CONFIG_CS752X_AHCI
  FILES:=$(G2_KMOD_DIR)/ata/cs752x_ahci.ko
  AUTOLOAD:=$(call AutoLoad,50,cs752x_ahci)
endef

define KernelPackage/g2-ahci/description
  This option enables support for Golden-Gate AHCI Serial ATA.
endef

$(eval $(call KernelPackage,g2-ahci))

# Character devices

define KernelPackage/g2-pwc
  SUBMENU:=G2 Char devices
  TITLE:=Cortina CS75XX power supply
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CS75XX_PWC
  FILES:=$(G2_KMOD_DIR)/char/cs75xx_pwr.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,cs75xx_pwr)
endef

define KernelPackage/g2-pwc/description
  Cortina CS75XX power control
endef

$(eval $(call KernelPackage,g2-pwc))

define KernelPackage/g2-reg-rw
  SUBMENU:=G2 Char devices
  TITLE:=Register R/W
  KCONFIG:=CONFIG_CS75XX_REG_RW
  FILES:=$(G2_KMOD_DIR)/char/cs75xx_reg.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,cs75xx_reg)
endef

define KernelPackage/g2-reg-rw/description
  Support to read/write register in command line for
  Cortex-A9 CPU on Cortina-Systems Baseboard.
endef

$(eval $(call KernelPackage,g2-reg-rw))

define KernelPackage/g2-ipc
  SUBMENU:=G2 Char devices
  TITLE:=IPC ARM<->RCPUs
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_CS75xx_IPC2RCPU CONFIG_CS75xx_IPC_WFO
  FILES:=$(G2_KMOD_DIR)/char/cs75xx_ipc_re.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,cs75xx_ipc_re)
endef

define KernelPackage/g2-ipc/description
  IPC to RCPU module provided by the Cortina Golden Gate
endef

$(eval $(call KernelPackage,g2-ipc))


define KernelPackage/g2-random
  SUBMENU:=G2 Char devices
  TITLE:=CS75XX hardware random number generator support
  KCONFIG:=CONFIG_HW_RANDOM=y CONFIG_HW_RANDOM_CS75XX
  FILES:=$(G2_KMOD_DIR)/char/hw_random/cs75xx-rng/cs75xx-rng.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,cs75xx-rng)
endef

define KernelPackage/g2-random/description
  This driver provides kernel-side support for the Random Number
  Generator hardware found on CS75XX Gate processors.
endef

define KernelPackage/g2-random/config
	menu "Configuration"
		depends on PACKAGE_kmod-g2-random

	config KERNEL_CS75XX_TRNG_DIAG_ON
		bool "Diagnostics"
		default n
		help
		  Turn on diagnostics
	endmenu
endef

$(eval $(call KernelPackage,g2-random))

# GPIO

define KernelPackage/g2-gpio
  SUBMENU:=G2 GPIO
  TITLE:=G2 GPIO support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_GPIOLIB=y CONFIG_GPIO_SYSFS=y CONFIG_GPIO_CS75XX
  FILES:=$(G2_KMOD_DIR)/gpio/cs75xx-gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,cs75xx-gpio)
endef

define KernelPackage/g2-gpio/description
  Goldengate GPIO sub-system.
endef

$(eval $(call KernelPackage,g2-gpio))

# I2C

define KernelPackage/g2-i2c
  SUBMENU:=G2 I2C Support
  TITLE:=Cortina CS75XX I2C(BIW) controller
  DEPENDS:=+kmod-i2c-core
  KCONFIG:=CONFIG_I2C=y CONFIG_I2C_BOARDINFO=y CONFIG_I2C_CS75XX
  FILES:=$(G2_KMOD_DIR)/i2c/busses/i2c-cs75xx.ko
  AUTOLOAD:=$(call AutoLoad,50,i2c-cs75xx)
endef

define KernelPackage/g2-i2c/description
  Built-in I2C interface on the Cortina CS75XX of host bridges.
endef

$(eval $(call KernelPackage,g2-i2c))

# IR

define KernelPackage/g2-ir
  SUBMENU:=G2 IR Support
  TITLE:=Cortina CS75XX IR Receiver
  KCONFIG:=CONFIG_MEDIA_SUPPORT=y CONFIG_IR_CORE=y CONFIG_RC_MAP=y \
	CONFIG_IR_CS75XX
  FILES:=$(G2_KMOD_DIR)/media/IR/cs75xx_ir.ko
#  AUTOLOAD:=$(call AutoLoad,50,cs75xx_ir)
endef

define KernelPackage/g2-ir/description
  IR Receiver on the GoldenGate Soc.
endef

$(eval $(call KernelPackage,g2-ir))

# LCD Panel

g2-lcd-files-$(CONFIG_FB_CFB_COPYAREA)   += $(G2_KMOD_DIR)/video/cfbcopyarea.ko
g2-lcd-files-$(CONFIG_FB_CFB_FILLRECT)   += $(G2_KMOD_DIR)/video/cfbfillrect.ko
g2-lcd-files-$(CONFIG_FB_CFB_IMAGEBLIT)   += $(G2_KMOD_DIR)/video/cfbimgblt.ko

#g2-lcd-files-$(CONFIG_KERNEL_PANEL_HX8238A)  += $(G2_KMOD_DIR)/video/hx8238a_panel.ko
#g2-lcd-files-$(CONFIG_KERNEL_HDMI_ANX9805)   += $(G2_KMOD_DIR)/video/anx9805/anx9805_hdmi.ko

g2-lcd-files-$(CONFIG_FB_CS752X_CLCD) += $(G2_KMOD_DIR)/video/cs752x_clcdfb.ko
g2-lcd-files-$(CONFIG_PANEL_HX8238A_TOUCH) += $(G2_KMOD_DIR)/video/pcap7200.ko

ifeq ($(CONFIG_PANEL_HX8238A_TOUCH),y)
  LCD_TOUCH=CONFIG_PANEL_HX8238A_TOUCH
endif

ifeq ($(CONFIG_PANEL_HX8238A_TOUCH),m)
  LCD_TOUCH=CONFIG_PANEL_HX8238A_TOUCH=m
endif

g2-lcd-modules-m := $(foreach m,$(g2-lcd-files-m),$(shell basename $(m) .ko))

g2-lcd-depends:=
ifneq ($(CONFIG_KERNEL_HDMI_ANX9805),)
  g2-lcd-depends += +kmod-g2-i2c
endif
ifneq ($(CONFIG_KERNEL_PANEL_HX8238A),)
  g2-lcd-depends += +kmod-g2-spi
endif

# Make it select kmod-g2-spi just in case hx8838a is chosen
define KernelPackage/g2-lcd
  SUBMENU:=G2 LCD Support
  TITLE:=Driver for LCD controller
  DEPENDS:=$(g2-lcd-depends)
  KCONFIG:=CONFIG_FB=y CONFIG_FB_CS752X_CLCD $(LCD_TOUCH) \
	CONFIG_FB_CFB_FILLRECT \
	CONFIG_FB_CFB_COPYAREA \
	CONFIG_FB_CFB_IMAGEBLIT
  FILES:=$(g2-lcd-files-m)
  AUTOLOAD:=$(call AutoLoad,50,$(g2-lcd-modules-m))
endef

define KernelPackage/g2-lcd/description
  Driver for on-chip ARM PrimeCell PL111 colour LCD controller
endef

define KernelPackage/g2-lcd/config
  source "$(SOURCE)/modules/Config-lcd.in"
endef

$(eval $(call KernelPackage,g2-lcd))

# Real time clock

define KernelPackage/g2-rtc
  SUBMENU:=G2 RTC Support
  TITLE:=G2 real time clock support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_RTC_CLASS=y CONFIG_RTC_DRV_G2
  FILES:=$(G2_KMOD_DIR)/rtc/rtc-cs75xx.ko
  AUTOLOAD:=$(call AutoLoad,50,rtc-g2)
endef

define KernelPackage/g2-rtc/description
  Support for the RTC found on the GoldenGate Soc.
endef

$(eval $(call KernelPackage,g2-rtc))

# SD card reader

define KernelPackage/g2-sd
  SUBMENU:=G2 SD card
  TITLE:=G2 SD card reader support
  DEPENDS:=@TARGET_g2||TARGET_ag2 +kmod-mmc
  KCONFIG:=CONFIG_MMC=y CONFIG_MMC_BLOCK=y CONFIG_MMC_TEST=n \
	CONFIG_CS752X_SD
  FILES:=$(G2_KMOD_DIR)/mmc/host/cs752x_sd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,cs752x_sd)
endef

define KernelPackage/g2-sd/config
	source "$(SOURCE)/modules/Config-sd.in"
endef

define KernelPackage/g2-sd/description
  Goldengate SD card reader drivers.
endef

$(eval $(call KernelPackage,g2-sd))

# SPI Master

define KernelPackage/g2-spi
  SUBMENU:=G2 SPI Support
  TITLE:=G2 SPI master support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_SPI=y CONFIG_SPI_MASTER=y CONFIG_SPI_CS75XX
  FILES:=$(G2_KMOD_DIR)/spi/spi_cs75xx.ko
  AUTOLOAD:=$(call AutoLoad,50,spi_cs75xx)
endef

define KernelPackage/g2-spi/description
  This enables using the SPI master controller on the CS75XX chips.
endef

$(eval $(call KernelPackage,g2-spi))

# Transport stream

define KernelPackage/g2-ts
  SUBMENU:=G2 Media Support
  TITLE:=G2 transport stream support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_MEDIA_SUPPORT=y CONFIG_DVB_CORE \
	CONFIG_DVB_CAPTURE_DRIVERS=y \
	CONFIG_DVB_CS75XX_TS
  FILES:=$(LINUX_DIR)/drivers/media/dvb/dvb-core/dvb-core.ko \
	$(G2_KMOD_DIR)/media/dvb/cs752x-ts/cs75xx_ts.ko
  AUTOLOAD:=$(call AutoLoad,50,dvb-core cs75xx_ts)
endef

define KernelPackage/g2-ts/description
  Since this module have no MPEG decoder onboard, they transmit
  only compressed MPEG data over the AXI bus, so you need
  an external software decoder to watch TV on your computer.
endef

define KernelPackage/g2-ts/config
	menu "Configuration"
		depends on PACKAGE_kmod-g2-ts

	config KERNEL_DVB_MAX_ADAPTERS
		int "Number of attached adapters"
		default 12
		help
		  The number of attached TS ports.  One
		  Maxlinear daughter card has 4 TS ports.
		  If you have one card, set this to 4.  Two cards,
		  set this to 8.  Three cards, 12.

	config KERNEL_DVB_MXL241SF
		bool "Use with MxL241SF Tuner/Demodulator kernel module"
		select PACKAGE_kmod-mxl241sf-source
		default y
		help
		  If you are using this module along with the MxL241SF, then
		  you should select this option.

	endmenu
endef

$(eval $(call KernelPackage,g2-ts))

# Watchdog timer

define KernelPackage/g2-watchdog
  SUBMENU:=G2 Watchdog
  TITLE:=G2 Watchdog Timer
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_WATCHDOG=y CONFIG_G2_WATCHDOG
  FILES:=$(G2_KMOD_DIR)/watchdog/cs75xx_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,g2_wdt)
endef

define KernelPackage/g2-watchdog/description
  Watchdog timer block in Goldengate chips. This will
  reboot the system when the timer expires with the watchdog
  enabled.
endef

$(eval $(call KernelPackage,g2-watchdog))

# DMA

define KernelPackage/g2-dma
  SUBMENU:=G2 DMA
  TITLE:=G2 DMA Support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_DMA_ENGINE=y CONFIG_CORTINA_G2_ADMA
  FILES:=$(G2_KMOD_DIR)/dma/cs75xx_adma.ko
  AUTOLOAD:=$(call AutoLoad,50,cs75xx_adma)
endef

define KernelPackage/g2-dma/description
  Enable support for the Cortina-Systems(R) CS75xx Series built-in DMA engines.
endef

$(eval $(call KernelPackage,g2-dma))

# Serial port

define KernelPackage/g2-serial
  SUBMENU:=G2 Serial Port
  TITLE:=G2 Serial Support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_SERIAL_CORE=y CONFIG_SERIAL_CORE_CONSOLE=y \
	CONFIG_SERIAL_CORTINA
  FILES:=$(G2_KMOD_DIR)/serial/serial_cortina.ko
  AUTOLOAD:=$(call AutoLoad,50,serial_cortina)
endef

define KernelPackage/g2-serial/description
  If you have a board based on a Cortina, you
  can enable its onboard serial port by enabling this option.
endef

define KernelPackage/g2-serial/config
	menu "Configuration"
		depends on PACKAGE_kmod-g2-serial

	config KERNEL_SERIAL_CORTINA_CONSOLE
		bool "Console on CORTINA serial port"
		default y

		help
		  If you have enabled the serial port on the Cortina baseboard
		  you can make it the console by answering Y to this option.

		  Even if you say Y here, the currently visible virtual console
		  (/dev/tty0) will still be used as the system console by default, but
		  you can alter that using a kernel command line option such as
		  "console=ttyS0". (Try "man bootparam" or see the documentation of
		  your boot loader (lilo or loadlin) about how to pass options to the
		  kernel at boot time.)
	endmenu
endef

$(eval $(call KernelPackage,g2-serial))

define KernelPackage/usb-power-management
  SUBMENU:=USB Support
  TITLE:=USB Power Management
  DEPENDS:=+kmod-usb-core +kmod-usb-hid
  KCONFIG:=CONFIG_USB_SUSPEND=y
endef

define KernelPackage/usb-power-management/description
 USB "remote wakeup" signaling is supported, whereby some 
 USB devices (like keyboards and network adapters) can wake up
 their parent hub.  That wakeup cascades up the USB tree, and
 could wake the system from states like suspend-to-RAM.
endef

$(eval $(call KernelPackage,usb-power-management))

define KernelPackage/usb-uvc
  SUBMENU:=USB Support
  TITLE:=USB UVC
  DEPENDS:=+kmod-usb-core +kmod-usb-hid
  KCONFIG:=CONFIG_MEDIA_SUPPORT=y CONFIG_USB_VIDEO_CLASS=y \
	   CONFIG_V4L_USB_DRIVERS=y CONFIG_VIDEO_CAPTURE_DRIVERS=y \
	   CONFIG_VIDEO_DEV=y CONFIG_VIDEO_MEDIA=y \
	   CONFIG_VIDEO_V4L2=y CONFIG_VIDEO_V4L2_COMMON=y
endef

define KernelPackage/usb-uvc/description
 USB UVC supported.
endef

$(eval $(call KernelPackage,usb-uvc))

define KernelPackage/g2-vfs-fastpath
  SUBMENU:=G2 Networking
  TITLE:=Cortina VFS support
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_VFS_FASTPATH=y
endef

define KernelPackage/g2-vfs-fastpath/description
     Allows large packets to be delivered to the
     TCP stack for VFS
endef

$(eval $(call KernelPackage,g2-vfs-fastpath))

define KernelPackage/g2-smb-tuning
  SUBMENU:=G2 Networking
  TITLE:=Cortina Samba Performance Tuning
  DEPENDS:=@TARGET_g2||TARGET_ag2
  KCONFIG:=CONFIG_SMB_TUNING=y
endef

define KernelPackage/g2-smb-tuning/description
     Enhance Samba Read/Write Performance
endef

$(eval $(call KernelPackage,g2-smb-tuning))
