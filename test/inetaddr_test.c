#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "inetaddr.h"

void inet_addr_test()
{
	char buff[20];
	inet_address addr1 = addr_create("192.168.1.123", 2015);
	
	assert(ntohs(addr1.addr.sin_port) == 2015);
	assert(strcmp(inet_ntop(AF_INET, &addr1.addr.sin_addr, buff, sizeof(buff)), "192.168.1.123") == 0);

	inet_address addr2 = addr_create("any", 2016);
	assert(ntohs(addr2.addr.sin_port) == 2016);
	assert(ntohl(addr2.addr.sin_addr.s_addr) == INADDR_ANY);
}

int main()
{
	inet_addr_test();
	printf("inet addr test OK!\n");
	return 0;
}

