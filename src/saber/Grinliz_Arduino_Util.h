#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include <string.h>
#include <stdint.h>
#include <Arduino.h>

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert <true> {};
#define STATIC_ASSERT(e) (CompileTimeAssert <(e) != 0>())
class SPISettings;

class LEDManager;
typedef void (*BlinkHandler)(const LEDManager&);

class LEDManager
{
public:
	enum {
		BLINK_TRAILING,
		BLINK_LEADING,
	};
    LEDManager(uint8_t pin, bool on=true);

    void set(bool on);
    void blink(uint8_t n, uint32_t cycle, BlinkHandler handler = 0, uint8_t leadOrTrail = BLINK_LEADING);
    void process();
    int  numBlinks() const;
    bool blinking() const {
        return m_nBlink > 0;
    }

private:
    uint8_t  m_pin = 0;
    uint8_t  m_nBlink = 0;
    bool 	 m_on = false;
    uint8_t  m_bias = BLINK_LEADING;
    uint32_t m_cycle = 0;
    uint32_t m_startTime = 0;
    BlinkHandler m_handler = 0;
    uint8_t m_nCallbacks = 0;
};

/**
  * Returns 'true' if 2 strings are equal.
  * If one or both are null, they are never equal.
  */
inline bool strEqual(const char* a, const char* b) {
    return a && b && strcmp(a, b) == 0;
}

/**
 * Returns 'true' if 'str' strarts with 'prefix'
 */
bool strStarts(const char* str, const char* prefix);

/**
  *	The CStr class is a "c string": a simple array of
  * char and an int size bound in a class. It allocates
  * no memory, and is very efficient.
  */
template< int ALLOCATE >
class CStr
{
public:
    CStr() {
        clear();
    }
    CStr( const char* src )	{
        clear();
        append(src);
    }

    CStr( const CStr<ALLOCATE>& other ) {
        memcpy( buf, other.buf, ALLOCATE );
        len = other.len;
    }

    ~CStr()	{}

    const char* c_str()	const	{
        return buf;
    }

    int size() const			{
        return len;
    }
    bool empty() const			{
        return buf[0] == 0;
    }
    int capacity() const		{
        return ALLOCATE-1;
    }
    void clear()				{
        buf[0] = 0;
        len = 0;
    }

    bool operator==( const char* str ) const						{
        return strEqual(buf, str);
    }
    bool operator!=( const char* str ) const						{
        return !strEqual(buf, str);
    }
    char operator[]( int i ) const									{
        return buf[i];
    }
    template < class T > bool operator==( const T& str ) const		{
        return strEqual(buf, str.buf);
    }
    bool operator<(const CStr<ALLOCATE>& str) const					{
        return strcmp(buf, str.buf) < 0;
    }

    void operator=( const char* src )	{
        clear();
        append(src);
    }

    void operator+=( const char* src ) {
        append(src);
    }

    void append(const char* src) {
        for(const char* q = src; q && *q; ++q) {
            append(*q);
        }
    }

    void append(char c) {
        if (len < ALLOCATE-1) {
            buf[len] = c;
            ++len;
            buf[len] = 0;
        }
    }

private:
    int len;
    char buf[ALLOCATE];
};

bool TestCStr();

// --- Hex / Dec Utility --- //
/// Convert a char ('0'-'9', 'a'-'f', 'A'-'F') to the integer value.
int hexToDec(char h);
/// Convert an integer from 0-15 to the hex character. '0' - 'f'
char decToHex(int v);

bool TestHexDec();

/**
 * Convert a string in the form: aabbcc to decimal.
 */
void parseHex(const CStr<7>& str, uint8_t* color3);
void parseHex(const CStr<4>& str, uint8_t* color3);

/** 
 *  Convert a numbers to a CStr.
 */
void writeHex(const uint8_t* color3, CStr<6>* str);

bool TestHex();

// --- Range / Min / Max --- //
template<class T>
bool inRange(T a, T b, T c) {
    return a >= b && a <= c;
}

// --- Algorithm --- //

template <class T> inline void	Swap( T* a, T* b )	{
    T temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

template <class T>
inline void combSort(T* mem, int size)
{
    int gap = size;
    for (;;) {
        gap = gap * 3 / 4;
        if (gap == 0) gap = 1;

        bool swapped = false;
        const int end = size - gap;
        for (int i = 0; i < end; i++) {
            int j = i + gap;
            if (mem[j] < mem[i]) {
                Swap(mem+i, mem+j);
                swapped = true;
            }
        }
        if (gap == 1 && !swapped) {
            break;
        }
    }
}

class Random
{
public:
	Random() : s0(1234), s1(5678)	{}
	void setSeed(uint32_t s) {
		s0 = s;
		s1 = s + 5678;
	}

	uint32_t rand16() {
		s0 = (s0 * 10103) & 0xffff;
		s1 = (s1 *  1103) & 0xffff;
		return s0 ^ s1;
	}

	uint32_t rand16(uint32_t limit) {
		return rand16() % limit;
	}

private:
	uint32_t s0;
	uint32_t s1;
};

// --- Interupts & Time --- //
/*
template<class T>
T atomicRead(T* ptr) {
    T ret;
    do {
        ret = *(volatile T*)ptr;
    } while (ret != *(volatile T*)ptr);
    return ret;
}
*/

class Timer
{
public:
    Timer() : time(0), trigger(1000) {}
    Timer(uint32_t triggerTime) : time(0), trigger(triggerTime) {}

    bool tick();
    uint32_t period() const {
        return trigger;
    }

private:
    uint32_t time, trigger;
};

class SPITransaction
{
public:
    SPITransaction(uint8_t cs, const SPISettings& settings);
    ~SPITransaction();

private:
    uint8_t cs;
};

class SPClass
{
public:
	const SPClass& p(const char v[]) const 					{ Serial.print(v);    return *this; }
    const SPClass& p(char v) const							{ Serial.print(v);    return *this; }
    const SPClass& p(unsigned char v, int p = DEC) const	{ Serial.print(v, p); return *this; }
    const SPClass& p(int v, int p = DEC) const				{ Serial.print(v, p); return *this; }
    const SPClass& p(unsigned int v, int p = DEC) const		{ Serial.print(v, p); return *this; }
    const SPClass& p(long v, int p = DEC) const				{ Serial.print(v, p); return *this; }
    const SPClass& p(unsigned long v, int p = DEC) const	{ Serial.print(v, p); return *this; }
    const SPClass& p(double v, int p = 2) const				{ Serial.print(v, p); return *this; }
    void eol() const 										{ Serial.println(""); }
};

extern SPClass SPrint;

class SPLog
{
public:
	void attachSerial(Stream* stream);
	void attachLog(Stream* stream);

	const SPLog& p(const char v[]) const;
    const SPLog& p(char v) const;
    const SPLog& p(unsigned char v, int p = DEC) const;
    const SPLog& p(int v, int p = DEC) const;
    const SPLog& p(unsigned int v, int p = DEC) const;
    const SPLog& p(long v, int p = DEC) const;
    const SPLog& p(unsigned long v, int p = DEC) const;
    const SPLog& p(double v, int p = 2) const;
    void eol() const;

    void event(const char* event);
    void event(const char* event, const char* data);
    void event(const char* event, int data);

    const char* popEvent(const char** name, const char** data = 0, int* dataI = 0);
    bool hasEvent() const { return _hasEvent; }

private:
	Stream* serialStream = 0;
	Stream* logStream = 0;  

    bool     _hasEvent = false;
	CStr<40> eventName;
	CStr<40> eventStrData;
	int      eventIData;
};

extern SPLog Log;
bool TestEvent();

#define ASSERT(x) if (!(x)) { Log.p("ASSERT: ").p(#x).p(" ").p(__FILE__).p(" ").p(__LINE__).eol(); }

#endif // CSTR_INCLUDED
