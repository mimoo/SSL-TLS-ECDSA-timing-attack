This is a patch for OpenSSL.1.0.1j
although it might work on other version since it is so simple

This is to remove the fix here:

https://git.openssl.org/?p=openssl.git;a=blobdiff;f=CHANGES;h=1633d27975c91f122c4e9266b2c3cf4e56e8ffbf;hp=22749650b701d91cc43af24a226369116c2a46f8;hb=992bdde62d2eea57bb85935a0c1a0ef0ca59b3da;hpb=bbcf3a9b300bc8109bb306a53f6f3445ba02e8e9

And to add functions to get the signatures, hash, etc...
