objects := $(patsubst %.c,%.o,$(wildcard *.c))

OBIN= xml.o
INCLUDES += -I../ -I../include/ -I../auth/
ifeq ($(strip $(PLATFORM)),LINUX_32)
INCLUDES += -I../pal/pal_linux_32/
endif


$(OBIN): $(objects)
	$(LD) -r -o $(OBIN) $(objects)

clean:
	rm -f *.o

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $(<:%.c=%.o)

