#!/bin/sh
./ne_cfg -m fe -t Pktlen_Range -s -c add -b 0

./ne_cfg -m fe -t Pktlen_Range -f pktlen_low -v 64
./ne_cfg -m fe -t Pktlen_Range -f pktlen_high -v 1024
./ne_cfg -m fe -t Pktlen_Range -f valid -v 1

./ne_cfg -m fe -t Pktlen_Range -e
