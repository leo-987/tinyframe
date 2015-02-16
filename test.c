#include <sys/timerfd.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include "event_base.h"
#include "event.h"

void read_callback(int fd)
{
	char buff[50];
	fgets(buff, 50, stdin);
	fputs(buff, stdout);
}

void timer_callback(int fd)
{
	char buff[8];
	read(fd, buff, 8);
	printf("time out!\n");
}

int main()
{
	event_base *base = create_event_base();
	event *ev = create_event(base, 0, EVENT_READ, read_callback, NULL);
	event_add(ev);

	int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;
	howlong.it_interval.tv_sec = 1;
	timerfd_settime(timer_fd, 0, &howlong, NULL);
	event *timer_ev = create_event(base, timer_fd, EVENT_READ, timer_callback, NULL);
	event_add(timer_ev);

	event_base_run(base);
	
	return 0;
}

