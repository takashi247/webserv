#!/bin/bash

RED="\033[31m"
GREEN="\033[32m"
RESET="\033[m"

# build webserver
cd .. && ./webserv test/confs/test.conf > /dev/null &

printf "${GREEN}[autoindex]\n"
siege -r 100 http://localhost:8080/autoindex/

printf "${GREEN}[CGI]\n"
siege -r 100 http://localhost:8082/

# end webserver
killall webserv
