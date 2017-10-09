default: dctest

CC = g++
LINKER = g++
.SUFFIXES: .S .c .cxx .o .s

C_OBJS = workqueue.o dctest.o

COPTS = -Wall -g -O2

LINKOPTS =

dctest: $(C_OBJS)
	$(CC) $(COPTS) $(C_OBJS) $(LINKOPTS) -o dctest -lpthread

clean: 
	- rm -f *.o
	- rm -f dctest

%.o: %.c
	$(CC) -c $(COPTS) $< -o $@

%.o: %.cpp
	$(CC) -c $(COPTS) $< -o $@

%.o: %.cxx
	$(CC) -c $(COPTS) $< -o $@
