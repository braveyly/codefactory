#! /bin/bash

free -m | sed -n 's/Swap: *\([0-9]*\) *\([0-9]*\) *\([0-9]*\)/\1 \2/p'

free -m | sed -n '/Swap/p' | awk '{print $2}'

