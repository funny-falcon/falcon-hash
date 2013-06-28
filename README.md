FalconHash
===========

Attepmpt to make relatively safe (safer than MurmurHash3) and relatively fast (faster than SipHash on x86 and x86_64)
hash function.

Main weakness of MurmurHash3 (and CityHash) is existance of hash collisions which are independant of starting seed.
This happens cause MurmurHash3 tries to produce some hash value for block and than mix it into seed.
Also it do very little shuffling between iterations, and generally depends on very good finalization.

This hash functions tries to solve it by mixing seed earlier into hashed block, applying every byte of block at least
twice, doing much more shuffling at every step, so that (half of) "finalization" occures between every block.

SipHash-2-4 is considerably slower than MurmurHash3 _(variant suitable for platform)_ for every message (at least 2 times slower 
on short message and up to 6 times slower for long messages). SipHash-1-3 is lightest version with good avalance, and
it is still upto 1.5x slower on short messages and upto 4 times slower on long messages _(compared with variant suitable
for a chosen platform)_.

This hash uses different block ciphers for short messages and long messages, so that it is alway ~ 1.5 slower than
corresponding MurmurHash3 on corresponding platform.

It is faster than SipHash-2-4 on best suited and not slower on not matched platform.
It could be slower than SipHash-1-3 when platform is not best suitable.

Comparisons of speed against **SipHash-1-3** *(x faster)*

| Platform and message | FalconHash64 | FalconHash128\_64\_fast | FalconHash128 |
|----------------------|-------------:|----------------------:|--------------:|
| x86 <= 8bytes        |         1.5  |                 0.7   |         0.7   |
| x86 long messages    |         3.0  |                 1.4   |         1.4   |
| x86_64 <= 8bytes     |         1.0  |                 1.2   |         1.1   |
| x86_64 long messages |         0.7  |                 1.4   |         1.4   |


Comparisons of speed against **SipHash-2-4** *(x faster)*

| Platform and message | FalconHash64 | FalconHash128_64_fast | FalconHash128 |
|----------------------|-------------:|----------------------:|--------------:|
| x86 <= 8bytes        |         2.1  |                 1.0   |         1.0   |
| x86 long messages    |         4.0  |                 2.0   |         2.0   |
| x86_64 <= 8bytes     |         1.3  |                 1.3   |         1.2   |
| x86_64 long messages |         1.4  |                 2.7   |         2.7   |

Disclaimer
==========

I have no strong matematical education, so that I could not claim this is "criptographic" hash.
Use it when you wish to use Murmur hash - for general purpose table lookup, consistent hashing or very short time
signs with rotating secrets.

Also, FalconHash128 relies on presence of uint64_t type (so that platform ought to have at least 
32bit*32bit=64bit multiplication, otherwise it will be extra slow).
