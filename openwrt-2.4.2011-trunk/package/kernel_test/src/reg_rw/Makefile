ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: reg_rw 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

reg_rw: read_write.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

parse_axi: parse_axi.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o reg_rw parse_axi
