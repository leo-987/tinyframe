#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "listener.h"
#include "inetaddr.h"
#include "event.h"
#include "debug.h"

/* 新连接事件发生时的回调函数
 * @listenfd : 监听套接字描述符
 * @arg : 接收新连接的listener对象
 */
static void event_accept_callback(int listenfd, void *arg)
{
	listener *ls = (listener *)arg;
	socklen_t clilen = sizeof(ls->client_addr.addr);

	int connfd = accept4(listenfd, (struct sockaddr *)&ls->client_addr.addr,
						&clilen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0)
	{
		/* 出错处理,参考muduo和<<UNP>>16.6 */
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		int save = errno;
	    switch (save)
	    {
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EPERM:
			case EMFILE:
				return;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				debug_sys("file: %s, line: %d", __FILE__, __LINE__);
			default:
				debug_sys("file: %s, line: %d", __FILE__, __LINE__);
	    }
	}

	/* 打印客户端地址信息 */
	char buff[50];
	debug_msg("connection from %s, port %d",
			inet_ntop(AF_INET, &ls->client_addr.addr.sin_addr, buff, sizeof(buff)),
			ntohs(ls->client_addr.addr.sin_port));

	ls->accept_callback(connfd, ls);	/* accept_callback() */
}

/* 创建一个listener监听对象
 * @server : 新生成listener所属的server
 * @callback : 新连接到来时调用的回调函数
 * @ls_addr : 监听的TCP地址
 */
listener *listener_create(server *server, accept_callback_pt accept_cb, inet_address ls_addr)
{
	listener *ls = malloc(sizeof(listener));
	if (ls == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	ls->server = server;
	ls->accept_callback = accept_cb;
	ls->listen_addr = ls_addr;
	
	/* 创建非阻塞套接字 */
	int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (listen_fd < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(ls);
		return NULL;
	}

	int ret = bind(listen_fd, (struct sockaddr *)&ls_addr.addr, sizeof(ls_addr.addr));
	if (ret < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(listen_fd);
		free(ls);
		return NULL;
	}

	/* SOMAXCONN : /proc/sys/net/core/somaxconn */
	ret = listen(listen_fd, SOMAXCONN);
	if (ret < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(listen_fd);
		free(ls);
		return NULL;
	}

	ls->ls_event = event_create(server->manager, listen_fd, EPOLLIN | EPOLLPRI,
									event_accept_callback, ls, NULL, NULL);
	if (ls->ls_event == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(listen_fd);
		free(ls);
		return NULL;
	}
	
	event_start(ls->ls_event);

	return ls;
}

void listener_free(listener *ls)
{
	event_free(ls->ls_event);
	free(ls);
}

