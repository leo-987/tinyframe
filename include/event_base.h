#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include "epoll.h"
#include "queue.h"

typedef struct backend_t backend;
typedef struct event_list_t event_list;

typedef struct event_base_t {
	/* 事件驱动机制 */
	backend *epoller;

	/* 就绪事件链表 */
	event_list *actives;
}event_base;

event_base *create_event_base();
void event_base_run(event_base *base);

#endif
