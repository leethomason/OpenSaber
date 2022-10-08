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

#include "Grinliz_Util.h"
#include <math.h>

//#define DEBUG_EVENT

bool TestUtil()
{
    //clamp()
    TEST_IS_EQ(glClamp(-10, 0, 100), 0);
    TEST_IS_EQ(glClamp(10,  0, 100), 10);
    TEST_IS_EQ(glClamp(110, 0, 100), 100);

    // lerp255()
	TEST_IS_EQ(lerp255(0, 128, 128), 64);
	TEST_IS_EQ(lerp255(0, 128, 0), 0);
	TEST_IS_EQ(lerp255(0, 128, 255), 128);

	TEST_IS_EQ(lerp255(0, 255, 128), 128);
	TEST_IS_EQ(lerp255(0, 255, 0), 0);
	TEST_IS_EQ(lerp255(0, 255, 255), 255);

    // lerp1204
    TEST_IS_EQ(lerp1024(0, 16, 512), 8);
    TEST_IS_EQ(lerp1024(-16, 0, 512), -8);

    // Combsort
    {
        int set[10] = { 0, 4, 4, 0, 1, 3, 3, 1, 2, 2};
        combSort(set, 10);
        for(int i=1; i<10; ++i)
            TEST_IS_TRUE(set[i] >= set[i - 1]);
    }

    // Angle range
    {
        TEST_IS_TRUE(distBetweenAngle(350, 10, 360) == 20);
        TEST_IS_TRUE(distBetweenAngle(10, 350, 360) == 20);
        TEST_IS_TRUE(distBetweenAngle(-10, 10, 360) == 20);
        TEST_IS_TRUE(distBetweenAngle(10, -10, 360) == 20);
        TEST_IS_TRUE(distBetweenAngle(10, 40, 360) == 30);
        TEST_IS_TRUE(distBetweenAngle(40, 10, 360) == 30);
        TEST_IS_TRUE(distBetweenAngle(200, 210, 360) == 10);
        TEST_IS_TRUE(distBetweenAngle(210, 200, 360) == 10);
        TEST_IS_TRUE(distBetweenAngle(180, -180, 360) == 0);
    }

    return true;
}

bool strStarts(const char *str, const char *prefix)
{
    if ((str == 0) || (prefix == 0))
        return false;

    if ((*str == 0) || (*prefix == 0))
        return false;

    while (*prefix)
    {
        if (*prefix++ != *str++)
            return false;
    }
    return true;
}

void intToDigits(int value, int* digits, int nDigits)
{
    uint32_t range = 1;
    for (int i = 1; i < nDigits; ++i) {
        range *= 10;
    }
    for (int i = 0; i < nDigits; ++i) {
        digits[i] = 0;
        uint32_t digit = value / range;
        if (digit < 10) {
            digits[i] = digit;
            value -= range * digit;
            range = range / 10;
        }
    }
}


void intToString(int value, char* buf, int allocated, bool writeZero)
{
	uint32_t range = 1;
	for (int i = 1; i < allocated - 1; ++i) {
		range *= 10;
		buf[i] = 0;
	}
	for (int i = 0; i < allocated - 1; ++i) {
		uint32_t digit = value / range;
		if (digit < 10) {
			buf[i] = ' ';
			if (digit || (i == allocated - 2) || writeZero) {
				buf[i] = '0' + digit;
				writeZero = true;
			}
			value -= range * digit;
			range = range / 10;
		}
	}
	buf[allocated - 1] = 0;
}

char ToLower(char c) {
    if (c >= 'A' && c <= 'Z')
        return (c - 'A') + 'a';
    return c;
}

bool istrStarts(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return false;

    while(*prefix) {
        if (ToLower(*prefix++) != ToLower(*str++))
            return false;
    }
    return true;
}


/*
    Modified Bernstein hash.
*/
uint32_t hash32(const char* v, const char* end, uint32_t h)
{
    for (; v < end; ++v) {
        // Simple form of the hash:
        // h = h * 33 ^ (*v);
        // But M0+ doesn't have 64 intermediate multiply. So use shifts:
        h = ((h << 5) + h) ^ (*v);
    }
    return h;
}

uint32_t hash32(const char* v, uint32_t h)
{
    for (; *v; ++v) {
        h = ((h << 5) + h) ^ (*v);
    }
    return h;
}

bool TestAverageSample()
{
    {
        AverageSample<Vec3<int>, 4> ave(Vec3<int>(2, 4, 8));
        Vec3<int> r = ave.average();
        TEST_IS_TRUE(r.x == 2);
        TEST_IS_TRUE(r.y == 4);
        TEST_IS_TRUE(r.z == 8);

        ave.push(Vec3<int>(0, 0, 0));
        ave.push(Vec3<int>(0, 0, 0));

        r = ave.average();
        TEST_IS_TRUE(r.x == 1);
        TEST_IS_TRUE(r.y == 2);
        TEST_IS_TRUE(r.z == 4);

        for (int i = 0; i < 8; ++i) {
            ave.push(Vec3<int>(0, 0, 0));
        }
        TEST_IS_TRUE(ave.average().isZero());
    }
    {
        AverageSample<uint32_t, 256> ave(4000);
        TEST_IS_TRUE(ave.average() == 4000);
        for (int i = 0; i < 128; i++) {
            ave.push(8000);
        }
        TEST_IS_TRUE(ave.average() == 6000);
    }
    {
        AverageSample<float, 2> ave(0);
        ave.push(1.0f);
        ave.push(1.0f);
        ave.push(0.0f);
        ave.push(2.0f);
        TEST_IS_TRUE(ave.average() == 1.0f);
    }
    return true;
}


bool TestCStr()
{
    //Log.p("sizeof(CStr<3>=").p(sizeof(CStr<3>)).eol();  // 8
    //Log.p("sizeof(CStr<4>=").p(sizeof(CStr<4>)).eol();  // 8
    //Log.p("sizeof(CStr<7>=").p(sizeof(CStr<7>)).eol();  // 12
    //Log.p("sizeof(CStr<10>=").p(sizeof(CStr<10>)).eol();// 16

    TEST_EQUAL(4, sizeof(CStr<3>));
    TEST_EQUAL(5, sizeof(CStr<4>));
    TEST_EQUAL(8, sizeof(CStr<7>));
    TEST_EQUAL(11, sizeof(CStr<10>));

    CStr<2> a = "a";
    CStr<2> b = "b";
    TEST(a < b);

    TEST_IS_TRUE(strStarts("FooBar", "Foo"));
    TEST_IS_FALSE(strStarts("FooBar", "Bar"));
    TEST_IS_FALSE(strStarts("Foo", "FooBar"));
    TEST_IS_FALSE(strStarts(0, "Foo"));
    TEST_IS_FALSE(strStarts("Foo", 0));

    TEST_IS_TRUE(istrStarts("FooBar", "Foo"));
    TEST_IS_FALSE(istrStarts("FooBar", "Bar"));
    TEST_IS_FALSE(istrStarts("Foo", "FooBar"));
    TEST_IS_FALSE(istrStarts(0, "Foo"));
    TEST_IS_FALSE(strStarts("Foo", 0));
    TEST_IS_TRUE(istrStarts("FooBar", "foo"));
    TEST_IS_TRUE(istrStarts("FOOBAR", "Foo"));

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
	{
		static const uint32_t values[] = {
			0, 1, 12, 123,
			0, 1, 12, 123
		};
		static const bool zeroFill[] = {
			false, false, false, false,
			true, true, true, true
		};
		static const char* answer[] = {
			" 0", " 1", "12", "",
			"00", "01", "12", ""
		};

		for (int i = 0; i < 8; ++i) {
			CStr<3> s = "999";
			s.setFromNum(values[i], zeroFill[i]);
			TEST_IS_EQ(s, answer[i]);
		}
	}
    { 
        CStr<10> val("value");
        TEST_IS_TRUE(val.beginsWith("v"));
        TEST_IS_TRUE(val.beginsWith("val"));
        TEST_IS_TRUE(val.beginsWith("value"));
        TEST_IS_FALSE(val.beginsWith("al"));
        TEST_IS_FALSE(val.beginsWith("values"));
    }
    {
        TEST_IS_FALSE(strEqual(0, 0));
        TEST_IS_FALSE(strEqual(0, ""));
        TEST_IS_TRUE(strEqual("", ""));
    }
    {
        // Short buffer test.
        char buf[11] = "0123456789";
        intToString(17, buf, 2, true);
        TEST_IS_TRUE(buf[1] == 0);
        TEST_IS_TRUE(buf[2] == '2');
    }
    {
        // Fits buffer test.
        char buf[11] = "0123456789";
        intToString(17, buf, 3, true);
        TEST_IS_TRUE(buf[0] == '1');
        TEST_IS_TRUE(buf[1] == '7');
        TEST_IS_TRUE(buf[2] == 0);
        TEST_IS_TRUE(buf[3] == '3');
    }
    {
        // Larger buffer test.
        char buf[11] = "0123456789";
        intToString(17, buf, 5, false);
        TEST_IS_TRUE(buf[0] == ' ');
        TEST_IS_TRUE(buf[1] == ' ');
        TEST_IS_TRUE(buf[2] == '1');
        TEST_IS_TRUE(buf[3] == '7');
        TEST_IS_TRUE(buf[4] == 0);
    }
    {
        // Larger buffer test.
        char buf[11] = "0123456789";
        intToString(17, buf, 5, true);
        TEST_IS_TRUE(buf[0] == '0');
        TEST_IS_TRUE(buf[1] == '0');
        TEST_IS_TRUE(buf[2] == '1');
        TEST_IS_TRUE(buf[3] == '7');
        TEST_IS_TRUE(buf[4] == 0);
    }
    {
        CStr<4> a = "a";
        CStr<5> b = "b";
        TEST_IS_FALSE(a == b);
        TEST_IS_TRUE(a != b);
        a = b;
        TEST_IS_TRUE(a == b);
    }
    {
        CStrBuf<5> a("Hello");
        CStrBuf<6> b("Hello");
        CStr<10> c = "Hello";
        CStr<10> d = "Goodbye";
        CStrBuf<4> e = "Four";

        TEST_IS_TRUE(a == b);
        TEST_IS_TRUE(b == c);
        TEST_IS_TRUE(a == c);
        TEST_IS_TRUE(a != d);
        TEST_IS_TRUE(b != d);

        TEST_IS_TRUE(sizeof(e) == 4);
        TEST_IS_TRUE(sizeof(a) >= 5 && sizeof(a) <= 8);
    }
    {
        CStrBuf<4> a("Hello");
        CStrBuf<5> b("Hello2");
        TEST_IS_TRUE(a != b);
    }
    {
        CStrBuf<6> a("Hello");
        CStrBuf<6> b("Hello2");
        TEST_IS_TRUE(a != b);
    }
    {
        CStr<9> a;
        a = "test";
        a = (const unsigned char*) "test2";
        a = (const char*) "test3";
    }
    {
        static const int NUM = 15;
        const char* tests[NUM] = {
            "clash", "clash0", "clash1", "clash2", "clash3", "clash4",
            "hum", "poweron", "poweroff",
            "swing", "swing0", "swing1", "swing2", "swing3", "swing4"
        };
        uint32_t result[NUM] = { 0 };
        for (int i = 0; i < NUM; ++i) {
            uint32_t v = hash32(tests[i], tests[i] + strlen(tests[i]));
            TEST_IS_TRUE(v > 0);
            for (int j = 0; j < i; ++j) {
                TEST_IS_TRUE(result[j] != v);
            }
            result[i] = v;
        }
    }
    {
        CStr<12> src = " a   b   cc  ";
        CStr<12> t0, t1, t2;
        CStr<12>* in[3] = { &t0, &t1, &t2 };
        src.tokenize(' ', in, 3);
        TEST_IS_TRUE(t0 == "a");
        TEST_IS_TRUE(t1 == "b");
        TEST_IS_TRUE(t2 == "cc");
    }
    {
        CStr<12> src = "aa bb   cc";
        CStr<12> t0, t1, t2;
        CStr<12>* in[3] = { &t0, &t1, &t2 };
        src.tokenize(' ', in, 3);
        TEST_IS_TRUE(t0 == "aa");
        TEST_IS_TRUE(t1 == "bb");
        TEST_IS_TRUE(t2 == "cc");
    }
    {
        CStr<12> src = " aa ";
        CStr<12> t0, t1, t2;
        CStr<12>* in[3] = { &t0, &t1, &t2 };
        src.tokenize(' ', in, 3);
        TEST_IS_TRUE(t0 == "aa");
        TEST_IS_TRUE(t1.empty());
        TEST_IS_TRUE(t2.empty());
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


char base64BitsToChar(int b)
{
    if (b >= 0 && b < 26)
        return 'A' + b;
    if (b >= 26 && b < 52)
        return 'a' + (b - 26);
    if (b >= 52 && b < 62)
        return '0' + (b - 52);
    if (b == 62)
        return '+';
    if (b == 63)
        return '-';
    return 0;
 }

int base64CharToBits(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    if (c >= '0' && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '-')
        return 63;
    return 0;
}

void encodeBase64(const uint8_t* src, int nBytes, char* dst, bool writeNull)
{
    // base64 - 6 bits per char of output
    // every 3 bytes (24 bits) is 4 char

    char* t = dst;
    for (int i = 0; i < nBytes; i += 3) {
        uint32_t accum = 0;
        accum = src[i];
        if (i + 1 < nBytes)
            accum |= src[i + 1] << 8;
        if (i + 2 < nBytes)
            accum |= src[i + 2] << 16;

        *t++ = base64BitsToChar(accum & 63);
        *t++ = base64BitsToChar((accum >> 6) & 63);
        *t++ = base64BitsToChar((accum >> 12) & 63);
        *t++ = base64BitsToChar((accum >> 18) & 63);
    }
    if (writeNull)
        *t = 0;
}

void decodeBase64(const char* src, int nBytes, uint8_t* dst)
{
    const char* p = src;
    for (int i = 0; i < nBytes; i += 3) {
        uint32_t accum = 0;
        accum = base64CharToBits(*p++);
        accum |= base64CharToBits(*p++) << 6;
        accum |= base64CharToBits(*p++) << 12;
        accum |= base64CharToBits(*p++) << 18;

        dst[i] = accum & 0xff;
        if (i + 1 < nBytes) dst[i + 1] = (accum >> 8) & 0xff;
        if (i + 2 < nBytes) dst[i + 2] = (accum >> 16) & 0xff;
    }
}

bool TestBase64()
{
    uint8_t src0[32];
    char dst[64];
    uint8_t src1[32];

    for (int i = 1; i < 32; ++i) {
        Random random(i);
        for (int j = 0; j < i; ++j) {
            src0[j] = random.rand();
        }
        encodeBase64(src0, i, dst, true);
        decodeBase64(dst, i, src1);
        TEST_IS_TRUE(memcmp(src0, src1, i) == 0);
    }
    return true;
}


bool TestCQueue()
{
    {
        CQueue<int, 4> queue;
        TEST_IS_TRUE(queue.empty());
        queue.push(1);
        queue.push(2);
        int r = queue.pop();
        TEST_IS_EQ(r, 1);
        TEST_IS_FALSE(queue.empty());
        queue.push(3);
        queue.push(4);
        queue.push(5);
        r = queue.pop();
        TEST_IS_EQ(r, 2);
        queue.push(6);
        TEST_IS_EQ(queue.pop(), 3);
        TEST_IS_EQ(queue.pop(), 4);
        TEST_IS_EQ(queue.pop(), 5);
        TEST_IS_EQ(queue.pop(), 6);
        TEST_IS_TRUE(queue.empty());
    }
    {
        CQueue<int16_t, 4> queue;
        queue.push(0);
        queue.push(1);
        queue.pushFront(-1);
        queue.pushFront(-2);
        TEST_IS_EQ(queue.pop(), -2);
        TEST_IS_EQ(queue.pop(), -1);
        TEST_IS_EQ(queue.pop(), 0);
        TEST_IS_EQ(queue.pop(), 1);
    }
    return true;
}

int AnimateProp::tick(uint32_t delta, int* target)
{
    m_time += delta;

    if (m_period == 0 || m_time >= m_period) {
        // All done.
        m_period = 0;
        m_value = m_end;
        if (target) *target = m_value;
        return m_end;
    }

    if (m_subPeriod) {
        // Breathy-blinking
        float t = float(m_time) / m_subPeriod;
        constexpr float shift = 1.0f / 8.0f;
        float s = sinf((t - shift) * k2Pi_float);
        if (s < 0) 
            s = 0;

        m_value = static_cast<int>(s * m_end);
    }
    else {
        // Straight animation.
        m_value = m_start + (m_end - m_start) * int(m_time) / int(m_period);
    }
    if (target) *target = m_value;
    return m_value;
}

int Timer2::tick(uint32_t delta)
{
	int fires = 0;
	m_accum += delta * m_scale;

	if (m_enable && delta && m_period && (m_accum >= m_period)) {
		fires = m_accum / m_period;
		if (m_repeating) {
			m_accum -= fires * m_period;
		}
		else {
			m_enable = false;
			fires = 1;
		}
	}
	return fires;
}

/*static*/ bool Timer2::Test()
{
	Timer2 t2;
	TEST_IS_EQ(t2.tick(900), 0);
	TEST_IS_EQ(t2.tick(100), 1);
	TEST_IS_EQ(t2.tick(2000), 2);

	t2.reset();
	t2.setRepeating(false);
	t2.setPeriod(500);
	TEST_IS_EQ(t2.tick(100), 0);
	TEST_IS_EQ(t2.tick(2000), 1);
	TEST_IS_FALSE(t2.enabled());
	TEST_IS_EQ(t2.tick(1200), 0);

	t2.setEnabled(true);
	TEST_IS_EQ(t2.tick(400), 0);
	TEST_IS_EQ(t2.tick(100), 1);
	TEST_IS_FALSE(t2.enabled());
	return true;
}

/* static */ bool Random::Test()
{
    static const int SAMPLES = 1000;
    static const uint32_t SERIES[] = { 2, 16, 100, 1000, 0 };
    Random r;

    for (int s = 0; SERIES[s]; ++s) {
        uint32_t range = SERIES[s];

        const uint32_t half = range / 2;
        int lessThanHalf = 0;
        int greaterThanHalf = 0;

        for (int i = 0; i < SAMPLES; ++i) {
            uint32_t v = r.rand(range);
            if (v < half) ++lessThanHalf;
            else ++greaterThanHalf;
        }
        TEST_IS_TRUE(greaterThanHalf >= SAMPLES * 4 / 10);  // 40%
        TEST_IS_TRUE(lessThanHalf >= SAMPLES * 4 / 10);
    }
    return true;
}

SPLog Log;
