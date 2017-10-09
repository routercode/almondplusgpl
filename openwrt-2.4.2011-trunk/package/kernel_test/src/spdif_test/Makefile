##ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/sound/oss/cs75xx_sound -I$(LINUX_DIR)/include
##endif

LDFLAGS=

all: spdif-test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

spdif-test: spdif-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	rm -rf *.o spdif-test

