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

    std::string s = "SOCKET PAIR TEST.";
    int qid = msgget((key_t)QUEUE_NO,0666);
    while(1){
        int ret = msgsnd(qid,s.c_str(),s.length(),IPC_NOWAIT);
        if (ret != 0){
            std::cout <<"send msg fail error no:" <<errno<<std::endl;
        }else{
            std::cout <<"send msg :" <<s<<std::endl;
        }
        sleep(5);
    }
    return EXIT_SUCCESS;    
} 