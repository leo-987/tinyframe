CC = gcc
CFLAGS = -I ../src -g -O2 -D_REENTRANT -Wall
LIBS = ../src/lib.a -lpthread

all: server_test client_test

server_test: server_test.o
		${CC} ${CFLAGS} -o $@ server_test.o ${LIBS}

client_test: client_test.o
		${CC} ${CFLAGS} -o $@ client_test.o ${LIBS}

clean:
	rm -f *.o server_test client_test



		