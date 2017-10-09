#!/bin/sh
#
#  lcd_test.sh
#
# Copyright (c) Cortina-Systems Limited 2011.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# Goldengate bash script for framebuffer overnight testing

export DIRPICS=yes
export ECHOPIC=no

cnt=0
if [ x"$DIRPICS" = x"yes" ]; then
    while(true)
    do
        echo "Iteration $cnt"
        date
        fbtest raw .
        let cnt+=1
    done
else
    while(true)
    do
        echo "Iteration $cnt"
        date
        if [ x"$ECHOPIC" = x"yes" ]; then
            echo "fbtest raw 1.raw"
            fbtest raw 1.raw
            echo "fbtest raw 2.raw"
            fbtest raw 2.raw
            echo "fbtest raw 3.raw"
            fbtest raw 3.raw
        else
            fbtest raw 1.raw
            fbtest raw 2.raw
            fbtest raw 3.raw
        fi
        fbtest color red
        fbtest color green
        fbtest color blue
        #fbtest reg
        let cnt+=1
    done
fi

