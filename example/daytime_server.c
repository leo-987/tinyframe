#include <stdio.h>
#include <time.h>
#include <string.h>

#include "servermanager.h"
#include "server.h"

void daytime_handler(connection *conn)
{
	char buff[50];
	
	time_t ticks = time(NULL);
	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
	connection_send(conn, buff, strlen(buff));
	connection_free(conn);
}

int main()
{
	server_manager *manager = server_manager_create();

	inet_address addr = addr_create("any", 13);
	server *daytime_server = server_create(manager, addr, NULL, daytime_handler);

	server_manager_run(manager);

	return 0;
}

