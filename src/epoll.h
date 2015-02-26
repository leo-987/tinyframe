#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "servermanager.h"
#include "event.h"

typedef struct epoller_t epoller;
typedef struct event_t event;
typedef struct server_manager_t server_manager;

/* epoll事件驱动机制 */
typedef struct epoller_t {
	const char *name;
	int fd;				/* epoll文件描述符 */
	
	void (*event_add)(epoller *ep, event *ev);
	void (*event_del)(epoller *ep, event *ev);
	void (*event_mod)(epoller *ep, event *ev);
	int (*event_dispatch)(server_manager *manager);
	
}epoller;


epoller *epoller_create();
void epoller_free(epoller *ep);

#endif

