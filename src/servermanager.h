#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include "epoll.h"
#include "listener.h"
#include "event.h"
#include "hash.h"
#include "array.h"
#include "heap.h"

typedef struct epoller_t epoller;
typedef struct heap_t heap;
typedef struct timer_t timer;

/* server_manager对象,管理server对象 */
typedef struct server_manager_t {

	/* 封装的事件驱动机制 */
	epoller *epoller;

	/* 所有监听事件 */
	//event *events;
	hash_table *events;

	/* 所有就绪事件链表 */
	event *actives;

	/* 定时器用的小根堆 */
	heap *timers;

	/* 已超时timer链表 */
	timer *timeout_timers;

	/* 是否退出event loop */
	int exiting;
	
}server_manager;

server_manager *server_manager_create();
void server_manager_run(server_manager *manager);
void print_running_events(server_manager *manager);


#endif
