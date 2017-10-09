ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: cs_qos_set

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

reg_rw: cs_qos_set.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o cs_qos_set
