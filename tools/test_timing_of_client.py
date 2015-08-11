# open files
with open('client.log') as f:
    clients = f.readlines()

with open('server.log') as f:
    servers = f.readlines()

#
time_sorted = []
datas = []
    
# parse
for ii in range(len(clients)):
    client = eval(clients[ii])
    server = eval(servers[ii])
    
    """
    client = {'client_random': '6d70d7a74344e7ccf7c3ace7c77ff39f9d9b2ea56d26ac9292224aa32f17c10b', 'server_signature': '302e021503d4414c054acf852ef69619094f424cb9bd6fe8ae021502e9f64958ae97c7bf9c15d96d6c2d24b5628d6d0c', 'server_params': '0300174104c1910e8aec8c9db07ebb49430f7ace6d1f5bd6b8f267a294b6ea8e797ae60dad9a21af631756098e4e60c0aaf5f31869fe91265ea6341905f90dc78ee1ed911a', 'server_random': '07991aeb30108f2ced273874ab8b0d8f7d17e5aff173efe614f207b779dabcb3', 'time': 3722992}
    server = {'s': 4258690496956394045859710327486628073107983527180L, 'r': 5596267126179850251055206297425178818572863924398L, 'm': 306686801322590080711639249229159028718448815200507L, 'k': 1723561017441024778458494094126805326882747377778L}
    """
    # create global object
    datas.append(dict(client.items() + server.items()))
    
    # sort by time
    time_sorted.append(client['time'])

time_sorted.sort()

# sort the list by time
nonces_sorted_by_time = sorted(datas, key=lambda k: k['time']) 

# helper to give stats on a set of nonces
def stats_nonces(nonces):
    lengths = {}
    total = 0

    for nonce in nonces:
        nonce = nonce['k']
        total += 1
        bin_nonce = bin(int(nonce))[2:]
        nonce_len = len(bin_nonce)
        if nonce_len in lengths:
            lengths[nonce_len] += 1
        else:
            lengths[nonce_len] = 1

    keylist = lengths.keys()
    keylist.sort()

    print "%s\t/%s\t%s" % ("length", total, "percentage")

    for length in keylist:
        amount = lengths[length]
        percentage = int(amount * 100 / total)
        print "%s\t%s\t%i" % (length, amount, percentage)

# stats by time
print "stats on first 50 nonces"
stats_nonces(nonces_sorted_by_time[:50])

print "stats on first 100 nonces"
stats_nonces(nonces_sorted_by_time[:100])

print "stats on first 500 nonces"
stats_nonces(nonces_sorted_by_time[:500])
