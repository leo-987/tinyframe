.PHONY: all clean

CC = gcc

RMFLAGS = -rf
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

LIBS = ../src/lib.a -pthread
CFLAGS = -I ../src -g -O2 -D_REENTRANT -Wall
EXES = array_test hashtable_test event_test heap_test inetaddr_test timer_test stress_tinyframe stress_libevent stress_muduo

all: $(EXES)

array_test: array_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)

event_test: event_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)
	
hashtable_test: hashtable_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)	

heap_test: heap_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)
	
inetaddr_test: inetaddr_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)
	
timer_test: timer_test.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)

stress_tinyframe: stress_tinyframe.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS)

stress_libevent: stress_libevent.o
	${CC} ${CFLAGS} -o $@ $^ $(LIBS) -levent
	
stress_muduo: stress_muduo.cc
	g++ -I/work/build/release-install/include -o $@ $^ -L/work/build/release-install/lib -lmuduo_net -lmuduo_base -lpthread
	
clean:
	rm -f $(OBJS) $(TESTS)
