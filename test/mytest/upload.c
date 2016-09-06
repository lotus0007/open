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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"


static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}

int main ()
{
    char **initialEnv = environ;
    char * data_read_buf = NULL;
    int count = 0;

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
   //         int str_head=0,str_end=0;
            char * p,*head,*end;
            FILE *fp;
            fp = fopen("helloworld.txt","w+");

	    printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
		}
                data_read_buf[i] = ch; 

                putchar(ch);
            }

            p = strtok(data_read_buf,"\n");
            p = strtok(NULL,"\n"); 
            p = strtok(NULL,"\n");
            p = strtok(NULL,"\n");
            p = strtok(NULL,"\n");
            head = p;
            p = strtok(NULL,"\n");
            end = p;
            for( p= head; p< end;p++)
            {
                fputc(*p,fp);
//                putchar(data_read_buf[i]);
//
            }

            /*
            for(i = 0; i< len ;i++)
            {
                if((data_read_buf[i] == '\r') && (data_read_buf[i+1] == '\n'))
                    break;
            }
            str_head = i +2;
            for(i = str_head;i < len;i++)
            {
                if((data_read_buf[i] == '\r') && (data_read_buf[i+1] == '\n'))
                    break;
            }
            str_end = i -1;
            for(i = str_head; i<= str_end;i++)
            {
                fputc(data_read_buf[i],fp);
//                putchar(data_read_buf[i]);
//
            }

            */
            fclose(fp);
            printf("\n</pre><p>\n");
        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
    } /* while */

    return 0;
}
