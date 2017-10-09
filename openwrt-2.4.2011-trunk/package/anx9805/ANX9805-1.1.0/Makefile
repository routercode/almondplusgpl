ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif

all: anx9805 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

anx9805:  DP_TX_DRV.o  I2C_intf.o  main.o   mcu.o timer.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o anx9805
