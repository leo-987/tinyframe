#ifndef __SERVER_H__
#define __SERVER_H__

#include "inetaddr.h"
#include "listener.h"
#include "servermanager.h"
#include "connection.h"
#include "hash.h"

typedef struct listener_t listener;
typedef struct connection_t connection;
typedef struct server_manager_t server_manager;

typedef void (*connection_callback_pt)(connection *conn);

/* server对象,管理connection对象和listener对象 */
typedef struct server_t {
	server_manager *manager;

	/* 该server所管理的所有connection对象 */
	//connection *connections;
	hash_table *connections;

	/* listener监听对象 */
	listener *listener;

	/* server管理的回调函数由connection层调用
	 * 同一server下的connection拥有相同的回调函数
	 * 写回调函数不由用户设定,connection内部自动处理
	 */
	connection_callback_pt new_connection_callback;
	connection_callback_pt readable_callback;

}server;

server *server_create(server_manager *manager, inet_address listen_addr,
		connection_callback_pt read_cb, connection_callback_pt new_conn_cb);


#endif

