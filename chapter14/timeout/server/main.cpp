#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
    int          n;
    socklen_t    len;
    char         mesg[4096];

    for(;;)
    {
        len = clilen;
        if((n = recvfrom(sockfd, mesg, 4096, 0, pcliaddr, &len)) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        if(sendto(sockfd, mesg, n, 0, pcliaddr, len) < 0)
        {
            perror("sendto");
            exit(1);
        }
    }    
}

int main(int argc, char **argv)
{
    int                   sockfd;
    struct sockaddr_in    servaddr, cliaddr;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9877);

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {    
        perror("bind");
        exit(1);
    }

    dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}