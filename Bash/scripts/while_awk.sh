#!/bin/bash

i=0
while read LINE
do
echo $LINE
echo $i
let i=$i+1
echo "print the 1st column of the 1st row `echo $LINE | awk '{print $1}'`"
done<5.txt

