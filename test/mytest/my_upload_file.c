#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include"make_log.h"

int main(void)
{
    pid_t pid;
    int pipefd[2];
    int ret;
    char buf[1024]={0};


    ret = pipe(pipefd);
    if(ret != 0)
    {
        printf("pipe error %d\n",ret);
        return -1;
    }
    pid = fork();
    if(pid == 0)//child
    {
        close(pipefd[0]);
        dup2(pipefd[1],STDOUT_FILENO);
        ret = execlp("../fdfs_upload_file","../fdfs_upload_file","/etc/fdfs/client.conf","make_log.c");
        if(ret == -1)
        {
            printf("execlp error\n");
            return 0;
        }
    }else
    {
        close(pipefd[1]);
        read(pipefd[0],buf,1024);
        LOG("test1","test_main","the upload file is %s",buf);
    }
    return 0;
}
