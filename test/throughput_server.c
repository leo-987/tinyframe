#include <stdio.h>
#include <time.h>
#include <string.h>

#include "servermanager.h"
#include "server.h"

void echo_handler(connection *conn)
{
	connection_send(conn, conn->input_buffer->elts, conn->input_buffer->nelts);
	array_clear(conn->input_buffer);
}

int main()
{
	server_manager *manager = server_manager_create();

	inet_address addr = addr_create("any", 2015);
	server *server = server_create(manager, addr, echo_handler, NULL);

	server_manager_run(manager);
	
	return 0;
}

