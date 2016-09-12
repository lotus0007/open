CC=gcc
CFLAGES=-Wall
WORKDIR=.
INCLUDEDIR=-I$(WORKDIR)/inc/ -I/usr/local/include/hiredis/ -I/home/itcast/fcgi-2.4.1-SNAP-0910052249/include -I/home/itcast/libevent-2.0.22-stable/include/event2
src=$(wildcard *.c)
obj=$(patsubst %.c,%.o,$(src))
LIBS=-lpthread -lhiredis -lfcgi -lm -levent

#REDIS_TEST=test_redis
FCGI_DEMO=demo
LIBEVENT_DEMO=libevent_mytest
FCGI_UPLOAD=upload
FCGI_DATA=data
#REDIS_MYTEST=redis_mytest
MY_UPLOAD_FILE=my_upload_file

target=$(REDIS_MYTEST) $(MY_UPLOAD_FILE) $(FCGI_DEMO) $(FCGI_UPLOAD) $(FCGI_DATA) $(LIBEVENT_DEMO)

ALL:$(target) 

$(obj):%.o:%.c
	$(CC) -c $< $(CFLAGES) $(INCLUDEDIR) -o $@

$(MY_UPLOAD_FILE):my_upload_file.o make_log.o
	$(CC) $^ -o $@ $(LIBS) 
#$(test_redis):test_redis.o make_log.o
#	$(CC) $^ -o $@ $(LIBS)
#
#$(REDIS_MYTEST):redis_mytest.o make_log.o redis_op.o
#	$(CC) $^ -o $@ $(LIBS)
$(FCGI_DEMO):fcgi_demo.o
	$(CC) $^ -o $@ $(LIBS)
$(FCGI_UPLOAD):upload.o make_log.o redis_op.o util_cgi.o
	$(CC) $^ -o $@ $(LIBS)
$(FCGI_DATA):data.o make_log.o redis_op.o util_cgi.o cJSON.o
	$(CC) $^ -o $@ $(LIBS)
$(LIBEVENT_DEMO):libevent_mytest.o
	$(CC) $^ -o $@ $(LIBS)

.PHONY:clean
clean:
	rm -rf $(obj) $(target)

