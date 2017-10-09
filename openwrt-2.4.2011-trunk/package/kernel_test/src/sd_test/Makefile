ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: clean gensequfile getimeofday

clean:
	rm -rf gensequfile  gensequfile.o  getimeofday  getimeofday.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

gensequfile: gensequfile.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

getimeofday: getimeofday.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

