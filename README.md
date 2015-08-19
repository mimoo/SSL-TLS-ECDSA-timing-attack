# Timing/Lattice Attack on the ECDSA (binary curves) nonces of OpenSSL

This is a work trying to reproduce and improve on *Billy Bob Brumley* and *Nicola Tuveri* - [Remote Timing Attacks are Still Practical](https://eprint.iacr.org/2011/232.pdf).

This is a **Work In Progress**. You can reproduce my setup with what you find here. The lattice attack works. The remote timing doesn't really. 

But first, if you want to know more about this research [check the latest draft of the whitepaper](whitepaper.pdf), and here are also direct links to the [Timing Attack](setup/client/attack.c) and the [Lattice Attack](setup/client/offline/lattice.sage).

If you know more about how to collect extremely accurate timing samples on a remote target I might need you. For now I get extremly bad results when attacking a remote target:

![sucky stats](http://i.imgur.com/mDaWP2B.png)

**update on august 18th 2015**. I disabled Nagel's algorith, got better results, tried isolating the CPU, got even better results!

![better](http://i.imgur.com/bF70cxr.png)

## Structure

* in `setup/` you can find how to setup the server and the client to reproduce the attack (and how to modify the server's openSSL to remove the fix)

* in `datasets/` you have data I got from my own experiments. You can play with that if you don't want to setup a client/server. Note that my measurements from the client sucks

* in `tools/` you have tools to play with the data in `datasets/`. Read the README there for more info.

* `PoC/` is an old proof of concept, it can run and find a key. It's not very pretty though


## Goal

The goal here is to find better techniques to get good timing results and cancel the noise.

The end goal is to release a script that targets an ip, request a bunch of signatures, try to mount the attack. If the server uses a vulnerable TLS implementation then it outputs the key.

What's to do to achieve that?

* get better timing results
* compute the hash and truncate it correctly (well actually we can copy the [openssl code](https://github.com/openssl/openssl/blob/master/crypto/ecdsa/ecs_ossl.c#L286) directly)
* maybe improve on the lattice attack? Nguyen seems to have better results with his.
* do the random subset algorithm to take care of false positives
* combine `attack.c` and `lattice.sage` in one file
