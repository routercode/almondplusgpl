# HOW THIS WORKS:
#
# u-boot runs "sb_process" to decide which super block to use for
# booting (based in the valid,commit and active flags).  Once it
# picks a SB, it'll grab THE FIRST kernel it sees and THE FIRST
# rootfs it sees as the images to boot from.  These two entries
# (the first two) should be marked "--dont_export".  The second
# two entries, because they are not marked "--dont_export" will
# end up in the mtdparts partition table that is exported to
# the kernel.  The kernel can THUS write them, and will during
# a system upgrade ... so if you're booting from 0th images,
# you want to expose the 1th images for upgrade.  And if you're
# booting from the 1th images, you want to expose the 0th images
# for upgrade.
#

define Image/Flash
	# create the flash
	$(call create_flash_common)
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank0/uImage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank1/uImage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE))
	# create the superblocks
	$(call create_sb_common)
	# sb0
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,rootfs,rootfs,uimage,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel_standby,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,rootfs_standby,rootfs,uimage,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel+rootfs,data,other,$(KERNEL1_SECTOR),$(call cs_plus,$(KERNEL1_SIZE),$(ROOTFS1_SIZE)),,)
	# sb1
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,rootfs,rootfs,uimage,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel_standby,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,rootfs_standby,rootfs,uimage,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel+rootfs,data,other,$(KERNEL0_SECTOR),$(call cs_plus,$(KERNEL0_SIZE),$(ROOTFS0_SIZE)),,)
	# write super blocks to flash
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb0.bin,$(SB0_SECTOR),$(SB0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb1.bin,$(SB1_SECTOR),$(SB1_SIZE))
endef

define uboot-http-dl-img
	python uboot-dl-gen  -n $(1) -a $(call cs_flash_addr, $(call sector_offset,$2)) -s $(call cs_times, $(call cs_hex2dec, $3), 1024)  -i $4 -m 0 -r 0 -o $(BIN_DIR)/uboot-http-dl/$5
endef

define Image/SWDL/uboot
	rm -rf $(BIN_DIR)/uboot-http-dl
	mkdir $(BIN_DIR)/uboot-http-dl

	# bootloader
	$(call uboot-http-dl-img, "bootloader", $(UBOOT0_SECTOR), $(UBOOT0_SIZE), $(BIN_DIR)/bootloader.bin,loader.bin)
	$(call uboot-http-dl-img, "uboot-env", $(UBOOT0_ENV_SECTOR), $(UBOOT0_ENV_SIZE), $(BIN_DIR)/uboot-env.bin,loader.bin)

	# rootfs+kenrel
	$(call uboot-http-dl-img, "sb0", $(SB0_SECTOR), $(SB0_SIZE), $(BIN_DIR)/scratch/sb0.bin,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb1", $(SB1_SECTOR), $(SB1_SIZE), $(BIN_DIR)/scratch/sb1.bin,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "kernel", $(KERNEL0_SECTOR), $(KERNEL0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/uImage,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "rootfs", $(ROOTFS0_SECTOR), $(ROOTFS0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img,kernel-rootfs.bin)

	# all  
	$(call uboot-http-dl-img, "bootloader", $(UBOOT0_SECTOR), $(UBOOT0_SIZE), $(BIN_DIR)/bootloader.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "uboot-env", $(UBOOT0_ENV_SECTOR), $(UBOOT0_ENV_SIZE), $(BIN_DIR)/uboot-env.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb0", $(SB0_SECTOR), $(SB0_SIZE), $(BIN_DIR)/scratch/sb0.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb1", $(SB1_SECTOR), $(SB1_SIZE), $(BIN_DIR)/scratch/sb1.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "kernel", $(KERNEL0_SECTOR), $(KERNEL0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/uImage,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "rootfs", $(ROOTFS0_SECTOR), $(ROOTFS0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img,loader-kernel-rootfs.bin)
endef


define Image/SWDL
	# Just the kernel
	cs.mkimage -image $(BIN_DIR)/kernel-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage

	# Just the root file system
	cs.mkimage -image $(BIN_DIR)/rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	# Both the kernel and root file system
	cs.mkimage -image $(BIN_DIR)/kernel-rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	# The kernel+rootfs combined image
	$(RM) -f $(BIN_DIR)/scratch/k+r.bin
	dd if=$(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	   of=$(BIN_DIR)/scratch/k+r.bin \
	   bs=1K seek=0 \
	   count=$(KERNEL0_SIZE) conv=notrunc
	dd if=$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img \
	   of=$(BIN_DIR)/scratch/k+r.bin \
	   bs=1K seek=$(call cs_div,$(call sector_offset,$(call cs_minus,$(ROOTFS0_SECTOR),$(KERNEL0_SECTOR))),1024) \
	   count=$(ROOTFS0_SIZE) conv=notrunc
	cs.mkimage -image $(BIN_DIR)/kernel+rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel+rootfs \
	  -destination mtd:kernel+rootfs \
	  -payload $(BIN_DIR)/scratch/k+r.bin

	$(call Image/SWDL/uboot)
endef
