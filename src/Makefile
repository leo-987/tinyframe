OBJS = epoll.o event.o servermanager.o listener.o inetaddr.o server.o connection.o timer.o
OBJS += heap.o hash.o array.o
OBJS += debug.o

#test: ${OBJS}
#	${CC} ${CFLAGS} -o $@ $?

LIB_NAME = lib.a
RANLIB = ranlib

all: ${OBJS}
	ar rv ${LIB_NAME} $?
	${RANLIB} ${LIB_NAME}

clean:
	rm -f *.o lib.a