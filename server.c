#include <stdlib.h>
#include <sys/socket.h>

#include "server.h"
#include "connection.h"
#include "debug.h"

/* 接受新连接,建立connection
 * @connfd : 已连接套接字描述符
 * @ls : 接受新连接的listener对象
 */
void accept_callback(int connfd, listener *ls)
{
	connection *conn = connection_create(connfd, ls->server);
	if (conn == NULL)
	{
		debug_quit("file: %s, line: %d", __FILE__, __LINE__);
	}
	
	if (ls->server->new_connection_callback)
		ls->server->new_connection_callback(conn);
}

void set_read_callback(server *server)
{

}

void set_write_callback(server *server)
{

}

server *server_create(server_manager *manager, inet_address listen_addr,
			connection_callback_pt readable_cb, connection_callback_pt new_conn_cb)
{
	server *srv = malloc(sizeof(server));
	if (srv == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	srv->manager = manager;
	srv->readable_callback = readable_cb;
	srv->new_connection_callback = new_conn_cb;

	srv->listener = listener_create(srv, accept_callback, listen_addr);
	if (srv->listener == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(srv);
		return NULL;
	}

	srv->connections = hash_table_create(100);
	if (srv->connections == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		listener_free(srv->listener);
		free(srv);
		return NULL;
	}
	
	return srv;
}


