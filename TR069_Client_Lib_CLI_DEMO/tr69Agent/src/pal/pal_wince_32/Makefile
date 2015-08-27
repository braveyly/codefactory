objects := $(patsubst %.c,%.o,$(wildcard *.c))

OBIN= pal.o
INCLUDES = -I../../ -I../../include

$(OBIN): $(objects)
	$(LD) -r -o $(OBIN) $(objects)

clean:
	rm -f *.o

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $(<:%.c=%.o)

