#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<hiredis.h>
#include<redis_op.h>
#include<redis_keys.h>

#define IP "127.0.0.1"
#define PORT "6379"

int rop_hset_append(redisCContext *conn,char *key,RFIELDS fields,RVALUES values,int val_num)
{
    int i;
    int ret = 0;
    redisReply * reply = NULL;
    for(i = 0; i < val_num; i++)
    {
       ret = redisAppendCommand(conn,"hset %s %s ",fields[i],values[i]); 
       if(ret == NULL)
       {
           printf("redisAppendCommand is error %d\n",i);
           ret = -1;
           goto END;
       }
       ret = 0;
    }

    for(i = 0; i < val_num; i++)
    {
        ret = redisGetReply(conn,(void **)&reply);
        if(ret != REDIS_OK)
        {
            printf("redisgetreply error %d\n",ret);
            freeReplyObject(reply);
            ret = -1;
            break;
        }
        freeReplyObject(reply);
        ret =0;
    }
END:
    return ret;
}

int main(void)
{
    redisContext * conn = NULL;
    conn = rop_connectdb_nopwd(IP,PORT);
    if(conn != NULL)
    {
        printf("redis conn succ !\n");
    }
    rop_hset_append()

    rop_disconnect(conn);
    return 0;
}
