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

typedef struct u128 {
    uint32_t h1, h2, h3, h4;
} u128_t;

#define r16(a) ROTL32((a), 16)
#define r15(a) ROTL32((a), 15)
#define r7(a) ROTL32((a), 7)
#define r8(a) ROTL32((a), 8)
#define r9(a) ROTL32((a), 9)
#define r12(a) ROTL32((a), 12)
#define r11(a) ROTL32((a), 11)
#define r24(a) ROTL32((a), 24)
#define x2m(a, b, m) (((a)^(b))*(m))
#define x2r16m(a, b, m) (r16((a)^(b))*(m))
#define r16x2m(a, b, m) ((r16(a)^(b))*(m))
#define x2p(a, b, c) (((a)^(b))+(c))
#define x3m(a, b, c, m) (((a)^(b)^(c))*(m))

static inline u128_t
d64_step16(u128_t h, const uint32_t block[4])
{
    u128_t t, k;
    t.h1 = x2m(block[0], h.h1, C1)+x2r16m(h.h4, block[1], C5);
    t.h2 = x2m(block[1], h.h2, C2)+x2r16m(h.h1, block[2], C6);
    t.h3 = x2m(block[2], h.h3, C3)+x2r16m(h.h2, block[3], C7);
    t.h4 = x2m(block[3], h.h4, C4)+x2r16m(h.h3, block[0], C8);
    k.h1 = (t.h1^r15(t.h3))+t.h2;
    k.h2 = (t.h2^r15(t.h4))+t.h3;
    k.h3 = (t.h3^r16(t.h1))+t.h4;
    k.h4 = (t.h4^r16(t.h2))+t.h1;
    return k;
}

static inline u128_t
d64_step8(u128_t h, const uint32_t block[2])
{
    u128_t t;
    t.h1 = x2m(block[0], h.h1, C1)+x2r16m(h.h4, block[0], C5);
    t.h2 = x2m(block[1], h.h2, C2)+x2r16m(h.h1, block[1], C6);
    t.h3 = x2m(block[0], h.h3, C3)+x2r16m(h.h2, block[1], C7);
    t.h4 = x2m(block[1], h.h4, C4)+x2r16m(h.h3, block[0], C8);
    return t;
}

static inline u128_t
d64_step4(u128_t h, const uint32_t block[1])
{
    u128_t t;
    t.h1 = x2m(block[0], h.h1, C1);
    t.h2 = x2m(r16(block[0]), h.h2, C2);
    t.h3 = x2m(r8(block[0]), h.h3, C3);
    t.h4 = x2m(r24(block[0]), h.h4, C4);
    return t;
}

static inline u128_t
_Dragon64_x32(u128_t hash, const void *key, int len)
{
    int cnt = len / 16;
    int i = len % 16;
    const uint32_t *chunk = (const uint32_t*)key;
    uint32_t block[4] = {0, 0, 0, 0};
    u128_t t;
    if (len == 0) {
        block[0] = C1;
	return d64_step4(hash, block);
    }
    while(cnt--) {
        hash.h1 += 16;
        hash = d64_step16(hash, chunk);
	chunk += 4;
    }
    hash.h1 -= i; hash.h2 += i;
    switch (i) {
    case 0: case 1: case 2: case 3: case 4:
        block[0] = mem4cpy(chunk, i);
        hash = d64_step4(hash, block);
        break;
    case 5: case 6: case 7: case 8:
        mem16cpy(block, chunk, i);
        hash = d64_step8(hash, block);
        break;
    case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
        mem16cpy(block, chunk, i);
        hash = d64_step16(hash, block);
        break;
    }
    t.h1 = x2m(r12(hash.h1), r11(hash.h2), C9);
    t.h2 = x2m(r12(hash.h2), r11(hash.h3), CA);
    t.h3 = x2m(r12(hash.h3), r11(hash.h4), CB);
    t.h4 = x2m(r12(hash.h4), r11(hash.h1), CC);
    return t;
}

void
Dragon64_x32(const void *key, int len, void *seed, void *out)
{
    u128_t hash = {
        ((uint32_t*)seed)[0],
        ((uint32_t*)seed)[1],
        ((uint32_t*)seed)[2],
        ((uint32_t*)seed)[3],
    };
    uint32_t r1, r2;
    hash = _Dragon64_x32(hash, key, len);
    r1 = x2m(r12(hash.h1), r11(hash.h3), CD) ^
         x2m(r11(hash.h1), r12(hash.h3), CF);
    r2 = x2m(r12(hash.h2), r11(hash.h4), CD) ^
         x2m(r11(hash.h2), r12(hash.h4), CF);
    *(uint32_t*)out = r1;
    *((uint32_t*)out+1) = r2;
}

void
Dragon32A_x32(const void *key, int len, void *seed, void *out)
{
    u128_t hash = {
        ((uint32_t*)seed)[0],
        ((uint32_t*)seed)[1],
        ((uint32_t*)seed)[2],
        ((uint32_t*)seed)[3],
    };
    uint32_t r1, r2;
    hash = _Dragon64_x32(hash, key, len);
    r1 = x2m(r12(hash.h1), r11(hash.h3), CD) ^
         x2m(r11(hash.h1), r12(hash.h3), CF);
    *(uint32_t*)out = r1;
}

void
Dragon32B_x32(const void *key, int len, void *seed, void *out)
{
    u128_t hash = {
        ((uint32_t*)seed)[0],
        ((uint32_t*)seed)[1],
        ((uint32_t*)seed)[2],
        ((uint32_t*)seed)[3],
    };
    uint32_t r1, r2;
    hash = _Dragon64_x32(hash, key, len);
    r1 = x2m(r12(hash.h2), r11(hash.h4), CD) ^
         x2m(r11(hash.h2), r12(hash.h4), CF);
    *(uint32_t*)out = r1;
}

void
Dragon64(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    //uint32_t seedx[4] = {seed+C1, seed+C2, seed+C3, seed+C4};
    Dragon64_x32(key, len, seedx, out);
}

void
Dragon64_32_fast(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t seedx[4] = {seed+C1, seed+C2, seed+C3, seed+C4};
    Dragon32A_x32(key, len, seedx, out);
}

void
Dragon64_32_sec(const void *key, int len, uint32_t seed, void * out)
{
    uint32_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    Dragon32B_x32(key, len, seedx, out);
}

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
#define R15(a) ROTL64((a), 15)
#define R16(a) ROTL64((a), 16)
#define R17(a) ROTL64((a), 17)
#define R24(a) ROTL64((a), 24)
#define R23(a) ROTL64((a), 23)
#define R48(a) ROTL64((a), 48)
#define X2M(a, b, m) (((a)^(b))*(m))
#define X2R32M(a, b, m) (R32((a)^(b))*(m))
#define R32X2M(a, b, m) ((R32(a)^(b))*(m))
#define X2P(a, b, c) (((a)^(b))+(c))
#define X3M(a, b, c, m) (((a)^(b)^(c))*(m))


static inline u256_t
d128_step32(u256_t h, const uint64_t block[4])
{
    u256_t t, k;
    t.h1 = X2M(block[0], h.h1, BC1)+X2R32M(h.h4, block[1], BC5);
    t.h2 = X2M(block[1], h.h2, BC2)+X2R32M(h.h1, block[2], BC6);
    t.h3 = X2M(block[2], h.h3, BC3)+X2R32M(h.h2, block[3], BC7);
    t.h4 = X2M(block[3], h.h4, BC4)+X2R32M(h.h3, block[0], BC8);
    k.h1 = X2P(t.h1, R31(t.h3), t.h2);
    k.h2 = X2P(t.h2, R31(t.h4), t.h3);
    k.h3 = X2P(t.h3, R32(t.h1), t.h4);
    k.h4 = X2P(t.h4, R32(t.h2), t.h1);
    return k;
}

static inline u256_t
d128_step16(u256_t h, const uint64_t block[2])
{
    u256_t t;
    t.h1 = X2M(block[0], h.h1, BC1)+X2R32M(h.h4, block[0], BC5);
    t.h2 = X2M(block[1], h.h2, BC2)+X2R32M(h.h1, block[1], BC6);
    t.h3 = X2M(block[0], h.h3, BC3)+X2R32M(h.h2, block[1], BC7);
    t.h4 = X2M(block[1], h.h4, BC4)+X2R32M(h.h3, block[0], BC8);
    return t;
}

static inline u256_t
d128_step8(u256_t h, const uint64_t block[2])
{
    u256_t t;
    t.h1 = X2M(block[0], h.h1, BC1);
    t.h2 = X2M(R32(block[0]), h.h2, BC2);
    t.h3 = X2M(R16(block[0]), h.h3, BC3);
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
    u256_t t, k;
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
    t.h1 = X2M(R24(hash.h1), R23(hash.h2), BC9);
    t.h2 = X2M(R24(hash.h2), R23(hash.h3), BCA);
    t.h3 = X2M(R24(hash.h3), R23(hash.h4), BCB);
    t.h4 = X2M(R24(hash.h4), R23(hash.h1), BCC);
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
    r1 = X2M(R23(hash.h1), R24(hash.h3), BCD) ^
         X2M(R24(hash.h1), R23(hash.h3), BCF);
    r2 = X2M(R23(hash.h2), R24(hash.h4), BCD) ^
         X2M(R24(hash.h2), R23(hash.h4), BCF);
    *(uint64_t*)out = r1;
    *((uint64_t*)out+1) = r2;
}

void
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
    r1 = X2M(R23(hash.h1), R24(hash.h3), BCD) ^
         X2M(R24(hash.h1), R23(hash.h3), BCF);
    *(uint64_t*)out = r1;
}

void
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
    r1 = X2M(R23(hash.h2), R24(hash.h4), BCD) ^
         X2M(R24(hash.h2), R23(hash.h4), BCF);
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
    uint64_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    Dragon64A_x64(key, len, seedx, out);
}

void
Dragon128_64_sec(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    Dragon64B_x64(key, len, seedx, out);
}

void
Dragon128_32_fast(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed+1, seed+2, seed+3, seed+4};
    uint32_t hash[2];
    Dragon64A_x64(key, len, seedx, &hash);
    *(uint32_t*)out = hash[1];
}

void
Dragon128_32_sec(const void *key, int len, uint32_t seed, void * out)
{
    uint64_t seedx[4] = {seed, 0, 0, 1};
    uint32_t hash[2];
    Dragon64B_x64(key, len, seedx, &hash);
    *(uint32_t*)out = hash[1];
}
