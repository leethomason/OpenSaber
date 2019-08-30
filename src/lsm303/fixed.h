#ifndef FIXED_16_INCLUDED	
#define FIXED_16_INCLUDED

#include <stdint.h>

// Note the auto-included Arduino headers
// shoves min() and max() into the global preprocessor
// namespace, so that the C++ namespace min() and max()
// are overwritten. Arduino compilation tools suck.

#include <limits.h>
#include <stdint.h>

#define DEBUG_FIXED_OVERFLOW

#ifdef DEBUG_FIXED_OVERFLOW
#include "grinliz_assert.h"
#endif

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

template<typename LONG, typename SHORT, int DECBITS>
class FixedT
{
private:

    static inline SHORT FixedMul(SHORT a, SHORT b) {
        static_assert(sizeof(LONG) >= sizeof(SHORT) * 2, "Long must have more bits that short.");

        LONG c = LONG(a) * LONG(b);
        return SHORT(c >> DECBITS);
    }

    static inline SHORT FixedDiv(SHORT a, SHORT b) {
        static_assert(sizeof(LONG) >= sizeof(SHORT) * 2, "Long must have more bits that short.");
        static_assert(DECBITS * 2 <= (sizeof(LONG) - sizeof(SHORT)) * 2, "Need enough space to shift for division");

        LONG c = (LONG(a) << (DECBITS*2)) / (LONG(b) << DECBITS);
        return SHORT(c);
    }

    static inline SHORT IntToFixed(int v) {
        ASSERT((v << DECBITS) <= Limit<SHORT>::pos());
        ASSERT((v << DECBITS) >= Limit<SHORT>::neg());

        return SHORT(v << DECBITS);
    }
    static const int FIXED_1 = 1 << DECBITS;
    int16_t x;

public:
    FixedT() {}
    FixedT(const FixedT& f) : x(f.x) {}
    FixedT(int v) : x(v << DECBITS) {}
    FixedT(int32_t num, int32_t den) {
        const int32_t o_num = num;
        const int32_t o_den = den;
        (void) o_num;
        (void) o_den;

        while(num > Limit<SHORT>::pos() || num < Limit<SHORT>::neg()) {
            num /= 2;
            den /= 2;
        }
        ASSERT2(den > 0, o_num, o_den);
        if (den == 0) den = 1;
        x = FIXED_1 * num / den;
    }
    FixedT(float v) : x(SHORT(FIXED_1 * v)) {}
    FixedT(double v) : x(SHORT(FIXED_1 * v)) {}

    void set(float v) { x = SHORT(v * FIXED_1); }
    void set(double v) { x = SHORT(v * FIXED_1); }

    // Scale up to an int, potentially out of the range of this fixed.
    int32_t scale(int32_t s) const { 
        int32_t r = s * x / FIXED_1; 
        return r; 
    }

    // Return the floor part (as a copy).
    FixedT getFloor() const { 
        int fraction = x & ~(FIXED_1 - 1); 
        FixedT f; 
        f.x = fraction; 
        return f; 
    }

    // Query the integer part. Positive or negative.
    int32_t getInt() const { return x >> DECBITS; }
    // Query the DECBITSimal part - return as a fraction of 2^16.
    // Always positive.
    uint32_t getDec() const { return (unsigned(x) & (FIXED_1 - 1)) << (16 - DECBITS); }

    FixedT sqrt() const {
        if (x < 0) return 0;
        LONG target = x << DECBITS;
        LONG low = 0;
        LONG high = x;
        LONG mid = low;

        while (low <= high) {
            mid = (low + high) / 2;
            LONG guess = mid * mid;
            if (guess > target) {
                high = mid - 1;
            }
            else if (guess < target) {
                low = mid + 1;
            }
            else {
                break;
            }
        }
        FixedT val;
        val.x = mid;
        return val;
    }

    FixedT& operator = (const FixedT &v) { x = v.x; return *this; }
    FixedT& operator = (const int v) { x = IntToFixed(v); return *this; }

    FixedT& operator +=  (const FixedT v) { x += v.x; return *this; }
    FixedT& operator +=  (const int v) { x += IntToFixed(v); return *this; }

    FixedT& operator -=  (const FixedT v) { x -= v.x; return *this; }
    FixedT& operator -=  (const int v) { x -= IntToFixed(v); return *this; }

    FixedT& operator *=  (const FixedT v) { x = FixedMul(x, v.x); return *this; }
    FixedT& operator *=  (const int v) { x *= v; return *this; }

    FixedT& operator /=  (const FixedT v) { x = FixedDiv(x, v.x); return *this; }
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
typedef FixedT<int32_t, int16_t, 8> Fixed88;
// Strange def, but useful range:
// Max: 1023
typedef FixedT<int32_t, int16_t, 6> Fixed115;

// Good for [1, -1] type functions, general near one.
typedef FixedT<int32_t, int16_t, 12> FixedNorm;

template<typename FIXED>
struct Vector3F
{
    FIXED x, y, z;
};

#endif // FIXED_16_INCLUDED
