#ifndef SABER_CMD_PARSER
#define SABER_CMD_PARSER

#include <stdint.h>

class SaberDB;

class CMDParser 
{
  public:
    CMDParser(SaberDB* database);

    void push(int c);
    bool processCMD(uint8_t* color);
    const char* getBuffer() { return token; }
    void clearBuffer() { token[0] = 0; }

  private:
    void tokenize(char** action, char** value);
    void printHexColor(const uint8_t* color);
    void parseHexColor(const char* str, uint8_t* c);
    void printLead(const char* str);
  
    SaberDB* database;
    static const int TOKEN_LEN = 20;
    char token[TOKEN_LEN];
};


#endif // SABER_CMD_PARSER
