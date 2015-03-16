#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "servermanager.h"
#include "inetaddr.h"
#include "connection.h"
#include "debug.h"
#include "event_loop.h"
#include "config.h"

void *spawn_thread(void *arg)
{
	int i = (int)arg;
	
	loops[i] = event_loop_create();
	event_loop_run(loops[i]);
}

server_manager *server_manager_create()
{
	int i;
	pthread_t tid;
	server_manager *manager = malloc(sizeof(server_manager));
	if (manager == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	manager->epoll_fd= epoller_create();
	if (manager->epoll_fd == -1)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(manager);
		return NULL;
	}

	manager->timers = heap_create(30);
	if (manager->timers == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		epoller_free(manager->epoll_fd);
		free(manager);
		return NULL;
	}

	for (i = 0; i < MAX_LOOP; i++)
	{
		pthread_create(&tid, NULL, spawn_thread, (void *)i);
	}
	
	return manager;
}

int calc_timeout(server_manager *manager)
{
	int timeout = -1;
	struct timeval diff;
	timer *t;

	t = heap_top(manager->timers);
	if (t == NULL)
		return timeout;
	
	gettimeofday(&diff, NULL);
	diff.tv_sec = t->timeout_abs.tv_sec - diff.tv_sec;
	diff.tv_usec = t->timeout_abs.tv_usec - diff.tv_usec;
	timeout = tv_to_msec(&diff);
	
	if (timeout < 0)
		timeout = tv_to_msec(&(t->timeout_rel));
	
	return timeout;
}

void process_timer_event(server_manager *manager, struct timeval trigger_time)
{
	timer *t = heap_top(manager->timers);
	timer *next;
	
	//debug_msg("%d %d", t->timeout_abs.tv_sec, t->timeout_abs.tv_usec);
	//debug_msg("%d %d", now.tv_sec, now.tv_usec);

	if (!timer_cmp(&trigger_time, &t->timeout_abs, >))
		return;

	/* 超时timer全部放入manager->timeout_timers管理 */
	while (timer_cmp(&trigger_time, &t->timeout_abs, >))
	{
		t = heap_delete(manager->timers);

#if 0
		t->next = manager->timeout_timers;
		if (t->next)
			t->next->prev = t;
		manager->timeout_timers = t;
#endif

		if (t->timeout_handler&& t->option != TIMER_OPT_NONE)
			t->timeout_handler(t, t->arg);

		switch (t->option)
		{
			case TIMER_OPT_NONE:
				break;
			case TIMER_OPT_ONCE:
				break;
			case TIMER_OPT_REPEAT:
				/* 更新时间 */
				t->timeout_abs.tv_sec += t->timeout_rel.tv_sec;
				t->timeout_abs.tv_usec += t->timeout_rel.tv_usec;
				heap_insert(manager->timers, t);
				break;
		}

		t = heap_top(manager->timers);
		if (t == NULL)
			break;
	}

#if 0
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
				break;
			case TIMER_OPT_ONCE:
				break;
			case TIMER_OPT_REPEAT:
				/* 更新时间 */
				t->timeout_abs.tv_sec += t->timeout_rel.tv_sec;
				t->timeout_abs.tv_usec += t->timeout_rel.tv_usec;
				heap_insert(manager->timers, t);
				break;
		}
	}
#endif
}

void server_manager_run(server_manager *manager)
{
	int timeout = -1;
	struct timeval trigger_time;
	
	while (1)
	{
		timeout = calc_timeout(manager);
		trigger_time = epoller_dispatch(manager->epoll_fd, timeout);

		if (timeout == -1)
			continue;
		
		process_timer_event(manager, trigger_time);
	}
}

