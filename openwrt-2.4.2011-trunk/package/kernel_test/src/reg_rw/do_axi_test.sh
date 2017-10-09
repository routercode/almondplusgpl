#!/bin/sh
cd /dev
mknod reg_rw c 10 242
cd /tmp

echo ===Begin AXI master test===
date
for i in `seq 1 1000`; do
    echo "CONFIGURATION $i"
    parse_axi /etc/axi_reg1000.txt $i 98000

    /etc/test_ts.sh &
    /etc/test_nand.sh &
            
    echo [BEGIN-check-test_ts.sh]
    while [ 1 ]
    do
        count=`exec ps | grep "test_ts.sh" -c`
        if [ $count = 1 ]; then
                echo "test_ts.sh is finished!!!"
                break
        fi
    done
    echo [DONE]

    echo [BEGIN-check-test_nand.sh]
    while [ 1 ]
    do
        count=`exec ps | grep "test_nand.sh" -c`
        if [ $count = 1 ]; then
                echo "test_nand.sh is finished!!!"
                break
        fi
    done
    echo [DONE]
done

date
echo ===End AXI master test===


