#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include "epoll.h"
#include "list.h"
#include "listener.h"
#include "event.h"

typedef struct epoller_t epoller;

/* server manager层,管理server层 */
typedef struct server_manager_t {
	/* 事件驱动机制 */
	epoller *epoller;

	/* 监听事件链表 */
	event *events;

	/* 就绪事件链表 */
	event *actives;

}server_manager;

server_manager *create_server_manager();
void server_manager_run(server_manager *manager);

#endif
