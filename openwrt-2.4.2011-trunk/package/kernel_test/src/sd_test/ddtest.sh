#!/bin/sh
#
#  ddtest.sh
#
# Copyright (c) Cortina-Systems Limited 2010-2011.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# test data copy/comparison for mass storage devices using dd
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
#   getimeofday
#     get the minisecond of current date and mask the second with 0xfffff.
#   cmp
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Editors ==> cmp
#   diff
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Editors ==> diff
#   date
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Coreutils ==> date ==> Support weird 'date MMDDhhmm[[YY]YY][.SS]' format
#   fdisk
#     make menuconfig ==> Utilities ==> disc ==> fdisk
#   mke2fs
#     make menuconfig ==> Utilities ==> Filesystem ==> e2fsprogs
#   mkfs.vfat
#     make menuconfig ==> Base system ==> busybox ==> Configuration ==> Linux System Utilities ==> mkfs_vfat
#
# Sample Configuration:
#   Config1:
#    Only two partition: source and target1 partitions in device
#    Format two partition, prepare two files of size 10MB in source and target1 partition respectively.
#      GENFILE=yes, _FILESIZE=10
#  

# default configuration:
export QUIET=no
export VERBOSE=no
export GENFILE=yes
export SLEEPING=3
export WAITING=7
export ENDURANCETEST=no
export ITERATIONCOUNT=100
# ITERATIONCOUNT=0 for endless loop test
export DD_BLOCKSIZE=no
export MISMATCHEXIT=no
export SEQUENCEFILE=no
export SRCISRAM=no
export _FILESIZE=10
export MAXDDCOUNT=${_FILESIZE}
export CMP_OPT=-l
export ECHOPERFORM=no
export MINISECOND=no
export _1K=1024
export _1M=$(($_1K * $_1K))
export HDPARM=no

export SRCDEV=sda1
export TAR1DEV=mmcblk0

# mass storage dependent configuration
if [ x"$1" = x ]; then
    if [ -f "sd.cfg" ]; then	# default: sd.cfg
        source sd.cfg
        echo "list the configuration:"
        cat $1
        echo ""
    fi
else
    source $1
    echo "list the configuration:"
    cat $1
    echo ""
fi

export _FILESIZE_KB=$(($_FILESIZE * $_1K))

export SRCNODE=/dev/$SRCDEV
export TAR1NODE=/dev/$TAR1DEV

export ROOTDIR=
if [ x"$SRCISRAM" = x"yes" ]; then
    export _SOURCEMNT=tmp
else
    export _SOURCEMNT=mnt
fi
export SOURCEDIR=${ROOTDIR}/$_SOURCEMNT/$SRCDEV
echo "SOURCEDIR=$SOURCEDIR"
echo "TAR1NODE=$TAR1NODE"

# configure the file size
if [ x"$SEQUENCEFILE" = x"yes" ]; then
    export FILESIZE=$_FILESIZE
    export FILENAMEsrc=sequence_src.${FILESIZE}M
else
    export FILESIZE=${_FILESIZE}M
    export FILENAMEsrc=urandom_src.${FILESIZE}
fi

set -x

# Check if device nodes exist?
    if [ x"$SRCISRAM" = x"no" ]; then
        if [ ! -b ${SRCNODE} ] ; then
            echo "${SRCNODE} device node doesn't exist"
            return
        fi
    fi
    if [ ! -b ${TAR1NODE} ] ; then
        echo "${TAR1NODE} device node doesn't exist"
        return
    fi
    free
    cat /proc/cpuinfo
    cat /proc/interrupts

#createdir
    if [ ! -d ${SOURCEDIR} ] ; then
        mkdir -p ${SOURCEDIR}
    fi

# mount source
    if [ x"$SRCISRAM" = x"no" ]; then
        mount $SRCNODE ${SOURCEDIR}
    fi

# prepare the data for copying
    if [ x"$GENFILE" = x"yes" ]; then
    #preparedata
        if [ x"$SEQUENCEFILE" = x"yes" ]; then
            PWD=$PWD
            cd ${SOURCEDIR}
            gensequfile $FILENAMEsrc $FILESIZE
            cd $PWD
        else
            dd if=/dev/urandom of=${SOURCEDIR}/${FILENAMEsrc} count=${_FILESIZE} bs=1M
            if [ ! $? = 0 ]; then
                echo "create file failed"
                exit 1
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
    if [ x"$MINISECOND" = x"yes" ]; then
        startwritetime=`getimeofday`
    else
        startwritetime=`date  +%s`
    fi
    dd if=${SOURCEDIR}/${FILENAMEsrc}  of=${TAR1NODE}   count=${_FILESIZE} bs=1M
    sync; sync; sync
    if [ x"$MINISECOND" = x"yes" ]; then
        stopwritetime=`getimeofday`
    else
        stopwritetime=`date  +%s`
    fi
    writeproctime=$(($stopwritetime - $startwritetime))
    if [ x"$MINISECOND" = x"yes" ]; then
        writeperf=$(($_FILESIZE_KB * 1000 / $writeproctime))
    else
        writeperf=$(($_FILESIZE_KB/$writeproctime))
    fi

    if [ x"$MINISECOND" = x"yes" ]; then
        startreadtime=`getimeofday`
    else
        startreadtime=`date  +%s`
    fi
    dd if=${TAR1NODE} of=${SOURCEDIR}/${FILENAMEsrc}.2  count=${_FILESIZE} bs=1M
    sync; sync; sync
    if [ x"$MINISECOND" = x"yes" ]; then
        stopreadtime=`getimeofday`
    else
        stopreadtime=`date  +%s`
    fi
    readproctime=$(($stopreadtime - $startreadtime))
    if [ x"$MINISECOND" = x"yes" ]; then
        readperf=$(($_FILESIZE_KB * 1000 / $readproctime))
    else
        readperf=$(($_FILESIZE_KB/$readproctime))
    fi

    echo "bandwidth: read: $readperf KBytes/s; write: $writeperf KBytes/s"

# source initiator
  # differ the data
    diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2
    if [ $? = 0 ]; then
        echo "diff ok"
    else
        echo "diff NOT ok"
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  # cmp the data
    cmp ${CMP_OPT} ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2
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

# test done
    if [ x"$SRCISRAM" = x"no" ]; then
        umount ${SOURCEDIR}
    fi

# hdparm performance test
    if [ x"$HDPARM" = x"yes" ]; then
        hdparm -tT /dev/${TAR1DEV}
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
if [ x"$SRCISRAM" = x"no" ]; then
    mount $SRCNODE ${SOURCEDIR}
fi

cnt=0
ddcount=0
while(true)
do
    #cat /proc/uptime
    if [ ! x"$QUIET" = x"yes" ]; then
        date
        if [ x"$VERBOSE" = x"yes" ]; then
            echo "Iteration $cnt, cfgfile=$1, SRCDEV=$SRCDEV, TAR1NODE=$TAR1NODE"
        else
            echo "Iteration $cnt"
        fi
    fi

# copy the data between source and target partitions
    if [ x"$VERBOSE" = x"yes" ]; then
        echo "dd if=${SOURCEDIR}/${FILENAMEsrc}  of=${TAR1NODE}   count=${_FILESIZE} bs=1M  seek=${ddcount}"
    fi
    if [ x"$MINISECOND" = x"yes" ]; then
        startwritetime=`getimeofday`
    else
        startwritetime=`date  +%s`
    fi
    dd if=${SOURCEDIR}/${FILENAMEsrc}  of=${TAR1NODE}   count=${_FILESIZE} bs=1M  seek=${ddcount}
    sync; sync; sync
    if [ x"$MINISECOND" = x"yes" ]; then
        stopwritetime=`getimeofday`
    else
        stopwritetime=`date  +%s`
    fi
    writeproctime=$(($stopwritetime - $startwritetime))
    if [ x"$MINISECOND" = x"yes" ]; then
        writeperf=$(($_FILESIZE_KB * 1000 / $writeproctime))
    else
        writeperf=$(($_FILESIZE_KB/$writeproctime))
    fi

    if [ x"$VERBOSE" = x"yes" ]; then
        echo "dd if=${TAR1NODE} of=${SOURCEDIR}/${FILENAMEsrc}.2  count=${_FILESIZE} bs=1M  skip=${ddcount}"
    fi
    if [ x"$MINISECOND" = x"yes" ]; then
        startreadtime=`getimeofday`
    else
        startreadtime=`date  +%s`
    fi
    dd if=${TAR1NODE} of=${SOURCEDIR}/${FILENAMEsrc}.2  count=${_FILESIZE} bs=1M  skip=${ddcount}
    sync; sync; sync
    if [ x"$MINISECOND" = x"yes" ]; then
        stopreadtime=`getimeofday`
    else
        stopreadtime=`date  +%s`
    fi
    readproctime=$(($stopreadtime - $startreadtime))
    if [ x"$MINISECOND" = x"yes" ]; then
        readperf=$(($_FILESIZE_KB * 1000 / $readproctime))
    else
        readperf=$(($_FILESIZE_KB/$readproctime))
    fi

    if [ x"$ECHOPERFORM" = x"yes" ]; then
        echo "bandwidth: read: $readperf KBytes/s; write: $writeperf KBytes/s"
    fi

# source initiator
  #diffdata
    if [ x"$VERBOSE" = x"yes" ]; then
        echo "diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2"
    fi
    diff -rq ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2
    if [ ! $? = 0 ]; then
        echo "diff NOT ok"
    fi
    sync
    if [ ! $? = 0 ]; then
        echo "sync file failed"
        exit 1
    fi

  #cmpdata
    if [ x"$VERBOSE" = x"yes" ]; then
        echo "cmp ${CMP_OPT} ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2"
    fi
    cmp ${CMP_OPT} ${SOURCEDIR}/${FILENAMEsrc} ${SOURCEDIR}/${FILENAMEsrc}.2
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

# remove the files
    rm -f ${SOURCEDIR}/${FILENAMEsrc}.2

    # write ddcount*block offset in file
    let ddcount+=${_FILESIZE}
    if [ $MAXDDCOUNT -le $ddcount ]; then
        ddcount=0
    fi

    let cnt+=1
    if [ x"$ITERATIONCOUNT" = x"$cnt" ]; then
        break
    fi

done
date

if [ x"$SRCISRAM" = x"no" ]; then
    sync; sleep 2; sync; sleep 2; sync
    umount ${SOURCEDIR}
fi

free
cat /proc/interrupts
echo "dd test ok"
