#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <wait.h>
#include <algorithm>
#include <errno.h>
#include <syslog.h>
#define SERV_PORT 9527
#define MAX_LINE 2048

void sig_chld(int signo)
{
    pid_t   pid;
    int     stat;

    pid = wait(&stat);
    printf("child %d terminated\n", pid);
    return;
}


void str_echo(int acceptFd)
{
    ssize_t n;
    char buf[MAX_LINE];
again:
    while((n = read(acceptFd,buf,MAX_LINE))>0)
        write(acceptFd,buf,n);
    if(n<0 & errno==EINTR)
        goto again;
    else if(n<0)
        perror("str_echo:read error");
}

int main(int argc,char **argv){
    syslog(LOG_ERR,"test syslog");
    struct servent* sptr;
    int listenfd,connfd,udpfd,nready,maxfdp1;

    char mesg[MAX_LINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr,servaddr;
    void sig_chld(int);
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    bind(listenfd,(__CONST_SOCKADDR_ARG)&servaddr,sizeof(servaddr));
    listen(listenfd,20);

    udpfd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(udpfd,(__CONST_SOCKADDR_ARG)&servaddr,sizeof(servaddr));

    signal(SIGCHLD,sig_chld);

    FD_ZERO(&rset);
    maxfdp1 = std::max(listenfd,udpfd) + 1;
    for(;;){
        FD_SET(listenfd,&rset);
        FD_SET(udpfd,&rset);
        if((nready = select(maxfdp1,&rset,NULL,NULL,NULL)) < 0){
            if (errno == EINTR)
                continue;
            else
                printf("select error");
        }
        if(FD_ISSET(listenfd,&rset)){
            len = sizeof(cliaddr);
            connfd = accept(listenfd,(__SOCKADDR_ARG)&cliaddr,&len);
            if((childpid = fork()) == 0){
                close(listenfd);
                str_echo(connfd);
                exit(0);
            }
            close(connfd);
        }
        if(FD_ISSET(udpfd,&rset)){
            len = sizeof(cliaddr);
            n = recvfrom(udpfd,mesg,MAX_LINE,0,(__SOCKADDR_ARG)&cliaddr,&len);
            sendto(udpfd,mesg,n,0,(__SOCKADDR_ARG)&cliaddr,len);
        }
    }
}