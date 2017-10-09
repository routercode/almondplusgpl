#!/bin/sh
./ne_cfg -m fe -t LPM -s -c add -b 0

#192.168.1.1
./ne_cfg -m fe -t LPM -f ip_addr.0 -v 0xc0a8010a
./ne_cfg -m fe -t LPM -f ip_addr.1 -v 0x00000000
./ne_cfg -m fe -t LPM -f ip_addr.2 -v 0x00000000
./ne_cfg -m fe -t LPM -f ip_addr.3 -v 0x00000000

./ne_cfg -m fe -t LPM -f mask -v 0x20
./ne_cfg -m fe -t LPM -f priority -v 0x7
./ne_cfg -m fe -t LPM -f result_idx -v 0x191a
./ne_cfg -m fe -t LPM -f ipv6 -v 0x0

./ne_cfg -m fe -t LPM -e
