#!/bin/bash

# compile
make -C ../

# mkdir for autoindex test
mkdir -p www/autoindex

# build webserver
cd .. && ./webserv test/confs/test.conf > /dev/null &

# test
if [ $? == 0 ]; then
	python3 request_test.py
	killall webserv
fi

rm -f www/delete_w
rm -f www/delete_x
rm -f www/delete_r
