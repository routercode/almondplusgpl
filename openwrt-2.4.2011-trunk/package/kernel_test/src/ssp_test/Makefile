##ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/sound/oss -I$(LINUX_DIR)/drivers/telephony  -I$(LINUX_DIR)/include -I$(LINUX_DIR)/arch/arm/mach-goldengate/include
##endif

LDFLAGS=

all: dac-test zphone-test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

dac-test: dac-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

zphone-test: zphone-test.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	rm -rf *.o dac-test zphone-test

