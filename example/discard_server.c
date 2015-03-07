#include <stdio.h>

#include "servermanager.h"
#include "server.h"

void discard_handler(connection *conn)
{
	array_clear(conn->input_buffer);
}

int main()
{
	server_manager *manager = server_manager_create();

	inet_address addr = addr_create("any", 9);
	server *discard_server = server_create(manager, addr, discard_handler, NULL);
	
	server_manager_run(manager);

	return 0;
}

