#! /bin/bash
spawn-fcgi -a 127.0.0.1 -p 8082 -f ./upload
spawn-fcgi -a 127.0.0.1 -p 8083 -f ./data
