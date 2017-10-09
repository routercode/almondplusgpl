ifndef CFLAGS
CFLAGS = -O2 -g -I ./ 
endif

all: watchdog-test 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

watchdog-test: watchdog-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o watchdog-test
