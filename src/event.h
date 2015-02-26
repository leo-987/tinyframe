#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/time.h>
#include <sys/epoll.h>

#include "servermanager.h"
#include "connection.h"

/* 读写事件回调函数指针 */
typedef void (*event_callback_pt)(int fd, void *arg);

typedef struct event_t event;
typedef struct server_manager_t server_manager;

typedef struct event_t {
	int fd;
	int events;				/* 关注的事件 */
	int actives;			/* 就绪事件集合 */
	int is_listening;		/* 1表示该事件处于监听状态中,且在server_manager的events散列表中 */
	int is_active;			/* 1表示该事件就绪但未被处理,且在server_manager的actives散列表中 */
	struct timeval time;	/* 事件发生时间 */
	connection *conn;		/* 事件对应的连接 */

	/* 等待事件链表 */
	event *listen_next;
	event *listen_pre;

	/* 就绪事件链表 */
	event *active_next;
	event *active_pre;

	/* 该事件所属的server_manager */
	server_manager *manager;

	/* 事件回调函数入口 */
	void (*event_handler)(event *ev);

	/* 可读回调 */
	event_callback_pt event_read_handler;
	void *r_arg;

	/* 可写回调 */
	event_callback_pt event_write_handler;
	void *w_arg;

	/* 关闭回调 */
	event_callback_pt event_close_handler;
	void *c_arg;
	
}event;

event* event_create(server_manager *manager, int fd, short events,
	event_callback_pt read_cb, void *r_arg, event_callback_pt write_cb, void *w_arg);
int event_start(event *ev);
void event_stop(event *ev);
void event_free(event *ev);
event *fd_to_event(server_manager *manager, int fd);

void write_event_enable(event *ev);
void write_event_disable(event *ev);

#endif

