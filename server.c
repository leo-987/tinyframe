#include <stdlib.h>
#include <sys/socket.h>

#include "server.h"
#include "connection.h"
#include "debug.h"

/* 建立connection */
void accept_callback(int connfd, listener *ls)
{
	connection *conn = create_connection(connfd, ls->server);
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

server *create_server(server_manager *manager, inet_address *listen_addr,
			readable_callback_pt read_cb, new_conn_callback_pt new_conn_cb)
{
	server *srv = malloc(sizeof(server));
	if (srv == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}

	srv->manager = manager;
	srv->listen_addr = listen_addr;
	srv->readable_callback = read_cb;
	srv->new_connection_callback = new_conn_cb;

	/* 新连接事件会通过listener层调用accept_callback函数 */
	srv->listener = create_listener(srv, accept_callback, listen_addr);
	if (srv->listener == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(srv);
		return NULL;
	}
	return srv;
}


