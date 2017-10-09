ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: pkt_fwd_ctl

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

reg_rw: pkt_fwd_ctl.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o pkt_fwd_ctl

