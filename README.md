# Timing Attack on the ECDSA nonces of OpenSSL

This is a work trying to reproduce and improve on *Billy Bob Brumley* and *Nicola Tuveri* - [Remote Timing Attacks are Still Practical](https://eprint.iacr.org/2011/232.pdf).

This is a work in progress. The lattice attack works. The remote timing doesn't. If you know more about how to collect extremly accurate timing samples on a remote target I might need you. For now I get extremly bad results when attacking a remote target:

![sucky stats](http://i.imgur.com/mDaWP2B.png)

If you want to know more about the lattice attack. It is following a paper from *N.A. Howgrave-Graham, N.P. Smart* - [Lattice Attacks on Digital Signature Schemes](http://www.hpl.hp.com/techreports/1999/HPL-1999-90.pdf). But Babai sucks, so I implemented the embedded strategy instead (it reduces CVP to SVP), you can learn more in these two papers:

* 

## Structure

* in `setup/` you can find how to setup the server and the client to reproduce the attack (and how to modify the server's openSSL to remove the fix)

* in `datasets/` you have data I got from my own experiments. You can play with that if you don't want to setup a client/server. Note that my measurements from the client sucks

* in `tools/` you have tools to play with the data in `datasets/`. Read the README there for more info.

* `PoC/` is an old proof of concept, it can run and find a key. It's not very pretty though
