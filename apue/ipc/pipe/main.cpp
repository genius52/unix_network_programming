#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <error.h> 
#include <errno.h> 
#include <stdlib.h>
#include <sys/msg.h>
#include <string>
#include <iostream>

int main(int argc, char* argv[]){
    int fd[2],n;
    char c;
    pid_t childpid;
    pipe(fd);
    if((childpid = fork()) == 0){
        sleep(3);
        if (( n = read(fd[0],&c,1)) != 1){
            std::cout<<"receive error "<<n<<std::endl;
        }
        std::cout<<"child read "<<c<<std::endl;
        exit(0);
    }
    write(fd[1],"p",1);
    if(( n = read(fd[1],&c,1)) != 1){
        std::cout<<"parent: read return "<<n<<std::endl;
    }
    std::cout<<"parent read "<< c <<std::endl;
    return EXIT_SUCCESS;    
} 