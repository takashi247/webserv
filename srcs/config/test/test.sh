#!/bin/bash

PROGRAM=a.out 

## STORE 
make store

./$PROGRAM store.conf

## ERROR
make error

./$PROGRAM
