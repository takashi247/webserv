#!/bin/bash

PROGRAM=parse
FILE="./confs/*"

## STORE 
make

for filepath in $FILE; do
  echo $filepath
  # valgrind ./$PROGRAM $filepath
  ./$PROGRAM $filepath
  echo ""
done
