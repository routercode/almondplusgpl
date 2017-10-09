# We'll place a buildid string in the image files.  When this
# make is launched from the cortina build server, BS_BUILDID
# will be set to the build id for this build.  Otherwise, a
# string based on the date will be used.
BS_BUILDID ?= $(shell date "+%s")
BUILDID=`perl -e '$$$$ARGV[0] =~ s/^.+-//g; print $$$$ARGV[0],"\n";' $(BS_BUILDID)`
define f_strtoint
    ver="$(1)" ;\
    list=$$$${ver//\./ } ;\
    count=0 ;\
    val=0  ;\
    for i in $$$$list; do count=$$$$(($$$$count + 1)); done ;\
    for i in $$$$list; do \
	count=$$$$(( $$$$count - 1 ))  ;\
	let "val |= (i<<(8*count))"  ;\
    done; \
    echo $$$$val
endef
define strtoint
  $(call f_strtoint,$(1))
endef
TOOL_VERSION=`$(call strtoint, $(CONFIG_G2_BUILDID_TOOL_VERSION))`
SW_VERSION=`$(call strtoint, $(CONFIG_G2_BUILDID_SW_VERSION))`
CHIP_ID=$(CONFIG_G2_BUILDID_CHIP_ID)
BOARD_ID=$(CONFIG_G2_BUILDID_BOARD_ID)
# Functions used to create flash images
#
# cs_fwrite_sector( input-file, sector-number, bytes-to-write(in Kb) )
define cs_fwrite_sector
	fsize=`stat -c%s $(1)`; \
	ssize=$(call cs_times,1024,$(3)); \
	willitfit=`expr $$$$fsize \<= $$$$ssize`; \
	if [ $$$$willitfit -eq 0 ]; then \
	  echo "$(1) is too big to fit into a sector of size $(strip $(3))Kb"; /bin/false; \
	else \
	  dd if=$(1) of=$(BIN_DIR)/flash.bin bs=1K seek=$(call cs_div,$(call sector_offset,$(2)),1024) count=$(3) conv=notrunc; \
	fi
endef

# mk_uboot_env( <args> )
#  where <args> is a string of var=value
define mk_uboot_env
	mkdir -p $(BIN_DIR)/scratch
	perl expand-tmpl.pl $(1) < uboot-env.tmpl > $(BIN_DIR)/scratch/uboot-env.sh
endef

# Given a sector number, calculate byte offset.  Returns a DEC.
define sector_offset
$(call cs_times,$(1),$(call cs_times,1024,$(SECTOR_SIZE)))
endef

define sector_offset_hex
$(call cs_dec2hex,$(call cs_times,$(1),$(call cs_times,1024,$(SECTOR_SIZE))))
endef

# Some common math functions
define cs_numk
$(shell expr $(1) \* 1024)
endef

define cs_times
$(shell expr $(1) \* $(2))
endef

define cs_div
$(shell expr $(1) / $(2))
endef

define cs_plus
$(shell expr $(1) + $(2))
endef

define cs_minus
$(shell expr $(1) - $(2))
endef

define cs_hex2dec
$(shell printf "%d" $(1))
endef

define cs_dec2hex
$(shell printf "0x%x" $(1))
endef

# Return RAM absolute address
define cs_ram_addr
$(call cs_dec2hex,$(call cs_plus,$(call cs_hex2dec,$(RAM_BASE)),$(1)))
endef

# Return FLASH absolute address
define cs_flash_addr
$(call cs_dec2hex,$(call cs_plus,$(call cs_hex2dec,$(FLASH_BASE)),$(1)))
endef

# Create the common part of flash.bin.  This includes u-boot 0 and 1,
# u-boot environments for 0 and 1, and the rootfs_data overlays for
# 0 and 1.  These parts are common no matter how you choose to store
# the kernel and rootfs images for management.
#
define create_flash_common
	$(RM) $(BIN_DIR)/flash.bin
	$(RM) -rf $(BIN_DIR)/scratch
	# Initialize flash image
	$(call cs_fwrite_sector,/dev/zero,0,$(FLASH_SIZE))

	# Write u-boot into flash twice; for u-boot0 and u-boot1
	$(call cs_fwrite_sector,$(BIN_DIR)/$(UBOOT_FILE),$(UBOOT0_SECTOR),$(UBOOT0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/$(UBOOT_FILE),$(UBOOT1_SECTOR),$(UBOOT1_SIZE))

	# Create the u-boot environment from a template
	if [ -z "$(ROOTFS_RAM_COPY_OFFSET)" ]; then \
	  ROOTFS_RAM_COPY_OFFSET=0 ;\
	fi
	$(call mk_uboot_env,\
	  flash_name=$(FLASH_NAME) \
	  basic_args=$(BASIC_BOOTARGS) \
	  ethaddr1=$(CONFIG_CS_UBOOT_ETHADDR1) \
	  ethaddr2=$(CONFIG_CS_UBOOT_ETHADDR2) \
	  ethaddr=$(CONFIG_CS_UBOOT_ETHADDR0) \
	  wifiaddr0=$(CONFIG_CS_UBOOT_WIFIADDR0) \
	  wifiaddr1=$(CONFIG_CS_UBOOT_WIFIADDR1) \
	  ipaddr=$(CONFIG_CS_UBOOT_IPADDR) \
	  netmask=$(CONFIG_CS_UBOOT_NETMASK) \
	  serverip=$(CONFIG_CS_UBOOT_SERVERIP) \
	  gatewayip=$(CONFIG_CS_UBOOT_GATEWAYIP) \
          mtd_args=$(CONFIG_CS_UBOOT_MTDPARTS_ARGS) \
	  rootfs_args=$(CONFIG_CS_UBOOT_ROOTFS_ARGS) \
	  more_args=$(CONFIG_CS_UBOOT_MORE_ARGS) \
	  kernel_ram_cp=$(call cs_ram_addr,$(call cs_hex2dec,$(KERNEL_RAM_COPY_OFFSET))) \
	  rootfs_ram_cp=$(call cs_ram_addr,$(call cs_hex2dec,$(ROOTFS_RAM_COPY_OFFSET))) \
	  sb0_addr=$(call cs_flash_addr,$(call sector_offset,$(SB0_SECTOR))) \
	  sb1_addr=$(call cs_flash_addr,$(call sector_offset,$(SB1_SECTOR))) \
	  qm_init_buff=0 \
	  ni_napi_budget=16 \
	  qm_acp_enable=0 \
	  ni_rx_noncache=0 \
	)

	# Use the 'uboot-flashbin-setenv' utility to write the
	# u-boot environment to the right place in the flash image.
	# Do it twice; for u-boot0 and u-boot1
	uboot-flashbin-setenv -binfile $(BIN_DIR)/flash.bin \
	  -envoffset $(call cs_dec2hex,$(call sector_offset,$(UBOOT0_ENV_SECTOR))) \
	  -envsize $(UBOOT_ENV_SIZE) \
	  -setenv $(BIN_DIR)/scratch/uboot-env.sh
	uboot-flashbin-setenv -binfile $(BIN_DIR)/flash.bin \
	  -envoffset $(call cs_dec2hex,$(call sector_offset,$(UBOOT1_ENV_SECTOR))) \
	  -envsize $(UBOOT_ENV_SIZE) \
	  -setenv $(BIN_DIR)/scratch/uboot-env.sh

	dd if=/dev/zero of=$(BIN_DIR)/uboot-env.bin bs=1 count=$(call cs_hex2dec, $(UBOOT_ENV_SIZE))
	uboot-flashbin-setenv -binfile $(BIN_DIR)/uboot-env.bin \
	  -envoffset 0 \
	  -envsize $(UBOOT_ENV_SIZE) \
	  -setenv $(BIN_DIR)/scratch/uboot-env.sh

	# Create a JFFS2 file system for "rootfs_data", the OpenWRT persistent
	# overlay (mini_fo) partition.
	if [ ! -z "$(ROOTFS_DATA_SIZE)" ]; then \
	  mkdir -p $(BIN_DIR)/scratch/overlay/etc ;\
	  echo "/dev/mtd0 0x0 $(UBOOT_ENV_SIZE) $(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024))" > \
	    $(BIN_DIR)/scratch/overlay/etc/fw_env.config ;\
	  mkfs.jffs2 --pad --little-endian --squash -v -m none \
	    --eraseblock=$(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024)) \
	    --root=$(BIN_DIR)/scratch/overlay -o $(BIN_DIR)/scratch/rootfs_data.jffs2 ;\
	  $(call cs_fwrite_sector,$(BIN_DIR)/scratch/rootfs_data.jffs2,$(ROOTFS_DATA_SECTOR),$(ROOTFS_DATA_SIZE)) ;\
	fi

	# Create a JFFS2 file system for "rootfs_log", the OpenWRT persistent
	# overlay (mini_fo) partition.
	if [ ! -z "$(ROOTFS_LOG_SIZE)" ]; then \
	  mkdir -p $(BIN_DIR)/scratch/log ;\
	  echo "This directory is used for logging purposes.  Do not remove.">$(BIN_DIR)/scratch/log/README;\
	  mkfs.jffs2 --pad --little-endian --squash -v -m none \
	    --eraseblock=$(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024)) \
	    --root=$(BIN_DIR)/scratch/log -o $(BIN_DIR)/scratch/rootfs_log.jffs2 ;\
	  $(call cs_fwrite_sector,$(BIN_DIR)/scratch/rootfs_log.jffs2,$(ROOTFS_LOG_SECTOR),$(ROOTFS_LOG_SIZE)) ;\
	fi

	# Create a scratch area
	mkdir -p $(BIN_DIR)/scratch/jffs-root/bank0
	mkdir -p $(BIN_DIR)/scratch/jffs-root/bank1

	# Copy the kernel uImage into the two banks
	cp $(BIN_DIR)/$(KERNEL_FILE) $(BIN_DIR)/scratch/jffs-root/bank0/uImage
	cp $(BIN_DIR)/$(KERNEL_FILE) $(BIN_DIR)/scratch/jffs-root/bank1/uImage

	# Make the u-image for the rootfs, store in bank0
	if [ ! -z "$(ROOTFS_FILE)" ]; then \
	  cp $(BIN_DIR)/$(ROOTFS_FILE) $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img; \
	  chmod oug+rw $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img; \
	  mkimage -A arm -T ramdisk -C none -n 'Root Filesystem' \
		-d $(BIN_DIR)/$(ROOTFS_FILE) -O linux \
		-a $(call cs_ram_addr,$(call cs_hex2dec,$(ROOTFS_RAM_IMAGE_OFFSET))) \
		-e $(call cs_ram_addr,$(call cs_hex2dec,$(ROOTFS_RAM_IMAGE_OFFSET))) \
		$(BIN_DIR)/scratch/rootfs.uimg ;\
	fi

	# Make the u-image for the rootfs, store in bank1
	if [ ! -z "$(ROOTFS_FILE)" ]; then \
	  cp $(BIN_DIR)/$(ROOTFS_FILE) $(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img; \
	  chmod oug+rw $(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img; \
	fi

endef

# This function writes common information into the super
# blocks; sb0 and sb1.  This includes information on
# the u-boot environment to expose to the kernel, the
# two super-blocks themselves, and the rootfs_data overlay.
#
define create_sb_common
	# Create super-block0 and write the information that
	# is common between the "jffs/parts" method of storing
	# kernel and rootfs
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --create \
	  --valid --active \
	  --mtdpart --name=uboot-env --what=data --type=data \
	  --size=$(strip $(UBOOT0_ENV_SIZE))K \
	  --offset=$(call sector_offset_hex,$(UBOOT0_ENV_SECTOR)) \
	  --mtdpart --name=sb0 --what=data --type=data \
	  --size=$(strip $(SB0_SIZE))K \
	  --offset=$(call sector_offset_hex,$(SB0_SECTOR)) \
	  --mtdpart --name=sb1 --what=data --type=data \
	  --size=$(strip $(SB1_SIZE))K \
	  --offset=$(call sector_offset_hex,$(SB1_SECTOR))
	if [ ! -z "$(ROOTFS_DATA_SIZE)" ]; then \
	  cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin \
	    --mtdpart --name=rootfs_data --what=data --type=jffs2 \
	    --size=$(strip $(ROOTFS_DATA_SIZE))K --offset=$(call sector_offset_hex,$(ROOTFS_DATA_SECTOR)) ;\
	fi
	if [ ! -z "$(ROOTFS_LOG_SIZE)" ]; then \
	  cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin \
	    --mtdpart --name=rootfs_log --what=fs --mountpoint=/log --type=jffs2 \
	    --size=$(strip $(ROOTFS_LOG_SIZE))K --offset=$(call sector_offset_hex,$(ROOTFS_LOG_SECTOR)) ;\
	fi

	# Create super-block1 and write the information that
	# is common between the "jffs/parts" method of storing
	# kernel and rootfs
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --create \
	  --valid \
	  --mtdpart --name=uboot-env --what=data --type=data \
	  --size=$(strip $(UBOOT1_ENV_SIZE))K \
	  --offset=$(call sector_offset_hex,$(UBOOT1_ENV_SECTOR)) \
	  --mtdpart --name=sb0 --what=data --type=data \
	  --size=$(strip $(SB0_SIZE))K \
	  --offset=$(call sector_offset_hex,$(SB0_SECTOR)) \
	  --mtdpart --name=sb1 --what=data --type=data \
	  --size=$(strip $(SB1_SIZE))K \
	  --offset=$(call sector_offset_hex,$(SB1_SECTOR))
	if [ ! -z "$(ROOTFS_DATA_SIZE)" ]; then \
	  cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin \
	    --mtdpart --name=rootfs_data --what=data --type=jffs2 \
	    --size=$(strip $(ROOTFS_DATA_SIZE))K --offset=$(call sector_offset_hex,$(ROOTFS_DATA_SECTOR)) ;\
	fi
	if [ ! -z "$(ROOTFS_LOG_SIZE)" ]; then \
	  cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin \
	    --mtdpart --name=rootfs_log --what=fs --mountpoint=/log --type=jffs2 \
	    --size=$(strip $(ROOTFS_LOG_SIZE))K --offset=$(call sector_offset_hex,$(ROOTFS_LOG_SECTOR)) ;\
	fi
endef

# cs_sbwrite_mtd( sb-file, name, what, type, sector, size, options, path )
define cs_sbwrite_mtd
	cs.mksb --sb=$(1) --mtdpart \
	  --name=$(2) --what=$(3) --type=$(4) \
	  --size=$(strip $(6))K --offset=$(call sector_offset_hex,$(5)) \
	  $(7) $(if $(8),--path=$(8),)
endef

# cs_sbwrite_file( sb-file, root, name, what, type, options, path )
define cs_sbwrite_file
	cs.mksb --sb=$(1) --file --root=$(2) \
	  --name=$(3) --what=$(4) --type=$(5) \
	  $(6) $(if $(7),--path=$(7),)
endef

