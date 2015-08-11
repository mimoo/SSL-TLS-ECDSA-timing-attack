# timing_attack_ecdsa_tls
Timing Attack on the ECDSA part of an ECDHE TLS handshake


* `attack.c` is a to request signatures from the server and collect its response time (partially from modifications of FAU Timer and some code grabbed on https://idea.popcount.org/2013-01-28-counting-cycles---rdtsc/)

* `lattice.sage` is the lattice attack, to execute with the Sage software.

* in `setup/` you can find how to setup the server and the client to reproduce the attack (and how to modify the server's openSSL to remove the fix)

* in `datasets/` you have data I got from my own experiments. You can play with that if you don't want to setup a client/server. Note that my measurements from the client sucks

* in `tools/` you have tools to play with the data in `datasets/`. Read the README there for more info.


