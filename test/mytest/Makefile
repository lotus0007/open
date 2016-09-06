CC=gcc
CFLAGES=-Wall
WORKDIR=.
INCLUDEDIR=-I$(WORKDIR)/inc/ -I/usr/local/include/hiredis/ -I/home/itcast/fcgi-2.4.1-SNAP-0910052249/include
src=$(wildcard *.c)
obj=$(patsubst %.c,%.o,$(src))
LIBS=-lpthread -lhiredis -lfcgi

#REDIS_TEST=test_redis
FCGI_DEMO=demo
#REDIS_MYTEST=redis_mytest
MY_UPLOAD_FILE=my_upload_file

target=$(REDIS_MYTEST) $(MY_UPLOAD_FILE) $(FCGI_DEMO)

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

.PHONY:clean
clean:
	rm -rf $(obj) $(target)

