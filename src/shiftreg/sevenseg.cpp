#include "sevenseg.h"

void SevenSeg::set(const char* str)
{
    static constexpr int NLETTERS = 75;
    static const uint8_t sevenSecABC[NLETTERS]= {
    /*  0     1     2     3     4     5     6     7     8     9     :     ;     */
        0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x00, 0x00, 
    /*  <     =     >     ?     @     A     B     C     D     E     F     G     */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E, 
    /*  H     I     J     K     L     M     N     O     P     Q     R     S     */
        0x37, 0x06, 0x3C, 0x57, 0x0E, 0x55, 0x15, 0x1D, 0x67, 0x73, 0x05, 0x5B, 
    /*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
        0x0F, 0x3E, 0x1C, 0x5C, 0x13, 0x3B, 0x6D, 0x00, 0x00, 0x00, 0x00, 0x00, 
    /*  `     a     b     c     d     e     f     g     h     i     j     k     */
        0x00, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E, 0x37, 0x06, 0x3C, 0x57, 
    /*  l     m     n     o     p     q     r     s     t     u     v     w     */
        0x0E, 0x55, 0x15, 0x1D, 0x67, 0x73, 0x05, 0x5B, 0x0F, 0x3E, 0x1C, 0x5C, 
    /*  x     y     z     */
        0x13, 0x3B, 0x6D
    };

    // Somewhat mind melting.
    //      Bubble
    // a1   g       (1 << 0)
    // a2   d       (1 << 1)
    // a3   f       (1 << 2)
    // a4   digit 2             (1 << 3)
    // a5   b       (1 << 4)
    // a6   a       (1 << 5)
    // a7   
    // b1   digit 1             (1 << 8)
    // b2   e       (1 << 9)
    // b3   c       (1 << 10)
    // b4   digit 3             (1 << 11)
    // b5
    // b6   digit 4             (1 << 13)
    // b7

    static constexpr uint16_t abcToBubble[8] = {
        (1<<0),
        (1<<2),
        (1<<9),
        (1<<1),
        (1<<10),
        (1<<4),
        (1<<5),
    };

    for(int i=0; i<4; i++) {
        m_value[i] = 0;
        uint8_t ascii = str[i] > 90 ? str[i] - 32 : str[i];
        if (ascii < 48 || ascii > 122) {
            continue;
        }
        uint8_t index = ascii - 48;
        uint8_t sevenSeg = sevenSecABC[index];
        for(int j=0; j<8; ++j) {
            if (sevenSeg & (1 << j)) {
                m_value[i] |= abcToBubble[j];
            }
        }
    }
}

void SevenSeg::loop(uint32_t now)
{
    static constexpr uint32_t DIV = 11;
    uint32_t cycle = now / DIV;
    uint32_t index = cycle % 4;
    uint32_t value = m_value[index];

    static const uint16_t digit[4] = {
        (1<<8),
        (1<<3),
        (1<<11),
        (1<<13),
    };

    value |= digit[index];
    m_reg.set(value);
}
