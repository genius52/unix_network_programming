#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#define MAXLINE 1024
 
static int	read_cnt;
static char	*read_ptr;
static char	read_buf[MAXLINE];
 
void str_cli
(FILE *fp,int acceptFd);
void Writen(int fd, void *ptr, size_t nbytes);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t Readline(int fd, void *ptr, size_t maxlen);
ssize_t readline(int fd, void *vptr, size_t maxlen);
 
 
int main(int argc,char *argv[])
{
    if(argc!=3){
        perror("please enter [IP] [PORT]");
       exit(EXIT_FAILURE);
    }
    int sockFd=socket(AF_INET,SOCK_STREAM,0);
    if(sockFd<0){
        perror("socket");
        exit(EXIT_FAILURE);
    }
 
    struct sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(atoi(argv[2]));
    if((inet_aton(argv[1],(struct in_addr*)&serverAddr.sin_addr))==0){
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }
    
    if(connect(sockFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        perror("connect");
        exit(EXIT_FAILURE);
    }
    
    printf("connect\n");
    str_cli(stdin,sockFd);
    
    exit(0);
}
 
void str_cli(FILE *fp,int sockFd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    while(fgets(sendline,MAXLINE,fp)!=NULL)
    {
        Writen(sockFd,sendline,strlen(sendline));
        if(Readline(sockFd,recvline,MAXLINE)==0)
            perror("read");
        fputs(recvline,stdout);
    }
}
 
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;
 
	ptr = reinterpret_cast<const char*>(vptr);
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = writen(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return(-1);
		}
 
		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
 
void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		perror("writen error");
}
 
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;
 
	ptr = reinterpret_cast<char*>(vptr);
	for (n = 1; n < maxlen; n++) {
again:
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else {
			if (errno == EINTR)
				goto again;
			return(-1);		/* error, errno set by read() */
		}
	}
 
	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}
 
ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;
 
	if ( (n = readline(fd, ptr, maxlen)) < 0)
		perror("readline error");
	return(n);
}
 