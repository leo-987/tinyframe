#include <netinet/in.h>  
#include <sys/socket.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <string.h>  
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>

 
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,  
                 struct sockaddr *sock, int socklen, void *arg);  
  
void socket_read_cb(struct bufferevent *bev, void *arg);  
void socket_event_cb(struct bufferevent *bev, short events, void *arg);  
  
int main()  
{  
    //evthread_use_pthreads();	//enable threads  
  
    struct sockaddr_in sin;  
    memset(&sin, 0, sizeof(struct sockaddr_in));  
    sin.sin_family = AF_INET;  
    sin.sin_port = htons(2016);  
  
    struct event_base *base = event_base_new();  
    struct evconnlistener *listener  
            = evconnlistener_new_bind(base, listener_cb, base,  
                                      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,  
                                      10, (struct sockaddr*)&sin,  
                                      sizeof(struct sockaddr_in));  
  
    event_base_dispatch(base);  
  
    evconnlistener_free(listener);  
    event_base_free(base);  
  
    return 0;  
}  
  
  
//一个新客户端连接上服务器了  
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的
//文件描述符为fd
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,  
                 struct sockaddr *sock, int socklen, void *arg)  
{  
    //printf("accept a client %d\n", fd); 
  
    struct event_base *base = (struct event_base*)arg;
  
    //为这个客户端分配一个bufferevent  
    struct bufferevent *bev =  bufferevent_socket_new(base, fd,  
                                               BEV_OPT_CLOSE_ON_FREE);  
  
    bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);  
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  
}

void socket_read_cb(struct bufferevent *bev, void *arg)  
{
  	bufferevent_flush(bev, EV_READ | EV_WRITE, BEV_FLUSH);
	
    static char buf[] = "HTTP/1.1 200 0K\r\nConnection:keep-alive\r\nContent-length:4\r\n\r\n"
						"test";
	
    bufferevent_write(bev, buf, strlen(buf));  
}


void socket_event_cb(struct bufferevent *bev, short events, void *arg)
{
    //if (events & BEV_EVENT_EOF)  
    //    printf("connection closed\n");  
    //else if (events & BEV_EVENT_ERROR)  
    //    printf("some other error\n");  

    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);
}

