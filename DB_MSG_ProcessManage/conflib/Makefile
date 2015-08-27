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
ROOTDIR=/home/nick/alllibs
INSTALL_DIR=/home/nick/alllibs
OBJ=configdata.o configapi.o 
CFLAGS=-DTCM_LOG3 -fPIC -g -Wall -c  -I$(ROOTDIR)/include
VERSION=1
LIBNAME=libconfapi
CC=gcc
dynamic: configapi install 

all: configapi  

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $*.c

configapi: $(OBJ)
	${CC} -shared -Wl,-soname,${LIBNAME}.so -o ${LIBNAME}.so.${VERSION} ${OBJ} -lc -lcrypt -lpthread 
	ln -sf ${LIBNAME}.so.${VERSION} ${LIBNAME}.so

test:
#	$(MAKE) -C ./test/
	#@echo "Run test:"
	#@echo "export LD_LIBRARY_PATH=./"
	#@echo "./test_config"

install:
#ifneq ($(strip $(INSTALL_DIR)),)
	cp $(LIBNAME).so.$(VERSION) $(INSTALL_DIR)/lib
	cd $(INSTALL_DIR)/lib; /sbin/ldconfig -n .
#endif

clean:
	rm -f *.o ${LIBNAME}.so*
	rm -f test1 test_config
#	@$(MAKE) -C ./test/ clean

#romfs:
#	if [ -f $(LIBNAME).so.$(VERSION) ] ; then \
		$(ROMFSINST) $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so.$(VERSION) ; \
		$(ROMFSINST) -s $(LIBNAME).so.$(VERSION) /lib/$(LIBNAME).so ; \
		$(ROMFSINST) /etc/os.conf; \
		$(ROMFSINST) os.conf /etc/default.conf; \
	fi

