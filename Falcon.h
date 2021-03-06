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

#ifndef _FALCON_H_
#define _FALCON_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void FalconHash64_x86(const void *key, int len, void *seed, void *out);
void FalconHash64(const void *key, int len, uint32_t seed, void * out);
void FalconHash64_32_fast(const void *key, int len, uint32_t seed, void * out);
void FalconHash64_32_sec(const void *key, int len, uint32_t seed, void * out);
void FalconHash128_x64(const void *key, int len, void *seed, void * out);
void FalconHash128(const void *key, int len, uint32_t seed, void * out);
void FalconHash128_64_fast(const void *key, int len, uint32_t seed, void * out);
void FalconHash128_64_sec(const void *key, int len, uint32_t seed, void * out);
void FalconHash128_32_fast(const void *key, int len, uint32_t seed, void * out);
void FalconHash128_32_sec(const void *key, int len, uint32_t seed, void * out);

#ifdef __cplusplus
}
#endif
#endif
