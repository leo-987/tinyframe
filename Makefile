OBJS = test.o epoll.o event.o list.o servermanager.o listener.o inetaddr.o server.o connection.o array.o
OBJS += debug/debug.o
CC = gcc
CFLAGS = -I include -g -O2 -D_REENTRANT -Wall

test: ${OBJS}
	${CC} ${CFLAGS} -o $@ $?

clean:
	rm -f *.o test