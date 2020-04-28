#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <mutex>

int main(){
    // int semid = semget(6666,2,IPC_CREAT|0666);//创建了一个权限为666的信号量
    // printf("semid=%d\n",semid);

    int free_space = 3;
    sem_t sem_space;
    sem_init(&sem_space,0,3);
    int total_apple = 0;
    sem_t sem_apple;
    sem_init(&sem_apple,0,0);

    std::mutex m;
    std::thread consumer([&sem_apple,&sem_space,&total_apple,&free_space,&m](){
        while(true){
            sleep(3);
            sem_wait(&sem_apple);
            std::unique_lock<std::mutex> lock(m);
            total_apple--;
            free_space++;
            // int val1, val2;
            // sem_getvalue(&sem_apple, &val1);  
            // sem_getvalue(&sem_space, &val2);
            // std::cout<<"val1 = " << val1 << ". val2 = "<<val2<<std::endl;
            std::cout<< "Consumer eat one apple. apples = "<< total_apple << ". Free spaces = "<<free_space <<std::endl;
            sem_post(&sem_space);
        }
    });
    consumer.detach();

    std::thread producter([&sem_apple,&sem_space,&total_apple,&free_space,&m](){
        while(true){
            sleep(1);
            sem_wait(&sem_space);
            std::unique_lock<std::mutex> lock(m);
            total_apple++;
            free_space--;
            std::cout<< "Producer generate one apple. apples = "<< total_apple << ". Free spaces = "<<free_space <<std::endl;
            sem_post(&sem_apple);
        }
    });
    producter.detach();

    //int ret = semctl(semid,0,IPC_RMID);//delete semphore
    getchar();
    return 0;
}