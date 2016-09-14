ROOTDIR=/home/nick/octopus
CFLAGS=-I./ -I$(ROOTDIR)/conflib/conflib -DTCM_LOG3
INCLUDES= $(CFLAGS)
LDFLAGS+=-L$(ROOTDIR)/lib  -lpthread -lconfapi

all:
	${CC} $(INCLUDES)  -g -o test -Wall test.c $(LDFLAGS)
	${CC} $(INCLUDES) -Wall -g  test1.c -o test_thread $(LDFLAGS)

clean:
	rm -f *.o test test_thread

