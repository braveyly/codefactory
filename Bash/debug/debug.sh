#!/bin/bash
DEBUG()
{
    if [ "$DEBUG" == "TRUE" ]
    then
        $@
    fi
}

a=1
DEBUG echo "a="$a
if [ "$a" -eq "1" ]
then
    b=1
else
    b=2
fi
DEBUG echo "b="$b
#(1) export DEBUG=TRUE
#(2) export PS4='+{$LINENO:${FUNCNAME[0]}} '
#    bash -x debug.sh
