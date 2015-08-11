# Unpatch OpenSSL

Check `unpatch.patch`, this is a [to revert this patch](https://git.openssl.org/?p=openssl.git;a=blobdiff;f=CHANGES;h=1633d27975c91f122c4e9266b2c3cf4e56e8ffbf;hp=22749650b701d91cc43af24a226369116c2a46f8;hb=992bdde62d2eea57bb85935a0c1a0ef0ca59b3da;hpb=bbcf3a9b300bc8109bb306a53f6f3445ba02e8e9) for OpenSSL.1.0.1j, although it might work on older version since it is so simple.

go to `openssl/crypto/ecdsa/` and run the patch:

```
patch ecs_oss.c < unpatch.patch
```

# Get nonces when querying

By applying this patch you will save the nonces generated every time the server is queried

it saves to `server_nonces.log`

# Get timings when querying

By applying this patch you will save the timings of the nonce multplication when the server is queried

it saves to `server_timings.log`

# Get signatures when querying

# Get
