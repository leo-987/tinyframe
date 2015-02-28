#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "servermanager.h"
#include "inetaddr.h"
#include "connection.h"
#include "debug.h"

/* 创建server_manager对象 */
server_manager *server_manager_create()
{
	server_manager *manager = malloc(sizeof(server_manager));
	if (manager == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	manager->epoller = epoller_create();
	if (manager->epoller == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(manager);
		return NULL;
	}

	manager->timers = heap_create(30);
	if (manager->timers == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		epoller_free(manager->epoller);
		free(manager);
		return NULL;
	}

	manager->events = hash_table_create(100);
	manager->actives = NULL;
	manager->timeout_timers = NULL;
	
	return manager;
}

/* 开始监听事件 */
void server_manager_run(server_manager *manager)
{
	int i;
	event *ev;
	timer *t;
	
	while (1)
	{
		/* epoll_dispatch() */
		int nfds = manager->epoller->event_dispatch(manager);
		
		if (nfds == 0)
		{
			/* 处理超时事件 */
			timer *next;
			for (t = manager->timeout_timers; t != NULL; t = next)
			{
				if (t->timeout_handler&& t->option != TIMER_OPT_NONE)
					t->timeout_handler(t, t->arg);

				next = t->next;
				
				manager->timeout_timers = t->next;
				if (t->next)
					t->next->prev = NULL;
				t->next = NULL;
				t->prev = NULL;

				switch (t->option)
				{
					case TIMER_OPT_NONE:
						debug_msg("timer do NONE");
						break;
					case TIMER_OPT_ONCE:
						debug_msg("timer do ONCE");
						break;
					case TIMER_OPT_REPEAT:
						debug_msg("timer do REPEAT");

						/* 更新时间 */
						t->timeout_abs.tv_sec += t->timeout_rel.tv_sec;
						t->timeout_abs.tv_usec += t->timeout_rel.tv_usec;
						heap_insert(manager->timers, t);
						break;
				}
			}
			continue;
		}

		/* 处理I/O事件 */
		ev = manager->actives;
		for (i = 0; i < nfds; i++)
		{
			ev->event_handler(ev);

			/* 将处理完的事件从actives队列中移除 */
			manager->actives = ev->active_next;
			if (ev->active_next)
				ev->active_next->active_pre = NULL;
			
			ev->active_next = NULL;
			ev->active_pre = NULL;
			ev->actives = 0;
			ev->is_active = 0;
		}
	}
}

/* 打印所有正在监听中的文件描述符 */
void print_running_events(server_manager *manager)
{
	hash_table_print(manager->events);
}


