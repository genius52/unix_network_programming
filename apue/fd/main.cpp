#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <error.h> 
#include <errno.h> 
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#define MAXLINE 2048

int main(int argc, char *argv[])
{
	int fd1 = 0;
	int fd2 = 0;
	struct stat s;
	int ret = lstat("test.txt",&s);
	// 打开文件
	fd1 = open("test.txt", O_CREAT|O_WRONLY, 0666);
	ret = fstat(fd1,&s);
	if (fd1 < 0){
		perror("open");
		exit(-1);
	}
	printf("fd1 ============== %d\n", fd1);
	
	// 通过 fd1 复制出 fd2, 最终 fd1, fd2 都指向 test.txt
	fd2 = dup(fd1);
	printf("fd2 ============== %d\n", fd2);
	char *buf1 = "this is a test for fd1\n";
	// 操作 fd1 文件描述符
	write(fd1, buf1, strlen(buf1));
	// 关闭文件描述符，两个都得关
	close(fd1);

	char *buf2 = "this is a test for fd2\n";
	// 操作 fd2 文件描述符
	int size = write(fd2, buf2, strlen(buf2));
	close(fd2);
	
	return 0;
}