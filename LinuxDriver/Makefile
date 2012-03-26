all:hello hello_insmod hello_rmmod

hello:hello.c
	gcc -D__KERNEL__ -DMODULE -DLINUX -I /usr/local/src/linux2.4/include -c -o hello.o hello.c

hello_insmod:
	insmod ./hello.o

hello_rmmod:
	rmmod hello
