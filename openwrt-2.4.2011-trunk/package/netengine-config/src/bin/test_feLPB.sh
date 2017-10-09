#!/bin/sh
./ne_cfg -m fe -t LPB -s -c add -b 0

./ne_cfg -m fe -t LPB -f lspid -v 0x01
./ne_cfg -m fe -t LPB -f pvid -v 0x0203
./ne_cfg -m fe -t LPB -f pvid_tpid_enc -v 0x04
./ne_cfg -m fe -t LPB -f olspid_en -v 0x05
./ne_cfg -m fe -t LPB -f olspid -v 0x06
./ne_cfg -m fe -t LPB -f olspid_preserve_en -v 0x07
./ne_cfg -m fe -t LPB -f parity -v 0x08

./ne_cfg -m fe -t LPB -e
