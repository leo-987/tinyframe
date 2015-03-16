#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "epoll.h"
#include "event.h"
#include "debug.h"
#include "hash.h"
#include "config.h"

#define MAX_EVENTS 32	/* 根据Libevent */

void epoller_add(int epoll_fd, event *e)
{
	struct epoll_event ev;

	//ev.data.fd = e->fd;
	ev.events = e->events;
	ev.data.ptr = e;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, e->fd, &ev) == -1)
	{
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

void epoller_del(int epoll_fd, event *e)
{
	struct epoll_event ev;
	
	ev.events = e->events;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, e->fd, &ev) == -1)
	{
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

void epoller_modify(int epoll_fd, event *e)
{
	struct epoll_event ev;

	//ev.data.fd = e->fd;
	ev.events = e->events;
	ev.data.ptr = e;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, e->fd, &ev) == -1)
	{
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

struct timeval epoller_dispatch(int epoll_fd, int timeout)
{
	int i;
	event *ev;
	struct epoll_event events[MAX_EVENTS];
	struct timeval now;

	/* 就绪事件在epoll内部排队,当就绪事件大于MAX_EVENTS时
	 * 剩余就绪事件可在下次epoll_wait时获得
	 */
	int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
	
	gettimeofday(&now, NULL);
	//debug_msg("tid = %ld", (long int)gettid());

	if (nfds == 0)
	{
		/* timeout */
	}
	else if (nfds == -1)
	{
		if (errno != EINTR)
			debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		
		/* EINTR */
	}

	/* 取出event对象 */
	for (i = 0; i < nfds; i++)
	{
		ev = events[i].data.ptr;
		ev->time = now;
		ev->actives = events[i].events;
		event_handler(ev);
	}
	
	return now;
}

/* 创建epoll */
int epoller_create()
{
	int epoll_fd;
	
	/* must be greater than zero */
	epoll_fd = epoll_create(32000);
	if (epoll_fd == -1)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return -1;
	}
	
	return epoll_fd;
}

/* 销毁epoll */
void epoller_free(int epoll_fd)
{
	close(epoll_fd);
}

