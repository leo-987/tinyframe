#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/time.h>
#include <sys/epoll.h>

#include "event_base.h"

#define EVENT_READ EPOLLIN
#define EVENT_WRITE EPOLLOUT

typedef void (*callback)(int fd);

typedef struct event_base_t event_base;
typedef struct event_t event;

typedef struct event_t {
	int fd;
	int events;				// 关注的事件
	int actives;			// 就绪事件
	struct timeval time;	// 事件发生时间
	struct event_t *next;

	event_base *base;

	void (*event_handler)(event *ev);
	void (*event_read_cb)(int fd);
	void (*event_write_cb)(int fd);
	
}event;

event* create_event(event_base *base, int fd, short events, callback read_cb, callback write_cb);
int event_add(event *ev);

#endif

