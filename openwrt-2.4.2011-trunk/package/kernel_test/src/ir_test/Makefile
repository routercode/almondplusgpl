ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/include
endif

LDFLAGS=

all: ir-test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

ir-test: ir-test.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -levent

clean:
	rm -rf *.o ir-test

