#!bin/bash

sudo /usr/bin/fdfs_trackerd /etc/fdfs/tracker.conf

ps -aux | grep tracker
