all:event_test signal_test time_test echosrv_buffered

event_test:event-test.c
	gcc event-test.c -I./include -L./lib -levent -o event_test

signal_test:signal-test.c
	gcc signal-test.c -I./include -L./lib -levent -o signal_test

time_test:time-test.c
	gcc time-test.c -I./include -L./lib -levent -o time_test

echosrv_buffered:libevent_echosrv_buffered.c
	gcc libevent_echosrv_buffered.c -I./include -L./lib -levent -o echosrv_buffered

clean:
	rm -f *.o
	rm -f event_test signal_test time_test echosrv_buffered
