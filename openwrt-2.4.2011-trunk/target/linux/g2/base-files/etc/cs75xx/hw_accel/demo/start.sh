#!/bin/sh
# Copyright (C) 2008-2010 OpenWrt.org
SCRIPT_DIR=/etc/cs75xx/hw_accel/demo

if [ ! -e $SCRIPT_DIR/mode_id ]; then
	echo "mode_id not exist, use default (-1)."
	echo -1 > $SCRIPT_DIR/mode_id
	chmod +x $SCRIPT_DIR/mode_id
fi

cnt=0
while read line
do
if [ "$line" != "" ]; then
value=$line
fi
cnt=$(($cnt+1))
done < $SCRIPT_DIR/mode_id

$SCRIPT_DIR/check_cs_wfo_mode.sh $value
SCRIPT_RET=$?
if [ "$SCRIPT_RET" == "1" ]; then
    echo "================================================================================"
    echo "ERROR: There is a mismatch between the mode id and the QM_INT_BUFF."
    fw_setenv QM_INT_BUFF 0
    echo "PKT_BUFF has been disabled for QM, please reboot."
    echo "If it still has problem after system reboot, please upgrade uboot and uboot_env."
    echo "================================================================================"
    exit 1
elif [ "$SCRIPT_RET" == "2" ]; then
    echo "================================================================================"
    echo "ERROR: There is a mismatch between the mode id and the QM_INT_BUFF."
    fw_setenv QM_INT_BUFF 256
    echo "PKT_BUFF has been enabled for QM, please reboot."
    echo "If it still has problem after system reboot, please upgrade uboot and uboot_env."
    echo "================================================================================"
    exit 1
fi

$SCRIPT_DIR/setup_cs_wfo_mode.sh $value
