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
#include "heap.h"
#include "timer.h"

void stdin_read_callback(int fd, void *arg)
{
	char buff[50];
	fgets(buff, 50, stdin);
	fputs(buff, stdout);
}

void timeout_callback(void *arg)
{
	printf("time out!\n");
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
#if 1
	server_manager *manager = server_manager_create();

	/* 1.监听标准输入 */
	event *ev_stdin = event_create(manager, 0, EPOLLIN, stdin_read_callback, NULL, NULL, NULL);
	event_start(ev_stdin);

	/* 2.监听定时器 */
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	timer *timer1 = timer_new(timeout, timeout_callback, NULL, TIMER_OPT_REPEAT);
	timer_add(manager, timer1);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	timer *timer2 = timer_new(timeout, timeout_callback, NULL, TIMER_OPT_REPEAT);
	timer_add(manager, timer2);

	/* 3.监听端口10001,模拟daytime服务器 */
	inet_address ls_addr_1 = addr_create("any", 10001);
	server *daytime_server = server_create(manager, ls_addr_1, NULL, daytime_accept_callback);

	/* 4.监听端口10002,模拟echo服务器 */
	inet_address ls_addr_2 = addr_create("any", 10002);
	server *echo_server = server_create(manager, ls_addr_2, echo_read_callback, NULL);

	server_manager_run(manager);
#endif

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

#if 0

	/* heap测试代码 */
	heap *h = heap_create(10);
	timer t1; t1.timeout_abs.tv_sec = 1;
	timer t2; t2.timeout_abs.tv_sec = 2;
	timer t3; t3.timeout_abs.tv_sec = 3;
	timer t4; t4.timeout_abs.tv_sec = 2;
	timer t5; t5.timeout_abs.tv_sec = 5;
	timer t6; t6.timeout_abs.tv_sec = 1;

	heap_insert(h, &t4);
	heap_insert(h, &t5);
	heap_insert(h, &t1);
	heap_insert(h, &t2);
	heap_insert(h, &t6);
	heap_insert(h, &t3);
	
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);
	printf("Min = %d\n", heap_delete(h)->timeout_abs.tv_sec);

#endif

	return 0;
}

