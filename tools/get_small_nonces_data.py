import argparse

# check arguments
parser = argparse.ArgumentParser()
parser.add_argument("size", help="the size of the nonces related to the signatures/hashes you will receive")
parser.add_argument("amount", help="the amount of tuples you want")
parser.add_argument("-u", "--upperbound", action="store_true")
args = parser.parse_args()

# open files
with open('server.log') as f:
    servers = f.readlines()

# parse
total = 0
for server in servers:
    server = eval(server)
    """
    server = {'s': 4258690496956394045859710327486628073107983527180L, 'r': 5596267126179850251055206297425178818572863924398L, 'm': 306686801322590080711639249229159028718448815200507L, 'k': 1723561017441024778458494094126805326882747377778L}
    """
    # is size of nonce good?
    size_nonce = len(bin(server['k'])) - 2
    if size_nonce == int(args.size) or (size_nonce < int(args.size) and args.upperbound):
        obj = {'r': server['r'], 's': server['s'], 'm': server['m']}
        print str(obj)
        total += 1
        if total >= int(args.amount):
            break

