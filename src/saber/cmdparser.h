/*
Copyright (c) 2016 Lee Thomason, Grinning Lizard Software

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

#include "../Grinliz_Arduino_Util/Grinliz_Arduino_Util.h"

class SaberDB;

class CMDParser 
{
  public:
    CMDParser(SaberDB* database);

    void push(int c)                { token.append(c); }
    bool processCMD(uint8_t* color);
    const char* getBuffer() const   { return token.c_str(); }
    void clearBuffer() { token.clear(); }

  private:
    void tokenize();
    void printHexColor(const uint8_t* color);
    void parseHexColor(const char* str, uint8_t* c);
    void printLead(const char* str);
  
    SaberDB* database;
    CStr<18> token;
    CStr<6>  action;
    CStr<10> value;
};


#endif // SABER_CMD_PARSER
