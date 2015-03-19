#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "event_loop.h"
#include "event.h"

void stdin_read_handler(int fd, event *ev, void *arg)
{
	char buff[1024];
	assert(fgets(buff, 1024, stdin) != NULL);
	fputs(buff, stdout);
}

void stdout_write_handler(int fd, event *ev, void *arg)
{
	static int cnt = 0;

	printf("%s writable!\n", (char *)arg);

	cnt++;
	if (cnt == 5)
	{
		event_stop(ev);
		printf("stdout event stop!\n");
	}
}

void timerfd_read_handler(int fd, event *ev, void *arg)
{
	static int cnt = 0;
	char buff[8];
	
	assert(read(fd, buff, 8) > 0);
	printf("%s readable!\n", (char *)arg);

	cnt++;
	if (cnt == 5)
	{
		event_stop(ev);
		printf("timerfd event stop!\n");
	}
}

int main()
{
	event_loop *loop = event_loop_create();
	
	event *ev_stdin = event_create(0, EPOLLIN, stdin_read_handler, NULL, NULL, NULL);
	io_event_add(loop, ev_stdin);
	event_start(ev_stdin);
	
	event *ev_stdout = event_create(1, EPOLLOUT, NULL, NULL, stdout_write_handler, "stdout");
	io_event_add(loop, ev_stdout);
	event_start(ev_stdout);

	int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;
	howlong.it_interval.tv_sec = 1;
	timerfd_settime(timer_fd, 0, &howlong, NULL);
	event *ev_timerfd = event_create(timer_fd, EPOLLIN,
									timerfd_read_handler, "timerfd", NULL, NULL);
	io_event_add(loop, ev_timerfd);
	event_start(ev_timerfd);

	event_loop_run(loop);
	
	return 0;
}


