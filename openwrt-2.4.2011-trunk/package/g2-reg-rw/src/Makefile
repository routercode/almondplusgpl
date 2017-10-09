CC ?= $(CROSS_COMPILE)gcc

TARGET = g2-reg-rw

CFILES = g2-reg-rw.c rw_target.c
HFILES = 

OFILES = $(CFILES:%c=%o)

$(TARGET): $(OFILES)
	$(CC) -o $(TARGET) $(OFILES)

%.o: %.c $(HFILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OFILES)

