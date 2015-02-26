#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h> 
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define MAXLINE 65536

static int	read_cnt;
static char	*read_ptr;
static char	read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr)
{

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

ssize_t readlinebuf(void **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
	{
		printf("readline error\n");
		exit(0);
	}

	return(n);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		printf("Writen error\n");
		exit(0);
	}
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
	{
		printf("fputs error");
		exit(0);
	}
}

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	int ret;
	
	while (fgets(sendline, MAXLINE, fp) != NULL)
	{
		Writen(sockfd, sendline, strlen(sendline));

		/* ÊÕµ½FIN,·µ»Ø0 */
		ret = Readline(sockfd, recvline, MAXLINE);

		Fputs(recvline, stdout);
	}
}

int main(int argc, char **argv)
{
	int	sockfd;
	char recvline[MAXLINE + 1];
	int n;
	
	struct sockaddr_in servaddr;
	if (argc != 3)
	{
		printf("usage: client <IP> <port>\n");
		return -1;
	}

	int port = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	if (port == 10001)
	{
		while ( (n = read(sockfd, recvline, MAXLINE)) > 0)
		{
			recvline[n] = 0;
			fputs(recvline, stdout);
		}
	}
	else if (port == 10002)
	{
		str_cli(stdin, sockfd);
	}

	close(sockfd);
	
	return 0;
}

