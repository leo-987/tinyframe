#include <stdio.h>

#include "servermanager.h"
#include "server.h"

void request_handler(connection *conn)
{
	static char buf[] = "HTTP/1.1 200 0K\r\nConnection:keep-alive\r\nContent-length:4\r\n\r\n"
						"test";

	connection_send(conn, buf, sizeof(buf) - 1);
	array_clear(conn->input_buffer);
}

int main()
{
	server_manager *manager = server_manager_create();

	inet_address addr = addr_create("any", 2016);
	server *server = server_create(manager, addr, request_handler, NULL);
	
	server_manager_run(manager);

	return 0;
}


