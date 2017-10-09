ifndef CFLAGS
CFLAGS = -O2 -g   -I$(LINUX_DIR)/include
endif

LDFLAGS=

all: crypto-hash-test crypto-cipher-test

%.o: %.c
	$(CC) $(CFLAGS) -I$(LINUX_DIR)/include -c -o $@ $^

%: %.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	rm -rf *.o crypto-hash-test crypto-cipher-test