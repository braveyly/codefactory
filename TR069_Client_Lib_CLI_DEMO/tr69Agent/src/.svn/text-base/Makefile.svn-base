SRCS = tr69_agent.c

OBJS =%(SRCS:%.c =%.o)

OBIN = tr69_agent.o

INCLUDES += -Iinclude/

ifeq ($(strip $(PLATFORM)),LINUX_32)
INCLUDES += -Ipal/pal_linux_32/
endif

$(OBIN): $(SRCS)
	$(CC) -o $(OBIN) $(CFLAGS) $(INCLUDES) $(DEFS) -c $(SRCS)

clean:
	rm -f *.o


