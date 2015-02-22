#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "epoll.h"
#include "event.h"
#include "debug.h"

#define MAX_EVENTS 32	/* 根据Libevent */

static int epoll_add(epoller *ep, event *e)
{
	struct epoll_event ev;
	
	ev.events = e->events;
	ev.data.fd = e->fd;
	ev.data.ptr = e;
	if (epoll_ctl(ep->fd, EPOLL_CTL_ADD, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
	return 0;
}

static int epoll_del(epoller *ep, event *e)
{
	/* 判断事件e是否在epoll中,防止重复删除同一事件 */
	if (e->is_listening == 0)
		return 0;
	
	struct epoll_event ev;
	ev.events = e->events;
	if (epoll_ctl(ep->fd, EPOLL_CTL_DEL, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
	return 0;
}

/* 等待事件就绪 */
static int epoll_dispatch(server_manager *manager)
{
	struct epoll_event events[MAX_EVENTS];
	
	int nfds = epoll_wait(manager->epoller->fd, events, MAX_EVENTS, -1);
	if (nfds == 0)
	{
		/* timeout */
	}
	else if (nfds == -1)
	{
		if (errno != EINTR)
			debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		
		/* errno == EINTR,直接返回,在server_run函数中再次进入 */
		return 0;
	}

	/* 记录时间 */
	struct timeval tv;
  	gettimeofday(&tv, NULL);
	
	/* 将就绪事件保存在server_manager的actives链表开头 */
	int i;
	for (i = 0; i < nfds; i++)
	{
		event *ev = events[i].data.ptr;
		ev->time = tv;
		ev->actives = events[i].events;
		ev->active_next = manager->actives;
		ev->active_pre = NULL;
		if (ev->active_next)
			ev->active_next->active_pre = ev;
		manager->actives = ev;
	}
	return nfds;
}

/* 修改epoll中的事件 */
static int epoll_modify(epoller *ep, event *e)
{
	struct epoll_event ev;
	ev.events = e->events;
	ev.data.fd = e->fd;
	ev.data.ptr = e;
	if (epoll_ctl(ep->fd, EPOLL_CTL_MOD, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
	return 0;
}

epoller* create_epoller()
{
	epoller *ep = malloc(sizeof(epoller));
	if (ep == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	/* 参数参考Libevent */
	ep->fd = epoll_create(32000);
	if (ep->fd == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(ep);
		return NULL;
	}

	ep->name = "epoll";
	ep->add_event = epoll_add;
	ep->mod_event = epoll_modify;
	ep->del_event = epoll_del;
	ep->dispatch = epoll_dispatch;

	return ep;
}

void epoller_free(epoller *ep)
{
	close(ep->fd);
	free(ep);
	return;
}

