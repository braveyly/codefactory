#****************************************************************************
#
#  Copyright (c) 2008 TECOM Corporation
#  All Rights Reserved
#  No portions of this material may be reproduced in any form without the
#  written permission of:
#          TECOM Corporation
#          #18, Huaguang Road
#          Wuhan, PR China 430074
#  All information contained in this document is TECOM Corporation
#  company private, proprietary, and trade secret.
# #***************************************************************************** 
ROOTDIR=/home/nick/octopus
INSTALL_DIR=/home/nick/octopus
OBJ=assert.o  eid.o  memory.o  strconv.o  timer.o  timestamp.o logging.o
VERSION=1
LIBNAME=liboal
CFLAGS+=-DTCM_LOG3 -fPIC -g -Wall -c
CFLAGS+=-I$(ROOTDIR)/liboal/liboal
CC=gcc

dynamic: liboal 

all: liboal 

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $*.c

liboal: $(OBJ)
	${CC} -shared -Wl,-soname,${LIBNAME}.so -o ${LIBNAME}.so.${VERSION} ${OBJ} -lpthread -lc -lcrypt
	ln -sf ${LIBNAME}.so.${VERSION} $(LIBNAME).so 

clean:
	rm -f *.o ${LIBNAME}.so*

install:
	cp $(LIBNAME).so* $(INSTALL_DIR)/lib 
#romfs:
#	if [ -f ${LIBNAME}.so.${VERSION} ] ; then \
		$(ROMFSINST) ${LIBNAME}.so.${VERSION} /lib/${LIBNAME}.so.${VERSION} ; \
		$(ROMFSINST) -s $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so ;  \
	fi

