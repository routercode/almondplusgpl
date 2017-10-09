ifndef CFLAGS
CFLAGS = -O2 -g -I ./
endif
CFLAGS += -DCONFIG_FB_G2_LCD_PINGPONG_BUFFER
CFLAGS += -DCONFIG_FB_G2_LCD_POLLING_FB
#CFLAGS += -DREPEATCNT=3

all: clean fbtest

clean:
	#rm -rf fbtest fbtest.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

fbtest: fbtest.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

