#! /bin/sh
for i in /usr/lib/lib*; do if [ "`nm $i 2>/dev/null|grep timer_create`" ]; then echo $i;fi;done
