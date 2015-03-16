#include <stdlib.h>
#include <errno.h>

#include "event_loop.h"
#include "debug.h"
#include "config.h"

#define MAX_EVENTS 32	/* ¸ù¾ÝLibevent */

event_loop *event_loop_create()
{
	event_loop *loop = malloc(sizeof(event_loop));
	if (loop == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	loop->epoll_fd = epoller_create();
	if (loop->epoll_fd == -1)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(loop);
		return NULL;
	}
	
	loop->tid = pthread_self();
	loop->conn_num = 0;
	
	return loop;
}

void event_loop_run(event_loop *loop)
{
	int timeout = -1;

	while (1)
	{
		epoller_dispatch(loop->epoll_fd, timeout);
	}
}


