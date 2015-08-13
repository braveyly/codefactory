#! /bin/bash

for item in "a" "b" "c" "d" "e" "f"
do
echo "$item"
done

#######for result
#a
#b
#c
#d
#e
#f

a="abc def ghi jkl mno pqr"
for argu in `echo $a`
do
echo "$argu"
done
######for result
#abc
#def
#ghi
#jkl
#mno
#pqr

b=10
c=20
while [ "$b" -lt "$c" ]
do
echo "b=$b"
let "b=$b+1"
done
#######while result
#b=10
#b=11
#b=12
#b=13
#b=14
#b=15
#b=16
#b=17
#b=18
#b=19

b=10
c=20
while [ "$b" -lt "$c" ]
do
   let "b=$b+1"
   if [ "$b" -eq "13" ]
   then
   continue
   fi
   echo "b=$b"
done
#######while result
#b=11
#b=12
#b=14
#b=15
#b=16
#b=17
#b=18
#b=19
#b=20

while [ "$var" != "end" ]
do
echo "While input variable"
read var
done

until [ "$var" != "end" ]
do
echo "Until input variable"
read var
done


echo "Hit a key, then hit return"
read var
case "$var" in
"1" ) echo "var is 1" ;;
"2" ) echo "var is 2" ;;
*   ) echo "var is other" ;;
esac


select var in "123" "456" "789"
do
echo "ok is $var"
break
done

