#ifndef __HEAP_H__
#define __HEAP_H__

/* 存放timer的小根堆,参考Libevent */

#include "timer.h"

#define	timer_cmp(tvp, uvp, cmp)			\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?		\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :	\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))


typedef struct heap_t {
	timer **data;
	unsigned int max_size;
	unsigned int current_size;
}heap;

int heap_is_full(heap *h);
int heap_is_empty(heap *h);
heap *heap_create(unsigned int size);
void heap_insert(heap *h, timer *e);
timer *heap_delete(heap *h);
timer *heap_top(heap *h);


#endif

