#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "epoll.h"
#include "event.h"
#include "debug.h"

#define MAX_EVENTS 32

int epoll_fd;	/* epoll文件描述符 */

/* 创建epoll对象 */
static int epoll_init()
{
	epoll_fd = epoll_create(1024);
	if (epoll_fd == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_ret("file: %d, line: %d", __FILE__, __LINE__);
		return -1;
	}
	return 0;
}

static int epoll_add(event *e)
{
	struct epoll_event ev;
	
	ev.events = e->events;
	ev.data.fd = e->fd;
	ev.data.ptr = e;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %d, line: %d", __FILE__, __LINE__);
	}
	return 0;
}

static int epoll_del(event *e)
{
	struct epoll_event ev;
	ev.events = e->events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %d, line: %d", __FILE__, __LINE__);
	}
	return 0;
}

static int epoll_dispatch(event_base *base)
{
	struct epoll_event events[MAX_EVENTS];
	
	int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
	if (nfds == -1)
	{
		if (errno != EINTR)
			debug_sys("file: %d, line: %d", __FILE__, __LINE__);
	}

	/* 记录时间 */
	struct timeval tv;
  	gettimeofday(&tv, NULL);
	
	/* 将就绪事件保存在event_base的actives链表中 */
	int i;
	for (i = 0; i < nfds; i++)
	{
		event *ev = events[i].data.ptr;
		ev->time = tv;
		ev->actives = events[i].events;
		ev->next = base->actives->start;
		base->actives->start = ev;
	}
	
	return nfds;
}

static int epoll_dealloc()
{
	close(epoll_fd);
	return 0;
}

backend* create_backend()
{
	backend *back = (backend *)malloc(sizeof(backend));
	if (back == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	back->name = "epoll";
	back->init = epoll_init;
	back->add = epoll_add;
	back->del = epoll_del;
	back->dispatch = epoll_dispatch;
	back->dealloc = epoll_dealloc;

	return back;
}

