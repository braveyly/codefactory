#! /bin/bash

stringZ=abcdefghijklmnop

######Get string length
echo "Output stringZ length:"
echo ${#stringZ}
##result
#Output stringZ length:
#16


#######Get substring position
echo "def is at the index:"
echo `expr index "$stringZ" "def"`

substr=mno
echo `expr index "$stringZ" "$substr"`
##result
#def is at the index:
#4
#13

#########Get substing
echo `expr ${stringZ:3}`
echo `expr ${stringZ:3:5}`
#echo `expr substr "$stringZ" 2 `
echo `expr substr "$stringZ" 2 3 `
###result
#defghijklmnop
#defgh
#bcd



##########Replace substring
stringZ=abcdefgabc
echo `expr ${stringZ/abc/ABC}`
echo `expr ${stringZ//abc/ABC}`
###result
#ABCdefgabc
#ABCdefgABC

