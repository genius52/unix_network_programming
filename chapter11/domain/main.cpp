#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

int main(){
    struct servent* sptr;
    sptr = getservbyname("ftp","tcp");
    sptr = getservbyport(htons(443),"udp");
    printf(sptr->s_name);

    char name[155]; 
    char *ip; 
    hostent* hostinfo; 
    if( gethostname ( name, sizeof(name)) == 0) 
    { 
        if((hostinfo = gethostbyname(name)) != NULL) 
        { 
            ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
            printf("当前服务器IP:%s\n", ip); 
        } 
    } 
}
    
    