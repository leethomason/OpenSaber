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
