#!/bin/bash

trap 'echo "before execute line:$LINENO, a=$a,b=$b,c=$c"' DEBUG
a=1
if [ "$a" -eq 1 ]
then
  b=2
else
  b=1
fi
c=3
echo "end"
