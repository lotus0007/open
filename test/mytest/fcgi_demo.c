#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fcgi_stdio.h"




int main(int argc, char *argv[])
{
    int count = 0;

    while (FCGI_Accept() >= 0) {

        printf("Content-type: text/html\r\n");
        printf("\r\n");
        printf("<title>Fast CGI Hello!</title>");
        printf("<h1>Fast CGI Hello!</h1>");
        printf("Request number %d running on host <i>%s</i>\n", ++count, getenv("SERVER_NAME"));

        printf("the remote_addr is : %s:%s\r\n",getenv("SERVER_ADDR"),getenv("SERVER_PORT"));
        printf("the request_method is :%s,the query_string is %s\r\n",getenv("REQUEST_METHOD"),getenv("QUERY_STRING"));
        printf("documen_root is %s \r\n",getenv("SCRIPT_FILENAME"));
    }

    return 0;
}
