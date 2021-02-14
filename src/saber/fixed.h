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

#ifndef FIXED_16_INCLUDED	
#define FIXED_16_INCLUDED

#include <stdint.h>

// Note the auto-included Arduino headers
// shoves min() and max() into the global preprocessor
// namespace, so that the C++ namespace min() and max()
// are overwritten. Arduino compilation tools suck.

#include <limits.h>
#include <stdint.h>

#include "grinliz_assert.h"

inline int32_t intSqrt(int32_t x) 
{
    if (x < 0) return 0;

    uint32_t v = x;
    uint32_t res = 0;
    uint16_t add = 0x8000;
    for (int i = 0; i < 16; i++) {
        uint16_t temp = uint16_t(res) | add;
        uint32_t g2 = temp * temp;
        if (v >= g2)
            res = temp;
        add >>= 1;
    }
    return res;
}


template<class T> 
struct Limit {
    // Don't compile if the type isn't defined in the specializations below.
};

template<>
struct Limit<int8_t> {
    static intptr_t pos() { return SCHAR_MAX; }
    static intptr_t neg() { return SCHAR_MIN; }
};

template<>
struct Limit<int16_t> {
    static intptr_t pos() { return SHRT_MAX; }
    static intptr_t neg() { return SHRT_MIN; }
};

template<>
struct Limit<int32_t> {
    static intptr_t pos() { return INT_MAX; }
    static intptr_t neg() { return INT_MIN; }
};

template<typename SHORT, int DECBITS>
class FixedT
{
private:
    // Be careful that the M0+ has a 32 bit single instruction multiply,
    // but no overflow. Try to help out with that.
    static inline SHORT FixedMul(SHORT a, SHORT b) {
        SHORT v = ((a * b) >> DECBITS);
        return v;
    }

    static inline SHORT FixedDiv(SHORT a, SHORT b) {
        SHORT v = (a << DECBITS) / b;
        return v;
    }

    static inline SHORT IntToFixed(int32_t a) {
        SHORT v = (a << DECBITS);
        return v;
    }

public:
    static const int32_t FIXED_1 = 1 << DECBITS;
    SHORT x;

    FixedT() {}
    FixedT(const FixedT& f) : x(f.x) {}
    FixedT(int v) : x(v << DECBITS) {}
    FixedT(int32_t num, int32_t den) {
        while(num > Limit<SHORT>::pos() || num < Limit<SHORT>::neg()) {
            num /= 2;
            den /= 2;
        }
        if (den == 0) {
            ASSERT(false);
            x = 0;
        }
        else {
            x = FIXED_1 * num / den;
        }
    }
    FixedT(float v) : x(SHORT(FIXED_1 * v)) {}
    FixedT(double v) : x(SHORT(FIXED_1 * v)) {}

    void set(int v) { x = SHORT(v * FIXED_1); }
    void set(float v) { x = SHORT(v * FIXED_1); }
    void set(double v) { x = SHORT(v * FIXED_1); }

    int32_t convertRaw(int nBits) {
        if (nBits > DECBITS)
            return x << (nBits - DECBITS);
        else if (DECBITS < nBits)
            return x >> (nBits - DECBITS);
        return x;
    }

    // Scale up to an int, potentially out of the range of this fixed.
    int32_t scale(int32_t s) const { 
        int32_t r = s * x / FIXED_1; 
        return r; 
    }

    // obviously very expensive call 
    float toFloat() const {
        return (float)x / (float)FIXED_1;
    }

    // Query the integer part. Positive or negative.
    int32_t getInt() const { return x >> DECBITS; }

    // Query the DECBITSimal part - return as a fraction of 2^16.
    // Always positive.
    uint32_t getDec() const { return (unsigned(x) & (FIXED_1 - 1)) << (16 - DECBITS); }

    bool isZero() const { return x == 0; }
    
    FixedT sqrt() const {
        if (x < 0) return 0;

        uint32_t v = x << DECBITS;
        uint32_t res = 0;
        uint16_t add = 0x8000;
        for (int i = 0; i < 16; i++) {
            uint16_t temp = uint16_t(res) | add;
            uint32_t g2 = temp * temp;
            if (v >= g2)
                res = temp;
            add >>= 1;
        }
        FixedT val;
        val.x = res;
        return val;
    }

    FixedT absolute() const { return x > 0 ? *this : -(*this); }

    FixedT& operator = (const FixedT &v) { x = v.x; return *this; }
    FixedT& operator = (int v) { x = IntToFixed(v); return *this; }
    FixedT& operator = (float v) { x = v * FIXED_1; return *this; }

    FixedT& operator +=  (const FixedT& v) { x += v.x; return *this; }
    FixedT& operator +=  (const int v) { x += IntToFixed(v); return *this; }

    FixedT& operator -=  (const FixedT& v) { x -= v.x; return *this; }
    FixedT& operator -=  (const int v) { x -= IntToFixed(v); return *this; }

    FixedT& operator *=  (const FixedT& v) { x = FixedMul(x, v.x); return *this; }
    FixedT& operator *=  (const int v) { x *= v; return *this; }

    FixedT& operator /=  (const FixedT& v) { x = FixedDiv(x, v.x); return *this; }
    FixedT& operator /=  (const int v) { x /= v; return *this; }

    FixedT& operator <<= (const int v) { x <<= v; return *this; }
    FixedT& operator >>= (const int v) { x >>= v; return *this; }

    FixedT& operator ++ () { x += FIXED_1; return *this; }
    FixedT& operator -- () { x -= FIXED_1; return *this; }

    FixedT operator ++ (int) { FixedT t;  t.x = x; x += FIXED_1; return t; }
    FixedT operator -- (int) { FixedT t;  t.x = x; x -= FIXED_1; return t; }

    FixedT operator - () const { FixedT t;  t.x = -x;  return t; }

    inline friend FixedT operator +  (const FixedT a, const FixedT b) { FixedT t;  t.x = a.x + b.x;	return t; }
    inline friend FixedT operator +  (const FixedT a, const int b) { FixedT t;  t.x = a.x + IntToFixed(b);	return t; }
    inline friend FixedT operator +  (const int a, const FixedT b) { FixedT t;  t.x = IntToFixed(a) + b.x;	return t; }

    inline friend FixedT operator -  (const FixedT a, const FixedT b) { FixedT t;	t.x = a.x - b.x;	return t; }
    inline friend FixedT operator -  (const FixedT a, const int b) { FixedT t;	t.x = a.x - IntToFixed(b);	return t; }
    inline friend FixedT operator -  (const int a, const FixedT b) { FixedT t;	t.x = IntToFixed(a) - b.x;	return t; }

    inline friend FixedT operator *  (const FixedT a, const FixedT b) { FixedT t;	t.x = FixedMul(a.x, b.x);	return t; }
    inline friend FixedT operator *  (const FixedT a, const int b) { FixedT t;	t.x = a.x * b; return t; }
    inline friend FixedT operator *  (const int a, const FixedT b) { FixedT t;	t.x = a * b.x; return t; }

    inline friend FixedT operator /  (const FixedT a, const FixedT b) { FixedT t;	t.x = FixedDiv(a.x, b.x); return t; }
    inline friend FixedT operator /  (const FixedT a, const int b) { FixedT t;	t.x = a.x / b; return t; }
    inline friend FixedT operator /  (const int a, const FixedT b) { FixedT t;	t.x = FixedDiv(IntToFixed(a), b.x); return t; }

    inline friend FixedT operator << (const FixedT a, const int b) { FixedT t; t.x = a.x << b; return t; }
    inline friend FixedT operator >> (const FixedT a, const int b) { FixedT t; t.x = a.x >> b; return t; }

    inline friend bool operator == (const FixedT a, const FixedT b) { return a.x == b.x; }
    inline friend bool operator == (const FixedT a, const int b) { return a.x == IntToFixed(b); }
    inline friend bool operator == (const int a, const FixedT b) { return IntToFixed(a) == b.x; }

    inline friend bool operator != (const FixedT a, const FixedT b) { return a.x != b.x; }
    inline friend bool operator != (const FixedT a, const int b) { return a.x != IntToFixed(b); }
    inline friend bool operator != (const int a, const FixedT b) { return IntToFixed(a) != b.x; }

    inline friend bool operator <  (const FixedT a, const FixedT b) { return a.x < b.x; }
    inline friend bool operator <  (const FixedT a, const int b) { return a.x < IntToFixed(b); }
    inline friend bool operator <  (const int a, const FixedT b) { return IntToFixed(a) < b.x; }

    inline friend bool operator >  (const FixedT a, const FixedT b) { return a.x > b.x; }
    inline friend bool operator >  (const FixedT a, const int b) { return a.x > IntToFixed(b); }
    inline friend bool operator >  (const int a, const FixedT b) { return IntToFixed(a) > b.x; }

    inline friend bool operator <=  (const FixedT a, const FixedT b) { return a.x <= b.x; }
    inline friend bool operator <=  (const FixedT a, const int b) { return a.x <= IntToFixed(b); }
    inline friend bool operator <=  (const int a, const FixedT b) { return IntToFixed(a) <= b.x; }

    inline friend bool operator >=  (const FixedT a, const FixedT b) { return a.x >= b.x; }
    inline friend bool operator >=  (const FixedT a, const int b) { return a.x >= IntToFixed(b); }
    inline friend bool operator >=  (const int a, const FixedT b) { return IntToFixed(a) >= b.x; }
};

bool TestFixed();

template<typename T, typename FIXED>
T lerpFixed(T a, T b, FIXED t) {
    return T(a + t.scale(b));
}

// Max: 127
typedef FixedT<int16_t, 8> Fixed88;

// Strange def, but useful range:
// Max: 1023
typedef FixedT<int16_t, 6> Fixed115;

// Good for [1, -1] type functions, general near one.
// About -8 to 8
typedef FixedT<int16_t, 12> FixedNorm;

static const FixedNorm fixedNormHalf(1, 2);
static const FixedNorm fixedNormSqrt2Over2(0.7071067811);

inline Fixed115 operator* (const Fixed115& a, const FixedNorm& b)
{
    FixedT<int16_t, 6> f;
    int32_t x = (a.x * b.x) >> 12;
    f.x = (int16_t)x;
    return f;
}

static const int32_t ISINE_360 = 32768;
static const int32_t ISINE_180 = (ISINE_360 / 2);
static const int32_t ISINE_90  = (ISINE_360 / 4);
static const int32_t ISINE_45  = (ISINE_360 / 8);
static const int32_t ISINE_30  = (2731);
static const int32_t ISINE_ONE  = 4096;
static const int32_t ISINE_HALF = 2048;

// Sine approximation.
// theta: angle with 2^15 units/circle (32768)
// return: sin, 12 bits (range -4096 to 4096)
int32_t iSin_S3(int32_t theta);

inline int32_t iCos_S3(int32_t x) {
    return iSin_S3(x + ISINE_90);
}

// Inverse sine.
// x: -4096 to 4096 (12 bits)
// returns: angle as above (-2^15 to +2^15) but range -ISINE_90 to ISINE_90
int32_t iInvSin_S3(int32_t x);

inline int32_t iInvCos_S3(int32_t x) {
    return -iInvSin_S3(x) + ISINE_90;
}

inline FixedNorm iSin(FixedNorm f) {
    int32_t s12 = iSin_S3(f.scale(ISINE_360));
    return FixedNorm(s12, ISINE_ONE);
}

inline FixedNorm iCos(FixedNorm f) {
    int32_t c12 = iCos_S3(f.scale(ISINE_360));
    return FixedNorm(c12, ISINE_ONE);
}


// [-90, 90]
inline FixedNorm iInvSin(FixedNorm f) {
    int32_t angle = iInvSin_S3(f.scale(ISINE_ONE));
    return FixedNorm(angle, ISINE_360);
}

// [180, 0]
inline FixedNorm iInvCos(FixedNorm f) {
    int32_t angle = iInvCos_S3(f.scale(ISINE_ONE));
    return FixedNorm(angle, ISINE_360);
}

#endif // FIXED_16_INCLUDED
