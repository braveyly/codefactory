#! /bin/sh
usage='The usage:./myfmt.sh *.c'

if [ "$#" -lt 1 ]
then
echo "The parameter must include one source file at least"
echo $usage
exit
fi

config_path='./etc/defaults.cfg'
cmd='./uncrustify -c '$config_path' --no-backup'
echo $cmd
$cmd $1
