#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "epoll.h"
#include "event.h"
#include "debug.h"
#include "hash.h"

#define MAX_EVENTS 32	/* 根据Libevent */

static void epoll_add(epoller *ep, event *e)
{
	struct epoll_event ev;
	
	ev.events = e->events;
	//ev.data.fd = e->fd;
	ev.data.ptr = e;
	
	if (epoll_ctl(ep->fd, EPOLL_CTL_ADD, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

static void epoll_del(epoller *ep, event *e)
{
	/* 判断事件e是否在epoll中,防止重复删除同一事件 */
	if (e->is_listening == 0)
		return;
	
	struct epoll_event ev;
	ev.events = e->events;
	
	if (epoll_ctl(ep->fd, EPOLL_CTL_DEL, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

static void epoll_modify(epoller *ep, event *e)
{
	struct epoll_event ev;
	
	ev.events = e->events;
	//ev.data.fd = e->fd;
	ev.data.ptr = e;
	
	if (epoll_ctl(ep->fd, EPOLL_CTL_MOD, e->fd, &ev) == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

/* 等待事件就绪,事件触发机制的最底层 */
static int epoll_dispatch(server_manager *manager)
{
	int i;
	event *ev;
	struct epoll_event events[MAX_EVENTS];
	int timeout = -1;
	struct timeval now;
	
	if (!heap_is_empty(manager->timers))
	{
		timer *t = heap_top(manager->timers);
		struct timeval diff;
		gettimeofday(&diff, NULL);
		diff.tv_sec = t->timeout_abs.tv_sec - diff.tv_sec;
		diff.tv_usec = t->timeout_abs.tv_usec - diff.tv_usec;
		timeout = tv_to_msec(&diff);
		
		if (timeout < 0)
			timeout = tv_to_msec(&(t->timeout_rel));
	}

	/* 就绪事件在epoll内部排队,当就绪事件大于MAX_EVENTS时
	 * 剩余就绪事件可在下次epoll_wait时获得
	 */
	int nfds = epoll_wait(manager->epoller->fd, events, MAX_EVENTS, timeout);
	
  	gettimeofday(&now, NULL);
	
	if (nfds == 0)
	{
		/* timeout */

		timer *t = heap_top(manager->timers);
		
		//debug_msg("%d %d", t->timeout_abs.tv_sec, t->timeout_abs.tv_usec);
		//debug_msg("%d %d", now.tv_sec, now.tv_usec);

		/* 超时timer全部放入manager->timeout_timers管理 */
		while (timer_cmp(&now, &t->timeout_abs, >))
		{
			t = heap_delete(manager->timers);
			t->next = manager->timeout_timers;
			if (t->next)
				t->next->prev = t;
			manager->timeout_timers = t;

			if (heap_is_empty(manager->timers))
				return 0;
			
			t = heap_top(manager->timers);
		}
	}
	else if (nfds == -1)
	{
		if (errno != EINTR)
			debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		
		/* errno == EINTR,直接返回,在server_manager_run函数中再次进入 */
		return 0;
	}
	
	for (i = 0; i < nfds; i++)
	{
		/* 取出event对象 */
		ev = events[i].data.ptr;
		ev->time = now;
		ev->actives = events[i].events;

#if 0
		ev->is_active = 1;
		
		/* 将就绪事件保存在server_manager中 */
		ev->active_next = manager->actives;
		ev->active_pre = NULL;
		if (ev->active_next)
			ev->active_next->active_pre = ev;
		manager->actives = ev;
#else
		ev->event_handler(ev);
		ev->actives = 0;
#endif

	}
	return nfds;
}

/* 创建封装的事件驱动机制 */
epoller *epoller_create()
{
	epoller *ep = malloc(sizeof(epoller));
	if (ep == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	/* must be greater than zero */
	ep->fd = epoll_create(32000);
	if (ep->fd == -1)
	{
		/* 出错返回-1,errno被设置 */
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(ep);
		return NULL;
	}

	ep->name = "epoll";
	ep->event_add = epoll_add;
	ep->event_mod = epoll_modify;
	ep->event_del = epoll_del;
	ep->event_dispatch = epoll_dispatch;

	return ep;
}

/* 销毁封装的事件驱动机制 */
void epoller_free(epoller *ep)
{
	close(ep->fd);
	free(ep);
}

