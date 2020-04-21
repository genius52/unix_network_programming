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
#define QUEUE_NO 1234

int main(int argc, char* argv[]){
    char buf[128] = {0};
    int qid = msgget((key_t)QUEUE_NO,0666 |IPC_CREAT);
    while(1){
        int ret = msgrcv(qid,buf,128,0,0);
        if (ret <= 0){
            std::cout <<"receive msg fail error no:" <<errno<<std::endl;
        }else{
            std::cout<< "receive msg: " << buf<<std::endl;
        }
         sleep(5);
    }
    msgctl(qid,IPC_RMID,0);
    return EXIT_SUCCESS;    
} 