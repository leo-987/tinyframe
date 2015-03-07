#include <stdio.h>
#include <string.h>

#include "timer.h"

void timeout_handler(timer *t, void *arg)
{
	printf("%s time out!\n", (char *)arg);
}

int main()
{
	server_manager *manager = server_manager_create();

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	timer *timer1 = timer_new(timeout, timeout_handler, "timer 1", TIMER_OPT_REPEAT);
	timer_add(manager, timer1);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	timer *timer2 = timer_new(timeout, timeout_handler, "timer 2", TIMER_OPT_REPEAT);
	timer_add(manager, timer2);

	timeout.tv_sec = 3;
	timeout.tv_usec = 500000;
	timer *timer3 = timer_new(timeout, timeout_handler, "timer 3", TIMER_OPT_ONCE);
	timer_add(manager, timer3);

	timeout.tv_sec = 4;
	timeout.tv_usec = 500000;
	timer *timer4 = timer_new(timeout, timeout_handler, "timer 4", TIMER_OPT_NONE);
	timer_add(manager, timer4);

	server_manager_run(manager);
	
	return 0;
}

