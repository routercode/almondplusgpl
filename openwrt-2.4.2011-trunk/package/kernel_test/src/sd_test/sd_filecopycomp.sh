#!/bin/sh
#
#  sd_filecopycomp.sh
#
# Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# Goldengate bash script for sd testing
#
# usage:
# If you select sequenctial pattern file, type
#     sd_filecopycomp.sh sequence
# else type
#     sd_filecopycomp.sh
#
# need program:  createTestFiles
#     Please contact aaron Tseng.

export ROOTDIR=
export TESTDIR=testdir
export SOURCEDIR=${ROOTDIR}/tmp/${TESTDIR}
export TARGETDIR=${ROOTDIR}/mnt/${TESTDIR}
export SDDEV=mmcblk0p1
export SDDEVNODE=/dev/$SDDEV
export _FILESIZE=5
if [ x$1 = x"sequence" ]; then
    export FILESIZE=$_FILESIZE
    export FILENAME=sequence.${FILESIZE}M
else
    export FILESIZE=${_FILESIZE}M
    export FILENAME=urandom.${FILESIZE}
fi

echo "SOURCEDIR=$SOURCEDIR"
echo "TARGETDIR=$TARGETDIR"

set -x
# Check device exist?
if [ ! -b ${SDDEVNODE} ] ; then
    echo "${SDDEVNODE} device node doesn't exist"
    return
fi

# Prepare
if [ ! -d ${SOURCEDIR} ] ; then
    mkdir -p ${SOURCEDIR}
fi

if [ ! -d ${TARGETDIR} ] ; then
    mkdir -p ${TARGETDIR}
fi

# Umount the $TARGETDIR point
mount | grep "$SDDEV"
if [ x"$?" = x"0" ]; then
    umount $TARGETDIR
fi

# All new and startup here
# Format the $SDDEVNODE device node
mke2fs $SDDEVNODE
if [ ! x"$?" = x"0" ]; then
    echo "mke2fs file system failed"
    return
fi

# Mount the $TARGETDIR point
mount $SDDEVNODE ${TARGETDIR}
if [ ! x"$?" = x"0" ]; then
    echo "mount failed"
    exit 1
fi

# Prepare urandom data
if [ x$1 = x"sequence" ]; then
    PWD=$PWD
    cd ${SOURCEDIR}
    createTestFiles $FILENAME $FILESIZE
    cd $PWD
else
    dd if=/dev/urandom of=${SOURCEDIR}/${FILENAME} bs=${FILESIZE} count=1
fi

# copy data from /tmp to /mnt (sd card)
cp ${SOURCEDIR}/${FILENAME} ${TARGETDIR}/${FILENAME}
if [ ! x"$?" = x"0" ]; then
    echo "copy file failed"
    return
fi
sync
if [ ! x"$?" = x"0" ]; then
    echo "sync file failed"
    return
fi

# Remount the $TARGETDIR point
umount ${TARGETDIR}
mount $SDDEVNODE ${TARGETDIR}
if [ ! x"$?" = x"0" ]; then
    echo "mount failed"
    exit 1
fi

# differ data
diff -rq ${SOURCEDIR}/${FILENAME} ${TARGETDIR}/${FILENAME}
if [ x"$?" = x"0" ]; then
    echo "diff ok"
else
    echo "diff NOT ok"
fi

# Remount the $TARGETDIR point
umount ${TARGETDIR}
mount $SDDEVNODE ${TARGETDIR}
if [ ! x"$?" = x"0" ]; then
    echo "mount failed"
    exit 1
fi

# compare data
cmp -l ${SOURCEDIR}/${FILENAME} ${TARGETDIR}/${FILENAME}
if [ x"$?" = x"0" ]; then
    echo "cmp ok"
else
    echo "cmp NOT ok"
fi

# test done
umount ${TARGETDIR}
set +x
