CC ?= $(CROSS_COMPILE)gcc
CPP ?= $(CROSS_COMPILE)g++

TARGET = g2_diags

CPPFILES = dct/dcttest_no_main.cpp dct/workqueue.cpp
CFILES = qm_list_walk.c ni_linked_list_test.c general.c errorlog.c mem_test.c fe_stats.c ddr_stats.c fabric.c
#IDIR = ../../../target/linux/g2-fpga/files/arch/arm/mach-goldengate/include/mach
IDIR = ../../../target/linux/g2/files/arch/arm/mach-goldengate/include/mach
IDIR_INCS ?= -I$(IDIR)

DCTIDIR = dct
HFILES = 

OFILES = $(CFILES:%c=%o)
OCPPFILES = $(CPPFILES:%cpp=%o)

$(TARGET): $(OFILES) $(OCPPFILES)
	$(CXX) -o $(TARGET) $(OFILES) $(OCPPFILES) -lrt -lpthread

%.o: %.c $(HFILES)
	$(CC) $(CFLAGS) -g ${IDIR_INCS} -I${DCTIDIR} -pthread -c $< -o $@

%.o: %.cpp $(HFILES)
	$(CXX) $(CPPFLAGS) ${IDIR_INCS} -I${DCTIDIR} -pthread -c $< -o $@



clean:
	$(RM) $(TARGET) $(OFILES)

