#!/bin/bash

PROGRAM=a.out 
FILE="./confs/*"

## STORE 
make

for filepath in $FILE; do
  echo $filepath
  ./$PROGRAM $filepath
  echo ""
done
