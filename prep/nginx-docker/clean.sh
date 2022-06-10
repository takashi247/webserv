#!/bin/bash

docker kill $(docker ps -q)
docker rm $(docker ps -a -q)
docker rmi -f $(docker images -q)
rm conf/webserv-docker.conf
