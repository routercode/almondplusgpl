#!/bin/sh
./ne_cfg -m fe -t Port_Range -s -c add -b 0

./ne_cfg -m fe -t Port_Range -f sp_dp_low -v 0x0102
./ne_cfg -m fe -t Port_Range -f sp_dp_high -v 0x0304
./ne_cfg -m fe -t Port_Range -f valid -v 0x05

./ne_cfg -m fe -t Port_Range -e
