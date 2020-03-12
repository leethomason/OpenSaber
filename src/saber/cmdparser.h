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

#ifndef SABER_CMD_PARSER
#define SABER_CMD_PARSER

#include <stdint.h>
#include "Grinliz_Arduino_Util.h"
#include "DotStar.h"

class SaberDB;
class Manifest;

class CMDParser
{
public:
    CMDParser(SaberDB* database, const Manifest& manifest);

    bool push(int c);
   
    const char* getBuffer() const     {
        return token.c_str();
    }
    
    void clearBuffer()                {
        token.clear();
    }

private:
    bool processCMD();
    void tokenize();
    void printHexColor(const osbr::RGB& color);
    void parseHexColor(const char* str, osbr::RGB* c);
    void printLead(const char* str);
    void printMAmps(const osbr::RGB& color);
    void printXYZ(const Vec3<int32_t>& v) const;
    void printXYZ(const Vec3<float>& v) const;

    SaberDB* database = 0;
    const Manifest& manifest;

    static const int ALLOCATE = 30;
    CStr<ALLOCATE> token;
    CStr<ALLOCATE> action;
    CStr<ALLOCATE> value;
    CStr<ALLOCATE> value2;
};


#endif // SABER_CMD_PARSER
