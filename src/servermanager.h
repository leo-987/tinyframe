#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include "epoll.h"
#include "list.h"
#include "listener.h"
#include "event.h"
#include "hash.h"
#include "array.h"
#include "heap.h"

typedef struct epoller_t epoller;

/* server_manager����,����server���� */
typedef struct server_manager_t {

	/* ��װ���¼��������� */
	epoller *epoller;

	/* ���м����¼� */
	//event *events;
	hash_table *events;

	/* ���о����¼����� */
	event *actives;

	/* ��ʱ���õ�С���� */
	heap *timers;

	/* �ѳ�ʱtimer���� */
	timer *timeout_timers;

}server_manager;

server_manager *server_manager_create();
void server_manager_run(server_manager *manager);
void print_running_events(server_manager *manager);


#endif