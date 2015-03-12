CC = g++
CPPFLAGS += -isystem include
CPPFLAGS += -isystem ../src
LIBS += lib/libgtest.a
LIBS += ../src/lib.a
LIBS += -pthread
CFLAGS = -I ../src -g -O2 -D_REENTRANT -Wall
TESTS = hashtable_test array_test inetaddr_test heap_test timer_test event_test pressure

all: $(TESTS)

hashtable_test: hashtable_test.o
		${CC} ${CPPFLAGS} -o $@ hashtable_test.o ${LIBS}

array_test: array_test.o
		${CC} ${CPPFLAGS} -o $@ array_test.o ${LIBS}
		
inetaddr_test: inetaddr_test.o
		${CC} ${CPPFLAGS} -o $@ inetaddr_test.o ${LIBS}
		
heap_test: heap_test.c
		gcc ${CFLAGS} -o $@ heap_test.c ../src/lib.a
		
timer_test: timer_test.c
		gcc ${CFLAGS} -o $@ timer_test.c ../src/lib.a

event_test: event_test.c
		gcc ${CFLAGS} -o $@ event_test.c ../src/lib.a

pressure: pressure.c
		gcc ${CFLAGS} -o $@ pressure.c ../src/lib.a
		
clean:
	rm -f *.o $(TESTS)
