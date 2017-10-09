ifndef CFLAGS
CFLAGS = -O2 -g -I ./ 
endif

LDFLAGS=-lpthread

all: ts_test ts_config ts_net_test ratetest

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

ts_test: test_pes.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

ts_config: ts_config.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

ratetest: ratetest.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

ts_net_test: ts_net_test.o sender.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o ts_test ts_config ts_net_test ratetest
