#!/bin/bash

docker build -t webserv .
docker container run -d -p 8080:8080 -p 8081:8081 webserv
