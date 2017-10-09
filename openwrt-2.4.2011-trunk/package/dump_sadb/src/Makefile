ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: dump_sadb

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

dump_sadb: dump_sadb.o printsadb.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -fr *.o dump_sadb
