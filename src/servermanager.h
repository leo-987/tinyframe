#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include "epoll.h"
#include "list.h"
#include "listener.h"
#include "event.h"
#include "hash.h"
#include "array.h"

typedef struct epoller_t epoller;

/* server_manager对象,管理server对象 */
typedef struct server_manager_t {

	/* 封装的事件驱动机制 */
	epoller *epoller;

	/* 所有监听事件 */
	//event *events;
	hash_table *events;

	/* 所有就绪事件 */
	event *actives;

}server_manager;

server_manager *server_manager_create();
void server_manager_run(server_manager *manager);

#endif
