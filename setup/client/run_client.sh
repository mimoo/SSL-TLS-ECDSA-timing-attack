#!/bin/bash
rm responses.log 2> /dev/null
sudo nice -n20 ./openssl "$1" & # CPU priority
sudo schedtool -a 0,1 $! # CPU affinity



