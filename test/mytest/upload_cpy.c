/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<hiredis.h>
#include<redis_op.h>
#include<redis_keys.h>
#include <sys/wait.h>

#include <time.h>
#include "util_cgi.h"


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"
#include "make_log.h"

#define CONTENT_BE "------"
#define FILE_NAME "filename"
#define REDIS_SER_IP "127.0.0.1"
#define REDIS_SER_PORT "6379"


static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}

int get_upload_context(char *data_read_buf,int len,char *file_name_buf)
{
    char *p = NULL,*head = NULL,*end =NULL,*temp = NULL;
    char head_buf[1024]={0};
    int fd;
    temp = head_buf;
    char content_buf[4096] = {0};
    int i = 0;
    //内容开始
    p = strstr(data_read_buf,CONTENT_BE);
    while((*p++) != '\r')
    {
        //存入内容头
        head_buf[i] = *p;
        i++;
    }
    head_buf[i-1] = '\0';
    //找到filename
    p = strstr(p,FILE_NAME);

    p = strstr(p,"=");
    p += 2;
    for(i = 0;*p != '\"';p++,i++)
    {
        file_name_buf[i] = *p; 
    }
    p = strstr(p,"Content-Type");
    p = strstr(p,"\r\n");
    p += 3;
    head = p;
    for(;;p++)
    {
        if(*temp == '\0')
        {
            break;
        }
        if(*p == *temp)
        {
            temp++;
            continue;
        }
    }
    end = p - strlen(head_buf) - 3;
    *end = '\0';
    memcpy(content_buf,head,(end - head)+1);
    fd = open(file_name_buf,O_CREAT|O_WRONLY|O_TRUNC,0644);
    write(fd,content_buf,sizeof(content_buf));
    close(fd);
    return 0;

}
int upload_fdfs(char * file_name_buf,char *fdfs_file_name)
{
    pid_t pid;
    int pipefd[2];
    int ret = 0;

    ret = pipe(pipefd);
    if(ret != 0)
    {
        LOG("mytest","mytest","pipe error ");
        return -1;
    }
    pid = fork();
    if(pid < 0)
    {
        LOG("mytest","mytest","fork error ");
        printf("fork error\n");
    }
    if(pid == 0)//child
    {
        close(pipefd[0]);
        dup2(pipefd[1],STDOUT_FILENO);
        ret = execlp("fdfs_upload_file","fdfs_upload_file","./conf/client.conf",file_name_buf,NULL);
        LOG("mytest","mytest","the upload file is %s",file_name_buf);
        if(ret == -1)
        {
            LOG("mytest","mytest","the upload file is errir");
            return 0;
        }
    }else
    {
        close(pipefd[1]);
        read(pipefd[0],fdfs_file_name,1024);
        //去掉结尾\n
        fdfs_file_name[strlen(fdfs_file_name) - 1] = '\0';
        LOG("mytest","mytest","the upload file is %s",fdfs_file_name);
        wait(NULL);
        //        ret = unlink(file_name_buf);
        //    if(ret != 0)
        //    {

        //  LOG("mytest","mytest","unlink %s\n error",file_name_buf);
        // return 0;
        //     }
    }
    return 0;
}

int upload_redis(char * fdfs_file_name,char * file_name_buf)
{
    char push_list[1024]={0};
    char file_id[256] = {0};
    char url[256] = {0};
    char filename[256] = {0};
    char create_time[256] = {0};
    char user[64] = {0};
    char type[64] = {0};
    int ret= 0;

    redisContext *conn = NULL;
    conn = rop_connectdb_nopwd(REDIS_SER_IP,REDIS_SER_PORT);
    if(conn != NULL)
    {
        LOG("mytest","mytest","redis connect succ");
    }
    //| Key:                    FILE_INFO_LIST                                                                                                                                              |
    //| Value:          file_id||url||filename||create  time||user||type        
    //file_id+ |
    strcpy(file_id,fdfs_file_name);
    file_id[strlen(fdfs_file_name)] = '|';
    //url + |
    strcpy(url,fdfs_file_name);
    url[strlen(fdfs_file_name)] = '|';
    //filename + |
    strcpy(filename,file_name_buf);
    filename[strlen(file_name_buf)] = '|';
    //create_time + |
    time_t now;
    now = time(NULL);
    struct tm *timeinfo;
    timeinfo = localtime(&now);
    strftime(create_time,64,"%a %b %d %I:%M:%S  %Y", timeinfo);
    create_time[strlen(create_time)] = '|';
    //user
    strcpy(user,"lotus");
    user[strlen("lotus")] = '|';
    //type
    get_file_suffix(file_name_buf, type);
    //拼接所有value
    strcat(push_list,file_id);
    strcat(push_list,url);
    strcat(push_list,filename);
    strcat(push_list,create_time);
    strcat(push_list,user);
    strcat(push_list,type);
    LOG("mytest","mytest","push_list is %s",push_list);

    ret = rop_list_push(conn, "FILE_INFO_LIST", push_list); 
    if(ret == 0)
    {
    LOG("mytest","mytest","push list succ");

    }
    rop_disconnect(conn);
    return 0;

}


int main ()
{
    char **initialEnv = environ;
    char * data_read_buf = NULL;
    char file_name_buf[256]={0};
    char fdfs_file_name[1024] = {0};
    int count = 0;
    int ret = 0;


    while (FCGI_Accept() >= 0) {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
                "\r\n"
                "<title>FastCGI echo</title>"
                "<h1>FastCGI echo</h1>\n"
                "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
            data_read_buf = (char *)malloc(sizeof(char)*len);
            memset(data_read_buf,0,sizeof(data_read_buf));
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            printf("No data from standard input.<p>\n");
        }
        else {
            int i, ch;
            int fd;

            printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                data_read_buf[i] = ch; 

                putchar(ch);
            }
            //提取文件内容，和文件名,写入本地文件
            get_upload_context(data_read_buf,len,file_name_buf);
            free(data_read_buf);
            //close(fd);
            printf("\n</pre><p>\n");
        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
        //将文件上传到fastDFS
        upload_fdfs(file_name_buf,fdfs_file_name);
        //将本地文件删除
        ret = unlink(file_name_buf);
        if(ret != 0)
        {

            LOG("mytest","mytest","unlink %s\n error",file_name_buf);
            return 0;
        }
        //将文件以一定格式上传到redis
        upload_redis(fdfs_file_name,file_name_buf);

    } /* while */

    return 0;
}
