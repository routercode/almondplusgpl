ifndef CFLAGS
CFLAGS = -O2 -g -march=armv7-a
endif

all: dynamic_suspend

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

dynamic_suspend: dynamic_suspend.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -fr *.o dynamic_suspend
