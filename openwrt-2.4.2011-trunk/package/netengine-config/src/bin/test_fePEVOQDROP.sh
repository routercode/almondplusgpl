#!/bin/sh
./ne_cfg -m fe -t PE_VOQ_DROP -s -c add -b 0

./ne_cfg -m fe -t PE_VOQ_DROP -f voq_id -v 0x0102
./ne_cfg -m fe -t PE_VOQ_DROP -f f_drop_enbl -v 0x03

./ne_cfg -m fe -t PE_VOQ_DROP -e
