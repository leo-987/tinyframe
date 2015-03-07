#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "wrap.h"

#define MAXLINE 4096

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	
	if (argc != 2)
	{
		printf("usage: client <IP>\n");
		return -1;
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	while ( (n = read(sockfd, recvline, MAXLINE)) > 0)
	{
		recvline[n] = 0;
		Fputs(recvline, stdout);
	}
	
	return 0;
}


