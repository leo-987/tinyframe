#include <stdio.h>

#include "timer.h"
#include "heap.h"

static void heap_test()
{
	heap *h = heap_create(3);

	timer t1;
	t1.timeout_abs.tv_sec = 1;
	t1.timeout_abs.tv_usec = 100;

	timer t2;
	t2.timeout_abs.tv_sec = 2;
	t2.timeout_abs.tv_usec = 200;
	
	timer t3;
	t3.timeout_abs.tv_sec = 3;
	t3.timeout_abs.tv_usec = 300;
	
	timer t4;
	t4.timeout_abs.tv_sec = 2;
	t4.timeout_abs.tv_usec = 400;
	
	timer t5;
	t5.timeout_abs.tv_sec = 5;
	t5.timeout_abs.tv_usec = 500;
	
	timer t6;
	t6.timeout_abs.tv_sec = 1;
	t6.timeout_abs.tv_usec = 200;

	if (heap_is_empty(h))
		printf("OK\n");
	
	heap_insert(h, &t4);
	heap_insert(h, &t5);
	heap_insert(h, &t1);
	heap_insert(h, &t2);
	heap_insert(h, &t6);
	heap_insert(h, &t3);

	if (heap_is_full(h))
		printf("OK\n");

	timer *top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}
	top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}
	top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}
	top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}
	top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}
	top = heap_top(h);
	if (top != NULL)
	{
		top = heap_delete(h);
		printf("Top = %ld.%ld\n", top->timeout_abs.tv_sec, top->timeout_abs.tv_usec);
	}

	if (heap_is_empty(h))
		printf("Success\n");
	else
		printf("Failed\n");
}

int main(int argc, char** argv)
{
	heap_test();
	return 0;
}

