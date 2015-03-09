#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/tcp.h>

#define MAXLINE 16384

char send_buf[MAXLINE];
char recv_buf[MAXLINE];

int exiting = 0;
unsigned long long total_bytes_read = 0;
unsigned long long total_messages_read = 0;

void sigalrm_handler(int sig)
{
	exiting = 1;
}

void str_cli(FILE *fp, int sockfd)
{
	int n;
	
	while (!exiting)
	{
		n = write(sockfd, send_buf, MAXLINE);

		n = read(sockfd, recv_buf, MAXLINE);
		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			
			exit(1);
		}

		++total_messages_read;
		total_bytes_read += n;
	}

	shutdown(sockfd, SHUT_WR);

	while ((n = read(sockfd, recv_buf, MAXLINE)) != 0)
		;
}

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	int timeout_sec = 10;
	int i, n;

	for (i = 0; i < MAXLINE; i++)
		send_buf[i] = i % 128;
	
	if (argc != 2)
	{
		printf("usage: ./pingpong_client <IP> <Time> <Concurrency Level>\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(2015);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	signal(SIGALRM, sigalrm_handler);
    alarm(timeout_sec);
	
	str_cli(stdin, sockfd);

	printf("%llu total bytes read\n", total_bytes_read);
	printf("%llu total messages read\n", total_messages_read);
	printf("%.3f average messages size\n", (double)total_bytes_read / total_messages_read);
	printf("%.3f MiB/s throughtput\n", (double)total_bytes_read / (timeout_sec * 1024 * 1024));
	
	return 0;
}

