#################################################
#
#demo for linux
#
################################################
MYCC:=$(CC)
MYLD:=$(LD)
#CC = arm-linux-gcc -mbig-endian -msoft-float -march=armv5te -fsigned-char 
STRIP = strip

#objects := $(patsubst %.c,%.o,$(wildcard *.c))
objects :=tcmAgent.o
PROG = tcmAgent
all:$(PROG)

#AGENTLIB = ../../lib/tr69Agent/libtcmAgent.o -L/usr/local/arm-linux/openssl/lib
#INCLUDES = -I../../lib/tr69Agent/include -I/usr/local/arm-linux/openssl/inlude -I./
LDFLAGS = $(ROOTDIR)/lib/tr69Agent/libtcmAgent.o $(COMMON_LDFLAGS) -L$(ROOTDIR)/lib/mxml -lpthread -lc -lssl -lcrypto -lmxml
INCLUDES = $(COMMON_CFLAGS) -I./ -I$(ROOTDIR)/lib/tr69Agent/include -I$(ROOTDIR)/lib/tr69Agent/src/include -I$(ROOTDIR)/lib/tr69Agent/src -DTCM_LOG3
$(PROG): $(objects)
	$(CC) $(objects) $(LDFLAGS)  -o $(PROG)
clean:
	rm -f *.o $(PROG)
%.o:%.c
	$(CC) -g $(INCLUDES) -c $< -o $(<:%.c=%.o)
romfs:
	$(ROMFSINST) /bin/$(PROG)
	$(ROMFSINST) -r conf /bin/conf
