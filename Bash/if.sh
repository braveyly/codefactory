#! /bin/bash
echo "test you"
a=12
b=13

# number compare
# -eq: equal
# -ne: not equal
# -gt: great
# -ge: great and equal
# -lt: little
# -le: little and equal
if [ "$a" -ne "$b" ]
then
echo "$a is not equel to $b"
fi

if [ "$a" -eq "$b" ]
then 
echo "$a is equal to $b"
else
echo "$a is not equal to $b"
fi

if [ "$a" -eq "$b" ]
then
echo "$a is equal to $b"
elif [ "$a" -lt "$b" ]
then
echo "$a is little to $b"
elif [ "$a" -gt "$b" ]
then
echo "$a is great to $b"
else
echo "Default"
fi
#########Key Point##############
#########1."then" after "if" or "elif"############
#########2."elif" is ok, not "else if"############

c=abc
d=abd
e=
#string compare

if [ "$c" == "$d" ]
then
echo "string $c is equal to $d"
elif [ "$c" \> "$d" ]
then
echo "string $c is great to $d"
elif [ "$c" \< "$d" ]
then
echo "string $c is little to $d"
elif [ "$c" != "$d" ]
then
echo "string $c is not equal to $d"
fi

if [ -z "$e" ]
then
echo " $e is NULL"
else
echo " $e is not NULL"
fi

if [ -n "$e" ]
then 
echo "$e is not NULL"
else
echo "$e is NULL"
fi
###########KEy point#################
###########1.For string comparing, ">" "<"need escape symbol
###########2."z": zero is used to test whether the string is NULL
###########3."n": not zero is used to test whether the string is not NULL


f=8
if [ "$f" -gt 0 ]
then 
echo "f is positive"
   if [ "$f" -gt 5 ]
   then
   echo "f is a big positive"
   else
   echo "f is a small positive"
   fi
else
echo "f is negative"
   if [ "$f" -gt -5 ]
   then
   echo "f is a big negative"
   else
   echo "f is a small negative"
   fi
fi

