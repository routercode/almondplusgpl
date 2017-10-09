ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: mousetest usb_gpio_1 usbled

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

mousetest: mousetest.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

usb_gpio_1: usb_gpio_1.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

usbled: usbled.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o mousetest usb_gpio_1 usbled
