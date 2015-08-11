# open all files
with open('nonces.log') as f:
    nonces = f.readlines()
with open('signatures.log') as f:
    signatures = f.readlines()
with open('truncated_digests.log') as f:
    digests = f.readlines()

# create our object
data = []

# iterate
for ii in range(len(nonces)):
    signature = eval(signatures[ii]) # dictionnary stringified {'r': ..., 's': ...}
    rr = int(signature["r"], 16)
    ss = int(signature["s"], 16)
    digest = int(digests[ii], 16) # hexstring
    nonce = int(nonces[ii]) # int
    
    #
    data.append({"r": rr, "s": ss, "m": digest, "k": nonce})

print str(data)
