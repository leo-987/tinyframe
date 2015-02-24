#ifndef __INETADDR_H__
#define __INETADDR_H__

#include <netinet/in.h>

/* 封装的IPv4地址和port */
typedef struct inet_addr_t {
	struct sockaddr_in addr;
}inet_address;

inet_address addr_create(const char *ip, int port);

#endif

