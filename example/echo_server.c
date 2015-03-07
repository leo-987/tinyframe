#include <stdio.h>

#include "servermanager.h"
#include "server.h"

void echo_read_handler(connection *conn)
{
	connection_send(conn, conn->input_buffer->elts, conn->input_buffer->nelts);

	/* 使用完input_buffer后要清空 */
	array_clear(conn->input_buffer);
}

int main()
{
	server_manager *manager = server_manager_create();

	inet_address addr = addr_create("any", 7);
	server *echo_server = server_create(manager, addr, echo_read_handler, NULL);
	
	server_manager_run(manager);

	return 0;
}


