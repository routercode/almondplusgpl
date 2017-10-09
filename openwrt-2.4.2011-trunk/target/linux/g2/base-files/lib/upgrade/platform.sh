# Custom hooks to use Cortina Systems image downloader/upgrader with
# OpenWRT's "sysupgrade" script.  
#

# Make sure our software image downloader is available on the
# ramdisk during upgrade.
#
RAMFS_COPY_BIN="/usr/bin/cs.extract /bin/rm $RAMFS_COPY_BIN"

# Ensure the logic surrounding saving the rootfs_data partition
# works correctly
USE_REFRESH="yes"

# This hook validates the image before a real upgrade is attempted.
# In our case, we are going to download the entire image into
# /tmp now.  This is not ideal, but is how LuCI works anyway when
# you upload an image from browser.  We have to do this because
# tftp ptotocol is broken in libcurl (which cs.swdl uses), so
# we fall back on busybox tftp.
#
platform_check_image() {
    [ "$ARGC" -gt 1 ] && return 1
    local image="$1"
    local Q=""
    local q="-q"
    [ "$VERBOSE" -gt 1 ] && Q="--verbose"
    [ "$VERBOSE" -gt 1 ] && q=""

    cd /tmp

    # If $image starts with tftp:// then we need to
    # treat special.  libcurl tftp protocol does not
    # work, so we need to get with busybox 'tftp'
    # program.
    #
    case "$image" in
	tftp://*) 
	    s=${image#tftp://}
	    hostport=${s%%/*}
	    file=${s#*/}
	    host=${hostport%:*}
	    port=${hostport#*:}
	    [ "$host" == "$port" ] && port=
	    image=$file
	    v "tftp -g -r $image $host $port"
	    tftp -g -r $image -l firmware.img $host $port || return 1
	    image=/tmp/firmware.img
	    ;;
	http://*|ftp://*)
	    v "wget $q $image"
	    wget $q -O firmware.img $image || return 1
	    image=/tmp/firmware.img
	    ;;
	*)  ;;
    esac

    if [ ! -f "$image" ]; then
	v "Image not found: $image"
	return 1
    fi

    sb0_device=$(awk '$4 ~ /"sb0"/ { sub( /:$/, "", $1); print "/dev/" $1}' /proc/mtd)
    sb1_device=$(awk '$4 ~ /"sb1"/ { sub( /:$/, "", $1); print "/dev/" $1}' /proc/mtd)
    esz=$(awk '$4 ~ /"sb0"/ { sub( /^0*/, "", $3); print "0x" $3}' /proc/mtd)


    # Construct command line from the paramters:
    #
    cmdline="$Q --sb0-device=$sb0_device --sb1-device=$sb1_device --erase-blksz=$esz"

    # Save the upgrade command line, which we'll use during
    # the actual upgrade.
    #
    echo "--commit --burn $cmdline" > /tmp/cs-swdl-cmdline

    # Make sure the cs.swdl handlers and the command line we just
    # saved are preserved across the ramdisk, so they are available
    # to the upgrade hook.
    #
    install_file /tmp/cs-swdl-cmdline /etc/hosts

    # Validate the image
    #
    cs.extract --validate $cmdline $image
}

# This hook does the actual download and upgrade.
#
platform_do_upgrade() {
    local image="$1"
    local s hostport file host port

    sync
    cd /tmp

    # If it was a remote access, then we already have the file
    # now locally and the filename is the basename of the original
    # url.
    #
    case "$image" in
	http://*|ftp://*|tftp://*) 
	    image=/tmp/firmware.img
	    ;;
	*)  ;;
    esac

    # Restore the command line we need for using raw cs.swdl
    CS_SWDL_CMDLINE=`cat /tmp/cs-swdl-cmdline`

    v "/usr/bin/cs.extract $CS_SWDL_CMDLINE $image"
    /usr/bin/cs.extract $CS_SWDL_CMDLINE $image || return 1

    # If the user selects not to preserve the configuration,
    # then shouldn't we erase the rootfs_data so that the
    # system comes up clean?
    #
    [ "$SAVE_CONFIG" -eq 0 ] && {
	v "Config not being saved, deleting /overlay partition ..."
	mtd erase rootfs_data
    }
    return 0
}

# Not exactly sure what this does.  Forum post suggested that
# if not performed, could lead to file system corruption.
#
platform_refresh_partitions() {
    mtd refresh rootfs
    mtd refresh rootfs_standby
}

# We'll want to kill the watchdog while the upgrade is
# in progress.
#
disable_watchdog() {
    v "Killing watchdog timer ..."
    killall watchdog
    ( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
        echo 'Could not disable watchdog'
        return 1
    }
}
append sysupgrade_pre_upgrade disable_watchdog

# This hook function will remove /etc/config/version from the
# list of files being preserved across an upgrade.  We need this
# file coming from the new image, not the current one!
#
platform_adjust_conffiles() {
    local file="$1"
    v "Adjusting conffile list ..."
    grep -v /etc/config/version $file >/tmp/platform_adjust_conffiles.lst
    mv /tmp/platform_adjust_conffiles.lst $file
    return 0
}
append sysupgrade_init_conffiles platform_adjust_conffiles
