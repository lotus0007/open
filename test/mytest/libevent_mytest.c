#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <event2/event.h>

int main(int argc, char *argv[])
{
    struct event_base *base = NULL;

    //创建一个base
    base = event_base_new();
    //
    //
    //
    //        //给base添加一些IO事件
    //
    //
    //            //打印出base支持哪些iO复用机制
    int i;
    const char **methods = event_get_supported_methods();

    printf("Starting Libevent %s.  Available methods are:\n",
            event_get_version());
    for (i=0; methods[i] != NULL; ++i) {
        printf("    %s\n", methods[i]);
    }


    //循环
    event_base_dispatch(base);




    //释放这个base
    event_base_free(base);


    return 0;
}

