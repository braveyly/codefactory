ROOTDIR=/home/nick/alllibs
CFLAGS=-I./ -I$(ROOTDIR)/include -DTCM_LOG3
INCLUDES= $(CFLAGS)
LDFLAGS+=-L$(ROOTDIR)/lib  -loal -lconfapi -lconfact -lmsg -lmsg -lconfral -lrt

all:
	${CC} $(INCLUDES) $(LDFLAGS) -o test -Wall test.c 
	${CC} $(INCLUDES) $(LDFLAGS) -Wall test1.c -o test_thread 

clean:
	rm -f *.o test test_thread

