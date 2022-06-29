#!/bin/bash

docker build -t webserv .
docker container run -d -p 8080:8080 -p 8081:8081 -p 8082:8082 webserv
