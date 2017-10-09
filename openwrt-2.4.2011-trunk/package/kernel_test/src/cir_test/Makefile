ifndef CFLAGS
CFLAGS = -O2 -g -I ./ -I$(LINUX_DIR)/include 
endif

LDFLAGS=

all: cir-test 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

cir-test: cir-test.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean: 
	rm -rf *.o cir-test 

