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
    void attachDigit(int count, const uint8_t* bits) {
        for(int i=0; i<count; ++i) attachDigit(i, int(bits[i]));
    }

    // Attach segment 0-7 (a-h) to output 0-15 of the shift register.
    void attachSegment(int segment, int bit);
    void attachSegment(int count, const uint8_t* bits) {
        for(int i=0; i<count; ++i) attachSegment(i, int(bits[i]));
    }

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
    void attachRow(int count, const uint8_t* rows) {
        for(int i=0; i<count; ++i) attachRow(i, int(rows[i]));
    }
    void attachCol(int col, int bit) { m_colMap[col] = uint8_t(bit); }   // 5 columns, anodes.
    void attachCol(int count, const uint8_t* cols) {
        for(int i=0; i<count; ++i) attachCol(i, int(cols[i]));
    }

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

