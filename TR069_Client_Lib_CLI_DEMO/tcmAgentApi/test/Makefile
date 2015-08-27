TR069DIR = $(ROOTDIR)/lib/tr69Agent
INCLUDES = -I../
INCLUDES += -I$(TR069DIR)/include
INCLUDES += -I$(TR069DIR)/src/include -I$(TR069DIR)/src/pal/pal_linux_32
INCLUDES += -I$(TR069DIR)/src
#AGENTLIB =../../libtcmAgent.o
LIBFLAGS = ../libtcmAgentApi.so $(COMMON_LDFLAGS)
all:test
test:tcmAgentApiTest.c
	$(CC) -D_TR069_NOT_DEFINE_ $(INCLUDES) $(LIBFLAGS) $(COMMON_CFLAGS)  tcmAgentApiTest.c -o tcmAgentApiTest

clean:
	rm -f *.o test

romfs:
	$(ROMFSINST) tcmAgentApiTest /bin/; 
