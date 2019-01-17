#ifndef FIXED_16_INCLUDED	
#define FIXED_16_INCLUDED

#include <stdint.h>

template<typename LONG, typename SHORT, int DECBITS>
class FixedT
{
private:
    static inline SHORT FixedMul(SHORT a, SHORT b) {
        LONG c = LONG(a) * LONG(b);
        return SHORT(c >> DECBITS);
    }

    static inline SHORT FixedDiv(SHORT a, SHORT b) {
        LONG c = (LONG(a) << (DECBITS*2)) / (LONG(b) << DECBITS);
        return SHORT(c);
    }

    static inline SHORT IntToFixed(int v) {
        return SHORT(v << DECBITS);
    }
    static const int FIXED_1 = 1 << DECBITS;
    int16_t x;

public:
    FixedT() {}
    FixedT(const FixedT& f) : x(f.x) {}
    FixedT(int v) : x(v << DECBITS) {}
    FixedT(int32_t num, int32_t den) {
        x = FIXED_1 * num / den;
    }
    FixedT(float v) : x(SHORT(FIXED_1 * v)) {}
    FixedT(double v) : x(SHORT(FIXED_1 * v)) {}

    void set(float v) { x = SHORT(v * FIXED_1); }
    void set(double v) { x = SHORT(v * FIXED_1); }

    // Scale up to an int, potentially out of the range of this fixed.
    int32_t scale(int32_t s) { int32_t r = s * x / FIXED_1; return r; }
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

// Max: 127
typedef FixedT<int32_t, int16_t, 8> Fixed88;
// Strange def, but useful range:
// Max: 1023
typedef FixedT<int32_t, int16_t, 6> Fixed115;

// Good for [1, -1] type functions, general near one.
typedef FixedT<int32_t, int16_t, 12> FixedNorm;
#endif // FIXED_16_INCLUDED
