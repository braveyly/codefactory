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
#***************************************************************************** 
CC=gcc
ROOTDIR=/home/nick/alllibs
INSTALL_DIR=$(ROOTDIR)/lib
OBJ := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS=-DTCM_LOG3 -fPIC -g -Wall -c -I$(ROOTDIR)/include/
VERSION=1
LIBNAME=libconfral

dynamic: configral install 

all: configral

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $*.c

configral: $(OBJ)
	${CC} -shared -Wl,-soname,${LIBNAME}.so -o ${LIBNAME}.so.${VERSION} ${OBJ} -lc -lcrypt -lpthread 
	ln -sf ${LIBNAME}.so.${VERSION} ${LIBNAME}.so

install:
ifneq ($(strip $(INSTALL_DIR)),)
	cp $(LIBNAME).so.$(VERSION) $(INSTALL_DIR)
	cd $(INSTALL_DIR); /sbin/ldconfig -n .
endif

clean:
	rm -f *.o ${LIBNAME}.so*

romfs:
	if [ -f $(LIBNAME).so.$(VERSION) ] ; then \
		$(ROMFSINST) $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so.$(VERSION) ; \
		$(ROMFSINST) -s $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so ; \
	fi

