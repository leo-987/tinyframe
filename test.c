#include <sys/timerfd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#include "servermanager.h"
#include "event.h"
#include "inetaddr.h"
#include "listener.h"
#include "server.h"
#include "connection.h"
#include "array.h"
#include "hash.h"

void stdin_read_callback(int fd, void *arg)
{
	char buff[50];
	fgets(buff, 50, stdin);
	fputs(buff, stdout);
}

void timer_read_callback(int fd, void *arg)
{
	char buff[8];
	read(fd, buff, 8);
	printf("time out!\n");
	server_manager *manager = (server_manager *)arg;
	event *ev = fd_to_event(manager, fd);
	if (ev)
		event_free(ev);
}

void daytime_accept_callback(connection *conn)
{
	char buff[65536];
	time_t ticks = time(NULL);
	//snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

	/* 测试: 发送大量数据 */
	int i;
	for (i = 0; i < 65536; i++)
		buff[i] = 'x';
	buff[i - 3] = 'w';
	buff[i - 2] = '\r';
	buff[i - 1] = '\n';
	connection_send(conn, buff, strlen(buff));

	/* 这里不能主动关闭连接,因为send有可能没有完全发送 */
	//connection_free(conn);
}

void echo_read_callback(connection *conn)
{
	char *buf = malloc(conn->input_buffer->nelts);
	int n = array_copy(conn->input_buffer, buf, conn->input_buffer->nelts);
	connection_send(conn, buf, n);
	free(buf);
}

int main()
{
	server_manager *manager = server_manager_create();
	if (manager == NULL)
		return -1;

	/* 1.监听标准输入 */
	event *ev_stdin = event_create(manager, 0, EVENT_READ, stdin_read_callback, NULL, NULL, NULL);
	event_start(ev_stdin);

	/* 2.监听定时器 */
	int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;
	howlong.it_interval.tv_sec = 1;
	timerfd_settime(timer_fd, 0, &howlong, NULL);
	event *timer_ev = event_create(manager, timer_fd, EVENT_READ,
								timer_read_callback, manager, NULL, NULL);
	event_start(timer_ev);

	/* 3.监听端口2015,模拟daytime服务器 */
	inet_address ls_addr_1 = addr_create("any", 2015);
	server *daytime_server = server_create(manager, ls_addr_1, NULL, daytime_accept_callback);

	/* 4.监听端口2016,模拟echo服务器 */
	inet_address ls_addr_2 = addr_create("any", 2016);
	server *echo_server = server_create(manager, ls_addr_2, echo_read_callback, NULL);

	server_manager_run(manager);


#if 0
	hash_table *ht = hash_table_create(100);
	hash_table_insert(ht, 1, NULL);
	hash_table_insert(ht, 11, NULL);
	hash_table_insert(ht, 101, NULL);
	hash_table_insert(ht, 2, NULL);
	hash_table_insert(ht, 123, NULL);
	
	hash_node *node = hash_table_find(ht, 123);
	if (node)
		printf("find %d\n", node->key);
	
	node = hash_table_find(ht, 100);
	if (node)
		printf("find %d\n", node->key);

	hash_table_remove(ht, 101);
	hash_table_remove(ht, 101);
	hash_table_remove(ht, 1);

	hash_table_print(ht);
#endif


	return 0;
}

