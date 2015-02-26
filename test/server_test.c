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
	char buff[50];
	time_t ticks = time(NULL);
	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
	connection_send(conn, buff, strlen(buff));
	connection_free(conn);
}

void echo_read_callback(connection *conn)
{
	connection_send(conn, conn->input_buffer->elts, conn->input_buffer->nelts);

	/* 使用完input_buffer后要清空 */
	array_clear(conn->input_buffer);
}


int main()
{
	server_manager *manager = server_manager_create();
	if (manager == NULL)
		return -1;

	/* 1.监听标准输入 */
	event *ev_stdin = event_create(manager, 0, EPOLLIN, stdin_read_callback, NULL, NULL, NULL);
	event_start(ev_stdin);

	/* 2.监听定时器 */
	int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;
	howlong.it_interval.tv_sec = 1;
	timerfd_settime(timer_fd, 0, &howlong, NULL);
	event *timer_ev = event_create(manager, timer_fd, EPOLLIN,
								timer_read_callback, manager, NULL, NULL);
	event_start(timer_ev);

	/* 3.监听端口10001,模拟daytime服务器 */
	inet_address ls_addr_1 = addr_create("any", 10001);
	server *daytime_server = server_create(manager, ls_addr_1, NULL, daytime_accept_callback);

	/* 4.监听端口10002,模拟echo服务器 */
	inet_address ls_addr_2 = addr_create("any", 10002);
	server *echo_server = server_create(manager, ls_addr_2, echo_read_callback, NULL);

	server_manager_run(manager);


#if 0
	/* hash table测试代码 */
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

