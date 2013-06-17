/* vim:set sts=4 sw=4 expandtab: */
/*
Copyright (c) 2013 Sokolov Yura aka funny-falcon

MIT License

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Falcon.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#if defined(_MSC_VER)
#define ROTL64(x,y)  _rotl64(x,y)
#define BIG_CONSTANT(x) (x)
#else
static inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}
#define ROTL64(x,y) rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)
#endif

static inline uint64_t
_mem8cpy64(const unsigned char *buf, int i)
{
    uint32_t r = 0;
    uint64_t x = 0;
    switch(i) {
        case 7: x |= (uint64_t)buf[6] << 48;
        case 6: x |= (uint64_t)buf[5] << 40;
        case 5: x |= (uint64_t)buf[4] << 32;
        case 4:
            return x | *(uint32_t*)buf;
        case 3: r |= buf[2] << 16;
        case 2: r |= buf[1] << 8;
        case 1: r |= buf[0];
        case 0:
            return r;
    }
}

static inline uint64_t
mem8cpy64(const unsigned char *buf, int i)
{
    switch(i) {
        case 8: return *(uint64_t*)buf;
        case 7: return _mem8cpy64(buf, 7); break;
        case 6: return _mem8cpy64(buf, 6); break;
        case 5: return _mem8cpy64(buf, 5); break;
        case 4: return _mem8cpy64(buf, 4); break;
        case 3: return _mem8cpy64(buf, 3); break;
        case 2: return _mem8cpy64(buf, 2); break;
        case 1: return _mem8cpy64(buf, 1); break;
        case 0: return 0;
    }
}

static inline void
mem16cpy64(uint64_t *block, const unsigned char *buf, int i)
{
    switch(i) {
        case 16:
            block[1] = *((uint64_t*)buf + 1);
            block[0] = *((uint64_t*)buf + 0);
            break;
        case 15: block[1] = _mem8cpy64(buf+8, 7); goto block0;
        case 14: block[1] = _mem8cpy64(buf+8, 6); goto block0;
        case 13: block[1] = _mem8cpy64(buf+8, 5); goto block0;
        case 12: block[1] = _mem8cpy64(buf+8, 4); goto block0;
        case 11: block[1] = _mem8cpy64(buf+8, 3); goto block0;
        case 10: block[1] = _mem8cpy64(buf+8, 2); goto block0;
        case 9: block[1] = _mem8cpy64(buf+8, 1); goto block0;
        case 8:
block0:
            block[0] = *(uint64_t*)buf;
            break;
        case 7: block[0] = _mem8cpy64(buf, 7); break;
        case 6: block[0] = _mem8cpy64(buf, 6); break;
        case 5: block[0] = _mem8cpy64(buf, 5); break;
        case 4: block[0] = _mem8cpy64(buf, 4); break;
        case 3: block[0] = _mem8cpy64(buf, 3); break;
        case 2: block[0] = _mem8cpy64(buf, 2); break;
        case 1: block[0] = _mem8cpy64(buf, 1); break;
        case 0:
            break;
    }
}

static const uint64_t BC1 = BIG_CONSTANT(0x87c37b911142559d);
static const uint64_t BC2 = BIG_CONSTANT(0x4cf5ad43274593b9);
static const uint64_t BC3 = BIG_CONSTANT(0xa951afd7ed558e95);
static const uint64_t BC4 = BIG_CONSTANT(0xc4ceb9fe1a85ed6b);
static const uint64_t BC5 = BIG_CONSTANT(0xc3a5c85c97cb3199);
static const uint64_t BC6 = BIG_CONSTANT(0xb492b66fbe98f273);
static const uint64_t BC7 = BIG_CONSTANT(0x9ae16a3b2d8542ab);
static const uint64_t BC8 = BIG_CONSTANT(0xc949d7c7529e6733);

typedef struct u128 {
    uint64_t h1, h2;
} u128_t;

static inline uint64_t
xxxa(uint64_t h, uint64_t b0, uint64_t c, uint64_t b1, uint64_t d)
{
    return ((h ^ b0) * c) ^ (ROTL64(b1, 32) * d);
}

static inline uint64_t
xxxb(uint64_t h0, uint64_t b0, uint64_t c0, uint64_t h1, uint64_t b1, uint64_t c1)
{
    return ((h0 ^ ROTL64(b0, 32)) * c0) ^ ((h1 ^ ROTL64(b1, 32)) * c1);
}

static inline uint64_t
xxxc(uint64_t h, uint64_t b0, uint64_t c, uint64_t b1, uint64_t d)
{
    return ((ROTL64(h, 15) ^ ROTL64(b0, 16)) * c) ^ (ROTL64(b1, 32) * d);
}

static inline u128_t
fh128_step(u128_t hash, const uint64_t block[4])
{
      uint64_t v1, v2, v3, v4, v5;
      uint64_t ht = hash.h1 ^ hash.h2;
      u128_t fin;
      v1 = xxxa(hash.h1, block[0], BC1, block[1], BC5);
      v2 = xxxa(hash.h1, block[1], BC2, block[2], BC6);
      v3 = xxxa(hash.h2, block[2], BC3, block[3], BC7);
      v4 = xxxa(hash.h2, block[3], BC4, block[0], BC8);
      v5 = xxxb(v3, v1, BC6, v4, v2, BC8);
      hash.h2 = xxxb(v5 ^ v2, v3, BC1, v1, v4, BC3);
      hash.h1 = xxxb(hash.h2, v5, BC5, 0, hash.h2, BC7);
      return hash;
}

static inline u128_t
__fh128_step(u128_t hash, const uint64_t block[2])
{
      uint64_t tmp;
      tmp = xxxa(hash.h1, block[0], BC1, block[0], BC3);
      hash.h2 = xxxb(hash.h2, block[0], BC2, 0, tmp, BC4);
      hash.h1 = xxxa(hash.h2, tmp, BC5, hash.h2, BC6);
      return hash;
}

static inline u128_t
_fh128_step(u128_t hash, const uint64_t block[2])
{
      u128_t tmp;
      tmp.h1 = xxxa(hash.h1, block[0], BC1, block[1], BC2);
      tmp.h2 = xxxa(hash.h2, block[1], BC3, block[0], BC4);
      hash.h2 = xxxa(tmp.h1, tmp.h2, BC7, tmp.h1, BC8);
      hash.h1 = xxxc(hash.h2, tmp.h1, BC5, tmp.h2, BC6);
      return hash;
}

void
FalconHash128(const void *key, int len, uint32_t seed, void * out)
{
    u128_t hash, fin;
    int cnt = len >> 5;
    int i = len & 31;
    const uint64_t *chunk = (const uint64_t*)key;
    hash.h1 = seed;
    hash.h2 = seed + BC7;
    while(cnt--) {
	uint64_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh128_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint64_t tmp, block[4] = {0, 0, 0, 0};
        hash.h1 ^= i; hash.h2 ^= i;
        if (i <= 8) {
            block[0] = mem8cpy64((const uint8_t*)chunk, i);
            hash = __fh128_step(hash, block);
        } else
        if (i <= 16) {
            mem16cpy64(block, (const uint8_t*)chunk, i);
            hash = _fh128_step(hash, block);
        }
        else {
            block[0] = chunk[0];
            block[1] = chunk[1];
            mem16cpy64(block + 2, (const uint8_t*)(chunk + 2), i - 16);
            hash = fh128_step(hash, block);
        }
    }
    hash.h2 = xxxa(0, hash.h2, BC2, hash.h1, BC4);
    *(u128_t*)out = hash;
}

void
FalconHash128_0(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[4] = {0, 0, 0, 0};
    FalconHash128(key, len, seed, hash);
    *(uint32_t*)out = hash[0];
}

void
FalconHash128_1(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[4] = {0, 0, 0, 0};
    FalconHash128(key, len, seed, hash);
    *(uint32_t*)out = hash[1];
}

void
FalconHash128_2(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[4] = {0, 0, 0, 0};
    FalconHash128(key, len, seed, hash);
    *(uint32_t*)out = hash[2];
}

void
FalconHash128_3(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[4] = {0, 0, 0, 0};
    FalconHash128(key, len, seed, hash);
    *(uint32_t*)out = hash[3];
}

void
FalconHash128_02(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[4] = {0, 0, 0, 0};
    FalconHash128(key, len, seed, hash);
    *(uint32_t*)out = hash[0] ^ hash[2];
}
