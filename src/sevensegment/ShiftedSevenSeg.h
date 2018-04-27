#ifndef SHIFTED_SEVEN_SEG_INCLUDE
#define SHIFTED_SEVEN_SEG_INCLUDE

#include <stdint.h>

class ShiftedSevenSegment
{
public:
    ShiftedSevenSegment();

    /*
         aaa 
        f   b 
        f   b 
         ggg 
        e   c 
        e   c 
         ddd   h

        hgfedcba

        Current code assumes 
        - digits are cathodes,
        - segments are anodes
        This could be improved in loop() 
    */
    // Attach digit 0-3 to bit 0-15
    void attachDigit(int digit, int bit);
    // Attach segment 0-7 (a-h) to bit 0-15
    void attachSegment(int segment, int bit);

    void set(const char* str);

    void loop(uint32_t microSeconds, uint8_t* a, uint8_t* b);

private:
    static const int TIME_PER_DIGIT = 10000;
    static const int NUM_DIGITS = 4;
    static const int NUM_SEGMENTS = 8;  // includes the decimal point

    uint8_t m_digits[NUM_DIGITS];
    uint8_t m_digitMap[NUM_DIGITS];
    uint8_t m_segmentMap[NUM_SEGMENTS];
};

#endif 

