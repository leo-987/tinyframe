CC = gcc
CFLAGS = -I ../src -g -O2 -D_REENTRANT -Wall
LIBS = ../src/lib.a -lpthread
EXAMPLE = echo_server echo_client daytime_server daytime_client discard_server
all: $(EXAMPLE)

echo_server: echo_server.o
		${CC} ${CFLAGS} -o $@ echo_server.o ${LIBS}

echo_client: echo_client.o wrap.o
		${CC} ${CFLAGS} -o $@ echo_client.o wrap.o ${LIBS}
	
daytime_server: daytime_server.o
		${CC} ${CFLAGS} -o $@ daytime_server.o ${LIBS}

daytime_client: daytime_client.o wrap.o
		${CC} ${CFLAGS} -o $@ daytime_client.o wrap.o ${LIBS}
	
discard_server: discard_server.o
		${CC} ${CFLAGS} -o $@ discard_server.o ${LIBS}
	
clean:
	rm -f *.o $(EXAMPLE)



		