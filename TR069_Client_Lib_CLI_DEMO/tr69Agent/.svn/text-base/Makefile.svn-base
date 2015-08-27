#The tcmAgent implements the TR069 protocol

############################################
#
#Define Basic Env Variables
#
###########################################
#CC = gcc
MYCC := $(CC)
MYLD := $(LD)
TOOLCHAIN=
PLATFORM = LINUX_32

ifeq ($(strip $(PLATFORM)),WINCE_32)
CROSS_COMPILE = arm-cegcc-
endif
ifeq ($(strip $(PLATFORM)), LINUX_32)
CROSS_COMPILE = arm-linux-
endif
ifeq ($(strip $(PLATFORM)), WXWORKS_32)
CROSS_COMPILE =
endif

CROSS_COMPILE = arm-linux-
export CROSS_COMPILE PLATFORM

#LD = $(CROSS_COMPILE)ld
#CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
CPP = $(CROSS_COMPILE)cpp
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as

############################################
#
#Define target file
#
############################################
AGENTLIB=libtcmAgent.o

#DEFS = -DUSE_SSL -DTR111
DEFS =  -DUSE_SSL
CFLAGS = -Wall

ifeq ($(strip $(PLATFORM)), WINCE_32)
DEFS += -D__USE_W32_SOCKETS -D_WIN32_IE=0x0400
endif

#CC = ucfront-gcc arm-linux-gcc -mbig-endian -msoft-float -march=armv5te -fsigned-char
#LD = ucfront-ld arm-linux-ld -EB
export DEFS CFLAGS  CXX CPP AR AS PLATFORM
 
LDLIBS = 


ifeq ($(strip $(PLATFORM)),LINUX_32)
LIBS = src/pal/pal_linux_32/pal.o
DIRS = src/pal/pal_linux_32
endif

ifeq ($(strip $(PLATFORM)),VXWORKS_32)
LIBS = src/pal/pal_vxworks_32/pal.o
DIRS += src/pal/pal_vxworks_32
endif

ifeq ($(strip $(PLATFORM)),WINCE_32)
LIBS = src/pal/pal_wince_32/pal.o
DIRS += src/pal/pal_wince_32
endif

LIBS += src/stun/stunclient.o src/auth/auth.o src/http/http.o  src/ftp/ftpc.o
LIBS += src/soap/soap.o src/tr69_tree/libtr69_tree.o
LIBS += src/core/core.o src/tr69_agent.o src/xml/xml.o

DIRS += src/xml src/stun src/auth/ src/http  src/ftp/ src/soap src/tr69_tree 
DIRS += src/core src

all: cpfile subdirs
	$(LD) -r -o $(AGENTLIB) $(LIBS) $(LDLIBS)
ifeq ($(strip $(PLATFORM)),LINUX_32)
cpfile:
	cp src/tr69_agent.h include/tr69_agent.h
endif

ifeq ($(strip $(PLATFORM)),VXWORKS_32)
cpfile:
	cp src/tr69_agent.h include/tr69_agent.h
endif

ifeq ($(strip $(PLATFORM)),WINCE_32)
cpfile:
	cp src/tr69_agent.h include/tr69_agent.h
endif

subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%,$(DIRS)):
	echo $(DIRS)
	$(MAKE) -C $(patsubst _dir_%, %, $@)

clean:
	-rm -f *.o
	for n in $(DIRS); do $(MAKE) clean -C $$n || exit; done

romfs:
	#if [ -f $(AGENTLIB) ] ; then \
	#	$(ROMFSINST) $(AGENTLIB) /lib/$(AGENTLIB) ; \
	#fi
