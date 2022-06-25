#!/bin/bash

docker build -t my-apache2 .
docker run -dit --name my-running-app -p 5000:80 my-apache2