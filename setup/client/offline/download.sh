#!/bin/bash

# get server 
scp user@servermachine:/path_to/server.log server.log

# get client
scp user@clientmachine:/path_to/responses.log client.log

# remove the first results
echo "$(tail -n +11 server.log)" > server.log
echo "$(tail -n +11 client.log)" > client.log

# debug
wc -l *

# stats
# python test_timing_of_client.py
