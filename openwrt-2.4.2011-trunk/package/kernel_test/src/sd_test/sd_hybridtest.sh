#!/bin/sh
echo "test2part.sh /sbin/sd.cfg"
test2part.sh /sbin/sd.cfg

sleep 3
echo ""
echo "test2part.sh /sbin/sd_dd_blksz.cfg"
test2part.sh /sbin/sd_dd_blksz.cfg

sleep 3
echo ""
echo "test2part.sh /sbin/sd_secondendur.cfg"
test2part.sh /sbin/sd_secondendur.cfg

sleep 3
echo ""
echo "test2part.sh /sbin/sd_endurance.cfg"
test2part.sh /sbin/sd_endurance.cfg
