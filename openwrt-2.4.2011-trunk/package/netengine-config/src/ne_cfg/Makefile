#
# Proprietary Information of Cortina Technologies
# Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.
#
# file      : Makefile
# Author    : Axl Lee <axl.lee@cortina-systems.com>
#
NECFG_ROOT=..

BINS = ../bin

CFLAGS = -D__ARM__
CFLAGS += -I../include -I${KERNEL_INC1} -I${KERNEL_INC2}
CFLAGS += -O2 -Wall -g

OBJS = ne_cfg.o parser.o ne_ipc.o ni/ni.o fe/fe.o tm/tm.o sch/sch.o qm/qm.o qos/ingress_qos.o voqcntr/voq_counter.o tunnel/tunnel.o

OUTPUT=${BINS}/ne_cfg
LDFLAGS =  -Wl,-Map=${OUTPUT}.map

all: clean_obj ${OBJS} ${OUTPUT}

clean_obj:
	rm -f ${OBJS} ${OUTPUT}

clean:
	rm -f $(OUTPUT) *.o ni/*.o fe/*.o tm/*.o sch/*.o qm/*.o qos/*.o voqcntr/*.o *.core core *.bak *~


${OUTPUT}: Makefile ${OBJS}
	$(CC)  -o ${OUTPUT}  $(OBJS) ${CLIB}  $(LDFLAGS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c -g $<
