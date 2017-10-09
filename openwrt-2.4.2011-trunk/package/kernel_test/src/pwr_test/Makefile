ifndef CFLAGS
CFLAGS = -O2 -g -I ./ 
endif

LDFLAGS=

all: pwr-test 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

pwr-test: pwr-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	rm -rf *.o pwr-test 

