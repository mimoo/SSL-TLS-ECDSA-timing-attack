# Config
lattice_size = 35   # number of signatures
trick = 2^163 / 2^8 # 7 leading bits
print trick

# Get data
with open("nonces_155.log", "r") as f:
    content = f.readlines()

digests = []
signatures = []

# Parse it
for item in content[:lattice_size]:
    data = item.strip("\n")
    data = data.split(" ")
    data = list(truc.strip("L") for truc in data)
    data = map(int, data)
    digests.append(data[1])
    signatures.append((data[2], data[3]))

# get public key x coordinate
pubx = 0x04f3e6ddffc4ba45282f3fabe0e8a220b98980387a

# and public key modulo
# taken from NIST or FIPS (http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf)
modulo = 5846006549323611672814742442876390689256843201587

# Building Equations
nn = len(digests)

# getting rid of the first equation
r0_inv = inverse_mod(signatures[0][0], modulo)
s0 = signatures[0][1]
m0 = digests[0]

AA = [-1]
BB = [0]

for ii in range(1, nn):
    mm = digests[ii]
    rr = signatures[ii][0]
    ss = signatures[ii][1]
    ss_inv = inverse_mod(ss, modulo)

    AA_i = Mod(-1 * s0 * r0_inv * rr * ss_inv, modulo)
    BB_i = Mod(-1 * mm * ss_inv + m0 * r0_inv * rr * ss_inv, modulo)
    AA.append(AA_i.lift())
    BB.append(BB_i.lift())

# Embedding Technique (CVP->SVP)
lattice = Matrix(ZZ, nn + 1)

# Fill lattice
for ii in range(nn):
    lattice[ii, ii] = modulo
    lattice[0, ii] = AA[ii]

BB.append(trick)
lattice[nn] = vector(BB)

# LLL
lattice = lattice.LLL() # should get better results with BKZ instead of LLL

# If a solution is found, format it
if lattice[0,-1] % modulo == trick:
    # get rid of (..., 1)
    vec = list(lattice[0])
    vec.pop()
    vec = vector(vec)
    solution = -1 * vec
    
    # get d
    rr = signatures[0][0]
    ss = signatures[0][1]
    mm = digests[0]
    nonce = solution[0]

    key = Mod((ss * nonce - mm) * inverse_mod(rr, modulo), modulo)
    
    print "found a key"
    print key
