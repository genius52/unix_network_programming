#include<stdio.h>
#include <stdlib.h>
#include <iostream>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<ctype.h>
#include <wait.h>

void sig_chld(int signo){
    pid_t pid;
    int stat;
    pid = wait(&stat);
    printf("child %d terminated\n",pid);
    return;
}

int main(){
    signal(SIGCHLD,sig_chld);
    std::cout<<"begin"<<std::endl;
    int ret = fork();
    if(ret == 0){
        std::cout<<"I am child"<<std::endl;
        exit(0);
    }else if(ret > 0){
        std::cout<<"I am parent"<<std::endl;
        sleep(6);
        std::cout<<"After sleep"<<std::endl;
    }else{
        std::cout<<"Error occured"<<std::endl;
    }

    return 0;
}