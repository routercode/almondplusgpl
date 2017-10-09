#!/bin/sh

echo "===Begin NAND flash test==="
date
dd if=/dev/urandom of=/tmp/s.file bs=1M count=10
sync

for i in `seq 1 100`; do
        cp /tmp/s.file /dev/mtd1
        sync
        cp /dev/mtd1 /tmp/d.file
        sync

        cmp -l /tmp/s.file /tmp/d.file

        if [ $? != 0 ]; then
               echo  "File is not the same"
               exit
        fi

        rm /tmp/d.file
        sync

        if [ -e /tmp/d.file ]; then
                echo "Remove file failed"
		date
                exit 1
        fi
done

date
echo "===End NAND flash test==="
