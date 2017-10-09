#!/bin/sh
#
#  test2part.sh
#
# Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# test two partitions for mass storage devices
#
# usage:
# Select specific device.config
#     test2part.sh <mydevice.cfg>
# else type (default: sd.cfg)
#     test2part.sh
#
# requirement program:
#   gensequfile
#     to generate sequence file
#   cmp
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Editors ==> cmp
#   diff
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Editors ==> diff
#   date (optional)
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Coreutils ==> date ==> Support weird 'date MMDDhhmm[[YY]YY][.SS]' format
#   fdisk
#     make menuconfig ==> Utilities ==> disc ==> fdisk
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Linux System Utilities ==> fdisk
#   mke2fs
#     make menuconfig ==> Utilities ==> Filesystem ==> e2fsprogs
#   mkfs.vfat
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Linux System Utilities ==> mkfs_vfat
#
# Sample Configuration:
#   Config1:
#    Only two partition: source and target1 partitions in device
#    Format two partition, prepare two files of size 10MB in source and target1 partition respectively.
#      FORMAT=yes, GENFILE=yes, SUPPORTMANYTARGET=no, _FILESIZE=10
#  
#   Config2:
#    Only two partition: source and target1 partitions in device
#    Don't format these partition. Don't prepare two files of size 10MB in source and target1 partition respectively.
#      FORMAT=no, GENFILE=no, SUPPORTMANYTARGET=no, _FILESIZE=10
#  
#   Config3:
#    Three partition: source, target1 and target2 partitions in device
#    Don't format these partitions, prepare two files of size 20MB in source partition.
#      FORMAT=no, GENFILE=yes, SUPPORTMANYTARGET=yes, _FILESIZE=20, TARGET2ISRAM=no
#  
#   Config4:
#    Two partition: source and target1 partitions in device
#    Prepare a ram space (/tmp) for file copying. (TARGET2 is in ram fs)
#    Don't format these partitions. Don't prepare two files of size 20MB in source partition.
#      FORMAT=no, GENFILE=no, SUPPORTMANYTARGET=yes, _FILESIZE=20, TARGET2ISRAM=yes
#  
#   Config5:
#    Suposes the device nodes of usb disk are /dev/sda1, /dev/sda2
#    Two partition: source and target1 partitions in device
#    Prepare a ram space (/tmp) for file copying. (TARGET2 is in ram fs)
#    Don't format these partitions. Don't prepare two files of size 20MB in source partition.
#      FORMAT=no, GENFILE=no, SUPPORTMANYTARGET=yes, _FILESIZE=20, TARGET2ISRAM=yes,
#      SRCDEV=sda1, TAR1DEV=sda2
#

# default configuration:
export QUIET=no
export VERBOSE=no
export FORMAT=no
export GENFILE=yes
export SLEEPING=3
export WAITING=7
export SUPPORTMANYTARGET=no
export TARGET2ISRAM=no
export ENDURANCETEST=no
export ITERATIONCOUNT=100
# ITERATIONCOUNT=0 for endless loop test
export REMOUNT=no
export DD_BLOCKSIZE=no
export DUP_SECONDFILE=no
export SECONDFILE_EXT=.2nd
export MISMATCHEXIT=no
export SEQUENCEFILE=no
export VFAT=no
export HDPARM=no

export _FILESIZE=10

export SDDEV=mmcblk0
export SRCDEV=${SDDEV}p1
export TAR1DEV=${SDDEV}p2
export TAR2DEV=${SDDEV}p3

# mass storage dependent configuration
if [ x"$1" = x ]; then
    if [ -f "sd.cfg" ]; then	# default: sd.cfg
        source sd.cfg
        echo "List the configuration:"
        cat $1
        echo ""
    fi
else
    source $1
    echo "List the configuration:"
    cat $1
    echo ""
fi

export SRCNODE=/dev/$SRCDEV
export TAR1NODE=/dev/$TAR1DEV
export TAR2NODE=/dev/$TAR2DEV

export ROOTDIR=
export _SOURCEMNT=mnt
export _TARGET1MNT=${_SOURCEMNT}
if [ x"$TARGET2ISRAM" = x"yes" ]; then
    export _TARGET2MNT=tmp
else
    export _TARGET2MNT=${_SOURCEMNT}
fi

export SOURCEDIR=${ROOTDIR}/$_SOURCEMNT/$SRCDEV
export TARGET1DIR=${ROOTDIR}/$_TARGET1MNT/$TAR1DEV
export TARGET2DIR=${ROOTDIR}/$_TARGET2MNT/$TAR2DEV
echo "SOURCEDIR=$SOURCEDIR"
echo "TARGET1DIR=$TARGET1DIR"
echo "TARGET2DIR=$TARGET2DIR"

# configure the file size
# note: enable swap if filesize > 30M
if [ $_FILESIZE -gt 30 ]; then
    echo "WARNING: enable swap if filesize > 30M. Your filesize=${_FILESIZE}M"
fi

if [ x"$SEQUENCEFILE" = x"yes" ]; then
    export FILESIZE=$_FILESIZE
    export FILENAMEsrc=sequence_src.${FILESIZE}M
    export FILENAMEtar=sequence_tar.${FILESIZE}M
else
    export FILESIZE=${_FILESIZE}M
    export FILENAMEsrc=urandom_src.${FILESIZE}
    export FILENAMEtar=urandom_tar.${FILESIZE}
fi

set -x

# Check if device nodes exist?
    if [ ! -b ${SRCNODE} ] ; then
        echo "${SRCNODE} device node doesn't exist"
        return
    fi
    if [ ! -b ${TAR1NODE} ] ; then
        echo "${TAR1NODE} device node doesn't exist"
        return
    fi
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            if [ ! -b ${TAR1NODE} ] ; then
                echo "${TAR1NODE} device node doesn't exist"
                return
            fi
        fi
    fi

# Umount the $SOURCEDIR/$TARGET1DIR/$TARGET2DIR point
    mount | grep "$SRCDEV"
    if [ $? = 0 ]; then
        umount $SOURCEDIR
    fi
    mount | grep "$TAR1DEV"
    if [ $? = 0 ]; then
        umount $TARGET1DIR
    fi
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            mount | grep "$TAR2DEV"
            if [ $? = 0 ]; then
                umount $TARGET2DIR
            fi
        fi
    fi
    free
    cat /proc/cpuinfo
    cat /proc/interrupts

# format the source/target partition
    if [ x"$FORMAT" = x"yes" ]; then
    #formatsd
        if [ x"$VFAT" = x"yes" ] ; then
            mkfs.vfat $SRCNODE
            mkfs.vfat $TAR1NODE
            if [ ! $?= 0 ]; then
                echo "mkfs.vfat $TAR1NODE file system failed"
                exit 1
            fi
        else
            mke2fs $SRCNODE
            mke2fs $TAR1NODE
            if [ ! $? = 0 ]; then
                echo "mke2fs $TAR1NODE file system failed"
                exit 1
            fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync file failed"
            exit 1
        fi
        if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
            if [ x"$TARGET2ISRAM" = x"no" ]; then
                if [ x"$VFAT" = x"yes" ] ; then
                    mkfs.vfat $TAR2NODE
                    if [ ! $? = 0 ]; then
                        echo "mkfs.vfat $TAR2NODE file system failed"
                        exit 1
                    fi
                else
                    mke2fs $TAR2NODE
                    if [ ! $? = 0 ]; then
                        echo "mke2fs $TAR2NODE file system failed"
                        exit 1
                    fi
                fi
                sync
            fi
        fi
    fi

#create the mount directory of source/target nodes
#createdir
    if [ ! -d ${SOURCEDIR} ] ; then
        mkdir -p ${SOURCEDIR}
    fi
    if [ ! -d ${TARGET1DIR} ] ; then
        mkdir -p ${TARGET1DIR}
    fi
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        if [ ! -d ${TARGET2DIR} ] ; then
            mkdir -p ${TARGET2DIR}
        fi
    fi

# mount the sd card
#mountsd
    mount $SRCNODE ${SOURCEDIR}
    if [ ! $? = 0 ]; then
        echo "$SRCNODE mount failed"
        exit 1
    fi
    mount $TAR1NODE ${TARGET1DIR}
    if [ ! $? = 0 ]; then
        echo "$TAR1NODE mount failed"
        # don't umount for debugging purpose
        #umount ${SOURCEDIR}
        exit 1
    fi
    if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            mount $TAR2NODE ${TARGET2DIR}
            if [ ! $? = 0 ]; then
                echo "$TAR2NODE mount failed"
                # don't umount for debugging purpose
                #umount ${SOURCEDIR}
                #umount ${TARGET1DIR}
                exit 1
            fi
        fi
    fi

# prepare the data for copying
    if [ x"$FORMAT" = x"yes" ]; then
        export GENFILE=yes
    fi

    if [ x"$GENFILE" = x"yes" ]; then
    #preparedata
        if [ x"$SEQUENCEFILE" = x"yes" ]; then
            PWD=$PWD
            cd ${SOURCEDIR}
            gensequfile $FILENAMEsrc $FILESIZE
            cd $PWD
            cd ${TARGET1DIR}
            gensequfile $FILENAMEtar $FILESIZE
            cd $PWD
        else
            free
            if [ x"$DD_BLOCKSIZE" = x"yes" ]; then
                dd if=/dev/urandom of=${SOURCEDIR}/${FILENAMEsrc} bs=${FILESIZE} count=1
            else
                dd if=/dev/urandom of=${SOURCEDIR}/${FILENAMEsrc} count=${_FILESIZE} bs=1M
            fi
            if [ ! $? = 0 ]; then
                echo "create file failed"
                exit 1
            fi
            if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
                if [ x"$DD_BLOCKSIZE" = x"yes" ]; then
                    dd if=/dev/urandom of=${SOURCEDIR}/${FILENAMEtar} bs=${FILESIZE} count=1
                else
                    dd if=/dev/urandom of=${SOURCEDIR}/${FILENAMEtar} count=${_FILESIZE} bs=1M
                fi
                if [ ! $? = 0 ]; then
                    echo "create file failed"
                    exit 1
                fi
            else
                if [ x"$DD_BLOCKSIZE" = x"yes" ]; then
                    dd if=/dev/urandom of=${TARGET1DIR}/${FILENAMEtar} bs=${FILESIZE} count=1
                else
                    dd if=/dev/urandom of=${TARGET1DIR}/${FILENAMEtar} count=${_FILESIZE} bs=1M
                fi
                if [ ! $? = 0 ]; then
                    echo "create file failed"
                    exit 1
                fi
            fi
            free
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync file failed"
            exit 1
        fi
    fi

# copy the data between source and target partitions
#cpdata
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        cp -f ${SOURCEDIR}/${FILENAMEtar} ${TARGET2DIR}/${FILENAMEtar} &
    else
        cp -f ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar} &
    fi
    cp -f ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    if [ ! $? = 0 ]; then
        echo "copy file failed"
        exit 1
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

#cpdata - copy second test file
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            cp -f ${SOURCEDIR}/${FILENAMEtar} ${TARGET2DIR}/${FILENAMEtar}${SECONDFILE_EXT} &
        else
            cp -f ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT} &
        fi
        cp -f ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        if [ ! $? = 0 ]; then
            echo "copy second file failed"
            exit 1
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

# source initiator
  # differ the data
    diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    if [ $? = 0 ]; then
        echo "diff ok"
    else
        echo "diff NOT ok"
        #if [ x"$MISMATCHEXIT" = x"yes" ]; then
        #    exit 1
        #fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  # cmp the data
    cmp -l ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    if [ $? = 0 ]; then
        echo "cmp ok"
    else
        echo "cmp NOT ok"
        if [ x"$MISMATCHEXIT" = x"yes" ]; then
            exit 1
        fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

# source initiator - second test file
  # differ the data
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        if [ $? = 0 ]; then
            echo "diff second file ok"
        else
            echo "diff second file NOT ok"
            #if [ x"$MISMATCHEXIT" = x"yes" ]; then
            #    exit 1
            #fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file file failed"
            exit 1
        fi
    fi

  # cmp the data
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        cmp -l ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        if [ $? = 0 ]; then
            echo "cmp second file ok"
        else
            echo "cmp second file NOT ok"
            if [ x"$MISMATCHEXIT" = x"yes" ]; then
                exit 1
            fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

# Delay a while
    sleep $SLEEPING

# target initiator
  # differ the data
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        diff -rq ${TARGET2DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    else
        diff -rq ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    fi
    if [ $? = 0 ]; then
        echo "diff ok"
    else
        echo "diff NOT ok"
        #if [ x"$MISMATCHEXIT" = x"yes" ]; then
        #    exit 1
        #fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  # cmp the data
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        cmp -l ${TARGET2DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    else
        cmp -l ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    fi
    if [ $? = 0 ]; then
        echo "cmp ok"
    else
        echo "cmp NOT ok"
        if [ x"$MISMATCHEXIT" = x"yes" ]; then
            exit 1
        fi
    fi
    sync; sleep 2; sync; sleep 2; sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

# target initiator - second test file
  # differ the data
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            diff -rq ${TARGET2DIR}/${FILENAMEtar}${SECONDFILE_EXT} ${SOURCEDIR}/${FILENAMEtar}
        else
            diff -rq ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT}
        fi
        if [ $? = 0 ]; then
            echo "diff second file ok"
        else
            echo "diff second file NOT ok"
            #if [ x"$MISMATCHEXIT" = x"yes" ]; then
            #    exit 1
            #fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

  # cmp the data
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            cmp -l ${TARGET2DIR}/${FILENAMEtar}${SECONDFILE_EXT} ${SOURCEDIR}/${FILENAMEtar}
        else
            cmp -l ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT}
        fi
        if [ $? = 0 ]; then
            echo "cmp second file ok"
        else
            echo "cmp second file NOT ok"
            if [ x"$MISMATCHEXIT" = x"yes" ]; then
                exit 1
            fi
        fi
        sync; sleep 2; sync; sleep 2; sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

# test done
    umount ${SOURCEDIR}
    umount ${TARGET1DIR}
    if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            umount ${TARGET2DIR}
        fi
    fi

# hdparm performance test
    if [ x"$HDPARM" = x"yes" ]; then
        hdparm -tT /dev/${SDDEV}
    fi

    free
    cat /proc/interrupts

set +x

if [ x"$ENDURANCETEST" = x"no" ] ; then
    exit 0
fi

echo "press ctrl-c to quit in $WAITING seconds or continue endurance test"
sleep $WAITING

echo "endurance testing... (test loop: $ITERATIONCOUNT, filesize: $_FILESIZE MB)"
#echo "33 seconds per loop @ filesize=10MB, 1 core, 8G SDcard (approach)"
if [ x"$REMOUNT" = x"no" ]; then
    mount $SRCNODE ${SOURCEDIR}
    mount $TAR1NODE ${TARGET1DIR}
    if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            mount $TAR2NODE ${TARGET2DIR}
        fi
    fi
fi

cnt=0
mismatchcnt=0
while(true)
do
    #cat /proc/uptime
    if [ ! x"$QUIET" = x"yes" ]; then
        date
        if [ x"$VERBOSE" = x"yes" ]; then
            echo "Iteration $cnt, cfgfile=$1, SRCDEV=$SRCDEV, TAR1DEV=$TAR1DEV, TAR2DEV=$TAR2DEV"
        else
            echo "Iteration $cnt"
        fi
    fi

# mount device
    if [ x"$REMOUNT" = x"yes" ]; then
        mount $SRCNODE ${SOURCEDIR}
        mount $TAR1NODE ${TARGET1DIR}
        if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
            if [ x"$TARGET2ISRAM" = x"no" ]; then
                mount $TAR2NODE ${TARGET2DIR}
            fi
        fi
    fi

# copy the data between source and target partitions
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        cp -f ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar} &
        cp -f ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}
    else
        cp -f ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar} &
        cp -f ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    fi
    if [ ! $? = 0 ]; then
        echo "copy file failed"
        exit 1
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

# copy the data between source and target partitions - second test file
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            cp -f ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar}${SECONDFILE_EXT} &
            cp -f ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        else
            cp -f ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT} &
            cp -f ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        fi
        if [ ! $? = 0 ]; then
            echo "copy second test file failed"
            exit 1
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second test file failed"
            exit 1
        fi
    fi

# source initiator
  #diffdata
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        diff -rq ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}
    else
        diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    fi
    if [ ! $? = 0 ]; then
        echo "diff NOT ok"
        #if [ x"$MISMATCHEXIT" = x"yes" ]; then
        #    exit 1
        #fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  #cmpdata
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        cmp -l ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}
    else
        cmp -l ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}
    fi
    if [ ! $? = 0 ]; then
        echo "cmp NOT ok"
        if [ x"$MISMATCHEXIT" = x"yes" ]; then
            exit 1
        fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

# source initiator - second test file
  #diffdata
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            diff -rq ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        else
            diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        fi
        if [ ! $? = 0]; then
            echo "diff second file NOT ok"
            #if [ x"$MISMATCHEXIT" = x"yes" ]; then
            #    exit 1
            #fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

  #cmpdata
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            cmp -l ${TARGET1DIR}/${FILENAMEsrc} ${TARGET2DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        else
            cmp -l ${SOURCEDIR}/${FILENAMEsrc} ${TARGET1DIR}/${FILENAMEsrc}${SECONDFILE_EXT}
        fi
        if [ ! $? = 0 ]; then
            echo "cmp second file NOT ok"
            if [ x"$MISMATCHEXIT" = x"yes" ]; then
                exit 1
            fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

# target initiator
  #diffdata
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        diff -rq ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar}
    else
        diff -rq ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    fi
    if [ ! $? = 0 ]; then
        echo "diff NOT ok"
        #if [ x"$MISMATCHEXIT" = x"yes" ]; then
        #    exit 1
        #fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  #cmpdata
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        cmp -l ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar}
    else
        cmp -l ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}
    fi
    if [ ! $? = 0 ]; then
        echo "cmp NOT ok"
        if [ x"$MISMATCHEXIT" = x"yes" ]; then
            exit 1
        fi
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

# target initiator - second test file
  #diffdata
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            diff -rq ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar}${SECONDFILE_EXT}
        else
            diff -rq ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT}
        fi
        if [ ! $? = 0 ]; then
            echo "diff second file NOT ok"
            #if [ x"$MISMATCHEXIT" = x"yes" ]; then
            #    exit 1
            #fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

  #cmpdata
    if [ x"$DUP_SECONDFILE" = x"yes" ]; then
        if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
            cmp -l ${TARGET2DIR}/${FILENAMEtar} ${TARGET1DIR}/${FILENAMEtar}${SECONDFILE_EXT}
        else
            cmp -l ${TARGET1DIR}/${FILENAMEtar} ${SOURCEDIR}/${FILENAMEtar}${SECONDFILE_EXT}
        fi
        if [ ! $? = 0 ]; then
            echo "cmp second file NOT ok"
            if [ x"$MISMATCHEXIT" = x"yes" ]; then
                exit 1
            fi
        fi
        sync
        if [ ! $? = 0 ]; then
            echo "sync second file failed"
            exit 1
        fi
    fi

# remove the files
    if [ x"$SUPPORTMANYTARGET" = x"yes" ]; then
        rm -f ${TARGET1DIR}/${FILENAMEtar}
        rm -f ${TARGET2DIR}/${FILENAMEsrc}
    else
        rm -f ${SOURCEDIR}/${FILENAMEtar}
        rm -f ${TARGET1DIR}/${FILENAMEsrc}
    fi
    sync

# umount device
    if [ x"$REMOUNT" = x"yes" ]; then
        umount ${SOURCEDIR}
        umount ${TARGET1DIR}
        if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
            if [ x"$TARGET2ISRAM" = x"no" ]; then
                umount ${TARGET2DIR}
            fi
        fi
    fi

    let cnt+=1
    if [ x"$ITERATIONCOUNT" = x"$cnt" ]; then
        break
    fi

    #let mismatchcnt+=1
    #if [ x"$MISMATCHCOUNT" < x"$mismatchcnt" ]; then
    #    break
    #fi
done
date

if [ x"$REMOUNT" = x"no" ]; then
    sync; sleep 2; sync; sleep 2; sync
    umount ${SOURCEDIR}
    umount ${TARGET1DIR}
    if [ x"$SUPPORTMANYTARGET" = x"yes" ] ; then
        if [ x"$TARGET2ISRAM" = x"no" ]; then
            umount ${TARGET2DIR}
        fi
    fi
fi

free
cat /proc/interrupts
echo "Test OK and end of endurance test"
