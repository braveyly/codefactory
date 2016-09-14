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
CC=gcc
ROOTDIR=/home/nick/octopus_release
INSTALL_DIR=$(ROOTDIR)/lib
CFLAGS+=-fPIC -g -Wall -c
CFLAGS+=-I./ -I$(ROOTDIR)/liboal/ -L$(ROOTDIR)/liboal/ -loal
VERSION=1
LIBNAME=libmsg
OBJ=oal_msg.o tcm_msg.o

.PHONY: 

dynamic: libmsg

all: libmsg

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $*.c

libmsg: $(OBJ)
	${CC} -shared -Wl,-soname,${LIBNAME}.so -o ${LIBNAME}.so.${VERSION} ${OBJ} -lc -lcrypt -lpthread 
	ln -sf ${LIBNAME}.so.${VERSION} ${LIBNAME}.so

clean:
	rm -f *.o ${LIBNAME}.so*

install:
	cp *.so* $(INSTALL_DIR)
#romfs:
#	if [ -f ${LIBNAME}.so.${VERSION} ] ; then \
		$(ROMFSINST) ${LIBNAME}.so.${VERSION} /lib/${LIBNAME}.so.${VERSION} ; \
		$(ROMFSINST) -s $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so ;\
	fi

