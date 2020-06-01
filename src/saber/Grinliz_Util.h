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

#ifndef GRINLIZ_UTIL_INCLUDED
#define GRINLIZ_UTIL_INCLUDED

#include <string.h>
#include <stdint.h>

#include "grinliz_assert.h"
#include "fixed.h"

static const uint32_t NOMINAL_VOLTAGE = 3700;
static const uint32_t VOLTAGE_RANGE	  =  300;

inline int32_t lerp1024(int16_t a, int16_t b, int32_t t1024) {
    return (a * (1024 - t1024) + b * t1024) / 1024;
}

inline int32_t lerp255(int16_t a, int16_t b, int32_t t255) {
    return (a * (255 - t255) + b * t255) / 255;
}

inline int32_t lerp256(int16_t a, int16_t b, int32_t t256) {
    return (int32_t(a) * (256 - t256) + int32_t(b) * t256) / 256;
}

inline FixedNorm lerp(FixedNorm a, FixedNorm b, FixedNorm t) {
    return (a * (1 - t) + b * t);
}

bool TestUtil();

template<typename T>
struct Vec3
{
	T x;
	T y;
	T z;

	Vec3() {}
    Vec3(T t) { x = t; y = t; z = t; }
	Vec3(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }

	int size() const { return 3; }

	void set(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }
	void setZero() { x = y = z = 0; }
	bool isZero() const { return x==0 && y == 0 && z == 0; }
	void scale(T s) { x *= s; y *= s; z *= s; }

	T operator[](int i) const { return *(&x + i); }
	T& operator[](int i) { return *(&x + i); }

    Vec3<T>& operator += (const Vec3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3<T>& operator -= (const Vec3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

    inline friend Vec3<T> operator +  (const Vec3<T>& a, const Vec3<T>& b) { Vec3<T> t;  t.x = a.x + b.x; t.y = a.y + b.y; t.z = a.z + b.z; return t; }
    inline friend Vec3<T> operator -  (const Vec3<T>& a, const Vec3<T>& b) { Vec3<T> t;  t.x = a.x - b.x; t.y = a.y - b.y; t.z = a.z - b.z; return t; }
    inline friend Vec3<T> operator /  (const Vec3<T>& a, T b) { Vec3<T> t;  t.x = a.x / b; t.y = a.y / b; t.z = a.z / b; return t; }
};

/**
* Returns 'true' if 2 strings are equal.
* If one or both are null, they are never equal.
* (But two empty strings are equal.)
*/
inline bool strEqual(const char* a, const char* b) {
	return a && b && strcmp(a, b) == 0;
}

inline bool strEqual(const char* a, const char* b, int n) {
    return a && b && strncmp(a, b, n);
}

/**
* Returns 'true' if 'str' strarts with 'prefix'
*/
bool strStarts(const char* str, const char* prefix);
bool istrStarts(const char* str, const char* prefix);
void intToString(int value, char* str, int allocated, bool writeZero);
void intToDigits(int value, int* digits, int nDigits);

void encodeBase64(const uint8_t* bytes, int nBytes, char* target, bool writeNull);
void decodeBase64(const char* src, int nBytes, uint8_t* dst);
bool TestBase64();

// Modified Bernstein hash
uint32_t hash32(const char* v, const char* end, uint32_t h=0);
uint32_t hash32(const char* v, uint32_t h=0);

/**
* The CStr class is a "c string": a simple array of
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

	CStr(const char* src) {
		clear();
		append(src);
	}

	CStr(const CStr<ALLOCATE>& other) {
		memcpy(buf, other.buf, ALLOCATE);
		len = other.len;
	}

	~CStr() {}

	const char* c_str()	const {
		return buf;
	}

	int size() const {
		return len;
	}

	bool empty() const {
		return buf[0] == 0;
	}
	
	int capacity() const {
		return ALLOCATE - 1;
	}
	
	void clear() {
		buf[0] = 0;
		len = 0;
	}

	bool beginsWith(const char* prefix) const {
		return strStarts(buf, prefix);
	}

	bool operator==(const char* str) const {
		return strEqual(buf, str);
	}

	bool operator==(char c) const {
		return len == 1 and buf[0] == c;
	}

	bool operator!=(const char* str) const {
		return !strEqual(buf, str);
	}
	
	char operator[](int i) const {
		return buf[i];
	}

	template < class T > bool operator==(const T& str) const {
		return strEqual(this->c_str(), str.c_str());
	}
	
    template < class T > bool operator!=(const T& str) const {
        return !strEqual(this->c_str(), str.c_str());
    }

    bool operator<(const CStr<ALLOCATE>& str) const {
		return strcmp(buf, str.buf) < 0;
	}

	void operator=(const char* src) {
		clear();
		append(src);
	}

	void operator=(char* const src) {
		clear();
		append((const char*)src);
	}

    void operator=(const unsigned char* src) {
        clear();
        append((const char*) src);
    }

    template< class T > void operator=(const T& str) {
		// T is presumably a CStr of a different size or a CStrBuf
		// Possibly on c_str() method.
        //*this = str.c_str();
		this->clear();
		for(int i=0; i<str.size(); ++i) {
			this->append(str[i]);
		}
    }

	void operator+=(const char* src) {
		append(src);
	}

	void append(const char* src) {
		for (const char* q = src; q && *q; ++q) {
			append(*q);
		}
	}

	void append(const char* start, const char* end) {
		while(start < end && *start) {
			append(*start);
			++start;
		}
	}

	void append(char c) {
		if (len < ALLOCATE - 1) {
			buf[len] = c;
			++len;
			buf[len] = 0;
		}
	}

	void setFromNum(uint32_t value, bool writeZero) {
		clear();
		intToString(value, buf, ALLOCATE, writeZero);
		len = (int) strlen(buf);
	}
	
	uint32_t hash32() const {
		return ::hash32(buf, buf + len);
	}

	void tokenize(char sep, CStr<ALLOCATE>* tokens[], int n) const {
		int t = 0;
		const char* p = buf;

		while(*p && t < n) {
			p = skip(sep, p);
			while(*p && *p != sep) {
				tokens[t]->append(*p);
				p++;
			}
			t++;
		} 
	}

private:
	const char* skip(char sep, const char* p) const {
		while (*p == sep) ++p;
		return p;
	}

	int len;
	char buf[ALLOCATE];
};


/**
* The CStrBuf. Wraps a buffer of characters, that doesn't have
  to be null-terminated. Optimizes for space (the size of this structure
  should just be ALLOCATE) vs. performance. Note also the absence of the
  c_str() method, since it can't be implemented without allocating memory.
*/
template< int ALLOCATE >
class CStrBuf
{
public:
    CStrBuf() { clear(); }

    CStrBuf(const char* src) {
        set(src);
    }

    CStrBuf(const CStrBuf<ALLOCATE>& other) {
        memcpy(buf, other.buf, ALLOCATE);
    }

    ~CStrBuf() {}

	template <typename T>
	void toStr(T* str) const {
		str->clear();
		for(int i=0; i<size(); ++i) {
			str->append(buf[i]);
		}
	}

    int size() const {
        for (int i = 0; i < ALLOCATE; i++) {
            if (buf[i] == 0) return i;
        }
        return ALLOCATE;
    }

    bool empty() const {
        return buf[0] == 0;
    }

    int capacity() const {
        return ALLOCATE;
    }

    void clear() {
        buf[0] = 0;
    }

    char operator[](int i) const {
        return buf[i];
    }

    template < class T > bool operator==(const T& str) const {
        const int s = this->size();
        if (str.size() != s)
            return false;

        for (int i = 0; i < s; ++i) {
            if ((*this)[i] != str[i])
                return false;
        }
        return true;
    }

    template< class T > bool operator !=(const T& str) const {
        // Somewhat forced syntax because I don't want to re-implement the operator==
        return !(*this == str);
    }

    void set(const char* src) {
        int i = 0; 
        for (; i < ALLOCATE && src[i]; ++i) {
            buf[i] = src[i];
        }
        if (i < ALLOCATE)
            buf[i] = 0;
    }

	uint32_t hash32() const {
		return ::hash32(buf, buf + size());
	}


private:
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

template<typename T, int CAP>
class CQueue
{
public:
    CQueue() {}

	T& front() {
		ASSERT(!empty());
		return data[head];
	}

   	void pushFront(T val) {
        ASSERT(len < CAP);
        head = dec(head);
        data[head] = val;
        ++len;
    }

	void push(T val) {
		ASSERT(len < CAP);
        data[tail] = val;
        tail = inc(tail);
		++len;
	}

    T pop() {
        ASSERT(len > 0);
        T result = data[head];
        head = inc(head);
        --len;
        return result;
    }

	bool hasCap() const { return len < CAP; }
    int empty() const { return len == 0; }
	int size() const { return len; }

private:
    int inc(int c) const { return (c + 1) % CAP; }
    int dec(int c) const { return (c - 1 + CAP) % CAP; }

	int len = 0;
	int head = 0;
	int tail = 0;
    T data[CAP];
};


bool TestCQueue();

// --- Range / Min / Max --- //
template<class T>
bool glInRange(const T& val, const T& a, const T& b) {
	return val >= a && val <= b;
}

template<class T>
T glMin(T a, T b) { return (a < b) ? a : b; }

template<class T>
T glMax(T a, T b) { return (a > b) ? a : b; }

template<class T>
T glClamp(T x, T a, T b) {
	if (x < a) return a;
	if (x > b) return b;
	return x;
}
template<class T>
void glSwap(T &a, T &b) {
    T t = a;
    a = b;
    b = t;
}

template<class T>
T glAbs(T x) { return x >= 0 ? x : -x; }

// --- Algorithm --- //

template <class T>
inline void combSort(T* mem, int size)
{
	int gap = size;
	for (;;) {
		gap = (gap * 3) >> 2;
		if (gap == 0) gap = 1;

		bool swapped = false;
		const int end = size - gap;
		for (int i = 0; i < end; i++) {
			int j = i + gap;
			if (mem[j] < mem[i]) {
				glSwap(mem[i], mem[j]);
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
	Random() : s(1) {}
    Random(int seed) {
        setSeed(seed);
    }

	void setSeed(uint32_t seed) {
		s = (seed > 0) ? seed : 1;
	}

	uint32_t rand() {
		// Xorshift
        // My new favorite P-RNG
		s ^= s << 13;
		s ^= s >> 17;
		s ^= s << 5;
		return s;
	}

	uint32_t rand(uint32_t limit) {
		return rand() % limit;
	}

    static bool Test();

private:
	uint32_t s;
};


/**
 * Power changes over time, and higher
 * draw changes the power. A small class
 * to average out power changes.
 */
template<typename TYPE, typename SUMTYPE, int N>
class AverageSample
{
public:
    AverageSample(TYPE initialValue) {
        for (int i = 0; i < N; ++i) {
			m_sample[i] = initialValue;
			m_total += initialValue;
		}
        m_average = initialValue;
    }

    void push(TYPE value) {
		m_total -= m_sample[m_pos];
        m_sample[m_pos] = value;
		m_total += value;

        m_pos++;
        if (m_pos == N) 
			m_pos = 0;
		m_average = m_total / N;
    }

	void fill(TYPE value) {
		for(int i=0; i<N; ++i)
			push(value);
	}

    TYPE average() const { return m_average; }
    int numSamples() const { return N; }
	bool origin() const { return m_pos == 0; }

private:
    SUMTYPE m_average;
	SUMTYPE m_total = 0;
    int m_pos = 0;
    TYPE m_sample[N];
};

bool TestAverageSample();

class StepProp
{
public:
	StepProp() {}

	void set(Fixed115 f) { step = f; }
	void set(float f) { step = Fixed115(f); }

	int tick(uint32_t delta) {
		value += step * delta;
		int n = value.getInt();
		value -= n;
		return n;
	}

public:
	Fixed115 step = 0;
	Fixed115 value = 0;
};

class AnimateProp
{
public:
	AnimateProp() {}

	void start(uint32_t period, int start, int end) {
		m_time = 0;
		m_period = period;
		m_start = start;
		m_end = end;
		m_value = start;
	}
	int tick(uint32_t delta, int* target=0);
	int value() const { return m_value; }
	bool done() { return m_period == 0; }

	int start() const { return m_start; }
	int end() const { return m_end; }

private:
	uint32_t m_time = 0;
	uint32_t m_period = 0;
	int m_value = 0;
	int m_start = 0;
	int m_end = 0;
};

class Timer2
{
public:
	Timer2(uint32_t period = 1000, bool repeating = true, bool enable = true) {
		m_period = period;
		m_repeating = repeating;
		m_enable = enable;
	}
	
	uint32_t remaining() const { return m_period - m_accum; }
	uint32_t period() const { return m_period; }

	void setPeriod(uint32_t period) { m_period = period; m_scale = 1; }
	void setScaledPeriod(float period) {
		m_period = int(period * SCALE);
		m_scale = SCALE;
	}

	bool repeating() const { return m_repeating; }
	void setRepeating(bool repeating) { m_repeating = repeating; }
	bool enabled() const { return m_enable; }
	void setEnabled(bool enable) { 
		if (!m_enable && enable) {
			reset();
		}
		m_enable = enable; 
	}
	void reset() { m_accum = 0; }

	int tick(uint32_t delta);

	static bool Test();

private:
	const uint32_t SCALE = 16;
	uint32_t m_accum = 0;
	uint32_t m_scale = 1;
	uint32_t m_period;
	bool m_repeating;
	bool m_enable;
};

/* 
	Generally try to keep Ardunino and Win32 code very separate.
	But a log class is useful to generalize, both for utility
	and testing. Therefore put up with some #define nonsense here.
*/
#ifdef _WIN32
class Stream;
static const int DEC = 1;	// fixme: use correct values
static const int HEX = 2;
#endif

class SPLog
{
public:
	void attachSerial(Stream* stream);
	void attachLog(Stream* stream);

	const SPLog& p(const char v[], int width=0) const;
	const SPLog& p(char v) const;
	const SPLog& p(unsigned char v, int p = DEC) const;
	const SPLog& p(int v, int p = DEC) const;
	const SPLog& p(unsigned int v, int p = DEC) const;
	const SPLog& p(long v, int p = DEC) const;
	const SPLog& p(unsigned long v, int p = DEC) const;
	const SPLog& p(double v, int p = 2) const;
	const SPLog& p(FixedNorm v) const { return p(v.toFloat()); }
	const SPLog& v3(int32_t x, int32_t y, int32_t z, const char* bracket=0) const;
	const SPLog& v2(int32_t x, int32_t y, const char* bracket=0) const;
	const SPLog& v3(float x, float y, float z, const char* bracket=0) const;
	const SPLog& v2(float x, float y, const char* bracket=0) const;

	const SPLog& v3(const Vec3<float>& v, const char* bracket=0) const {
		return v3(v.x, v.y, v.z, bracket);
	}
	const SPLog& v3(const Vec3<int32_t>& v, const char* bracket=0) const {
		return v3(v.x, v.y, v.z, bracket);
	}

	// Templated print, generally of alternate string class.
	template<class T> const SPLog& pt(const T& str) const {
		for(int i=0; i<str.size(); ++i) {
			(*this).p(str[i]);
		}
		return *this;
	}

	// Templated print, with commas.
	template<class T> const SPLog& ptc(const T& str) const {
		(*this).p("[");
		for(int i=0; i<str.size(); ++i) {
			(*this).p(str[i]);
			if (i != str.size() - 1) {
				(*this).p(",");
			}
		}
		(*this).p("]");
		return *this;
	}

	void eol() const;

private:
	Stream* serialStream = 0;
	Stream* logStream = 0;
};

extern SPLog Log;

#endif // GRINLIZ_UTIL_INCLUDED

