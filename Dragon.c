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

#include "Dragon.h"
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


static inline uint32_t
_mem4cpy(const uint8_t *buf, int i)
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
    return 0;
}

static inline uint32_t
mem4cpy(const void *buf, int i)
{
    switch(i) {
        case 4: return _mem4cpy(buf, 4);
        case 3: return _mem4cpy(buf, 3);
        case 2: return _mem4cpy(buf, 2);
        case 1: return _mem4cpy(buf, 1);
        case 0: return 0;
    }
    return 0;
}

static inline void
mem16cpy(uint32_t *block, const void *buf_, int i)
{
    const uint8_t *buf = buf_;
    switch(i) {
        case 16:
            block[3] = _mem4cpy(buf + 12, 4);
            goto block2;
        case 15: block[3] = _mem4cpy(buf + 12, 3); goto block2;
        case 14: block[3] = _mem4cpy(buf + 12, 2); goto block2;
        case 13: block[3] = _mem4cpy(buf + 12, 1); goto block2;
        case 12:
block2:
            block[2] = _mem4cpy(buf + 8, 4);
            goto block1;
        case 11: block[2] = _mem4cpy(buf + 8, 3); goto block1;
        case 10: block[2] = _mem4cpy(buf + 8, 2); goto block1;
        case 9:  block[2] = _mem4cpy(buf + 8, 1); goto block1;
        case 8:
block1:
            block[1] = _mem4cpy(buf + 4, 4);
            goto block0;
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
_mem8cpy64(const uint8_t *buf, int i)
{
    uint64_t x = 0;
    switch(i) {
        case 8:
            return *(uint64_t*)buf;
        case 7: x |= (uint64_t)buf[6] << 48;
        case 6: x |= (uint64_t)buf[5] << 40;
        case 5: x |= (uint64_t)buf[4] << 32;
        case 4:
            return x | *(uint32_t*)buf;
        case 3: x |= buf[2] << 16;
        case 2: x |= buf[1] << 8;
        case 1: x |= buf[0];
        case 0:
            return x;
    }
    return 0;
}

static inline uint64_t
mem8cpy64(const uint8_t *buf, int i)
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
    return 0;
}

static inline void
mem16cpy64(uint64_t *block, const uint8_t *buf, int i)
{
    switch(i) {
        case 16:
            block[1] = _mem8cpy64(buf+8, 8);
            goto block0;
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

static inline void
mem32cpy64(uint64_t *block, const uint8_t *buf, int i)
{
    if (i > 16) {
        mem16cpy64(block, buf, 16);
        mem16cpy64(block+2, buf+16, i-16);
    } else {
        mem16cpy64(block, buf, i);
    }
}
/*
static const uint32_t C1 = 0x939b9659;
static const uint32_t C2 = 0xab1e9755;
static const uint32_t C3 = 0xb8b34b2b;
static const uint32_t C4 = 0xa1e38b95;
static const uint32_t C5 = 0xcc9e2d63;
static const uint32_t C6 = 0x85ebcacb;
static const uint32_t C7 = 0xd56153af;
static const uint32_t C8 = 0x942524b1;
static const uint32_t C9 = 0xa7a3b46d;
static const uint32_t CA = 0xe8293dbb;
static const uint32_t CB = 0x7294d1a7;
static const uint32_t CC = 0x8a5ed6b9;
static const uint32_t CD = 0xa9d1ae7d;
static const uint32_t CE = 0xbd658ba9;
static const uint32_t CF = 0x96d6c7a5;
static const uint32_t C0 = 0xb57246b7;

typedef struct u96 {
    uint32_t h1, h2, h3;
} u96_t;

static inline uint32_t
tta(uint32_t h, uint32_t b0, uint32_t c, uint32_t b1, uint32_t d)
{
    return ((h ^ b0) * c) + (ROTL32(b1, 16) * d);
}

static inline uint32_t
ttc(uint32_t h, uint32_t b0, uint32_t c, uint32_t h1, uint32_t b1, uint32_t d)
{
    return ((h ^ b0) * c) + ((h1 ^ ROTL32(b1, 16)) * d);
}

static inline uint32_t
ttb(uint32_t h0, uint32_t b0, uint32_t c, uint32_t h1, uint32_t b1, uint32_t d)
{
    return ((h0 ^ ROTL32(b0, 15)) * c) + ((h1 ^ ROTL32(b1, 16)) * d);
}

static inline uint32_t
ttd(uint32_t h0, uint32_t b0, uint32_t c0, uint32_t h1, uint32_t b1, uint32_t c1)
{
    return ((h0 ^ ROTL32(b0, 15)) * c0) ^ ((h1 ^ ROTL32(b1, 16)) * c1);
}

static inline uint32_t
tte(uint32_t k0, uint32_t h0, uint32_t b0, uint32_t c0, uint32_t h1, uint32_t b1, uint32_t c1)
{
    return ((k0 ^ ROTL32(h0, 5) ^ ROTL32(b0, 15)) * c0) ^ ((h1 ^ ROTL32(b1, 16)) * c1);
}

static inline uint32_t
ttf(uint32_t k0, uint32_t h0, uint32_t b0, uint32_t c0, uint32_t h1, uint32_t b1, uint32_t c1)
{
    return ((k0 ^ ROTL32(h0, 2) ^ ROTL32(b0, 15)) * c0) ^ ((h1 ^ ROTL32(b1, 16)) * c1);
}

static inline u96_t
fh_step32(u96_t hash, const uint32_t block[8])
{
      uint32_t va, vb, vc, vd;
      uint32_t v1, v2, v3, v4;
      va = tta(hash.h3, block[0], C1, block[1], C5);
      vb = tta(hash.h3, block[1], C2, block[2], C6);
      vc = tta(hash.h2, block[2], C3, block[3], C7);
      vd = tta(hash.h2, block[3], C4, block[4], C8);
      v1 = ttb(block[4], hash.h1 ^ va, C9, vd, block[5], CD);
      v2 = ttb(block[5], vb, CA, va, block[6], CE);
      v3 = ttb(block[6], vc, CB, vb, block[7], CF);
      v4 = ttb(block[7], hash.h1 ^ vd, CC, vc, block[0], C0);
      hash.h3 = ttd(v3, v1, C6, hash.h3 ^ v4, v2, C7);
      hash.h3 = ROTL32(hash.h3, 8);
      hash.h2 = tte(hash.h3, v2, v3, CC, hash.h2 ^ v1, v4, C0);
      return hash;
}

static inline u96_t
fh_step16(u96_t hash, const uint32_t block[4])
{
      uint32_t v1, v2, v3, v4;
      v1 = tta(hash.h1, block[0], C1, block[1], C5);
      v2 = tta(hash.h1, block[1], C2, block[2], C6);
      v3 = tta(hash.h2, block[2], C3, block[3], C7);
      v4 = tta(hash.h2, block[3], C4, block[0], C8);
      hash.h3 = ttd(v3, v1, C6, hash.h3 ^ v4, v2, C7);
      hash.h3 = ROTL32(hash.h3, 8) * CD;
      hash.h2 = tte(hash.h3, v2, v3, C4, hash.h2 ^ v1, v4, C8);
      return hash;
}

static inline u96_t
fh_step8(u96_t hash, const uint32_t block[2])
{
      uint32_t v1, v2;
      v1 = tta(hash.h1, block[0], C1, block[1], C3);
      v2 = tta(hash.h2, block[1], C2, block[0], C4);
      hash.h3 = ttd(hash.h3 ^ v1, v2, C7, v2, v1, C8);
      hash.h3 = ROTL32(hash.h3, 8) * CD;
      hash.h2 = ttd(hash.h2, v1, C1, hash.h3, v2, C3);
      return hash;
}

static inline u96_t
fh_step4(u96_t hash, const uint32_t block[1])
{
      hash.h3 = ttc(hash.h1, block[0], C1, hash.h3, block[0], C3);
      hash.h3 = ROTL32(hash.h3, 8) * CD;
      hash.h2 = ttd(hash.h2, block[0], C2, block[0], hash.h3, C4);
      return hash;
}

inline void
FalconHash64_x86(const void *key, int len, void *seed, void *out)
{
    u96_t hash = {
        ((uint32_t*)seed)[0],
        ((uint32_t*)seed)[1],
        ((uint32_t*)seed)[2]
    };
    int cnt = len / 32;
    int i = len % 32;
    const uint32_t *chunk = (const uint32_t*)key;
    uint32_t block[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t r1, r2, t1, t2, t3;
    if (len == 0) {
        i = sizeof(block[0]);
        block[0] = C1;
        goto zero;
    }
    while(cnt--) {
        hash.h1 += 32;
        hash = fh_step32(hash, chunk);
        hash.h1 ^= ttd(hash.h2, hash.h3, C5, hash.h3, hash.h2, C6);
	chunk += 8;
    }
    hash.h1 += len; hash.h2 -= len;
    switch (i) {
    case 0: case 1: case 2: case 3: case 4:
        block[0] = mem4cpy(chunk, i);
zero:
        hash = fh_step4(hash, block);
        break;
    case 5: case 6: case 7: case 8:
        mem16cpy(block, chunk, i);
        hash = fh_step8(hash, block);
        break;
    case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
        mem16cpy(block, chunk, i);
        hash = fh_step16(hash, block);
        break;
    case 17: case 18: case 19: case 20: case 21:
    case 22: case 23: case 24: case 25: case 26:
    case 27: case 28: case 29: case 30: case 31:
        mem16cpy(block, chunk, 16);
        mem16cpy(block+4, chunk+4, i-16);
        hash = fh_step32(hash, block);
        break;
    }
    t1 = ROTL32(hash.h1, 15) * C8;
    r1 = ttf(t1, hash.h2, hash.h3, CA, hash.h3, hash.h2, CB);
    t2 = ROTL32(hash.h2, 15) * CC;
    t3 = ROTL32(hash.h3, 16) * C0;
    r2 = ttf(t2, hash.h1, t3, CE, t3, t2, CF);
    *(uint32_t*)out = r1;
    *((uint32_t*)out+1) = r2;
}

void
FalconHash64(const void *key, int len, uint32_t seed, void * out)
{
    //uint32_t seedx[3] = {0, 0, seed};
    uint32_t seedx[3] = {seed+C0, seed-C1, seed+C2};
    FalconHash64_x86(key, len, seedx, out);
}

void
FalconHash64_32_fast(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t seedx[3] = {seed, seed+C1, seed+C2};
    uint32_t hash[2];
    FalconHash64_x86(key, len, seedx, &hash);
    *(uint32_t*)out = hash[0];
}

void
FalconHash64_32_sec(const void *key, int len, uint32_t seed, void * out)
{
    //uint32_t seedx[3] = {0, 0, seed};
    uint32_t seedx[3] = {seed, seed-C1, seed+C2};
    uint32_t hash[2];
    FalconHash64_x86(key, len, seedx, &hash);
    *(uint32_t*)out = hash[1];
}
*/

static const uint64_t BC1 = BIG_CONSTANT(0x87c37b916942566d);
static const uint64_t BC2 = BIG_CONSTANT(0xacf5ad43274593b9);
static const uint64_t BC3 = BIG_CONSTANT(0xa951abd7ed558e35);
static const uint64_t BC4 = BIG_CONSTANT(0xc673b96e1a85ed39);
static const uint64_t BC5 = BIG_CONSTANT(0xc3a5c85c97cb3199);
static const uint64_t BC6 = BIG_CONSTANT(0xb492b66f5e98f273);
static const uint64_t BC7 = BIG_CONSTANT(0x9ae16a3b2d853d67);
static const uint64_t BC8 = BIG_CONSTANT(0xc9d64a2ae57b2457);
static const uint64_t BC9 = BIG_CONSTANT(0x6789ab5de12356bd);
static const uint64_t BCA = BIG_CONSTANT(0x7ab98cd21ef34ac5);
static const uint64_t BCB = BIG_CONSTANT(0x939b961b3a3ef3df);
static const uint64_t BCC = BIG_CONSTANT(0x7294d1ab85f24c45);
static const uint64_t BCD = BIG_CONSTANT(0x96f6a7a1d23a526d);
static const uint64_t BCE = BIG_CONSTANT(0xbd658ba973d19b47);
static const uint64_t BCF = BIG_CONSTANT(0x8a5fd6b32d5a3a17);
static const uint64_t BC0 = BIG_CONSTANT(0xb57246b7493543bb);

typedef struct u256 {
    uint64_t h1, h2, h3, h4;
} u256_t;

#define R32(a) ROTL64((a), 32)
#define R31(a) ROTL64((a), 31)
#define R29(a) ROTL64((a), 29)
#define R24(a) ROTL64((a), 24)
#define R23(a) ROTL64((a), 23)
#define R16(a) ROTL64((a), 16)
#define R48(a) ROTL64((a), 48)
#define R1(a) ROTL64((a), 1)
#define R8(a) ROTL64((a), 8)
#define R56(a) ROTL64((a), 56)
#define X2M(a, b, m) (((a)^(b))*m)
#define X2R32M(a, b, m) (R32((a)^(b))*m)
#define R32X2M(a, b, m) ((R32(a)^(b))*m)
#define X3M(a, b, c, m) (((a)^(b)^(c))*m)
#define X2P(a, b, m) (((a)^(b))+(m))


static inline u256_t
d128_step32(u256_t h, const uint64_t block[4])
{
    u256_t t;
    t.h1 = X2M(block[0], h.h1, BC1)+X2R32M(h.h4, block[1], BC5);
    t.h2 = X2M(block[1], h.h2, BC2)+X2R32M(h.h1, block[2], BC6);
    t.h3 = X2M(block[2], h.h3, BC3)+X2R32M(h.h2, block[3], BC7);
    t.h4 = X2M(block[3], h.h4, BC4)+X2R32M(h.h3, block[0], BC8);
    h.h1 = (t.h1^R31(t.h3))+t.h2;
    h.h2 = (t.h2^R31(t.h4))+t.h3;
    h.h3 = (t.h3^R32(t.h1))+t.h4;
    h.h4 = (t.h4^R32(t.h2))+t.h1;
    return h;
}

static inline u256_t
d128_step16(u256_t h, const uint64_t block[2])
{
    u256_t t;
    t.h1 = X2M(block[0], h.h1, BC1)+X2R32M(h.h4, block[0], BC5);
    t.h2 = X2M(block[0], h.h2, BC2)+X2R32M(h.h1, block[1], BC6);
    t.h3 = X2M(block[1], h.h3, BC3)+X2R32M(h.h2, block[1], BC7);
    t.h4 = X2M(block[1], h.h4, BC4)+X2R32M(h.h3, block[0], BC8);
    return t;
}

static inline u256_t
d128_step8(u256_t h, const uint64_t block[2])
{
    u256_t t;
    t.h1 = X2M(block[0], h.h1, BC1);
    t.h2 = X2M(R16(block[0]), h.h2, BC2);
    t.h3 = X2M(R32(block[0]), h.h3, BC3);
    t.h4 = X2M(R48(block[0]), h.h4, BC4);
    return t;
}

static inline u256_t
_Dragon128_x64(u256_t hash, const void *key, int len)
{
    int cnt = len / 32;
    int i = len % 32;
    const uint64_t *chunk = (const uint64_t*)key;
    uint64_t block[4] = {0, 0, 0, 0};
    u256_t t;
    if (len == 0) {
        block[0] = BC1;
	return d128_step8(hash, block);
    }
    while(cnt--) {
        hash.h1 += 32;
        hash = d128_step32(hash, chunk);
	chunk += 4;
    }
    hash.h1 -= i; hash.h2 += i;
    if (i >= 0 && i <= 8) {
        block[0] = mem8cpy64((const uint8_t*)chunk, i);
        hash = d128_step8(hash, block);
    } else if (i >= 9 && i <= 16) {
        mem16cpy64(block, (const uint8_t*)chunk, i);
        hash = d128_step16(hash, block);
    } else if (i >= 17 && i <= 32) {
        mem16cpy64(block, (const uint8_t*)chunk, 16);
        mem16cpy64(block+2, (const uint8_t*)(chunk + 2), i - 16);
        hash = d128_step32(hash, block);
    }
#if 0
    t.h1 = (R32(hash.h1) ^ R16(hash.h3)) * BC9;
    t.h2 = (R32(hash.h2) ^ R16(hash.h4)) * BCA;
    t.h3 = (R32(hash.h3) ^ R16(hash.h1)) * BCB;
    t.h4 = (R32(hash.h4) ^ R16(hash.h2)) * BCC;
#elif 0
    t.h1 = R24(X2P(hash.h1, hash.h2, R56(hash.h3))) * BC9;
    t.h2 = R24(X2P(hash.h2, hash.h3, R56(hash.h4))) * BCA;
    t.h3 = R24(X2P(hash.h3, hash.h4, R56(hash.h1))) * BCB;
    t.h4 = R24(X2P(hash.h4, hash.h1, R56(hash.h2))) * BCC;
#else
    t.h1 = R24(hash.h1 ^ hash.h2) * BC9;
    t.h2 = R24(hash.h2 ^ hash.h3) * BCA;
    t.h3 = R24(hash.h3 ^ hash.h4) * BCB;
    t.h4 = R24(hash.h4 ^ hash.h1) * BCC;
#endif
    return t;
}

void
Dragon128_x64(const void *key, int len, void *seed, void *out)
{
    u256_t hash = {
        ((uint64_t*)seed)[0],
        ((uint64_t*)seed)[1],
        ((uint64_t*)seed)[2],
        ((uint64_t*)seed)[3],
    };
    uint64_t r1, r2;
    hash = _Dragon128_x64(hash, key, len);
    r1 = X2M(R16(hash.h1), R24(hash.h2), BCD) ^ 
	 X2M(R24(hash.h3), R24(hash.h4), BCF);
    r2 = X2M(R24(hash.h2), R16(hash.h3), BCE) ^
       	 X2M(R24(hash.h1), R24(hash.h4), BC0);
    *(uint64_t*)out = r1;
    *((uint64_t*)out+1) = r2;
}

inline void
Dragon64A_x64(const void *key, int len, void *seed, void *out)
{
    u256_t hash = {
        ((uint64_t*)seed)[0],
        ((uint64_t*)seed)[1],
        ((uint64_t*)seed)[2],
        ((uint64_t*)seed)[3],
    };
    uint64_t r1, r2;
    hash = _Dragon128_x64(hash, key, len);
    r1 = R32(hash.h1 ^ hash.h2) * BCF;
    *(uint64_t*)out = r1;
}

inline void
Dragon64B_x64(const void *key, int len, void *seed, void *out)
{
    u256_t hash = {
        ((uint64_t*)seed)[0],
        ((uint64_t*)seed)[1],
        ((uint64_t*)seed)[2],
        ((uint64_t*)seed)[3],
    };
    uint64_t r1, r2;
    hash = _Dragon128_x64(hash, key, len);
    r1 = R32(hash.h3 ^ hash.h4) * BCF;
    *(uint64_t*)out = r1;
}

void
Dragon128(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    Dragon128_x64(key, len, seedx, out);
}

void
Dragon128_64_fast(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {0, 0, seed};
    Dragon64A_x64(key, len, seedx, out);
}

void
Dragon128_64_sec(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {0, 0, seed};
    Dragon64B_x64(key, len, seedx, out);
}

void
Dragon128_32_fast(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed, 0, 0};
    uint32_t hash[4];
    Dragon128_x64(key, len, seedx, &hash);
    *(uint32_t*)out = hash[1];
}

void
Dragon128_32_sec(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed, 0, 0};
    uint32_t hash[4];
    Dragon128_x64(key, len, seedx, &hash);
    *(uint32_t*)out = hash[3];
}
