#################################################
#
#client interface
#
################################################
PLATFORM = LINUX_32
#BUILD_DIR = $(shell pwd)
#TOOLCHAIN=$(BUILD_DIR)/../../../toolchains_3_10/uclibc-crosstools
#CROSS_COMPILE = $(TOOLCHAIN)/bin/mips-linux-uclibc-
#CC = $(CROSS_COMPILE)gcc
CC = gcc
CFLAGS = -Os -Wall -lpthread
STRIP = strip

PAL_DIR = ../src/pal/pal_linux_32
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJECTS += $(PAL_DIR)/pal_sem.o $(PAL_DIR)/pal_thread.o 
OBJECTS += $(PAL_DIR)/pal_msg.o $(PAL_DIR)/pal_time.o

PROG = tcmAgentCli
AGENTLIB = ../tr69Agent/libtcmAgent.o
INCLUDES = -I../tr69Agent/include

INCLUDES = -I../src/include -I../src/pal/pal_linux_32
INCLUDES += -I../src

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(INCLUDES) $(CFLAGS) $(OBJECTS)

clean:
	rm -f *.o $(PROG)

%.o:%.c
	$(CC) $(INCLUDES) -c $< -o $(<:%.c=%.o)

