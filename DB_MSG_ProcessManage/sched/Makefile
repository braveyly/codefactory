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
ROOTDIR=/home/nick/alllibs
OBJ=launcher.o main.o tpciserv.o dls.o prctl.o timer.o interest.o
CFLAGS+=-fPIC -g -Wall -c
CFLAGS+=-I./ -I$(ROOTDIR)/include  -DTCM_LOG3
LDFLAGS+=-L$(ROOTDIR)/lib  -loal -lmsg -lrt
PROG=sched

all: $(PROG) 

.c.o:
	$(CC) $(CFLAGS) $*.c

$(PROG): $(OBJ)
	${CC} -o ${PROG} ${OBJ} -lpthread -lc -lcrypt ${LDFLAGS}

clean:
	rm -f *.o ${PROG}

romfs:
	$(ROMFSINST) /bin/$(PROG)

