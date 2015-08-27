OBJ:=$(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -fPIC -g -Wall -c -I$(ROOTDIR)/user/openssl-0.9.8e/include -I$(ROOTDIR)/lib/libtcmvoiputils
CFLAGS += $(COMMON_THREAD_CFLAGS) -DTCM_LOG3
PROG=ehcc

all: $(PROG)

$(OBJ): %.o : %.c
	$(CC) $(INCLUDES) $(CFLAGS) $*.c
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(PROG): $(OBJ) ehDeviceDef.h
	${CC} -o ${PROG} ${OBJ} -lpthread -lc -lcrypt -L$(ROOTDIR)/user/openssl-0.9.8e -lcrypto -lssl ${COMMON_THREAD_LDFLAGS} -L$(ROOTDIR)/lib/libtcmvoiputils -ltcmvoiputils

clean:
	rm -f *.o ${PROG}

romfs:
	$(ROMFSINST) /bin/$(PROG)

