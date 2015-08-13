#! /bin/bash

file="abc"
if [ -e "$file" ]
then
echo "$file is existed"
else
echo "$file is not existed"
fi

#if [ -N "$file" ]
#then 
#echo "$file is changed"
#else
#echo "$file is not changed"
#fi

file1="456"
file2="123"

if [ "$file1" -nt "$file2" ]
then
echo "$file1 is newer than $file2"
else
echo "$file1 is older than $file2"
fi
# -f
# check whether it is a normal file

# -s
# check whether the file size is zero

# -d
# check whether it is the dir file

# -b
# check whether it is the block type file

# -c
# check whether it is the char type file

# -p
# check whether it is the pipe type file

# -L
# check whether it is the symbol link file

# -S 
# check whether it is the socket type file

# -r/-w/-x
# check whether it is readable/writable/excuteable

# -O
# check whether you are the owner of the file

# -N
# check whether the file is changed since it is readed last time

# f1 -nt f2
# file 1 is newer than file 2

# f1 -ot f2
# file 1 is older than file 2

