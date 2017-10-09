This directory is designed to hold Non-OpenWRT specific patches for Cortina Reference boards ONLY.
Customers are discouraged from adding patches to or modify this directory as it will get overwritten
in subsequent Cortina releases. Please use the custom-board-2.6.36 directory for this purpose instead.

This directory is picked up and applied by the OpenWRT build process BEFORE
both any Custom Board patches found in custom-board-2.6.36 and then OpenWRT specific patches
found in openwrt-specific-2.6.36.

In summary, both Custom Board patches and OpenWRT specific patches should be kept out of this directory. 

