#!/bin/bash
rm responses.log 2> /dev/null
taskset -c 0 ./attack "$1"



