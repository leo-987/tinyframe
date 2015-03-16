#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "servermanager.h"
#include "event.h"

typedef struct event_t event;

void epoller_add(int epoll_fd, event *e);
void epoller_del(int epoll_fd, event *e);
void epoller_modify(int epoll_fd, event *e);
int epoller_create();
void epoller_free(int epoll_fd);
struct timeval epoller_dispatch(int epoll_fd, int timeout);

#endif

