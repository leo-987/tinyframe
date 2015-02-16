#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "event_base.h"
#include "event.h"

typedef struct event_base_t event_base;
typedef struct event_t event;

/* 事件驱动机制 */
typedef struct backend_t {
	const char *name;
	int (*init)();
	int (*add)(event *ev);
	int (*del)(event *e);
	int (*dispatch)(event_base *base);
	int (*dealloc)();
}backend;

backend* create_backend();

#endif

