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
  return (x << r) | (x >> (32 - r));
}
static inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define ROTL32(x,y) rotl32(x,y)
#define ROTL64(x,y) rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)
#endif



static const uint32_t C1 = 0x239b961b; 
static const uint32_t C2 = 0xab0e9789;
static const uint32_t C3 = 0x38b34ae5; 
static const uint32_t C4 = 0xa1e38b93;
static const uint32_t CM = 0xcc9e2d51;
static const uint32_t CA = 0x85ebca6b;
static const uint32_t CB = 0x53215229;

static inline void
mem16cpy(uint32_t *block, const unsigned char *buf, int i)
{
    switch(i) {
        case 15: block[3] |= buf[14] << 16;
        case 14: block[3] |= buf[13] << 8;
        case 13: block[3] |= buf[12];
        case 12:
            block[2] = *((uint32_t*)buf + 2);
            block[1] = *((uint32_t*)buf + 1);
            block[0] = *((uint32_t*)buf + 0);
            break;
        case 11: block[2] |= buf[10] << 16;
        case 10: block[2] |= buf[9] << 8;
        case 9: block[2] |= buf[8];
        case 8:
            block[1] = *((uint32_t*)buf + 1);
            block[0] = *((uint32_t*)buf + 0);
            break;
        case 7: block[1] |= buf[6] << 16;
        case 6: block[1] |= buf[5] << 8;
        case 5: block[1] |= buf[4];
        case 4:
            block[0] = *((uint32_t*)buf + 0);
            break;
        case 3: block[0] |= buf[2] << 16;
        case 2: block[0] |= buf[1] << 8;
        case 1: block[0] |= buf[0];
            break;
    }
}

static inline uint32_t
fh_step(uint32_t hash, const uint32_t block[4])
{
      uint32_t v1, v2, v3, v4;
      v1 = ((hash ^ block[0]) * C1) ^ ROTL32(block[1], 13);
      v2 = (block[2] * C2) ^ ROTL32(block[3], 17);
      v3 = (block[3] * C3) ^ ROTL32(block[2], 13);
      v4 = (block[1] * C4) ^ ROTL32(block[0], 17);
      v1 ^= v2 * C3;
      v3 ^= v4 * C2;
      hash = (ROTL32(v1, 13) * CA) ^ (ROTL32(v3, 15) * CB);
      return hash;
}

static inline uint32_t
fhs_step(uint32_t hash, const uint32_t block[4])
{
      uint32_t v1, v2, v3, v4;
      v1 = ((hash ^ block[0]) * C1) ^ ROTL32(block[1], 13);
      v2 = (block[2] * C2) ^ ROTL32(block[3], 17);
      v3 = (block[3] * C3) ^ ROTL32(block[2], 13);
      v4 = (block[1] * C4) ^ ROTL32(block[0], 17);
      v1 = (v1 * C4) ^ (v2 * C3);
      v3 = (v3 * C1) + (v4 * C2);
      hash = (ROTL32(v1, 13) * CA) ^ (ROTL32(v3, 15) * CB);
      return hash;
}

void
FalconHash(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash;
    int cnt = len >> 4;
    int i = len & 15;
    const uint32_t *chunk = (const uint32_t*)key;
    hash = (seed + 1) * CA;
    seed = hash |= !hash * CB;
    while(cnt--) {
	uint32_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint32_t block[4] = {0, 0, 0, 0};
        mem16cpy(block, (const unsigned char*)chunk, i);
        hash = fh_step(hash ^ i, block);
    }
    hash ^= seed;
    hash = (hash ^ (hash >> 15)) * CM;
    hash = (hash ^ (hash >> 15)) * CB;
    *(uint32_t*)out = hash;
}

void
FalconHashSec(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash;
    int cnt = len >> 4;
    int i = len & 15;
    const uint32_t *chunk = (const uint32_t*)key;
    hash = (seed + 1) * CA;
    seed = hash |= !hash * CB;
    while(cnt--) {
	uint32_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fhs_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint32_t block[4] = {0, 0, 0, 0};
        mem16cpy(block, (const unsigned char*)chunk, i);
        hash = fhs_step(hash ^ i, block);
    }
    hash ^= ROTL32(seed, 13);
    hash = (hash ^ (hash >> 15)) * CM;
    hash = (hash ^ (hash >> 15)) * CB;
    *(uint32_t*)out = hash;
}

static const uint64_t BC1 = BIG_CONSTANT(0x87c37b91114253d5);
static const uint64_t BC2 = BIG_CONSTANT(0x4cf5ad432745937f);
static const uint64_t BC3 = BIG_CONSTANT(0xff51afd7ed558ccd);
static const uint64_t BC4 = BIG_CONSTANT(0xc4ceb9fe1a85ec53);
static const uint64_t BC5 = BIG_CONSTANT(0xc3a5c85c97cb3127);
static const uint64_t BC6 = BIG_CONSTANT(0xb492b66fbe98f273);

static inline uint64_t
fh64_step(uint64_t hash, const uint64_t block[4])
{
      uint64_t v1, v2, v3, v4;
      v1 = ((hash ^ block[0]) * BC1) ^ ROTL64(block[1], 29);
      v2 = (block[2] * BC2) ^ ROTL64(block[3], 33);
      v3 = (block[3] * BC3) ^ ROTL64(block[2], 29);
      v4 = (block[1] * BC4) ^ ROTL64(block[0], 33);
      v1 ^= v2 * BC3;
      v3 ^= v4 * BC2;
      hash = (ROTL64(v1, 29) * BC5) ^ (ROTL64(v3, 31) * BC6);
      return hash;
}

static inline uint64_t
fh64s_step(uint64_t hash, const uint64_t block[4])
{
      uint64_t v1, v2, v3, v4;
      v1 = ((hash ^ block[0]) * BC1) ^ ROTL64(block[1], 29);
      v2 = (block[2] * BC2) ^ ROTL64(block[3], 33);
      v3 = (block[3] * BC3) ^ ROTL64(block[2], 29);
      v4 = (block[1] * BC4) ^ ROTL64(block[0], 33);
      v1 = (v1 * BC4) ^ (v2 * BC3);
      v3 = (v3 * BC1) + (v4 * BC2);
      hash = (ROTL64(v1, 29) * BC1) ^ (ROTL64(v3, 31) * BC6);
      return hash + 1;
}

void
FalconHash64(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t hash;
    int cnt = len >> 5;
    int i = len & 31;
    const uint64_t *chunk = (const uint64_t*)key;
    hash = (uint64_t)(seed + 1) * CA;
    seed = hash |= !hash * CB;
    while(cnt--) {
	uint64_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh64_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint64_t block[4] = {0, 0, 0, 0};
        if (i < 16)
            mem16cpy((uint32_t*)block, (const uint8_t*)chunk, i);
        else {
            block[0] = chunk[0];
            block[1] = chunk[1];
            mem16cpy((uint32_t*)(block + 2), (const uint8_t*)(chunk + 2), i - 16);
        }
        hash = fh64_step(hash ^ i, block);
    }
    hash ^= seed;
    hash = (hash ^ (hash >> 33)) * BC5;
    hash = (hash ^ (hash >> 33)) * BC6;
    *(uint64_t*)out = hash;
}

void
FalconHash64Sec(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t hash;
    int cnt = len >> 5;
    int i = len & 31;
    const uint64_t *chunk = (const uint64_t*)key;
    hash = (uint64_t)(seed + 1) * CA;
    seed = hash |= !hash * CB;
    while(cnt--) {
	uint64_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh64s_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint64_t block[4] = {0, 0, 0, 0};
        if (i < 16)
            mem16cpy((uint32_t*)block, (const uint8_t*)chunk, i);
        else {
            block[0] = chunk[0];
            block[1] = chunk[1];
            mem16cpy((uint32_t*)(block + 2), (const uint8_t*)(chunk + 2), i - 16);
        }
        hash = fh64s_step(hash ^ i, block);
    }
    hash ^= seed;
    hash = (hash ^ (hash >> 33)) * BC5;
    hash = (hash ^ (hash >> 33)) * BC6;
    *(uint64_t*)out = hash;
}

void
FalconHashF(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[2] = {0, 0};
    FalconHash64(key, len, seed, hash);
    *(uint32_t*)out = hash[0];
}

void
FalconHashG(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t hash[2] = {0, 0};
    FalconHash64(key, len, seed, hash);
    *(uint32_t*)out = hash[1];
}

typedef struct u128 {
    uint64_t h1, h2;
} u128_t;

static inline u128_t
fh128_step(u128_t hash, const uint64_t block[4])
{
      uint64_t v1, v2, v3, v4;
      v1 = ((hash.h1 ^ block[0]) * BC1) ^ ROTL64(block[1], 29);
      v2 = (block[2] * BC2) ^ ROTL64(block[3], 33);
      v3 = ((hash.h2 ^ block[3]) * BC3) ^ ROTL64(block[2], 29);
      v4 = (block[1] * BC4) ^ ROTL64(block[0], 33);
      v1 = (v1 * BC4) ^ (v2 * BC3);
      v3 = (v3 * BC1) + (v4 * BC2);
      hash.h1 = v1 + (ROTL64(v3, 31) * BC6);
      hash.h2 = v3 + (ROTL64(v1, 29) * BC5);
      return hash;
}

void
FalconHash128(const void *key, int len, uint32_t seed, void * out)
{
    u128_t hash, fin;
    int cnt = len >> 5;
    int i = len & 31;
    const uint64_t *chunk = (const uint64_t*)key;
    hash.h1 = ((uint64_t)seed - 1) * CA;
    hash.h2 = ((uint64_t)seed + 1) * CA;
    seed = hash.h1 + !hash.h1 * CB;
    while(cnt--) {
	uint64_t block[4] = { chunk[0], chunk[1], chunk[2], chunk[3] };
        hash = fh128_step(hash, block);
	chunk += 4;
    }
    if (i) {
	uint64_t block[4] = {0, 0, 0, 0};
        if (i < 16)
            mem16cpy((uint32_t*)block, (const uint8_t*)chunk, i);
        else {
            block[0] = chunk[0];
            block[1] = chunk[1];
            mem16cpy((uint32_t*)(block + 2), (const uint8_t*)(chunk + 2), i - 16);
        }
        hash.h1 ^= i; hash.h2 ^= i;
        hash = fh128_step(hash, block);
    }
    hash.h1 -= seed;
    hash.h2 += seed;
    fin.h1 = (hash.h1 ^ (hash.h2 >> 33)) * BC5;
    fin.h2 = (hash.h2 ^ (hash.h1 >> 29)) * BC6;
    hash.h1 = (fin.h1 ^ (fin.h2 >> 29)) * BC5;
    hash.h2 = (fin.h2 ^ (fin.h1 >> 33)) * BC6;
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
