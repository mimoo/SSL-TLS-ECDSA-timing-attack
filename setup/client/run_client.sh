#!/bin/bash
rm responses.log 2> /dev/null
sudo taskset -c 1 nice -n20 ./openssl "$1" # CPU priority (20) and affinity (1)




