#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/time.h>
#include <sys/epoll.h>

#include "servermanager.h"
#include "event_loop.h"
#include "connection.h"

enum EVENT_TYPE {
	IO_EVENT,
	ACCEPT_EVENT
};

union BELONG {
	server_manager *manager;
	event_loop *loop;
};

typedef struct event_t event;
typedef struct server_manager_t server_manager;
typedef void (*event_callback_pt)(int fd, event *ev, void *arg);

struct event_t {
	int fd;
	int events;				/* 关注的事件类型 */
	int actives;			/* 就绪事件类型 */
	int is_listening;		/* 1表示该事件处于监听状态中 */
	struct timeval time;	/* 事件发生时间 */
	connection *conn;

	enum EVENT_TYPE type;
	union BELONG belong;

	/* 可读回调 */
	event_callback_pt event_read_handler;
	void *r_arg;

	/* 可写回调 */
	event_callback_pt event_write_handler;
	void *w_arg;
};

event* event_create(int fd, short events, event_callback_pt read_cb,
					void *r_arg, event_callback_pt write_cb, void *w_arg);
int event_start(event *ev);
void event_stop(event *ev);
void event_free(event *ev);
event *fd_to_event(server_manager *manager, int fd);

void write_event_enable(event *ev);
void write_event_disable(event *ev);

#endif

