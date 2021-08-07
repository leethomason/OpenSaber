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

Manifest::Manifest()
{
    memset(memUnit, 0, sizeof(MemUnit) * MEM_IMAGE_TOTAL);
}

void Manifest::scan(IMemory* flash)
{
    flash->readMemory(0, (uint8_t*) memUnit, sizeof(MemUnit) * MEM_IMAGE_TOTAL);
}

const MemUnit& Manifest::getUnit(int id) const
{
    id = glClamp(id, 0, MEM_IMAGE_TOTAL-1);
    return memUnit[id];
}

uint32_t Manifest::dirHash() const
{
    uint32_t h = 0;
    for(int i=0; i<MEM_IMAGE_NUM_DIR; ++i) {
        const char *start = (const char *)&memUnit[i];
        const char* end = start + MemUnit::NAME_LEN;
        h = hash32(start, end, h);
    }
    return h;
}

int Manifest::find(const char* name, int start, int n) const
{
    CStrBuf<MemUnit::NAME_LEN> strBuf = name;
    for(int i=0; i<n; ++i) {
        if (strBuf == memUnit[i+start].name) {
            return i + start;
        }
    }
    return -1;
}


int Manifest::getDir(const char* name) const
{
    int dir = find(name, 0, MEM_IMAGE_NUM_DIR);
    return dir;
}

void Manifest::dirRange(int dir, int* start, int* count) const
{
    dir = glClamp(dir, 0, MEM_IMAGE_NUM_DIR-1);
    *start = memUnit[dir].offset;   // gah. ugly. need to fix in the generator.
    *count = memUnit[dir].size;
}

int Manifest::getFile(int dir, const char* fname) const
{
    int start = 0;
    int count = 0;
    dirRange(dir, &start, &count);

    int file = find(fname, start, count);
    return file;
}

