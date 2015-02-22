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

/*
 * 新连接到来时的用户回调函数
 * @sockfd : 已连接套接字描述符
 * @sock_addr : 客户端地址
 */
typedef void (*accept_cb_pt)(int connfd, listener *ls);

/* listener层,负责监听端口 */
typedef struct listener_t {
	int listen_fd;				/* 监听描述符 */
	server *server;				/* listener所属的event_base */	

	/* 监听套接字地址 */
	inet_address *listen_addr;
	
	/* 客户端地址,该地址应该迅速传给connection管理 */
	inet_address client_addr;	

	
	event *ls_event;			/* 监听事件 */
	accept_cb_pt accept_cb;		/* 新连接到来时的回调 */
}listener;


listener *create_listener(server *srv, accept_cb_pt callback, inet_address *ls_addr);
void listener_free(listener *ls);


#endif

