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

OBJS = neSendCmd.o \
       neSend_ioctl.o \
       misc.o \
       ./fe/feTblClassifier.o \
       ./fe/feTblSDB.o \
       ./fe/feTblHashMask.o \
       ./fe/feTblLPM.o \
       ./fe/feTblHashMatch.o \
       ./fe/feTblFWDRSLT.o \
       ./fe/feTblQOSRSLT.o \
       ./fe/feTblL3IP.o \
       ./fe/feTblL2MAC.o \
       ./fe/feTblVoQPolicer.o \
       ./fe/feTblLPB.o \
       ./fe/feTblANBNGMAC.o \
       ./fe/feTblPortRange.o \
       ./fe/feTblVLAN.o \
       ./fe/feTblACL.o \
       ./fe/feTblPEVOQDROP.o \
       ./fe/feTblETYPE.o \
       ./fe/feTblLLCHDR.o \
       ./fe/feTblFVLAN.o \
       ./fe/feTblHashHash.o \
       ./fe/feTblHashCheck.o \
       ./fe/feTblPktlenRange.o \
       ./fe/cal_hwHash.o \
       ./fe/cs_crc.o \
       ./qos/feTblShaperVoq.o \
       ./qos/feTblQos.o \
       ./qos/feTblShaperPort.o \
       ./qos/feTblQosApi.o \
       ./voqcntr/voqCntrApi.o\
       ./tunnel/tunnelIplipApi.o
OUTPUT=${BINS}/neSendCmd
LDFLAGS =  -Wl,-Map=${OUTPUT}.map

all: clean_obj ${OBJS} ${OUTPUT}

clean_obj:
	rm -f ${OBJS} ${OUTPUT}

clean:
	rm -f $(OUTPUT) *.o fe/*.o qos/*.o voqcntr/*.o *.core core *.bak *~


${OUTPUT}: Makefile ${OBJS}
	$(CC)  -o ${OUTPUT}  $(OBJS) ${CLIB}  $(LDFLAGS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c -g $<
