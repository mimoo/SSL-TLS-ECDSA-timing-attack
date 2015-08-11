#/bin/bash!
rm server_*
rm signatures.log
rm digests.log
openssl-1.0.1j/apps/openssl s_server -cert server.pem -key server.key -cipher "ECDHE-ECDSA-AES128-SHA256" -serverpref -quiet
