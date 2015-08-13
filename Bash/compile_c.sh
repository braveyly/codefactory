#!/bin/bash

for i in *.c
do
echo "$i"
echo `basename $i`
echo `basename $i .c`
gcc $i -o `basename $i .c`
done

