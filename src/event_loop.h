#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include <pthread.h>

#include "servermanager.h"
#include "connection.h"
#include "event.h"

typedef struct event_loop_t event_loop;
typedef struct event_t event;
typedef struct connection_t connection;
typedef void (*connection_callback_pt)(connection *conn);

struct event_loop_t {
	int epoll_fd;
	
	/* event_loop对应的线程ID */
	pthread_t tid;

	/* event_loop管理的连接个数 */
	int conn_num;
};

event_loop *event_loop_create();
void event_loop_run(event_loop *el);

#endif

