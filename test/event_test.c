#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

#include "servermanager.h"
#include "event.h"

server_manager *g_manager;

void stdin_read_handler(int fd, void *arg)
{
	char buff[1024];
	fgets(buff, 1024, stdin);
	fputs(buff, stdout);
}

void stdout_write_handler(int fd, void *arg)
{
	static int cnt = 0;

	printf("%s writable!\n", (char *)arg);

	cnt++;
	if (cnt == 5)
	{
		event *ev = fd_to_event(g_manager, fd);
		if (ev != NULL)
			event_stop(ev);
		printf("stdout event stop!\n");
	}
}

void timerfd_read_handler(int fd, void *arg)
{
	static int cnt = 0;
	char buff[8];
	
	read(fd, buff, 8);
	printf("%s readable!\n", (char *)arg);

	cnt++;
	if (cnt == 5)
	{
		event *ev = fd_to_event(g_manager, fd);
		if (ev != NULL)
			event_stop(ev);
		printf("timerfd event stop!\n");
	}
}

int main()
{
	server_manager *manager = server_manager_create();
	g_manager = manager;
	
	event *ev_stdin = event_create(manager, 0, EPOLLIN, stdin_read_handler, NULL, NULL, NULL);
	event_start(ev_stdin);
	
	event *ev_stdout = event_create(manager, 1, EPOLLOUT, NULL, NULL, stdout_write_handler, "stdout");
	event_start(ev_stdout);

	int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;
	howlong.it_interval.tv_sec = 1;
	timerfd_settime(timer_fd, 0, &howlong, NULL);
	event *ev_timerfd = event_create(manager, timer_fd, EPOLLIN,
									timerfd_read_handler, "timerfd", NULL, NULL);
	event_start(ev_timerfd);

	server_manager_run(manager);
	
	return 0;
}


