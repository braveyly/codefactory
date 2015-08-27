#################################################
#
#client interface
#
################################################
PLATFORM = LINUX_32
#CC = gcc
CCOBJECT = $(CC) -fPIC
CCSO = $(CC) -shared
CFLAGS = -Os -Wall -lpthread
STRIP = strip
HOMEDIR=$(ROOTDIR)/lib/tr69Agent
PAL_DIR = $(HOMEDIR)/src/pal/pal_linux_32
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJECTS += $(PAL_DIR)/pal_sem.o $(PAL_DIR)/pal_thread.o 
OBJECTS += $(PAL_DIR)/pal_msg.o $(PAL_DIR)/pal_time.o
#OBJECTS += ../src/tr69_agent.o

PROG = libtcmAgentApi.so
#AGENTLIB = ../libtcmAgent.o
INCLUDES = -DTCM_LOG3 -I$(HOMEDIR)/include

INCLUDES += -I$(HOMEDIR)/src/include -I$(HOMEDIR)/src/pal/pal_linux_32
INCLUDES += -I$(HOMEDIR)/src -I$(ROOTDIR)/user/tr69

$(PROG): $(OBJECTS)
	$(CCSO) -DTR069_NOT_DEFINE -o $(PROG)  $(INCLUDES) $(CFLAGS) $(OBJECTS)
	$(MAKE) -C ./test/

clean:
	rm -f *.o $(PROG)
	@$(MAKE) -C ./test/ clean

%.o:%.c
	$(CCOBJECT) -DTR069_NOT_DEFINE  $(INCLUDES) -c $< -o $(<:%.c=%.o)

romfs:
	$(ROMFSINST) /lib/$(PROG)
#	@$(MAKE) -C ./test/ romfs
