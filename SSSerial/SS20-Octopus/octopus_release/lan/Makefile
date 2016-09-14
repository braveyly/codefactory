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
ROOTDIR=/home/nick/octopus_release
CC=gcc
OBJ=lan_main.o
CFLAGS+=-fPIC -g -Wall -c
CFLAGS+=-I./ -I$(ROOTDIR)/libmsg/ -I$(ROOTDIR)/liboal/  -DTCM_LOG3
LDFLAGS+=-L$(ROOTDIR)/libmsg/ -L./ -L$(ROOTDIR)/liboal/ -lmsg -loal
PROG=lan

all: $(PROG) 

.c.o:
	$(CC) $(CFLAGS) $*.c

$(PROG): $(OBJ)
	${CC} -o ${PROG} ${OBJ}  ${LDFLAGS}

clean:
	rm -f *.o ${PROG}

romfs:
	$(ROMFSINST) /bin/$(PROG)

