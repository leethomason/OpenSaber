#ifndef GRINLIZ_ASSERT_INCLUDED
#define GRINLIZ_ASSERT_INCLUDED

#ifndef _WIN32
#include <Arduino.h>
#endif

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert <true> {};
#define STATIC_ASSERT(e) (CompileTimeAssert <(e) != 0>())

#if defined(_MSC_VER)
#   include <assert.h>
#	define ASSERT assert
#   define ASSERT2(x, y, z) assert( x )
#else
    #define ASSERT( x ) 		{}
    #define ASSERT2( x, y, z )  {}
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

#endif // GRINLIZ_ASSERT_INCLUDED
