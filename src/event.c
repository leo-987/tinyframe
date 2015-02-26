#include <sys/epoll.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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

	if (ev->actives & (EPOLLERR | EPOLLHUP))
	{
		/* event_close_callback() */
		ev->event_close_handler(ev->fd, ev->c_arg);
	}
	if (ev->actives & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (ev->event_read_handler)
			ev->event_read_handler(ev->fd, ev->r_arg);
	}
	if (ev->actives & EPOLLOUT)
	{
		if (ev->event_write_handler)
			ev->event_write_handler(ev->fd, ev->w_arg);
	}
}

static void event_close_callback(int fd, void *arg)
{
	event *ev = (event *)arg;
	connection_free(ev->conn);
}

/* 建立一个新的事件
 * @manager : 新事件所属的server_manager
 * @fd : 事件监听的文件描述符
 * @events : 监听的事件类型
 * @read_cb : 事件可读时的回调函数
 * @r_arg : read_cb函数的参数
 * @write_cb : 事件可读时的回调函数
 * @w_arg : write_cb函数的参数
 */
event* event_create(server_manager *manager, int fd, short events,
			event_callback_pt read_cb, void *r_arg, event_callback_pt write_cb, void *w_arg)
{
	event *ev = malloc(sizeof(event));
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
	ev->is_active = 0;
	//ev->listen_next = NULL;
	//ev->listen_pre = NULL;
	ev->active_next = NULL;
	ev->active_pre = NULL;
	ev->conn = NULL;
	
	ev->event_handler = event_handler;
	
	ev->event_read_handler = read_cb;
	ev->r_arg = r_arg;
	
	ev->event_write_handler = write_cb;
	ev->w_arg = w_arg;

	ev->event_close_handler = event_close_callback;		/* 关闭处理对用户不可见 */
	ev->c_arg = ev;

	return ev;
}

/* 将事件加入epoll机制中,让事件处于监听状态 */
int event_start(event *ev)
{
	server_manager *manager = ev->manager;

	manager->epoller->event_add(manager->epoller, ev);

	/* 将事件添加到它所属server_manager的events中 */
#if 0
	ev->listen_next = manager->events;
	ev->listen_pre = NULL;
	if (ev->listen_next)
		ev->listen_next->listen_pre = ev;
	manager->events = ev;
#endif

	ev->is_listening = 1;
	hash_table_insert(manager->events, ev->fd, ev);

	return 0;
}

/* 修改事件ev
 * 例如对fd从监听输入改为监听输出,则改变ev后需要调用此函数更新
 */
int event_modify(event *ev)
{
	server_manager *manager = ev->manager;

	/* epoll_modify() */
	manager->epoller->event_mod(manager->epoller, ev);
	return 0;
}

/* 将事件从epoll机制中移除 */
void event_stop(event *ev)
{
	server_manager *manager = ev->manager;

	/* epoll_del() */
	manager->epoller->event_del(manager->epoller, ev);

#if 0
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
#endif

	if (ev->is_listening)
	{
		hash_table_remove(manager->events, ev->fd);
		ev->is_listening = 0;
	}
}

/* 销毁事件,同时close对应的文件描述符 */
void event_free(event *ev)
{
	event_stop(ev);
	close(ev->fd);
	free(ev);
}

/* 由文件描述符映射到event结构体 */
event *fd_to_event(server_manager *manager, int fd)
{
#if 0
	while (ev)
	{
		if (fd == ev->fd)
			return ev;
		ev = ev->listen_next;
	}
#endif

	hash_node *hn = hash_table_find(manager->events, fd);
	if (hn)
		return hn->value;
	
	return NULL;
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

