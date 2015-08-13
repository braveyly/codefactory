CC = gcc
OBJS = fcntl_write fcntl_read
CFLAGS = -Wall -g

all: $(OBJS)

fcntl_write: fcntl_write.c lock_set.c
	$(CC) $(CFLAGS) $^ -o $@
fcntl_read: fcntl_read.c lock_set.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS)