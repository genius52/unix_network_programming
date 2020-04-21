#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#define MAXLINE 4096

int connect_nonb(int sockfd,const struct sockaddr* saptr,socklen_t salen,int nsec){
    int flags,n,error;
    socklen_t len;
    fd_set rset,wset;
    struct timeval tval;
    flags = fcntl(sockfd,F_GETFL,0);
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);

    error = 0;
    if((n = connect(sockfd,saptr,salen)) < 0){
        if (errno == EINPROGRESS){
            return - 1;
        }
    }

    if (n == 0)
        goto done;
    FD_ZERO(&rset);
    wset = rset;
    tval.tv_sec = nsec;
    tval.tv_usec = 0;
    if(( n = select(sockfd + 1,&rset,&wset,NULL,nsec? &tval:NULL)) == 0){
        close(sockfd);
        error = ETIMEDOUT;
        return -1;
    }
    if(FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd,&wset)){
        len = sizeof(error);
        if(getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len) < 0){
            return -1;
        }
    }else{
        exit(0);
    }
done:
    fcntl(sockfd,F_SETFL,flags);
    if(error){
        close(sockfd);
        errno = error;
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    connect_nonb(sockfd,&servaddr);
    return 0;
}