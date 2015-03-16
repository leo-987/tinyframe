#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include "listener.h"
#include "inetaddr.h"
#include "event.h"
#include "debug.h"
#include "config.h"

static void event_accept_callback(int listenfd, event *ev, void *arg)
{
	listener *ls = (listener *)arg;
	inet_address client_addr;
	socklen_t clilen = sizeof(client_addr.addr);
	int ret;
	int connfd;
	static int i;
	
	connfd = accept(listenfd, (struct sockaddr *)&client_addr.addr,	&clilen);
	if (connfd < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		
		int save = errno;
		if (save == EAGAIN || save == ECONNABORTED || save == EINTR
			|| save == EPROTO || save == EPERM || save == EMFILE)
		{
			return;
		}
		else
		{
			debug_sys("file: %s, line: %d", __FILE__, __LINE__);
		}
	}

	/* 打印客户端地址信息 */
	//char buff[50];
	//debug_msg("connection from %s, port %d",
	//		inet_ntop(AF_INET, &client_addr.addr.sin_addr, buff, sizeof(buff)),
	//		ntohs(client_addr.addr.sin_port));

	fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL) | O_NONBLOCK);

	/* epoll是线程安全的 */
	if (i == MAX_LOOP)
		i = 0;
	
	connection *conn = connection_create(loops[i], connfd, ls->readable_callback);
	if (conn == NULL)
	{
		debug_quit("file: %s, line: %d", __FILE__, __LINE__);
	}

	i++;
	
	/* 用户回调函数 */
	if (ls->new_connection_callback)
		ls->new_connection_callback(conn);

}

listener *listener_create(server_manager *manager, inet_address ls_addr,
						  connection_callback_pt read_cb, connection_callback_pt new_conn_cb)
{
	int ret;
	listener *ls;
	
	ls = malloc(sizeof(listener));
	if (ls == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	ls->listen_addr = ls_addr;
	ls->readable_callback = read_cb;
	ls->new_connection_callback = new_conn_cb;

	/* 创建非阻塞套接字 */
	int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (listen_fd < 0)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		free(ls);
		return NULL;
	}

	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	ret = bind(listen_fd, (struct sockaddr *)&ls_addr.addr, sizeof(ls_addr.addr));
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

	ls->ls_event = event_create(listen_fd, EPOLLIN | EPOLLPRI,
								event_accept_callback, ls, NULL, NULL);
	if (ls->ls_event == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		close(listen_fd);
		free(ls);
		return NULL;
	}
	
	accept_event_add(manager, ls->ls_event);
	event_start(ls->ls_event);

	return ls;
}

void listener_free(listener *ls)
{
	if (ls != NULL)
	{
		event_free(ls->ls_event);
		free(ls);
	}
}

