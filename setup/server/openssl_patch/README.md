# Get OpenSSL

For this we use OpenSSL 1.0.1j although a more recent version might work:

```
wget ftp://ftp.openssl.org/source/old/1.0.1/openssl-1.0.1j.tar.gz
tar xvfz openssl-1.0.1j.tar.gz
cd openssl-1.0.1j && ./config && make
```

# Patch OpenSSL

All the patch are here, to use them go in the relevant folder and do:

```
patch ecs_oss.c < unpatch.patch
```

Although you might want to do it by hand because I might have fucked the `diff`

## Unpatch OpenSSL

Check `unpatch.patch`, this is a [to revert this patch](https://git.openssl.org/?p=openssl.git;a=blobdiff;f=CHANGES;h=1633d27975c91f122c4e9266b2c3cf4e56e8ffbf;hp=22749650b701d91cc43af24a226369116c2a46f8;hb=992bdde62d2eea57bb85935a0c1a0ef0ca59b3da;hpb=bbcf3a9b300bc8109bb306a53f6f3445ba02e8e9) for OpenSSL.1.0.1j, although it might work on older version since it is so simple.

go to `openssl/crypto/ecdsa/` and run the patch.

## Store signatures

You can apply signatures.patch, this will save signatures in `signatures.log` 

go to `openssl/crypto/ecdsa/` and run the patch.

## Store nonces

By applying this patch you will save the nonces generated every time the server is queried

it saves to `server_nonces.log`

## Store timings

By applying this patch you will save the timings of the nonce multplication when the server is queried

it saves to `server_timings.log`


## Store hashes

What you want to calculate an ECDSA signature is a truncated digest

`truncated_digests.log`



