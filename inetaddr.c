#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "inetaddr.h"
#include "debug.h"

/* 创建一个地址(IP + port)
 * 例如: create_addr("192.168.1.2", 1234) 	特定IP地址
 * 		 create_addr("any", 1234)			任意IP地址
 */
inet_address *create_addr(const char *ip, int port)
{
	inet_address *addr = malloc(sizeof(inet_address));
	if (addr == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	bzero(&addr->addr, sizeof(addr->addr));
	addr->addr.sin_family = AF_INET;
	
	if (strcmp(ip, "any") == 0)
		addr->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if(inet_pton(AF_INET, ip, &addr->addr.sin_addr) <= 0)
		debug_quit("file: %s, line: %d", __FILE__, __LINE__);
	
	addr->addr.sin_port = htons(port);
	return addr;
}


