
/* 标准输入输出的包裹函数 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "wrap.h"

ssize_t	writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}


void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		printf("writen error\n");
		exit(1);
	}
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char c, *ptr;
	ptr = vptr;
	
	for (n = 1; n < maxlen; n++)
	{
again:
		if ( (rc = read(fd, &c, 1)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return(n - 1);
		}
		else
		{
			if (errno == EINTR)
				goto again;
			return -1;
		}
	}
	
	*ptr = 0;
	return n;
}

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n;
	if ( (n = readline(fd, ptr, maxlen)) < 0)
	{
		printf("readline error\n");
		exit(1);
	}
	return n;
}

char *Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;
	
	/* 键入EOF,fgets返回空指针
	 * ferror在IO函数出错时返回非0值
	 */
	if ((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
	{
		printf("fgets error\n");
		exit(1);
	}
	
	return rptr;
}


void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
	{
		printf("fputs error\n");
		exit(1);
	}
}

