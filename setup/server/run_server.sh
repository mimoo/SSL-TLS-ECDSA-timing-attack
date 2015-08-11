#!/bin/bash
rm nonces.log 2> /dev/null
rm signatures.log 2> /dev/null
rm digests.log 2> /dev/null
"$1" s_server -cert server.pem -key server.key -cipher "ECDHE-ECDSA-AES128-SHA256" -serverpref -quiet 2> /dev/null
