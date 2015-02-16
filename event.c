#include <sys/epoll.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "debug.h"

static void event_handler(event *ev)
{
	char time_string[40];
	struct tm *ptm;
	ptm = localtime(&ev->time.tv_sec);
	strftime(time_string, sizeof(time_string), "event ready time: %Y-%m-%d %H:%M:%S", ptm);
	debug_msg("%s", time_string);
	
	if (ev->actives& (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (ev->event_read_cb)
			ev->event_read_cb(ev->fd);
	}
	if (ev->actives & EPOLLOUT)
	{
		if (ev->event_write_cb)
			ev->event_write_cb(ev->fd);
	}
}

event* create_event(event_base *base, int fd, short events, callback read_cb, callback write_cb)
{
	event *ev;
	ev = (event *)malloc(sizeof(event));
	if (ev == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
		
	ev->base = base;
	ev->fd = fd;
	ev->events = events;
	ev->actives = 0;
	ev->next = NULL;
	ev->event_handler = event_handler;
	ev->event_read_cb = read_cb;
	ev->event_write_cb = write_cb;

	return ev;
}

int event_add(event *ev)
{
	ev->base->epoller->add(ev);
	return 0;
}


