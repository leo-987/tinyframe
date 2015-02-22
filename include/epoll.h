#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "servermanager.h"
#include "event.h"

typedef struct epoller_t epoller;
typedef struct event_t event;
typedef struct server_manager_t server_manager;

/* 后端事件驱动机制,目前只实现epoll */
typedef struct epoller_t {
	const char *name;
	int fd;			/* epoll文件描述符 */
	int (*add_event)(epoller *ep, event *ev);
	int (*del_event)(epoller *ep, event *ev);
	int (*mod_event)(epoller *ep, event *ev);
	int (*dispatch)(server_manager *manager);
}epoller;

/* 创建后端事件驱动机制 */
epoller* create_epoller();

/* 销毁后端事件驱动机制 */
void epoller_free(epoller *ep);

#endif

