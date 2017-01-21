#include "Grinliz_Util.h"
#include <math.h>

static int16_t gSinTable[256] = { 0 };

int16_t isin(uint16_t x)
{
	if (gSinTable[64] == 0) {
		for (int i = 0; i < 256; ++i) {
			float x = 2 * 3.14159f * float(i) / 256.0f;
			gSinTable[i] = int16_t(sin(x) * 256.0f);
		}
	}
	return gSinTable[x & 0xff];
}


bool strStarts(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return false;

    while(*prefix) {
        if (*prefix++ != *str++)
            return false;
    }
    return true;
}

bool TestCStr()
{
    TEST_IS_TRUE(strStarts("FooBar", "Foo"));
    TEST_IS_FALSE(strStarts("FooBar", "Bar"));
    TEST_IS_FALSE(strStarts("Foo", "FooBar"));
    TEST_IS_FALSE(strStarts(0, "Foo"));
    TEST_IS_FALSE(strStarts("Foo", 0));

    static const char* STR4 = "Four";
    static const char* STR8 = "ThisIs-8";
    static const char* STR9 = "ThisIs-8+";

    {
        CStr<9> s;
        TEST_IS_TRUE(s.empty());
        TEST_IS_TRUE(s == "");

        s = STR4;
        TEST_IS_TRUE(s[4] == 0);
        TEST_IS_TRUE(s.size() == 4);
        TEST_IS_FALSE(s.empty());
        TEST_IS_EQ(s.capacity(), 8);
        s.append("F");
        TEST_IS_TRUE(s == "FourF");
        s.append("ourF");
        TEST_IS_TRUE(s.size() == 8);
        TEST_IS_TRUE(s == "FourFour");
        TEST_IS_FALSE(s != "FourFour");
        TEST_IS_TRUE(s[8] == 0);
        TEST_IS_TRUE(s[0] == 'F');

        CStr<9> s2 = s;
        TEST_IS_TRUE(s == s2.c_str());
        s.clear();
        TEST_IS_TRUE(s.empty());
        TEST_IS_TRUE(s != s2.c_str());
    }
    {
        CStr<9> s = STR9;
        TEST_IS_TRUE(s == STR8);
        s += "TooLooong";
        TEST_IS_TRUE(s == STR8);
    }
    {
        CStr<9> s;
        s += STR4;
        TEST_IS_TRUE(s == STR4);
    }
    {
        CStr<5> s;
        s.append('F');
        s.append("our");
        TEST_IS_TRUE(s == "Four");
    }
    return true;
}


int hexToDec(char h) 
{
    if (h >= '0' && h <= '9') return h - '0';
    if (h >= 'a' && h <= 'f') return 10 + h - 'a';
    if (h >= 'A' && h <= 'F') return 10 + h - 'A';
    return 0;
}

char decToHex(int v) 
{
    if (v >= 0 && v < 10) return '0' + v;
    if (v>= 10 && v < 16) return 'a' + v - 10;
    return '0';
}

bool TestHexDec()
{
    TEST_IS_TRUE(hexToDec('A') == 10);
    TEST_IS_TRUE(hexToDec('F') == 15);
    TEST_IS_TRUE(hexToDec('a') == 10);
    TEST_IS_TRUE(hexToDec('f') == 15);
    TEST_IS_TRUE(hexToDec('0') == 0);
    TEST_IS_TRUE(hexToDec('9') == 9);
    TEST_IS_TRUE(hexToDec('G') == 0);
    TEST_IS_TRUE(hexToDec('#') == 0);

    TEST_IS_TRUE(decToHex(0) == '0');
    TEST_IS_TRUE(decToHex(9) == '9');
    TEST_IS_TRUE(decToHex(10) == 'a');
    TEST_IS_TRUE(decToHex(15) == 'f');
    TEST_IS_TRUE(decToHex(-1) == '0');
    TEST_IS_TRUE(decToHex(16) == '0');

    return true;
}


void parseHex(const CStr<7>& str, uint8_t* c)
{
    for (int i = 0; i < 3; ++i) {
        c[i] = hexToDec(str[i*2+0]) * 16 + hexToDec(str[i*2+1]);
    }
}

void parseHex(const CStr<4>& str, uint8_t* c)
{
    for (int i = 0; i < 3; ++i) {
        int h = hexToDec(str[i]);
        c[i] = h * 16 + h;
    }
}

void writeHex(const uint8_t* c, CStr<7>* str) 
{
    str->clear();
    for(int i=0; i<3; ++i) {
        str->append(decToHex(c[i] / 16));
        str->append(decToHex(c[i] & 15));
    }
}

bool TestHex()
{
    CStr<7> inLong("a1b2c3");
    CStr<4> inShort("abc");
    CStr<7> out;

    uint8_t rgb[3];

    parseHex(inLong, rgb);
    TEST_IS_TRUE(rgb[0] == 0xa1);    
    TEST_IS_TRUE(rgb[1] == 0xb2);    
    TEST_IS_TRUE(rgb[2] == 0xc3);

    parseHex(inShort, rgb);
    TEST_IS_TRUE(rgb[0] == 0xaa);    
    TEST_IS_TRUE(rgb[1] == 0xbb);    
    TEST_IS_TRUE(rgb[2] == 0xcc);    

    writeHex(rgb, &out);
    TEST_IS_TRUE(out == "aabbcc");
    
    return true;
}
