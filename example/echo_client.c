#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "wrap.h"

#define MAXLINE 4096

void str_cli(FILE *fp, int sockfd)
{
	char buffer[MAXLINE];
	
	while (Fgets(buffer, MAXLINE, fp) != NULL)
	{
		Writen(sockfd, buffer, strlen(buffer));

		/*  ’µΩFIN,∑µªÿ0 */
		if (Readline(sockfd, buffer, MAXLINE) == 0)
		{
			printf("server terminated prematurely\n");
			exit(1);
		}

		Fputs(buffer, stdout);
	}
}

int main(int argc, char **argv)
{
	int	sockfd;
	struct sockaddr_in servaddr;
	
	if (argc != 2)
	{
		printf("usage: client <IP>\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(7);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	str_cli(stdin, sockfd);

	return 0;
}

