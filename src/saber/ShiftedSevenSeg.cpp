#include <Arduino.h>
#include "ShiftedSevenSeg.h"

// Partially from https://github.com/DeanIsMe/SevSeg
static const uint8_t CODE_MAP[] = {
  //     GFEDCBA  Segments      7-segment map:
  B01000000, //     '-'  DASH / MINUS
  0,         // filler
  0,         // filler
  B00111111, // 0   "0"          AAA
  B00000110, // 1   "1"         F   B
  B01011011, // 2   "2"         F   B
  B01001111, // 3   "3"          GGG
  B01100110, // 4   "4"         E   C
  B01101101, // 5   "5"         E   C
  B01111101, // 6   "6"          DDD
  B00000111, // 7   "7"
  B01111111, // 8   "8"
  B01101111, // 9   "9"
  0,         // filler
  0,         // filler
  0,         // filler
  0,         // filler
  0,         // filler
  0,         // filler
  0,         // filler
  B01110111, // 65  'A'
  B01111100, // 66  'b'
  B00111001, // 67  'C'
  B01011110, // 68  'd'
  B01111001, // 69  'E'
  B01110001, // 70  'F'
  B00111101, // 71  'G'
  B01110110, // 72  'H'
  B00000110, // 73  'I'
  B00011110, // 74  'J'
  B01110110, // 75  'K'  Same as 'H'
  B00111000, // 76  'L'
  B01010100, // 77  'M'  Same as n
  B01010100, // 78  'n'
  B00111111, // 79  'O'
  B01110011, // 80  'P'
  B01100111, // 81  'q'
  B01010000, // 82  'r'
  B01101101, // 83  'S'
  B01111000, // 84  't'
  B00111110, // 85  'U'
  B00111110, // 86  'V'  Same as 'U'
  B00010100, // 87  'W'  | |
  B01110110, // 88  'X'  Same as 'H'
  B01101110, // 89  'y'
  B01011011, // 90  'Z'  Same as '2'
};

static const char MAP_START = '-';
static const char MAP_END = 'Z';


ShiftedSevenSegment::ShiftedSevenSegment()
{
    for(int i=0; i<NUM_DIGITS; ++i) m_digits[i] = 0;
    for(int i=0; i<NUM_DIGITS; ++i) m_digitMap[i] = i;
    for(int i=0; i<NUM_SEGMENTS; ++i) m_segmentMap[i] = i;

    m_digits[0] = CODE_MAP[0];
    m_digits[1] = CODE_MAP[1];
    m_digits[2] = CODE_MAP[2];
    m_digits[3] = CODE_MAP[3];
}


void ShiftedSevenSegment::attachDigit(int digit, int bit)
{
    m_digitMap[digit] = uint8_t(bit);
}


void ShiftedSevenSegment::attachSegment(int segment, int bit)
{
    m_segmentMap[segment] = uint8_t(bit);
}


void ShiftedSevenSegment::set(const char* str) 
{
    for(int i=0; i<NUM_DIGITS; ++i) 
      m_digits[i] = 0;

    for(int i=0, d=0; d < NUM_DIGITS && str[i]; ++i) {
        char c = toupper(str[i]);        
        if (c >= MAP_START && c <= MAP_END) {
            m_digits[d] = CODE_MAP[c - MAP_START];
            if (str[i+1] == '.') {
                m_digits[d] |= B10000000;
                ++i;
            }
        }
        ++d;
    }
}


void ShiftedSevenSegment::loop(uint32_t micro, uint8_t* low, uint8_t* high) 
{
    uint32_t digit = (micro / TIME_PER_DIGIT) % NUM_DIGITS;
    uint8_t  b = m_digits[digit];

    // Default everything high. Will fix the digits,
    // and this turns all the segments "off"
    uint16_t out = 0x00;

    // First turn on (low) or off (high) the digit.
    for(unsigned i=0; i<NUM_DIGITS; ++i) {
        bitWrite(out, m_digitMap[i], i == digit ? 0 : 1);
    }

    // Set to 1 (high) each segment that is on.
    for(int i=0; i<8; ++i) {
        if (bitRead(b, i)) {
            bitWrite(out, m_segmentMap[i], 1);
        }
    }
    
    *low  = lowByte(out);
    *high = highByte(out);
}


ShiftedDotMatrix::ShiftedDotMatrix()
{
    for(int i=0; i<NUM_COLS; ++i) {
        m_display[i] = 0;
        m_colMap[i] = 0;
    }
    for(int i=0; i<NUM_ROWS; ++i) {
        m_rowMap[i] = 0;
    }
}


void ShiftedDotMatrix::loop(uint32_t micro, uint8_t* low, uint8_t* high) 
{
    uint32_t col = (micro / TIME_PER_DIGIT) % NUM_COLS;
    uint8_t  b = m_display[col];

    uint16_t out = 0xff;

    // First turn on/off the columns.
    for(unsigned i=0; i<NUM_COLS; ++i) {
        bitWrite(out, m_colMap[i], i == col ? 1 : 0);
    }

    // And now the rows.
    for(int i=0; i<NUM_ROWS; ++i) {
        if (bitRead(b, i)) {
            bitWrite(out, m_segmentMap[i], 0);
        }
    }
    
    *low  = lowByte(out);
    *high = highByte(out);
}


void ShiftedDotMatrix::setFrom7_5(const uint8_t* pixel7_5ui)
{
    for(int i=0; i<NUM_COLS; ++i)
        m_display[i] = 0;

    for(int r=0; r<NUM_ROWS; ++r) {
        for(int c=0; c<NUM_COLS; ++c) {
            unsigned b = bitRead(pixel7_5ui[r], c);
            bitWrite(m_display[c], r, b):
        }
    }
}
