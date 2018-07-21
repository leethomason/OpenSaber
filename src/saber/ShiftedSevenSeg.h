#ifndef SHIFTED_SEVEN_SEG_INCLUDE
#define SHIFTED_SEVEN_SEG_INCLUDE

#include <stdint.h>

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
class ShiftedSevenSegment
{
public:
    ShiftedSevenSegment();

    // Attach digit 0-3 to output 0-15 of the shift register.
    void attachDigit(int digit, int bit);
    // Attach segment 0-7 (a-h) to output 0-15 of the shift register.
    void attachSegment(int segment, int bit);

    void set(const char* str);

    void loop(uint32_t microSeconds, uint8_t* a, uint8_t* b);

private:
    static const uint32_t TIME_PER_DIGIT = 4000; // microSeconds
    static const int NUM_DIGITS = 4;
    static const int NUM_SEGMENTS = 8;      // includes the decimal point

    uint8_t m_digits[NUM_DIGITS];
    uint8_t m_digitMap[NUM_DIGITS];
    uint8_t m_segmentMap[NUM_SEGMENTS];
};


/*
    Describes a dot matrix display and computes
    the shift bits needed to drive it.
*/
class ShiftedDotMatrix
{
public:
    ShiftedDotMatrix();

    // Attach row/col to output 0-15 of the shift register.
    void attachRow(int row, int bit) { m_rowMap[row] = uint8_t(bit); }   // 7 rows, cathodes
    void attachCol(int col, int bit) { m_colMap[col] = uint8_t(bit); }   // 5 columns, anodes.

    void loop(uint32_t microSeconds, uint8_t* a, uint8_t* b);

    void setFrom7_5(const uint8_t* pixel7_5ui);

private:
    static const uint32_t TIME_PER_DIGIT = 4000; // microSeconds
    static const int NUM_ROWS = 7;
    static const int NUM_COLS = 5;

    uint8_t m_display[NUM_COLS];    // map of the display - each column is 7 bits of the byte    
    uint8_t m_rowMap[NUM_ROWS];     // maps rows -> pins
    uint8_t m_colMap[NUM_COLS];     // maps cols -> pins
};

#endif

