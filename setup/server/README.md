You can use the `server.key` and `server.pem` as private key and certificate for the victim's server

You can apply patches to make the server vulnerable AND store useful data

You can use the utility `run_server.sh path_to_openssl/apps/openssl` to run the server with all the options (modify this file with your own openssl folder)

You can also use `python create_object.py > server.log` to create a list of tuples to use for verification, stats, etc...
