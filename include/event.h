#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/time.h>
#include <sys/epoll.h>

#include "servermanager.h"
#include "connection.h"

#define EVENT_READ EPOLLIN
#define EVENT_WRITE EPOLLOUT

/* 读写事件回调函数指针 */
typedef void (*event_cb_pt)(int fd, void *arg);

typedef struct event_t event;
typedef struct server_manager_t server_manager;

typedef struct event_t {
	int fd;
	int events;				/* 关注的事件 */
	int actives;			/* 就绪事件集合 */
	int is_listening;		/* 1表示该事件在epoll机制中 */
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

	/* 读写回调函数 */
	void (*event_read_cb)(int fd, void *arg);
	void *r_arg;
	void (*event_write_cb)(int fd, void *arg);
	void *w_arg;	
	void (*event_close_cb)(int fd, void *arg);
	void *c_arg;
	
}event;

event* create_event(server_manager *manager, int fd, short events,
					event_cb_pt read_cb, void *r_arg, event_cb_pt write_cb, void *w_arg);
int event_add(event *ev);
void event_remove(event *ev);
void event_free(event *ev);
event *fd_to_event(server_manager *manager, int fd);

void write_event_enable(event *ev);
void write_event_disable(event *ev);

#endif

