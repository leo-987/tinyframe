#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "event.h"
#include "inetaddr.h"
#include "server.h"
#include "array.h"

typedef struct connection_t connection;
typedef struct server_t server;

/* connection层,管理socket对的读写 */
typedef struct connection_t {
	int fd;
	event *conn_event;	/* 连接对应的事件结构体 */
	server *server;		/* connection所属server */
	
	inet_address server_addr;
	inet_address client_addr;
	
	//connection *prev;
	//connection *next;

	/* 连接对应的输入buffer */
	array *input_buffer;

	/* 连接对应的输出buffer */
	array *output_buffer;
	
}connection;

connection *connection_create(int conn_fd, server *server);
void connection_free(connection *conn);

#endif

