#include <sys/epoll.h>
#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "debug.h"
#include "connection.h"

/* 事件触发时的分发函数 */
static void event_handler(event *ev)
{
	char time_string[40];
	struct tm *ptm;
	ptm = localtime(&ev->time.tv_sec);
	strftime(time_string, sizeof(time_string), "event ready time: %Y-%m-%d %H:%M:%S", ptm);
	debug_msg("%s", time_string);

	if ((ev->actives & EPOLLHUP) && !(ev->actives & EPOLLIN))
	{
		/* 收到RST分节,调用event_close_callback函数 */
		ev->event_close_cb(ev->fd, ev->c_arg);
	}
	if (ev->actives & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (ev->event_read_cb)
			ev->event_read_cb(ev->fd, ev->r_arg);
	}
	if (ev->actives & EPOLLOUT)
	{
		if (ev->event_write_cb)
			ev->event_write_cb(ev->fd, ev->w_arg);
	}
}

static void event_close_callback(int fd, event *ev)
{
	connection_free(ev->conn);
}

/* 建立一个新的事件 */
event* create_event(server_manager *manager, int fd, short events,
					event_cb_pt read_cb, void *r_arg, event_cb_pt write_cb, void *w_arg)
{
	event *ev;
	ev = malloc(sizeof(event));
	if (ev == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	ev->manager = manager;
	ev->fd = fd;
	ev->events = events;
	ev->actives = 0;
	ev->is_listening = 0;
	ev->listen_next = NULL;
	ev->listen_pre = NULL;
	ev->active_next = NULL;
	ev->active_pre = NULL;
	ev->event_handler = event_handler;
	ev->event_read_cb = read_cb;
	ev->event_write_cb = write_cb;
	ev->r_arg = r_arg;
	ev->w_arg = w_arg;
	ev->conn = NULL;

	/* 关闭处理对用户不可见 */
	ev->event_close_cb = event_close_callback;
	ev->c_arg = ev;
	
	return ev;
}

/* 将事件加入epoll机制中,让事件处于监听状态 */
int event_add(event *ev)
{
	server_manager *manager = ev->manager;

	/* 将事件添加到它所属server_manager的events链表的开头 */
	ev->listen_next = manager->events;
	ev->listen_pre = NULL;
	if (ev->listen_next)
		ev->listen_next->listen_pre = ev;
	manager->events = ev;

	/* 将事件添加到epoll机制中 */
	manager->epoller->add_event(manager->epoller, ev);

	ev->is_listening = 1;
	return 0;
}

/* 修改事件ev
 * 例如对fd从监听输入改为监听输出,则改变ev后需要调用此函数更新
 */
int event_modify(event *ev)
{
	server_manager *manager = ev->manager;

	/* epoll_modify() */
	manager->epoller->mod_event(manager->epoller, ev);
	return 0;
}

/* 开启监听写事件 */
void write_event_enable(event *ev)
{
	ev->events |= EPOLLOUT;
	event_modify(ev);
}

/* 关闭监听写事件 */
void write_event_disable(event *ev)
{
	ev->events &= ~EPOLLOUT;
	event_modify(ev);
}


/* 将事件从epoll机制中移除 */
void event_remove(event *ev)
{
	server_manager *manager = ev->manager;

	/* epoll_del() */
	manager->epoller->del_event(manager->epoller, ev);

	/* ev一般不会出现在base的actives链表中,只需处理base的events链表 */
	if (ev->listen_pre == NULL)
	{
		manager->events = ev->listen_next;
		if (ev->listen_next != NULL)
			ev->listen_next->listen_pre = NULL;
	}
	else
	{
		event *pre_node = ev->listen_pre;
		pre_node->listen_next = ev->listen_next;
		if (ev->listen_next != NULL)
			ev->listen_next->listen_pre = pre_node;
	}
	
	ev->listen_next = NULL;
	ev->listen_pre = NULL;
	
	/* ev现在不属于任何链表,且不在epoll中 */

	ev->is_listening = 0;
}

/* 销毁事件,同时close对应的文件描述符 */
void event_free(event *ev)
{
	event_remove(ev);
	close(ev->fd);
	free(ev);
}

/* 由文件描述符映射到event结构体
 * 目前使用的是链表顺序查找
 */
event *fd_to_event(server_manager *manager, int fd)
{
	event *ev = manager->events;
	while (ev)
	{
		if (fd == ev->fd)
			return ev;
		ev = ev->listen_next;
	}
	return NULL;
}


