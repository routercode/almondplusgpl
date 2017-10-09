ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: input_button

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

input_button: input_button.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -levent

clean:
	rm -rf *.o input_button 
