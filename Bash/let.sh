#! /bin/bash

a=`expr 2 + 3`
echo "a = $a"

b=5
c=`expr $b - 4`
echo "c = $c"


e=9
f=4
g=`expr $e \* $f`
echo "g = $g"

h=10
i=3
j=`expr $h / $i`
echo "j = $j"

k=13
l=5
m=`expr $k % $l`
echo "m = $m"
############expr
#a = 5
#c = 1
#g = 36
#j = 3
#m = 3
###########Key Point
#1.blank symbols at the left and right sides of the operation symbol
#2.escape symbol for times operation
let "n+=10"
echo "n=$n"

let "o-=$n"
echo "o=$o"

let "p=$n*3"
echo "p=$p"

let "q=$n/2"
echo "q=$q"

let "r=10%3"
echo "r=$r"
#############let
#n=10
#o=-10
#p=30
#q=5
#r=1
#############Key Point
#1. let is easier than expr

