INCLUDES=-I../../include -I../../loggingc/dest/include/
LDFLAGS =-lconfapi -ltpci -L../../loggingc/dest/lib -lexpat -llog4c -Wl,--allow-shlib-undefined
all:
	${CC} $(INCLUDES) -o app exam_client.c $(LDFLAGS)

clean:
	rm -f *.o app

