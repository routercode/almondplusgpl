#!/bin/sh
#
#  drawbox.sh
#
# Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# Goldengate bash script for framebuffer testing

x_res=`fbtest dumpinfo varscreen | grep xres: | cut -f 2 -d':' | head -n 1`
y_res=`fbtest dumpinfo varscreen | grep yres: | cut -f 2 -d':' | tail -n 1`
#echo $x_res $y_res

delta=10
if [ $1 ]; then
    delta=$1
fi

# upper bar (x1, y1) - (x2, y2)
x1=0
y1=0
#echo $x1 $y1

x2=`expr $x_res - 1`
y2=`expr $y1 + $delta`
#echo $x2 $y2

# left bar (x1, y1) - (x3, y3)
x3=`expr $x1 + $delta`
y3=`expr $y_res - 1`
#echo $x3 $y3

# bottom bar (x4, y4) - (x6, y6)
x4=$x1
y4=`expr $y_res - $delta - 1`
#echo $x4 $y4

# right bar (x5, y5) - (x6, y6)
x5=`expr $x_res - $delta - 1`
y5=$y1
#echo $x5 $y5

x6=`expr $x_res - 1`
y6=`expr $y_res - 1`
#echo $x6 $y6

# change the base address of g2 framebuffer device
#echo "fbtest ioctl g2_toggle_base_addr"
#fbtest ioctl g2_toggle_base_addr
#sleep 3

# mask g2_lcd_toggled flag, don't change the framebuffer base address
# function g2_mask_toggle is avaliable only in ping-pong buffer mode
#fbtest ioctl g2_mask_toggle

for colorval in red green blue black white; do
    echo "$colorval"
    fbtest color ${colorval}
    #fbtest draw $x1 $y1 $x2 $y2
    #fbtest draw $x1 $y1 $x3 $y3
    #fbtest draw $x4 $y4 $x6 $y6
    #fbtest draw $x5 $y5 $x6 $y6
    #if [ $colorval != "black" ]; then
        sleep 3
    #fi
done
#fbtest ioctl g2_unmask_toggle

#fbtest raw .
