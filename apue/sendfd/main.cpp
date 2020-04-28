#include<sys/socket.h>
#include<sys/param.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include <iostream>
/*
struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh);
struct cmsghdr *CMSG_NXTHDR(struct msghdr *msgh,struct cmsghdr *cmsg);
size_t CMSG_ALIGN(size_t length);
size_t CMSG_SPACE(size_t length);
size_t CMSG_LEN(size_t length);
unsigned char* CMSG_DATA(struct cmsghdr *cmsg);
struct cmsghdr{
		socklen_t cmsg_len;
		int cmsg_level;
		int cmsg_type;
};
*/
static const int CONTROL_LEN = CMSG_LEN(sizeof(int));
/*发送文件描述符，fd参数是用来传递信息的UNIX域的socket，fd_to_send参数是待发送的文件描述符*/
 int send_fd(int sock, int fd)  
{  
    struct iovec iov[1];  
    char c = 0;  
    iov[0].iov_base = &c;  
    iov[0].iov_len  = 1;
 
    int cmsgsize = CMSG_LEN(sizeof(int));  
    struct cmsghdr* cmptr = (struct cmsghdr*)malloc(cmsgsize);  
    if(cmptr == NULL){  
        return -1;  
    }  
    cmptr->cmsg_level = SOL_SOCKET;  
    cmptr->cmsg_type = SCM_RIGHTS; // we are sending fd.  
    cmptr->cmsg_len = cmsgsize;  
  
    struct msghdr msg;  
    msg.msg_iov = iov;  
    msg.msg_iovlen = 1;  
    msg.msg_name = NULL;  
    msg.msg_namelen = 0;  
    msg.msg_control = cmptr;  
    msg.msg_controllen = cmsgsize;  
    *(int *)CMSG_DATA(cmptr) = fd;  
      
    int ret = sendmsg(sock, &msg, 0);  
    free(cmptr);  
    if (ret == -1){  
		return -1;  
    }  
	return 0;
}  
  
int recv_fd(int sock)  
{ 
    int cmsgsize = CMSG_LEN(sizeof(int));  
    struct cmsghdr* cmptr = (struct cmsghdr*)malloc(cmsgsize);  
	if (cmptr == NULL) {
		return -1;
	}
    char buf[32]; // the max buf in msg. 
    struct iovec iov[1];  
    iov[0].iov_base = buf;  
    iov[0].iov_len = sizeof(buf);  
    struct msghdr msg;  
    msg.msg_iov = iov;  
    msg.msg_iovlen  = 1;  
    msg.msg_name = NULL;  
    msg.msg_namelen = 0;  
    msg.msg_control = cmptr;  
    msg.msg_controllen = cmsgsize;  
      
    int ret = recvmsg(sock, &msg, 0);
    free(cmptr);  
    if (ret == -1) {  
        return -1; 
    }  
	int fd = *(int *)CMSG_DATA(cmptr);  
    return fd;  
}

int main()
{
    int pipefd[2];
    int fd_to_pass = 0;//文件描述符的传递值
    /*创建父，子进程间的管道，文件描述符 pipefd[0]和pipefd[1]*/

    int ret = socketpair( PF_UNIX,SOCK_DGRAM,0,pipefd);//管道函数
    assert(ret != -1);

    pid_t pid = fork();
    assert(pid >= 0);

    if(pid == 0)//子进程
    {
        close(pipefd[0]);//?????
        fd_to_pass = open("text.txt",O_RDWR,0666);
        //std::cout<<"fd_to_pass"<< fd_to_pass<<std::endl;
        /*子进程通过管道将文件描述符发送到父进程。如果文件text打开失败则子进程将标准输入文件描述符发送到父进程*/
        send_fd( pipefd[1],(fd_to_pass > 0)?fd_to_pass:0);
        close(fd_to_pass);
        exit(0);
    }

    close(pipefd[1]);
    fd_to_pass = recv_fd(pipefd[0]);//父进程从管道接收目标文件描述符
    char buff[128];
    memset(buff,0,sizeof(buff));
    read(fd_to_pass,buff,127);//读目标文件描述符，以验证其有效性
    printf("I Got fd: %d and Data: %s\n",fd_to_pass,buff);
    close(fd_to_pass);
}
 