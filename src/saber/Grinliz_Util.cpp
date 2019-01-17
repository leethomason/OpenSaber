#include "Grinliz_Util.h"
#include <math.h>

//#define DEBUG_EVENT

static const int SIZE_SIN_TABLE = 256;
static FixedNorm gSinTable[SIZE_SIN_TABLE] = { 0 };

uint8_t lerpU8(uint8_t a, uint8_t b, uint8_t t) 
{
    int16_t r = int16_t(a) + (int16_t(b) - int16_t(a)) * int16_t(t) / 255;
    return uint8_t(clamp(r, int16_t(0), int16_t(255)));
}

bool TestUtil()
{
    //clamp()
    TEST_IS_EQ(clamp(-10, 0, 100), 0);
    TEST_IS_EQ(clamp(10,  0, 100), 10);
    TEST_IS_EQ(clamp(110, 0, 100), 100);

    // lerpU8()
	TEST_IS_EQ(lerpU8(0, 128, 128), 64);
	TEST_IS_EQ(lerpU8(0, 128, 0), 0);
	TEST_IS_EQ(lerpU8(0, 128, 255), 128);

	TEST_IS_EQ(lerpU8(0, 255, 128), 128);
	TEST_IS_EQ(lerpU8(0, 255, 0), 0);
	TEST_IS_EQ(lerpU8(0, 255, 255), 255);

    // iSin, iSin255 madness
    TEST_IS_TRUE(iSin(0) == 0);
    TEST_IS_TRUE(iSin(FixedNorm(1, 4)) == 1);
    TEST_IS_TRUE(iSin(FixedNorm(2, 4)) == 0);
    TEST_IS_TRUE(iSin(FixedNorm(3, 4)) == -1);
    
    FixedNorm ref(0);
    for (int i = 0; i < 10; i++) {
        FixedNorm fn = iSin(FixedNorm(i, 1024));
        TEST_IS_TRUE(fn >= ref);
        ref = fn;
    }

    return true;
}

FixedNorm iSin(FixedNorm fx)
{
	if (gSinTable[SIZE_SIN_TABLE/4] == 0) {
		for (int i = 0; i < SIZE_SIN_TABLE; ++i) {
			float x = 2 * 3.14159f * float(i) / float(SIZE_SIN_TABLE);
            gSinTable[i].set(sin(x) * 8193./8192.);
		}
	}
    // Just need the decimal part for the lookup.
    STATIC_ASSERT(SIZE_SIN_TABLE == 256);   // If not 256, change the shift bits below.
    int x = fx.getDec() >> 8;
    FixedNorm sin0 = gSinTable[x];
    FixedNorm sin1 = gSinTable[(x + 1) % 256];

    // Goes from 0-1 in float.
    // Which is from 0-4096 fixed.
    // And 0 - 2^16 in getDec() - lots to keep track of
    // So the "step" is 16 in fixed, 256 in getDec()
    int32_t ifraction = fx.getDec() - (x * 256);
    FixedNorm fraction(ifraction, 256);
    FixedNorm r = sin0 * (1 - fraction) + sin1 * fraction;
    return r;
}


bool strStarts(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return false;

	if (!*str || !*prefix)
		return false;

    while(*prefix) {
        if (*prefix++ != *str++)
            return false;
    }
    return true;
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
		if (digit >= 0 && digit < 10) {
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
    This algorithm is ad-hoc. Need a consistent, fast
    hash for storing filenames as 16-bit ints instead
    of strings. The strings are capped at 8 length,
    which lets a super-simple approach probably stand.
*/
uint16_t hash8(const char* v, const char* end)
{
	unsigned shift = 0;
	uint16_t h = 0;
	while(v < end) {
		h = ((*v) << (shift & 7)) ^ h;
		++shift;
		++v;            
	}
	return h;
}


bool TestCStr()
{
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
        uint16_t result[NUM] = { 0 };
        for (int i = 0; i < NUM; ++i) {
            uint16_t v = hash8(tests[i], tests[i] + strlen(tests[i]));
            TEST_IS_TRUE(v > 0);
            for (int j = 0; j < i; ++j) {
                TEST_IS_TRUE(result[j] != v);
            }
            result[i] = v;
        }
        /*
        for (int i = 0; i < NUM; ++i) {
            Log.p(tests[i]).p(" ").p(result[i]).eol();
        }
        */
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


bool TestCQueue()
{
    CQueue<4> queue;
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
    return true;
}


int Timer2::tick(uint32_t delta)
{
	int fires = 0;
	m_accum += delta;
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

void EventQueue::event(const char* e, int data)
{
	ASSERT(e);
	ASSERT(m_nEvents < NUM_EVENTS);

    if (m_nEvents >= NUM_EVENTS) {
        Log.p("Overflow: ");
        for (int i = 0; i < NUM_EVENTS; ++i) {
            int slot = (m_head + i) % NUM_EVENTS;
            Log.p(m_events[slot].name).p(" ");
        }
        Log.eol();
        return;
    }

	int slot = (m_head + m_nEvents) % NUM_EVENTS;

	m_events[slot].name = e;
	m_events[slot].data = data;

    #if SERIAL_DEBUG == 1
	if (m_eventLogging)
		Log.p(m_events[slot].name).p(" data=").p(m_events[slot].data).eol();
    #endif
    #ifdef DEBUG_EVENT
    Log.p("Event::push ").p(m_events[slot].name).p(" nEvents=").p(m_nEvents).eol();
    #endif

	++m_nEvents;
}


const EventQueue::Event& EventQueue::peek(int i) const
{
	ASSERT(i < m_nEvents);
	int slot = (m_head + m_nEvents) % NUM_EVENTS;
	return m_events[slot];
}


EventQueue::Event EventQueue::popEvent()
{
	Event e;
	ASSERT(m_nEvents > 0);
	if (m_nEvents == 0)
		return e;

	e = m_events[m_head];
	++m_head;
	--m_nEvents;
	if (m_head == NUM_EVENTS)
		m_head = 0;

    #ifdef DEBUG_EVENT
    Log.p("Event::pop ").p(e.name).p(" nRemain=").p(m_nEvents).eol();
    #endif
	return e;
}

SPLog Log;
EventQueue EventQ;

void PushTestEvents(int n)
{
	if (n >= 1) EventQ.event("Event0");
	if (n >= 2) EventQ.event("Event1", 1);
	if (n >= 3) EventQ.event("Event2", 2);
	if (n >= 4) EventQ.event("Event3", 3);
	if (n >= 5) EventQ.event("Event4", 4);
	if (n >= 6) EventQ.event("Event5", 5);
	if (n >= 7) EventQ.event("Event6", 6);
	if (n >= 8) EventQ.event("Event7", 7);
}


bool TestEvent()
{
	EventQ.setEventLogging(false);
    while(EventQ.hasEvent()) 
        EventQ.popEvent();
	TEST_IS_FALSE(EventQ.hasEvent());
	{
		PushTestEvents(1);
		TEST_IS_TRUE(EventQ.hasEvent());
		EventQueue::Event e = EventQ.popEvent();
		TEST_IS_TRUE(strEqual(e.name, "Event0"));
		TEST_IS_TRUE(e.data == 0);
	}
	TEST_IS_FALSE(EventQ.hasEvent());
	{
		PushTestEvents(3);
		TEST_IS_TRUE(EventQ.hasEvent());
		EventQueue::Event e = EventQ.popEvent();
		TEST_IS_TRUE(strEqual(e.name, "Event0"));
		TEST_IS_TRUE(e.data == 0);
		e = EventQ.popEvent();
		TEST_IS_TRUE(strEqual(e.name, "Event1"));
		TEST_IS_TRUE(e.data == 1);	
		e = EventQ.popEvent();
		TEST_IS_TRUE(strEqual(e.name, "Event2"));
		TEST_IS_TRUE(e.data == 2);	
	}
	TEST_IS_FALSE(EventQ.hasEvent());
	{
		PushTestEvents(8);
		for (int i = 0; i < 7; ++i)
			EventQ.popEvent();
		EventQueue::Event e = EventQ.popEvent();
		TEST_IS_TRUE(strEqual(e.name, "Event7"));
		TEST_IS_TRUE(e.data == 7);
	}
	TEST_IS_FALSE(EventQ.hasEvent());
	EventQ.setEventLogging(true);
	return true;
}

