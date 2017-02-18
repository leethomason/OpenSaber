#ifndef GRINLIZ_UTIL_INCLUDED
#define GRINLIZ_UTIL_INCLUDED

#include <string.h>
#include <stdint.h>

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert <true> {};
#define STATIC_ASSERT(e) (CompileTimeAssert <(e) != 0>())

#if defined(_MSC_VER)
#	define ASSERT( x )	if ( !(x)) { _asm { int 3 } }
#else
	void AssertOut(const char* message, const char* file, int line);
	#	define ASSERT( x ) 	if (!(x)) { AssertOut(#x, __FILE__, __LINE__); }
#endif

#define TEST_IS_TRUE(x) {         \
    if((x)) {                     \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

#define TEST_IS_FALSE(x) {        \
    if(!(x)) {                    \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

#define TEST_IS_EQ(x, y) {        \
    if((x) == (y)) {              \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

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
bool istrStarts(const char* str, const char* prefix);

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
		return strEqual(buf, str.buf);
	}
	
	bool operator<(const CStr<ALLOCATE>& str) const {
		return strcmp(buf, str.buf) < 0;
	}

	void operator=(const char* src) {
		clear();
		append(src);
	}

	void operator+=(const char* src) {
		append(src);
	}

	void append(const char* src) {
		for (const char* q = src; q && *q; ++q) {
			append(*q);
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

		uint32_t range = 1;
		for (int i = 1; i < ALLOCATE - 1; ++i)
			range *= 10;
		for (int i = 0; i < ALLOCATE - 1; ++i) {
			uint32_t digit = value / range;
			if (digit >= 0 && digit < 10) {
				buf[i] = ' ';
				if (digit || (i == ALLOCATE - 2) || writeZero) {
					buf[i] = '0' + digit;
					writeZero = true;
				}
				value -= range * digit;
				range = range / 10;
			}
		}
		len = ALLOCATE - 1;
		buf[len] = 0;
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
bool inRange(const T& a, const T& b, const T& c) {
	return a >= b && a <= c;
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
	Random() : s0(1234), s1(5678) {}
	void setSeed(uint32_t s) {
		s0 = s;
		s1 = s + 5678;
	}

	uint32_t rand16() {
		s0 = (s0 * 10103) & 0xffff;
		s1 = (s1 * 1103) & 0xffff;
		return s0 ^ s1;
	}

	uint32_t rand16(uint32_t limit) {
		return rand16() % limit;
	}

private:
	uint32_t s0;
	uint32_t s1;
};


/**
Sin wave.
Input: 0-255 (range will be clipped correctly.)
Output:: -256 - 256
*/
int16_t isin(uint16_t x);


#endif // GRINLIZ_UTIL_INCLUDED

