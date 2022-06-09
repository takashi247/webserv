#!/bin/bash

sed "s/PORT/$1/g" conf/webserv-docker.conf.template > conf/webserv-docker.conf
docker build -t webserv:$1 .
docker container run -d -p $1:$1 webserv:$1
