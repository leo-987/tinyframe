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


/* 新连接到来时,该函数会被调用,
 * 这个函数先accept,再调用listener中的accept_cb回调函数
 */
static void event_accept_callback(int listenfd, void *arg)
{
	listener *ls = (listener *)arg;
	socklen_t clilen = sizeof(ls->client_addr.addr);
	
	//int connfd = accept(listenfd, (struct sockaddr *)&ls->client_addr.addr, &clilen);

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

	char buff[50];
	debug_msg("connection from %s, port %d",
			inet_ntop(AF_INET, &ls->client_addr.addr.sin_addr, buff, sizeof(buff)),
			ntohs(ls->client_addr.addr.sin_port));

	ls->accept_cb(connfd, ls);	/* accept_callback() */
}

/* 创建一个监听对象,监听地址由用户提供,从ls_addr参数传入 */
listener *create_listener(server *server, accept_cb_pt callback, inet_address *ls_addr)
{
	listener *ls = malloc(sizeof(listener));
	ls->server = server;
	ls->accept_cb = callback;
	ls->listen_addr = ls_addr;
	
	/* 创建非阻塞套接字 */
	ls->listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (ls->listen_fd < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(ls);
		return NULL;
	}

	int ret = bind(ls->listen_fd, (struct sockaddr *)&ls_addr->addr, sizeof(ls_addr->addr));
	if (ret < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(ls->listen_fd);
		free(ls);
		return NULL;
	}

	/* SOMAXCONN : /proc/sys/net/core/somaxconn */
	ret = listen(ls->listen_fd, SOMAXCONN);
	if (ret < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(ls->listen_fd);
		free(ls);
		return NULL;
	}

	ls->ls_event = create_event(server->manager, ls->listen_fd, EPOLLIN | EPOLLPRI,
									event_accept_callback, ls, NULL, NULL);
	if (ls->ls_event == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(ls->listen_fd);
		free(ls);
		return NULL;
	}
	
	event_add(ls->ls_event);

	/* 此时监听套接字处于监听状态,收到新连接调用event_accept_callback函数 */
	
	return ls;
}

void listener_free(listener *ls)
{
	close(ls->listen_fd);
	event_free(ls->ls_event);
	free(ls);
}

