ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/include
endif

all: i2c_test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

i2c_test: i2c_test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o i2c_test 
