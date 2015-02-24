#ifndef __LISTENER_H__
#define __LISTENER_H__

#include <sys/types.h>
#include <sys/socket.h>

#include "event.h"
#include "server.h"
#include "inetaddr.h"

typedef struct listener_t listener;
typedef struct server_t server;
typedef struct event_t event;

typedef void (*accept_callback_pt)(int connfd, listener *ls);

/* listener层,由server对象管理,负责监听端口,用户不可见 */
typedef struct listener_t {

	/* listener所属的event_base */
	server *server;

	/* 监听套接字地址 */
	inet_address listen_addr;
	
	/* 客户端地址,该地址应该迅速传给connection管理 */
	inet_address client_addr;	

	/* 监听事件 */
	event *ls_event;

	/* 新连接到来时的回调 */
	accept_callback_pt accept_callback;
}listener;


listener *listener_create(server *srv, accept_callback_pt callback, inet_address ls_addr);
void listener_free(listener *ls);

#endif

