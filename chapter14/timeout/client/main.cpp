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
#define MAXLINE 4096

void dg_cli(FILE* fp,int sockfd,const struct sockaddr* pservaddr,socklen_t servlen){
    int n;
    char sendline[MAXLINE],recvline[MAXLINE + 1];
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
    while(fgets(sendline,MAXLINE,fp) != NULL){
        sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen);
        n = recvfrom(sockfd,recvline,MAXLINE,0,NULL,NULL);
        if(n < 0){
            if(errno == EWOULDBLOCK){
                fprintf(stderr,"socket timeout!");
                continue;
            }else{
                printf("recvfrom error.");
            }
        }
        recvline[n] = 0;
        fputs(recvline,stdout);
    }
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
    dg_cli(stdin, sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    return 0;
}