#include <stdint.h>

static uint8_t ANODE_COL_1 = 20;
static uint8_t ANODE_COL_2 = 19;
static uint8_t ANODE_COL_3 = 18;
static uint8_t ANODE_COL_4 = 17;
static uint8_t ANODE_COL_5 = 16;

static uint8_t CATHODE_ROW_1 = 5;
static uint8_t CATHODE_ROW_2 = 6;
static uint8_t CATHODE_ROW_3 = 7;
static uint8_t CATHODE_ROW_4 = 8;
static uint8_t CATHODE_ROW_5 = 9;
static uint8_t CATHODE_ROW_6 = 10;
static uint8_t CATHODE_ROW_7 = 11;

static const int NUM_ANODES = 5;
static const int NUM_CATHODES = 7;

static uint8_t ANODES[NUM_ANODES] = { 
  ANODE_COL_1,
  ANODE_COL_2,
  ANODE_COL_3,
  ANODE_COL_4,
  ANODE_COL_5
};

static uint8_t CATHODES[NUM_CATHODES] = {
  CATHODE_ROW_1,
  CATHODE_ROW_2,
  CATHODE_ROW_3,
  CATHODE_ROW_4,
  CATHODE_ROW_5,
  CATHODE_ROW_6,
  CATHODE_ROW_7
};

static uint8_t testPx[NUM_ANODES] = {
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001111
};

/*
static uint8_t testPx[NUM_ANODES] = {
  B01110101,
  B01010101,
  B01110111,
  B01000001,
  B01000001
};
*/

const uint8_t tomthumbnewBitmaps[] =
{
  // ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### 
  // # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # ### 
  // # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # ### 
  // # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # ### 
  // ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### 
  //                                                                                                                                 
  //      #  # # # #  ## #   ##   #    # #   # #                   #  ##  #  ##  ##  # # ###  ## ### ### ###           #     #   ### 
  //      #  # # ### ##    # ##   #   #   #   #   #                # # # ##    #   # # # #   #     # # # # #  #   #   #  ###  #    # 
  //      #      # #  ##  #  ###      #   #  # # ###     ###      #  # #  #   #   #  ### ##  ###  #  ### ###         #         #  #  
  //             ### ##  #   # #      #   #       #   #          #   # #  #  #     #   #   # # # #   # #   #  #   #   #  ###  #      
  //      #      # #  #    #  ##       # #           #        #  #   ##   #  ### ##    # ##  ### #   ### ##      #     #     #    #  
  //                                                                                                                                 
  //  #   #  ##   ## ##  ### ###  ## # # ###   # # # #   # # # #  #  ##   #  ##   ## ### # # # # # # # # # # ### ###     ###  #      
  // # # # # # # #   # # #   #   #   # #  #    # # # #   ### ### # # # # # # # # #    #  # # # # # # # # # #   # #   #     # # #     
  // ### ### ##  #   # # ### ### ### ###  #    # ##  #   ### ### # # ##  # # ###  #   #  # # # # ###  #   #   #  #    #    #         
  // #   # # # # #   # # #   #   # # # #  #  # # # # #   # # ### # # #   ### ##    #  #  # #  #  ### # #  #  #   #     #   #         
  //  ## # # ##   ## ##  ### #    ## # # ###  #  # # ### # # # #  #  #    ## # # ##   #   ##  #  # # # #  #  ### ###     ###     ### 
  //                                                                                                                                 
  // #       #         #       #     #    #    # #   ##                               #                           ##  #  ##   ## ### 
  //  #  ##  ##   ##  ##  ##  #   ## ##          # #  #  ### ##   #  ##   ##  ##  ## ### # # # # # # # # # # ###  #   #   #  ##  ### 
  //      ## # # #   # # # # ### # # # #  #    # ##   #  ### # # # # # # # # #   ##   #  # # # # ###  #  # #  ## #         #     ### 
  //     # # # # #   # # ##   #  ### # #  #    # ##   #  ### # # # # # # # # #    ##  #  # # ### ###  #   ## ##   #   #   #      ### 
  //     ### ##   ##  ##  ##  #    # # #  #  # # # # ### # # # #  #  ##   ## #   ##   ##  ##  #  ### # #   # ###  ##  #  ##      ### 
  //                              #           #                      #     #                              #                          
  0x1F, 0x11, 0x1F, 0x00, 0x1F, 0xD1, 0x1F, 0x00, 0xDF, 0x11, 0xDF, 0x00, 0xDF, 0x91, 0xDF, 0x00, 0x9F, 0xD1, 0x5F, 0x00, 0x5F, 0x11, 0x9F, 0x00, 0xDF, 0xD1, 0x1F, 0x00, 0x1F, 0xD1, 0x1F, 0x00, 0x1F, 0x91, 0x5F, 0x00, 0x5F, 0x91, 0x1F, 0x00, 0x5F, 0x91, 0x5F, 0x00, 0x1F, 0x91, 0x1F, 0x00, 0x1F, 0x11, 0x1F, 0x00, 0x1F, 0x11, 0x1F, 0x00, 0x1F, 0x11, 0x1F, 0x00, 0x1F, 0x11, 0xDF, 0x00, 0x9F, 0x51, 0xDF, 0x00, 0x9F, 0xD1, 0x1F, 0x00, 0x5F, 0x51, 0x9F, 0x00, 0x5F, 0x51, 0x9F, 0x00, 0xDF, 0x11, 0xDF, 0x00, 0xDF, 0x51, 0x5F, 0x00, 0x9F, 0x51, 0x5F, 0x00, 0x5F, 0x51, 0xDF, 0x00, 0xDF, 0x51, 0xDF, 0x00, 0xDF, 0x51, 0xDF, 0x00, 0x1F, 0x91, 0x1F, 0x00, 0x1F, 0x91, 0x1F, 0x00, 0x1F, 0x91, 0x5F, 0x00, 0x9F, 0x91, 0x9F, 0x00, 0x5F, 0x91, 0x1F, 0x00, 0x5F, 0x5F, 0xDF, 0x00, 
  0xE0, 0x50, 0x60, 0x00, 0xE0, 0x55, 0xE0, 0x00, 0xF0, 0x50, 0xA0, 0x00, 0xE7, 0x12, 0x17, 0x00, 0xF2, 0x17, 0xE1, 0x00, 0xF2, 0x51, 0x54, 0x00, 0xF3, 0x55, 0x57, 0x00, 0xE0, 0x50, 0xD0, 0x00, 0xF0, 0x43, 0xF4, 0x00, 0x14, 0xF3, 0x10, 0x00, 0x81, 0x00, 0xF1, 0x00, 0xF1, 0x43, 0xB1, 0x00, 0xF4, 0x02, 0x00, 0x00, 0xF1, 0x61, 0xF1, 0x00, 0xF0, 0xE4, 0xF0, 0x00, 0xE6, 0x11, 0xE0, 0x00, 0xF7, 0x54, 0x23, 0x00, 0xE0, 0x97, 0xE0, 0x00, 0xF6, 0xD5, 0x64, 0x00, 0x24, 0x55, 0x92, 0x00, 0x11, 0xF1, 0x17, 0x00, 0xF5, 0x05, 0xF2, 0x00, 0x77, 0x85, 0x77, 0x00, 0xF6, 0xC1, 0xF0, 0x00, 0xB7, 0x45, 0xB7, 0x00, 0x35, 0xC5, 0x33, 0x00, 0x90, 0x52, 0x30, 0x00, 0xF4, 0x12, 0x10, 0x00, 0x21, 0x42, 0x84, 0x00, 0x12, 0x12, 0xF2, 0x00, 0x24, 0x12, 0x21, 0x00, 0x00, 0x05, 0x00, 0x00, 
  0x04, 0x09, 0x01, 0x00, 0x69, 0x58, 0x71, 0x00, 0x7D, 0x49, 0x30, 0x00, 0x30, 0x49, 0x49, 0x00, 0x31, 0x49, 0x7C, 0x00, 0x31, 0x69, 0x59, 0x00, 0x11, 0x78, 0x14, 0x00, 0x30, 0xA9, 0x79, 0x00, 0x7D, 0x08, 0x71, 0x00, 0x01, 0x75, 0x01, 0x00, 0x40, 0x81, 0x74, 0x00, 0x7D, 0x30, 0x49, 0x00, 0x45, 0x7D, 0x41, 0x00, 0x79, 0x38, 0x79, 0x00, 0x79, 0x08, 0x71, 0x00, 0x30, 0x49, 0x30, 0x00, 0xF9, 0x48, 0x30, 0x00, 0x30, 0x49, 0xF9, 0x00, 0x71, 0x08, 0x09, 0x00, 0x51, 0x79, 0x28, 0x00, 0x08, 0x7D, 0x48, 0x00, 0x38, 0x41, 0x79, 0x00, 0x38, 0x61, 0x38, 0x00, 0x79, 0x70, 0x79, 0x00, 0x49, 0x30, 0x49, 0x00, 0x18, 0xA1, 0x78, 0x00, 0x69, 0x79, 0x59, 0x00, 0x11, 0x6D, 0x45, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x45, 0x6D, 0x11, 0x00, 0x08, 0x0C, 0x04, 0x00, 0x7D, 0x7D, 0x7D, 0x00, 
};

// This could be more efficient; core loop goes in the wrong direction.
// Could use bit shift / mask to populate glyph[]
//
const uint8_t* getGlypth_tomThumb35(int charID)
{
  // 4x6
  // 3x5 usable

  static uint8_t glyph[4];

  if (charID > 0 && charID < 128) {
    const int row = charID / 32;
    const int col = charID % 32;
    const int x = col * 4;
    const int y = row * 6;

    for (int i = 0; i < 4; ++i) {
      glyph[i] = 0;
    }
    for (int j = 0; j < 5; ++j) {
      const int ty = y + j;
  
      for (int i = 0; i < 3; ++i) {
        const int tx = x + i;
        const int pillar = tx + (ty / 8) * 128;
        const int bit = 1 << (ty & 0x07);

        if (tomthumbnewBitmaps[pillar] & bit) {
          int targetBit = 1 << j;
          glyph[i] = glyph[i] | targetBit;
        }
      }
    }

    *advance = 4;
    *w = 3;
    *rows = 1; 
    return glyph;
  }
  return 0;
}


void setup() {
  for(int i=0; i<NUM_ANODES; ++i) {
    pinMode(ANODES[i], OUTPUT);
    digitalWrite(ANODES[i], LOW);
  }
  for(int i=0; i<NUM_CATHODES; ++i) {
    pinMode(CATHODES[i], OUTPUT);
    digitalWrite(CATHODES[i], HIGH);
  }
 }

void loop() {
  int advance = 0;
  int w = 0;
  int rows = 0;
  const uint8_t* g = Glypth_tomThumb5('3', &advance, &w, &rows);
  
  for(uint8_t i=0; i<NUM_ANODES; ++i) {
    digitalWrite(ANODES[i], HIGH);
    for(uint8_t r=0; r<NUM_CATHODES; ++r) {
      //digitalWrite(CATHODES[r], (testPx[i] & (1<<r)) ? LOW : HIGH);
      digitalWrite(CATHODES[r], (testPx[i] & (1<<r)) ? LOW : HIGH);
    }
    delay(2);
    digitalWrite(ANODES[i], LOW);
  }
}

