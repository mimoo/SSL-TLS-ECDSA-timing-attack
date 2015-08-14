import argparse

############################################
# Arguments
##########################################

parser = argparse.ArgumentParser()
parser.add_argument("file", help="the files to get the tuples of signatures + truncated hashes from")
parser.add_argument("amount", nargs='?', type=int, default=0, help="number of tuples to use from the file")
parser.add_argument("bits", nargs='?', type=int, default=1, help="number of MSB known")
parser.add_argument("-L", "--LLL", action="store_true")
parser.add_argument("-v", "--verbose", action="store_true")
args = parser.parse_args()

############################################
# Functions
##########################################

def lattice_overview(BB):
    for ii in range(BB.dimensions()[0]):
        a = ('%02d ' % ii)
        for jj in range(BB.dimensions()[1]):
            a += '0' if BB[ii,jj] == 0 else 'X'
            if BB.dimensions()[0] < 60:
                a += ' '
        print a



############################################
# Core
##########################################

def HowgraveGrahamSmart_ECDSA(digests, signatures, modulo, pubx, trick, reduction):
    print "# New attack"

    # Building Equations
    # getting rid of the first equation
    r0_inv = inverse_mod(signatures[0][0], modulo)
    s0 = signatures[0][1]
    m0 = digests[0]

    AA = [-1]
    BB = [0]

    nn = len(digests)
    print "building lattice of size", nn + 1

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
    if trick != -1:
        lattice = Matrix(ZZ, nn + 1)
    else:
        lattice = Matrix(ZZ, nn)

    # Fill lattice
    for ii in range(nn):
        lattice[ii, ii] = modulo
        lattice[0, ii] = AA[ii]

    # Add trick
    if trick != -1:
        print "adding trick:", trick
        BB.append(trick)
        lattice[nn] = vector(BB)
    else:
        print "not adding any trick"

    # Display lattice
    if args.verbose:
        lattice_overview(lattice)

    # BKZ or LLL
    if reduction == "LLL":
        print "using LLL"
        lattice = lattice.LLL()
    else:
        print "using BKZ"
        lattice = lattice.BKZ()

    # If a solution is found, format it
    if trick == -1 or Mod(lattice[0,-1], modulo) == trick or Mod(lattice[0,-1], modulo) == Mod(-trick, modulo):
        # did we found trick or -trick?
        if trick != -1:
            # trick
            if Mod(lattice[0,-1], modulo) == trick:
                solution = -1 * lattice[0] - vector(BB)
            # -trick
            else:
                print "we found a -trick instead of a trick" # this shouldn't change anything
                solution = lattice[0] + vector(BB)
        # if not using a trick, the problem is we don't know how the vector is constructed
        else:
            solution = -1 * lattice[0] - vector(BB) # so we choose this one, randomly :|
            #solution = lattice[0] + vector(BB)

        # get rid of (..., trick) if we used the trick
        if trick != -1:
            vec = list(solution)
            vec.pop()
            solution = vector(vec)

        # get d
        rr = signatures[0][0]
        ss = signatures[0][1]
        mm = digests[0]
        nonce = solution[0]

        key = Mod((ss * nonce - mm) * inverse_mod(rr, modulo), modulo)

        return True, key
    else:
        return False, 0

############################################
# Our Attack
##########################################

# get public key x coordinate
pubx = 0x04f3e6ddffc4ba45282f3fabe0e8a220b98980387a

# we have the private key for verifying our tests
priv = 0x0099ad4abb9a955085709d1dede97aedf230ec0ec9

# and public key modulo taken from NIST or FIPS (http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf)
modulo = 5846006549323611672814742442876390689256843201587

# trick
trick = int(modulo / 2^(args.bits - 1)) # using trick made for MSB known = args.bits

# LLL or BKZ?
if args.LLL:
    reduction = "LLL"
else:
    reduction = "BKZ"

# Get a certain amount of data
with open(args.file, "r") as f:
    tuples = f.readlines()

if args.amount == 0:
    nn = len(tuples)
elif args.amount <= len(tuples):
    nn = args.amount
else:
    print "can't use that many tuples, using max number of tuples available"
    nn = len(tuples)
    
print "building", nn, "equations"

# Parse the data
digests = []
signatures = []

for tuple in tuples[:args.amount]:
    obj = eval(tuple) # {'s': long, 'r': long, 'm': long}
    digests.append(obj['m'])
    signatures.append((obj['r'], obj['s']))

# Attack
for tt in [trick, 1, -1]:
    status, key = HowgraveGrahamSmart_ECDSA(digests, signatures, modulo, pubx, tt, reduction)
    if status:
        if tt != -1:
            print "found key with trick", trick
        else:
            print "since we are not using any trick, might not be the solution"
        print "key:", key
        if key == priv:
            print "the key is correct!"
        else:
            print "key is incorrect"
    else:
        print "found nothing"

    print "\n"
