globalvar:globalvarmode globalvartest
globalvarmode:globalvar.c
	gcc -D__KERNEL__ -DMODULE -DLINUX -I /usr/src/linux/include -c -o globalvar.o globalvar.c
globalvartest:globalvartest.c
	gcc -o globalvartest.o globalvartest.c
