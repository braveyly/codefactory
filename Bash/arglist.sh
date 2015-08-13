#! /bin/bash
#./arglist.sh abc def ghi jkl mno pqr stu

echo "There are $@"
echo "There are $*"
echo "There are $#"
echo "There are $1"
echo "There are ${10}"
##########result
#There are abc def ghi jkl mno pqr stu
#There are abc def ghi jkl mno pqr stu
#There are 7
#There are abc
#There are 
##########Key Point
##########1."$@" and "$*" represent all arguments
##########2."$#" indicates the number of arguments
##########3."$0" represents the name of the script
##########4."$1" represents the name of the first argument
##########5."${10}" represents the name of the ten argument

#########get script file name and dir path
echo "The script file name `basename $0`"
echo "The script file name"
echo `basename $0`

echo "The script file dir path `dirname $0`"
echo "The script file dir path"
echo `dirname $0`
#########result 
#The script file name arglist.sh
#The script file name
#arglist.sh
#The script file dir path .
#The script file dir path
#.

