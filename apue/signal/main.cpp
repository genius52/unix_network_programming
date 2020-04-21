#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <error.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <sys/wait.h>
#define MAXLINE 2048

static void sig_int(int){
    printf("interrupt \n");
}

int main(){
    char buf[MAXLINE];
    pid_t pid;
    int status;
    if(signal(SIGINT,sig_int) == SIG_ERR)
        printf("signal error\n");
    while( fgets(buf,MAXLINE,stdin) != NULL){
        if(buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        if( (pid = fork()) < 0)
            printf("fork error\n");
        else if(pid == 0){
            execlp(buf,buf,NULL);
            exit(127);
        }
        if(( pid = waitpid(pid,&status,0)) < 0)
            printf("waitpid error\n");
        printf("%%");
    }
    exit(0);
    return 0;
}