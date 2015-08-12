# The lattice attack

Now that you have done the attack you should have a set of signatures (r, s) and their relevant truncated digests. You can now mount the attack with `lattice.sage` and the Sage software.

You should:

* get rid of the first ~10 responses, they might be rubbish because of server caching or something.

* take the set of the 60 smallest timings, then take a subset of 43 tuples from it and try the attack. Rinse and repeat.
