#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "connection.h"
#include "servermanager.h"
#include "event.h"
#include "debug.h"

static void event_readable_callback(int fd, void *arg)
{
	connection *conn = (connection *)arg;
	char buf[65536];

	/* read应该在这一层进行 */
	ssize_t	n = read(fd, buf, 65536);
	if (n > 0)
	{
		if (conn->server->readable_callback)
		{
			/* 用户设定了写回调才push数据,防止input_buffer被撑爆 */
			char *p = array_push_n(conn->input_buffer, n);
			strncpy(p, buf, n);
			
			conn->server->readable_callback(conn);
		}
	}
	else if (n == 0)
	{
		/* FIN */
		connection_free(conn);
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}
}

static void event_writable_callback(int fd, void *arg)
{
	connection *conn = (connection *)arg;
	array *buffer = conn->output_buffer;
	ssize_t n_write = write(fd, buffer->elts, buffer->nelts);
	if (n_write > 0)
	{
		if (n_write == buffer->nelts)
		{
			/* 全部发送出去,清空buffer */
			write_event_disable(conn->conn_event);
			buffer->nelts = 0;
		}
		else
		{
			/* 部分发送,未发送部分搬移到buffer开头 */
			memcpy(buffer->elts, buffer->elts + n_write, buffer->nelts * buffer->size);
			buffer->nelts -= n_write;
		}
	}
	else if (n_write < 0)
	{
		if (errno != EWOULDBLOCK)
			debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		else
			debug_msg("file: %s, line: %d", __FILE__, __LINE__);
	}
}

connection *connection_create(int connfd, server *server)
{
	connection *conn = malloc(sizeof(connection));
	if (conn == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	conn->fd = connfd;
	conn->server = server;
	conn->client_addr = server->listener->client_addr;

	/* 默认大小参考muduo */
	conn->input_buffer = array_create(1024, sizeof(char));
	if (conn->input_buffer == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(conn);
		return NULL;
	}
	
	conn->output_buffer = array_create(1024, sizeof(char));
	if (conn->output_buffer == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		array_free(conn->input_buffer);
		free(conn);
		return NULL;
	}

	conn->conn_event = event_create(server->manager, connfd, EPOLLIN | EPOLLPRI,
				event_readable_callback, conn, event_writable_callback, conn);
	if (conn->conn_event == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		array_free(conn->input_buffer);
		array_free(conn->output_buffer);
		free(conn);
		return NULL;
	}

	/* 跟TCP相关的event才需要对应的connection */
	conn->conn_event->conn = conn;
	event_start(conn->conn_event);

	/* 获得服务器socket地址 */
	socklen_t addr_len = sizeof(conn->server_addr.addr);
	int ret = getsockname(connfd, (struct sockaddr *)&conn->server_addr.addr, &addr_len);
	if (ret < 0)
	{
		debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	}

	/* 将新建立的连接放入server->connections中统一管理 */
#if 0
	conn->next = server->connections;
	conn->prev = NULL;
	server->connections = conn;
	if (conn->next)
		conn->next->prev = conn;
#endif

	hash_table_insert(server->connections, conn->fd, conn);

	return conn;
}

/* 从server中移除一个特定的connection */
void connection_remove(connection *conn)
{
	server *server = conn->server;

#if 0
	if (conn->prev == NULL)
	{
		server->connections = conn->next;
		if (conn->next != NULL)
			conn->next->prev = NULL;
	}
	else
	{
		conn->prev->next = conn->next;
		if (conn->next != NULL)
			conn->next->prev = conn->prev;
	}
	conn->prev = NULL;
	conn->next = NULL;
#endif

	hash_table_remove(server->connections, conn->fd);
}

void connection_free(connection *conn)
{
	connection_remove(conn);
	event_free(conn->conn_event);
	array_free(conn->input_buffer);
	array_free(conn->output_buffer);
	free(conn);
}

void connection_send(connection *conn, char *buf, size_t len)
{
	array *output_buffer = conn->output_buffer;
	ssize_t n_write = 0;
	if (output_buffer->nelts == 0)
	{
		/* 输出buffer中没有数据,直接write */
		n_write = write(conn->fd, buf, len);
		if (n_write < 0)
		{
			n_write = 0;
			
			/* <<UNP>> P341 */
			if (errno != EWOULDBLOCK)
				debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		}
	}
	if (n_write < len)
	{
		/* 不足计数 */
		char *p = array_push_n(output_buffer, len - n_write);
		strncpy(p, buf + n_write, len - n_write);

		/* socket缓冲区可写后回调函数event_writable_callback() */
		write_event_enable(conn->conn_event);

		debug_msg("file: %s, line: %d", __FILE__, __LINE__);
	}
}

