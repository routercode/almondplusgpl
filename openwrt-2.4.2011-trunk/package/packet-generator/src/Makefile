#
# Proprietary Information of Cortina Technologies
# Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.
#
# file      : Makefile
# Author    : Axl Lee <axl.lee@cortina-systems.com>
#
NECFG_ROOT=..

BINS = ./bin

CFLAGS = -D__ARM__
CFLAGS += -O2 -Wall -g

OBJS = pktgen.o fileProcess.o public.o \
       desc_L2.o desc_L3.o desc_L4.o desc_L5.o desc_Payload.o \
       signature.o rawSocket.o

OUTPUT=${BINS}/pktgen
LDFLAGS =  -Wl,-Map=${OUTPUT}.map
LIBS = -lpthread

all: clean_obj ${OBJS} ${OUTPUT}

clean_obj:
	rm -f ${OBJS} ${OUTPUT}

clean:
	rm -f $(OUTPUT) *.o fe/*.o *.core core *.bak *~


${OUTPUT}: Makefile ${OBJS}
	$(CC)  -o ${OUTPUT}  $(OBJS) ${CLIB}  $(LDFLAGS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c -g $<



