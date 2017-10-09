ifndef CFLAGS
CFLAGS = -O2 -g -I ./ 
endif

all: rtc-test 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

rtc-test: rtc-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o rtc-test
