ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: rboot

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

rboot: rboot.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -fr *.o rboot
