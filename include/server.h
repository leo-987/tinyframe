#ifndef __SERVER_H__
#define __SERVER_H__

#include "inetaddr.h"
#include "listener.h"
#include "servermanager.h"
#include "connection.h"

typedef struct listener_t listener;
typedef struct connection_t connection;
typedef struct server_manager_t server_manager;

typedef void (*readable_callback_pt)(connection *conn);
typedef void (*new_conn_callback_pt)(connection *conn);

/* server层,管理connection和listener层 */
typedef struct server_t {
	server_manager *manager;
	inet_address *listen_addr;

	/* connection链表 */
	connection *connections;

	/* listener监听对象 */
	listener *listener;

	/* server管理的回调函数由connection层调用
	 * 同一server下的connection拥有相同的回调函数
	 * 写回调函数不由用户设定,connection内部自动处理
	 */
	new_conn_callback_pt new_connection_callback;
	readable_callback_pt readable_callback;

}server;

server *create_server(server_manager *manager, inet_address *listen_addr,
		readable_callback_pt read_cb, new_conn_callback_pt new_conn_cb);


#endif

