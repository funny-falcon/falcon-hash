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
#define ROTL32(x,y)  _rotl(x,y)
#define ROTL64(x,y)  _rotl64(x,y)
#define BIG_CONSTANT(x) (x)
#else
static inline uint32_t
rotl32 ( uint32_t x, int8_t r )
{
  if (r == 0) return x;
  return (x << r) | (x >> (32 - r));
}
static inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  if (r == 0) return x;
  return (x << r) | (x >> (64 - r));
}

#define ROTL32(x,y) rotl32(x,y)
#define ROTL64(x,y) rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)
#endif


static inline uint32_t
_mem4cpy(const unsigned char *buf, int i)
{
    uint32_t r = 0;
    switch(i) {
        case 4:
            return *(uint32_t*)buf;
        case 3: r |= buf[2] << 16;
        case 2: r |= buf[1] << 8;
        case 1: r |= buf[0];
        case 0:
            return r;
    }
}

static inline uint32_t
mem4cpy(const unsigned char *buf, int i)
{
    switch(i) {
        case 4: return _mem4cpy(buf, 4);
        case 3: return _mem4cpy(buf, 3);
        case 2: return _mem4cpy(buf, 2);
        case 1: return _mem4cpy(buf, 1);
        case 0: return 0;
    }
}

static inline void
mem16cpy(uint32_t *block, const unsigned char *buf, int i)
{
    switch(i) {
        case 15: block[3] = _mem4cpy(buf + 12, 3); goto block2;
        case 14: block[3] = _mem4cpy(buf + 12, 2); goto block2;
        case 13: block[3] = _mem4cpy(buf + 12, 1); goto block2;
        case 12:
block2:
            block[2] = _mem4cpy(buf + 8, 4);
            block[1] = _mem4cpy(buf + 4, 4);
            block[0] = _mem4cpy(buf, 4);
            break;
        case 11: block[2] = _mem4cpy(buf + 8, 3); goto block1;
        case 10: block[2] = _mem4cpy(buf + 8, 2); goto block1;
        case 9:  block[2] = _mem4cpy(buf + 8, 1); goto block1;
        case 8:
block1:
            block[1] = _mem4cpy(buf + 4, 4);
            block[0] = _mem4cpy(buf, 4);
            break;
        case 7: block[1] = _mem4cpy(buf + 4, 3); goto block0;
        case 6: block[1] = _mem4cpy(buf + 4, 2); goto block0;
        case 5: block[1] = _mem4cpy(buf + 4, 1); goto block0;
        case 4:
block0:
            block[0] = _mem4cpy(buf, 4);
            break;
        case 3: block[0] = _mem4cpy(buf, 3); break;
        case 2: block[0] = _mem4cpy(buf, 2); break;
        case 1: block[0] = _mem4cpy(buf, 1); break;
        case 0:
            break;
    }
}

static inline uint64_t
_mem8cpy64(const unsigned char *buf, int i)
{
    uint32_t r = 0;
    uint64_t x = 0;
    switch(i) {
        case 8:
            return *(uint64_t*)buf;
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
        case 8: return _mem8cpy64(buf, 8);
        case 7: return _mem8cpy64(buf, 7);
        case 6: return _mem8cpy64(buf, 6);
        case 5: return _mem8cpy64(buf, 5);
        case 4: return _mem8cpy64(buf, 4);
        case 3: return _mem8cpy64(buf, 3);
        case 2: return _mem8cpy64(buf, 2);
        case 1: return _mem8cpy64(buf, 1);
        case 0: return 0;
    }
}

static inline void
mem16cpy64(uint64_t *block, const unsigned char *buf, int i)
{
    switch(i) {
        case 16:
            block[1] = _mem8cpy64(buf+8, 8);
            block[0] = _mem8cpy64(buf, 8);
            break;
        case 15: block[1] = _mem8cpy64(buf+8, 7); goto block0;
        case 14: block[1] = _mem8cpy64(buf+8, 6); goto block0;
        case 13: block[1] = _mem8cpy64(buf+8, 5); goto block0;
        case 12: block[1] = _mem8cpy64(buf+8, 4); goto block0;
        case 11: block[1] = _mem8cpy64(buf+8, 3); goto block0;
        case 10: block[1] = _mem8cpy64(buf+8, 2); goto block0;
        case 9:  block[1] = _mem8cpy64(buf+8, 1); goto block0;
        case 8:
block0:
            block[0] = _mem8cpy64(buf, 8);
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

static const uint32_t C1 = 0x239b961b;
static const uint32_t C2 = 0xab0e9789;
static const uint32_t C3 = 0x38b34ae5;
static const uint32_t C4 = 0xa1e38b93;
static const uint32_t C5 = 0xcc9e2d51;
static const uint32_t C6 = 0x85ebca6b;
static const uint32_t C7 = 0x53215229;

typedef struct fh_u64 {
    uint32_t h1, h2;
} u64_t;

static inline uint32_t
ttt(uint32_t h, int rh, uint32_t b0, int rb0, uint32_t c, uint32_t b1, int rb1, uint32_t d)
{
    return ((ROTL32(h, rh) ^ ROTL32(b0, rb0)) * c) ^ (ROTL32(b1, rb1) * d);
}

static inline uint32_t
tta(uint32_t h, uint32_t b0, uint32_t c, uint32_t b1, uint32_t d)
{
    return ((h ^ b0) * c) ^ (ROTL32(b1, 16) * d);
}

static inline uint32_t
ttb(uint32_t h, uint32_t b0, uint32_t c, uint32_t k, uint32_t b1, uint32_t d)
{
    return ((h ^ ROTL32(b0, 16)) * c) ^ ((k ^ ROTL32(b1, 16)) * d);
}

static inline u64_t
fh_step(u64_t hash, const uint32_t block[4])
{
      uint32_t v1, v2, v3, v4, v5;
      uint32_t ht = hash.h1 ^ hash.h2;
      v1 = tta(hash.h1, block[0], C1, block[1], C5);
      v2 = tta(hash.h1, block[1], C2, block[2], C6);
      v3 = tta(hash.h2, block[2], C3, block[3], C1);
      v4 = tta(hash.h2, block[3], C4, block[0], C7);
      v5 = ttb(v3, v1, C5, v4, v2, C6);
      hash.h2 = ttb(v5 ^ v2, v3, C4, v1, v4, C3);
      hash.h1 = ttb(hash.h2, v5, C7, 0, hash.h2, C2);
      return hash;
}

static inline u64_t
_fh_step(u64_t hash, const uint32_t block[2])
{
      uint32_t v1, v2, v3, v4;
      uint32_t ht = hash.h1 ^ hash.h2;
      v1 = tta(hash.h1, block[0], C1, block[1], C2);
      v2 = tta(hash.h2, block[1], C3, block[0], C4);
      hash.h2 = tta(v1, v2, C5, v1, C6);
      hash.h1 = ttt(hash.h2, 7, v1, 16, C7, v2, 16, C7);
      return hash;
}

void
FalconHash64(const void *key, int len, uint32_t seed, void * out)
{
    u64_t hash;
    int cnt = len / 16;
    int i = len % 16;
    const uint32_t *chunk = (const uint32_t*)key;
    hash.h1 = seed;
    hash.h2 = seed + C7;
    while(cnt--) {
	uint32_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint32_t block[4] = {0, 0, 0, 0};
        hash.h1 ^= i; hash.h2 ^= i;
        if (i < 8) {
            mem16cpy(block, (const unsigned char*)chunk, i);
            hash = _fh_step(hash, block);
        } else {
            mem16cpy(block, (const unsigned char*)chunk, i);
            hash = fh_step(hash, block);
        }
    }
    hash.h2 = ttt(hash.h1, 0, hash.h2, 16, C1, hash.h1, 16, C7);
    *(u64_t*)out = hash;
}

void
FalconHash64_0(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[2] = {0, 0};
    FalconHash64(key, len, seed, hash);
    *(uint32_t*)out = hash[0];
}

void
FalconHash64_1(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[2] = {0, 0};
    FalconHash64(key, len, seed, hash);
    *(uint32_t*)out = hash[1];
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
