#!/bin/bash


g++ -c test_$1.cpp
g++ ../objs/parser_utils.o \
	../objs/location_config.o \
	../objs/server_config.o \
	../objs/config.o \
	../objs/config_parser.o \
	test_$1.cpp -o a.out
