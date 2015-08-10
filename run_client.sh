#/bin/bash!
rm output.csv
rm responses/*
rm responses.tar.gz
gcc attack.c -o attack && taskset -c 0 ./attack
tar -zcvf responses.tar.gz responses/


