This directory is designed to hold OpenWRT specific patches for Cortina Reference boards ONLY.
Customer are discouraged from adding patches to or modify this directory as it will get overwritten
in subsequent Cortina releases. Please use the custom-board-2.6.36 directory for this purpose instead.

This directory is picked up and applied by the OpenWRT build process AFTER
both NON-OpenWRT specific Linux are applied from patches-2.6.36 and then any Custom Board
patches found in custom-board-2.6.36.

In summary, both NON-OpenWRT specific Linux and Custom Board patches should be kept out of this directory. 
Any patches found in this directory which are determined to be NON-OpenWRT specific 
in nature should be migrated to patches-2.6.36.

