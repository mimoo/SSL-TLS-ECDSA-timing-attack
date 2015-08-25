#!/bin/bash
rm responses.log 2> /dev/null
sudo nice -n20 ./openssl "$1" & # CPU priority (20)
sudo schedtool -a 1 $! # CPU affinity (on cpu 1)



