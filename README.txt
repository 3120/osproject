This is our cache branch, separate from our main branch to ensure that
the original code would work as intended.

All methods for the cache.c file have been constructed, with references 
made in the sws.c file.

The code in its current state will not run properly, as there were issues
with both reading and writing to a list of files.

The methods instead use arrays (meaning that the LRU methodology is
not used) to store cfd, file references, and access counts.

Further work would be needed to ensure that the sws would fully use 
the cacheing methods, including proper linked lists, deadlock prevention,
and null checks.