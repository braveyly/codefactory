
ROOT = ../../../../..


LIBNAME = libpal.a

SRC = \
   pal_time.c\
   pal_thread.c\
   pal_sem.c\
   pal_mutex.c\
   pal_msg.c\
   pal_log.c\
   pal_debug.c\
   pal_file.c\
   pal_memory.c

CFLAGS =-Ic:/radv/include -I../mcu/inc -I../mcu/cc/callmgr -I../bsp/common/bcm911xx -I../drv/inc -I$(_WIND_BASE)/target/config/all -D__VXWORKS__ -D_VXWORKS 

CFLAGS += $(TR069_C_FLAG) 

CINC = \
	$(CINC_APP)\
   $(CINC_OS)

ASFLAGS =

AINC =

VPATH =

RELEASE = \
   makefile \
   $(SRC)


#-------------------------------------
# Rules
#-------------------------------------

include $(ROOT)/build/build.mk