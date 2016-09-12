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
#include "redis_keys.h"


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
#include "cJSON.h"

#define CONTENT_BE "------"
#define FILE_NAME "filename"
#define REDIS_SER_IP "127.0.0.1"
#define REDIS_SER_PORT "6379"


int get_query(int iformid,int icount)
{


    LOG("data","data_test","get_quert start");
    //从redis获取数据
    redisContext *conn = NULL;
    //redis 返回的结果集
    int i = 0;
    RVALUES values = NULL;
    values = malloc(1024*( icount));

    //结果集个数
    int get_num;
    conn = rop_connectdb_nopwd(REDIS_SER_IP,REDIS_SER_PORT);
    if(conn != NULL)
    {
        LOG("data","data_test","redis conn succ");
    }
    int ret =  rop_range_list(conn, "FILE_INFO_LIST", iformid, icount, values, &get_num);
        LOG("data","data_test","get_num = %d",get_num);
    //应id、kind、title_m、title_s、descrip、picurl_m、url、pv、hot
    char id[256]={0};
    char title_m[256]={0};
    char descrip[256]={0};
    char picurl_m[256] = {0};
    char url[256]={0};
    char user1[64]={0};
    char type[64]={0};
    char *p = NULL,*temp = NULL;
    int j = 0;

    //file_id||url||filename||create  time||user||type
    //把每组数据转换为JS，发送给前台
    //将字符串转换为JS格式
    char *out;
    cJSON *root;
    cJSON *array;
    //{} 
    root = cJSON_CreateObject();
    //[]
    array = cJSON_CreateArray();

    for(j = 0;j < get_num;j++)
    {
        //get file_id
        p = values[j];
        temp = strstr(p,"|");
        strncpy(id,p,temp - p);        
        temp++;
        p = temp;
        //get url
        temp = strstr(p,"|");
        strncpy(url,p,temp - p);        
        temp++;
        p = temp;
        //get title_m
        temp = strstr(p,"|");
        strncpy(title_m,p,temp - p);        
        temp++;
        p = temp;
        //get descrip 
        temp = strstr(p,"|");
        strncpy(descrip,p,temp - p);        
        temp++;
        p = temp;
        //get user
        temp = strstr(p,"|");
        strncpy(user1,p,temp - p);        
        temp++;
        p = temp;
        //get type
        strncpy(type,p,strlen(p));        
        //get picurl_m
        sprintf(picurl_m,"http://192.168.21.110/static/file_png/%s.png",type);
        //存入一组元素
        cJSON *game;
        cJSON_AddItemToArray(array,game = cJSON_CreateObject());
        cJSON_AddStringToObject(game,"id",id);
        cJSON_AddNumberToObject(game,"kind",2);
        cJSON_AddStringToObject(game,"title_m",title_m);
        cJSON_AddStringToObject(game,"title_s","NULL");
        cJSON_AddStringToObject(game,"descrip",descrip);
        cJSON_AddStringToObject(game,"picurl_m",picurl_m);
        cJSON_AddStringToObject(game,"url",url);
        cJSON_AddNumberToObject(game,"pv",1);
        cJSON_AddNumberToObject(game,"hot",1);
    }
    cJSON_AddItemToObject(root,"games",array);
    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n",out);
    free(out);
    rop_disconnect(conn);

    return 0;
}
int main ()
{
      char query_buf[1024]= {0};
        char cmd[64]={0};
        char formid[64]={0};
        int iformid = 0;
        char count[64]={0};
        int icount = 0;
        char user[64]={0};
        int ret = 0;

 

    while (FCGI_Accept() >= 0) {

        memset(query_buf,0,sizeof(query_buf));
        strcpy(query_buf,getenv("QUERY_STRING"));
        LOG("data","data_test","the query_string is %s",query_buf);
        memset(cmd,0,sizeof(cmd));
        memset(formid,0,sizeof(formid));
        memset(count,0,sizeof(count));
        memset(user,0,sizeof(user));
        //cmd,formid,count,user
        //得到cmd
        query_parse_key_value(query_buf, "cmd", cmd, NULL);
        query_parse_key_value(query_buf, "fromId", formid, NULL);
        query_parse_key_value(query_buf, "count", count, NULL);
        query_parse_key_value(query_buf, "user", user, NULL);

        iformid = atoi(formid);
        LOG("data","data_test","iformid is %d",iformid);
        icount  = atoi(count);
        LOG("data","data_test","icount is %d",icount);

        if(strncmp(cmd,"newFile",strlen("newFile")) == 0)
        {
 printf("Content-type: text/html\r\n"
                "\r\n");
            get_query(iformid,icount);
       
    LOG("data","data_test","get_quert end");
        }


    } /* while */

    return 0;
}
