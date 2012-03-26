#!/bin/bash

ifconfig | grep 'inet addr' | grep -v '127.0.0.1' | cut -d : -f3 | awk '{print $1}'

ifconfig | grep 'inet addr' | grep -v '127.0.0.1' | cut -d : -f3 | cut -d ' ' -f1

ifconfig | tee 1.txt | grep 'inet addr' | tee 2.txt | grep -v '127.0.0.1' | tee 3.txt | cut -d : -f3 | tee 4.txt | awk '{print $1}'
