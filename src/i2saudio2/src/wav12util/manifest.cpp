/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "manifest.h"
#include <memory.h>
#include <assert.h>

#define TEST(x) { if (!(x)) { assert(false); return false; }}

uint32_t MemUnit::nameHash(uint32_t h) const
{
    for (int i = 0; i < NAME_LEN; ++i) {
        // Simple form of the hash:
        // h = h * 33 ^ (*v);
        // But M0+ doesn't have 64 intermediate multiply. So use shifts:
        h = ((h << 5) + h) ^ name[i];
    }
    return h;
}

bool MemUnit::nameMatch(const char* n) const
{
    if (n == 0 || *n == 0 || *name == 0)
        return false;

    const char* q = name;
    const char* p = n;
    for (; *p && *q && q < (name + NAME_LEN); ++p, ++q) {
        if (*p != *q)
            return false;
    }
    if (q < name + NAME_LEN)
        return *q == 0 && *p == 0;  // both null terminated
    return *p == 0; // if all characters read, just n needs to be null terminated
}


Manifest::Manifest()
{
    memset(image.unit, 0, MemImage::SIZE_MEMUNITS);
}

const MemUnit& Manifest::getUnit(int id) const
{
    if (id < 0) id = 0;
    if (id >= MemImage::NUM_MEMUNITS) id = MemImage::NUM_MEMUNITS - 1;
    return image.unit[id];
}

int Manifest::find(const char* name, int start, int n) const
{
    for(int i=0; i<n; ++i) {
        if (image.unit[i + start].nameMatch(name)) {
            return i + start;
        }
    }
    return -1;
}

int Manifest::getDir(const char* name) const
{
    int dir = find(name, 0, MemImage::NUM_DIR);
    return dir;
}

void Manifest::dirRange(int dir, int* start, int* count) const
{
    if (dir < 0) dir = 0;
    if (dir >= MemImage::NUM_DIR) dir = MemImage::NUM_DIR - 1;

    *start = image.unit[dir].offset;   // gah. ugly. need to fix in the generator.
    *count = image.unit[dir].size;
}

int Manifest::getFile(int dir, const char* fname) const
{
    if (dir < 0 || dir >= MemImage::NUM_DIR)
        return -1;

    int start = 0;
    int count = 0;
    dirRange(dir, &start, &count);

    return find(fname, start, count);
}


bool Manifest::Test()
{
    {
        MemUnit mu;
        memset(&mu, 0, sizeof(mu));

        memcpy(mu.name, "foo", 3);
        TEST(mu.nameMatch("foo"));
        TEST(!mu.nameMatch("fo"));
        TEST(!mu.nameMatch("fooo"));

        memcpy(mu.name, "12345678", 8);
        TEST(mu.nameMatch("12345678"));
        TEST(!mu.nameMatch("12345678a"));
        TEST(!mu.nameMatch("1234567"));
    }

    return true;
}
