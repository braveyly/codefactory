all:two_dimention_pointer shm_test sem_test syslog msg_test msg_test1 \
printf_16 time unicode_ansi file_size temp_file_name filelock filelocktest

two_dimention_pointer:
	gcc two_dimention_pointer.c -o ./bin/two_dimention_pointer

shm_test:shm_set.c shm_get.c
	gcc shm_set.c -o ./bin/shm_set
	gcc shm_get.c -o ./bin/shm_get

sem_test:sem_test.c
	gcc sem_test.c -lpthread -o sem_test

syslog:syslog.c
	gcc syslog.c -o ./bin/syslog

msg_test:msg_srv.c msg_cli.c
	gcc msg_srv.c -o  ./bin/msg_srv
	gcc msg_cli.c -o  ./bin/msg_cli

msg_test1:msg_snd.c msg_rcv.c
	gcc msg_snd.c -o ./bin/msg_snd
	gcc msg_rcv.c -o ./bin/msg_rcv

printf_16:
	gcc printf_16.c -o ./bin/printf_16

time:
	gcc time.c -o ./bin/time

unicode_ansi:
	gcc unicode_ansi.c -o ./bin/unicode_ansi

file_size:get_file_size.c
	gcc get_file_size.c -o ./bin/get_file_size

temp_file_name:temp_name.c
	gcc temp_name.c -o ./bin/temp_name
	
filelock:fileLockUnlock.c
	gcc fileLockUnlock.c -o ./bin/filelock

filelocktest:fileLockTest.c
	gcc fileLockTest.c -o ./bin/filelocktest

clean:
	rm -rf *.o
	rm -f ./bin/*
