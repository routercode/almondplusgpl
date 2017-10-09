ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/include/spi
endif

all: spi_test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

spi_test: spi_test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o spi_test 