#ifndef GRINLIZ_UTIL_INCLUDED
#define GRINLIZ_UTIL_INCLUDED

#include <string.h>
#include <stdint.h>

class Stream;

#include "grinliz_assert.h"
#include "fixed.h"

template<class T>
T clamp(T value, T lower, T upper) {
	if (value < lower) return lower;
	if (value > upper) return upper;
	return value;
}

template<class T>
T lerp256(T a, T b, T t256) {
    return (a * (256 - t256) + b * t256) / 256;
}

template<class T>
T lerp1024(T a, T b, T t1024) {
    return (a * (1024 - t1024) + b * t1024) / 1024;
}

bool TestUtil();

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

// Hash suitible for short (8 char or so) strings.
uint16_t hash8(const char* v, const char* end);

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
	
	uint16_t hash8() const {
		return ::hash8(buf, buf + len);
	}

private:
	int len;
	char buf[ALLOCATE];
};


/**
* The CStrBuf. Wraps a buffer of characters, that doesn't have
  to be null-terminated. Optimizes for space (the size of this structure
  should just be ALLOCATE) vs. performance. Note also the abscence of the
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

	uint16_t hash8() const {
		return ::hash8(buf, buf + size());
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

template<int CAP>
class CQueue
{
public:
    CQueue() {}

    void push(int val) {
        ASSERT(len < CAP);
        int index = (head + len) % CAP;
        data[index] = val;
        ++len;
    }

    int pop() {
        ASSERT(len > 0);
        int result = data[head];
        head = (head + 1) % CAP;
        --len;
        return result;
    }

	bool hasCap() const { return len < CAP; }
    int empty() const { return len == 0; }

private:
    int len = 0;
    int head = 0;
    int data[CAP];
};


bool TestCQueue();

// --- Range / Min / Max --- //
template<class T>
bool inRange(const T& val, const T& a, const T& b) {
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

// --- Algorithm --- //

template <class T> inline void	Swap(T* a, T* b) {
	T temp = *a;
	*a = *b;
	*b = temp;
};


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
				Swap(mem + i, mem + j);
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
	void setPeriod(uint32_t period) { m_period = period; }
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
	uint32_t m_accum = 0;
	uint32_t m_period;
	bool m_repeating;
	bool m_enable;
};

// iSin goes [0,1], not [0, 2pi]
FixedNorm iSin(FixedNorm x);

/* Generally try to keep Ardunino and Win332 code very separate.
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

	const SPLog& p(const char v[]) const;
	const SPLog& p(char v) const;
	const SPLog& p(unsigned char v, int p = DEC) const;
	const SPLog& p(int v, int p = DEC) const;
	const SPLog& p(unsigned int v, int p = DEC) const;
	const SPLog& p(long v, int p = DEC) const;
	const SPLog& p(unsigned long v, int p = DEC) const;
	const SPLog& p(double v, int p = 2) const;
	
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

class EventQueue
{
public:
    // Note that the event is stored *by pointer*, so the 
    // string needs to be in static memory.
	void event(const char* event, int data = 0);

	struct Event {
		const char* name = 0;
		int			data = 0;
	};

	Event popEvent();
	bool hasEvent() const { return m_nEvents > 0; }
	int numEvents() const			{ return m_nEvents; }
	const Event& peek(int i) const;

    // For testing.
    void setEventLogging(bool enable) { m_eventLogging = enable; }

private:
	static const int NUM_EVENTS = 8;
	int m_nEvents = 0;
	int m_head = 0;
	bool m_eventLogging = true;
	Event m_events[NUM_EVENTS];
};

extern SPLog Log;
extern EventQueue EventQ;

bool TestEvent();

#endif // GRINLIZ_UTIL_INCLUDED

