#ifndef __LISTENER_H__
#define __LISTENER_H__

#include <sys/types.h>
#include <sys/socket.h>

#include "event.h"
#include "server.h"
#include "inetaddr.h"
#include "servermanager.h"

typedef struct listener_t listener;
typedef struct event_t event;
typedef struct connection_t connection;
typedef struct server_manager_t server_manager;

typedef void (*connection_callback_pt)(connection *conn);

struct listener_t {

	/* ¼àÌıÌ×½Ó×ÖµØÖ· */
	inet_address listen_addr;

	/* ¼àÌıÊÂ¼ş */
	event *ls_event;

	connection_callback_pt new_connection_callback;
	connection_callback_pt readable_callback;
};


listener *listener_create(server_manager *manager, inet_address ls_addr,
						  connection_callback_pt read_cb, connection_callback_pt new_conn_cb);
void listener_free(listener *ls);

#endif

