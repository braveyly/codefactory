EXEC = zwave

OBJS = zwave.o zwctrl.o zwlist.o zwdb.o

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char $(COMMON_CFLAGS) -DTCM_LOG3 -lpthread

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) ${COMMON_LDFLAGS}  -o $@ $(OBJS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

