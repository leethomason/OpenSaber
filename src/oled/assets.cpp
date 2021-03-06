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

#include "assets.h"
#include <stdint.h>

char ToUpper(char c) {
	if (c >= 'a' && c <= 'z') {
		c = 'A' + c - 'a';
	}
	return c;
}

struct GlypthInfo {
	uint8_t width;
	uint16_t offset;
};

const uint8_t jBirdBitmap[] =
{
	//                                 
	//        #                #       
	//       ##                ##      
	//       ##       ##       ##      
	//      ##        ##        ##     
	//     ###        ##        ###    
	//     ###        ##        ###    
	//    ####        ##        ####   
	//    ####        ##        ####   
	//  # #####       ##        #### # 
	//  # #####       ##       ##### # 
	// ## ######      ##      ###### ##
	// ## ######      ##      ###### ##
	// ## #######     ##     ####### ##
	// ## ######      ##      ###### ##
	// ## ######      ##      ###### ##
	// ### #####      ##      ##### ###
	// ### ####       ##       #### ###
	//  #######       ##       ####### 
	//  #######       ##       ####### 
	//   ######       ##       ######  
	//  #######     # ## #     ####### 
	//  ########     ####      ####### 
	//   #######   ########   #######  
	//    ######     ####     #######  
	//    #######   # ## #   #######   
	//     #######    ##    #######    
	//      #######   ##   #######     
	//        ######  ##  ######       
	//         ################        
	//            ##########           
	//                                 
	0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xFC, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xFC, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00,
	0xF8, 0xFE, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xF8, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFE, 0xF8,
	0x03, 0x6F, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xC1, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xC0, 0xFF, 0xFF, 0xC0, 0xA0, 0x80, 0x00, 0x00, 0x00, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0x6F, 0x03,
	0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x0F, 0x1F, 0x3F, 0x3E, 0x3C, 0x78, 0x70, 0x62, 0x61, 0x7F, 0x7F, 0x61, 0x62, 0x70, 0x78, 0x3C, 0x3E, 0x3F, 0x1F, 0x0F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00,
};

const uint8_t* get_jBird(int* width, int* height)
{
	if (width) *width = 32;
	if (height) *height = 32;
	return jBirdBitmap;
}


// 
//  Font data for Aurek-Besh 6pt
// 

// Character bitmaps for Aurek-Besh 6pt
const uint8_t aurekBesh_6ptBitmaps[] = 
{
	// @0 ',' (1 pixels wide)
	//  
	//  
	//  
	//  
	// #
	// #
	// #
	// #
	0xF0, 

	// @1 '.' (4 pixels wide)
	//     
	//     
	//     
	//     
	// # ##
	// # ##
	// # ##
	// # ##
	0xF0, 0x00, 0xF0, 0xF0, 

	// @5 'A' (13 pixels wide)
	// ##       ####
	// ##     ####  
	// #########    
	// #######      
	//              
	// #########    
	// ##     ####  
	// ##       ### 
	0xEF, 0xEF, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x66, 0x66, 0xC3, 0xC3, 0x81, 0x01, 

	// @18 'B' (12 pixels wide)
	//   ########  
	//  ########## 
	// ###       ##
	//     #####   
	//     #####   
	// ##        ##
	//  ########## 
	//   ######### 
	0x24, 0x66, 0xC7, 0xC3, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xC3, 0xE6, 0x24, 

	// @30 'C' (7 pixels wide)
	// ##     
	// ##     
	// ##     
	// ## # ##
	// ## # ##
	// ## # ##
	//      ##
	//      ##
	0x3F, 0x3F, 0x00, 0x38, 0x00, 0xF8, 0xF8, 

	// @37 'D' (9 pixels wide)
	// #########
	// ######## 
	//      ##  
	//  #####   
	// #####    
	//   ##     
	//  ##      
	// ###      
	0x93, 0xDB, 0xFB, 0x3B, 0x1B, 0x0F, 0x07, 0x03, 0x01, 

	// @46 'E' (12 pixels wide)
	// ##      ####
	//  ##    #####
	//  ##    #### 
	//   ##  ## ## 
	//   ##  ## ## 
	//    ####  ## 
	//    ####  ## 
	//     ##   ## 
	0x01, 0x07, 0x1E, 0x78, 0xE0, 0xE0, 0x78, 0x1E, 0x07, 0xFF, 0xFF, 0x03, 

	// @58 'F' (12 pixels wide)
	//      ##    #
	//      ##  ###
	//     ####### 
	// ########    
	// ##   ##     
	// ##   ##     
	// ############
	// ############
	0xF8, 0xF8, 0xC8, 0xC8, 0xCC, 0xFF, 0xFF, 0xCC, 0xC4, 0xC6, 0xC6, 0xC3, 

	// @70 'G' (10 pixels wide)
	// # ########
	// # ########
	// #      ## 
	// #      ## 
	// #     ##  
	// #     ##  
	// #######   
	// #######   
	0xFF, 0xC0, 0xC3, 0xC3, 0xC3, 0xC3, 0xF3, 0x3F, 0x0F, 0x03, 

	// @80 'H' (12 pixels wide)
	// ############
	// ############
	//             
	//   ########  
	//   ########  
	//             
	// ############
	// ############
	0xC3, 0xC3, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xC3, 0xC3, 

	// @92 'I' (5 pixels wide)
	//   ###
	//  ####
	// #####
	//  # ##
	//    ##
	//    ##
	//    ##
	//    ##
	0x04, 0x0E, 0x07, 0xFF, 0xFF, 

	// @97 'J' (11 pixels wide)
	//          ##
	//         ###
	//       #### 
	// ########## 
	// ###### ##  
	//        ##  
	// ########   
	// ########   
	0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xCC, 0xFC, 0x3E, 0x0F, 0x03, 

	// @108 'K' (12 pixels wide)
	// ############
	// ############
	//           ##
	//           ##
	//           ##
	//           ##
	// ############
	// ############
	0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 

	// @120 'L' (7 pixels wide)
	//      ##
	//      ##
	//      ##
	// #    ##
	// ##   ##
	//  ### ##
	//   #####
	//     ###
	0x18, 0x30, 0x60, 0x60, 0xC0, 0xFF, 0xFF, 

	// @127 'M' (7 pixels wide)
	//    ### 
	//    ### 
	//   ##   
	//   ##   
	//  ##    
	//  ##    
	// #######
	// ###### 
	0xC0, 0xF0, 0xFC, 0xCF, 0xC3, 0xC3, 0x40, 

	// @134 'N' (12 pixels wide)
	//  ##   ##    
	//  #   ####   
	// ##   ####   
	// ##  ##  ##  
	// ##  ##   #  
	//  # ##    ## 
	//  ####     ##
	//  ###      ##
	0x1C, 0xFF, 0xC1, 0xE0, 0x78, 0x1E, 0x07, 0x07, 0x0E, 0x38, 0xE0, 0xC0, 

	// @146 'O' (11 pixels wide)
	//    #####   
	//   #######  
	//   ##   ##  
	//  ##     ## 
	//  ##     ## 
	//  #       ##
	// ###########
	//  ######### 
	0x40, 0xF8, 0xDE, 0xC7, 0xC3, 0xC3, 0xC3, 0xC7, 0xDE, 0xF8, 0x60, 

	// @157 'P' (8 pixels wide)
	//   ### ##
	//  #### ##
	//  #    ##
	// ##    ##
	// ##    ##
	// ##    ##
	//  #######
	//  #######
	0x38, 0xFE, 0xC3, 0xC3, 0xC3, 0xC0, 0xFF, 0xFF, 

	// @165 'Q' (10 pixels wide)
	// ##########
	// ##########
	// ##       #
	// ##        
	// ##        
	// ##        
	// #######   
	// ######    
	0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0x43, 0x03, 0x03, 0x07, 

	// @175 'R' (9 pixels wide)
	//  ########
	// ######## 
	//      ##  
	//     ##   
	//    ###   
	//   ###    
	//   ##     
	//  ##      
	0x02, 0x83, 0xE3, 0x73, 0x3B, 0x1F, 0x07, 0x03, 0x01, 

	// @184 'S' (11 pixels wide)
	//   ##      #
	//    ##     #
	//     ##    #
	//      ##   #
	//       ##  #
	// ####   ## #
	//   ##### ###
	//      #   ##
	0x20, 0x20, 0x61, 0x63, 0x46, 0xCC, 0x58, 0x30, 0x60, 0xC0, 0xFF, 

	// @195 'T' (11 pixels wide)
	//      #     
	//      #     
	//      #     
	//      #     
	// ##   #   ##
	//  ##  #  ## 
	//   #### ##  
	//    #####   
	0x10, 0x30, 0x60, 0xC0, 0xC0, 0xFF, 0x80, 0xC0, 0x60, 0x30, 0x10, 

	// @206 'U' (11 pixels wide)
	// ##    #####
	// ##   ######
	// ##  ###  ##
	// ##  ##   ##
	// ##       ##
	// ##       ##
	// ###########
	// ###########
	0xFF, 0xFF, 0xC0, 0xC0, 0xCC, 0xCE, 0xC7, 0xC3, 0xC3, 0xFF, 0xFF, 

	// @217 'V' (9 pixels wide)
	// ##    ###
	//  ##  ### 
	//   #####  
	//    ###   
	//    ##    
	//    ##    
	//    ##    
	//    ##    
	0x01, 0x03, 0x06, 0xFC, 0xFC, 0x0E, 0x07, 0x03, 0x01, 

	// @226 'W' (12 pixels wide)
	// ########### 
	// ############
	// ##        ##
	// ##        ##
	// ##        ##
	// ##        ##
	// ############
	// ############
	0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFE, 

	// @238 'X' (8 pixels wide)
	//    ##   
	//   ###   
	//   ####  
	//  ##  #  
	//  ##  ## 
	// ##    ##
	// ########
	// ########
	0xE0, 0xF8, 0xDE, 0xC7, 0xC7, 0xDC, 0xF0, 0xE0, 

	// @246 'Y' (11 pixels wide)
	// ####     ##
	//  # ##    # 
	//  ## ##  ## 
	//   ##    #  
	//   ##   ##  
	//    ## ##   
	//    ## ##   
	//     ###    
	0x01, 0x07, 0x1D, 0x7B, 0xE6, 0x84, 0xE0, 0x70, 0x1C, 0x07, 0x01, 

	// @257 'Z' (10 pixels wide)
	//         ##
	//         ##
	//    #######
	//   ########
	//  ###    ##
	// ##      ##
	// ##########
	// ##########
	0xE0, 0xF0, 0xD8, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0xFF, 
};

// Character descriptors for Aurek-Besh 6pt
// { [Char width in bits], [Offset into aurekBesh_6ptCharBitmaps in bytes] }
const GlypthInfo aurekBesh_6ptDescriptors[] = 
{
	{1, 0}, 		// , 
	{0, 0}, 		// - 
	{4, 1}, 		// . 
	{0, 0}, 		// / 
	{0, 0}, 		// 0 
	{0, 0}, 		// 1 
	{0, 0}, 		// 2 
	{0, 0}, 		// 3 
	{0, 0}, 		// 4 
	{0, 0}, 		// 5 
	{0, 0}, 		// 6 
	{0, 0}, 		// 7 
	{0, 0}, 		// 8 
	{0, 0}, 		// 9 
	{0, 0}, 		// : 
	{0, 0}, 		// ; 
	{0, 0}, 		// < 
	{0, 0}, 		// = 
	{0, 0}, 		// > 
	{0, 0}, 		// ? 
	{0, 0}, 		// @ 
	{13, 5}, 		// A 
	{12, 18}, 		// B 
	{7, 30}, 		// C 
	{9, 37}, 		// D 
	{12, 46}, 		// E 
	{12, 58}, 		// F 
	{10, 70}, 		// G 
	{12, 80}, 		// H 
	{5, 92}, 		// I 
	{11, 97}, 		// J 
	{12, 108}, 		// K 
	{7, 120}, 		// L 
	{7, 127}, 		// M 
	{12, 134}, 		// N 
	{11, 146}, 		// O 
	{8, 157}, 		// P 
	{10, 165}, 		// Q 
	{9, 175}, 		// R 
	{11, 184}, 		// S 
	{11, 195}, 		// T 
	{11, 206}, 		// U 
	{9, 217}, 		// V 
	{12, 226}, 		// W 
	{8, 238}, 		// X 
	{11, 246}, 		// Y 
	{10, 257}, 		// Z 
};


const uint8_t* getGlypth_aurekBesh6(int charID, int* advance, int* w, int* rows)
{
	charID = ToUpper(charID);
	if (charID >= ',' && charID <= 'Z') {
		int index = charID - ',';
		int width = aurekBesh_6ptDescriptors[index].width;
		int offset = aurekBesh_6ptDescriptors[index].offset;
		*advance = width + 2;
		*w = width;
		*rows = 1;
		return aurekBesh_6ptBitmaps + offset;
	}
	*advance = 4;
	*w = 0;
	*rows = 0;
	return 0;
}


// 
//  Font data for Calibri 8pt
// 

// Character bitmaps for Calibri 8pt
const uint8_t calibri_8ptBitmaps[] = 
{
	// @0 '-' (3 pixels wide)
	//    
	//    
	//    
	//    
	// ###
	//    
	//    
	//    
	//    
	0x10, 0x10, 0x10,
	0x00, 0x00, 0x00,

	// @6 '.' (1 pixels wide)
	//  
	//   
	//   
	//   
	//   
	//   
	//  #
	//  
	//  
	0x40,
	0x00,

	// @8 '0' (4 pixels wide)
	//  ###
	// #  #
	// #  #
	// #  #
	// #  #
	// #  #
	// ### 
	//     
	//     
	0x7E, 0x41, 0x41, 0x3F,
	0x00, 0x00, 0x00, 0x00,

	// @16 '1' (4 pixels wide)
	//  ## 
	// # # 
	//  #
	// # 
	//   # 
	//   # 
	// ####
	//     
	//     
	0x42, 0x41, 0x7F, 0x40,
	0x00, 0x00, 0x00, 0x00,

	// @24 '2' (4 pixels wide)
	//  ## 
	// #  #
	//    #
	//   ##
	//   # 
	//  #  
	// ####
	//     
	//     
	0x42, 0x61, 0x59, 0x4E,
	0x00, 0x00, 0x00, 0x00,

	// @32 '3' (4 pixels wide)
	// ####
	//    #
	//    #
	//  ## 
	//    #
	//    #
	// ### 
	//     
	//     
	0x41, 0x49, 0x49, 0x37,
	0x00, 0x00, 0x00, 0x00,

	// @40 '4' (5 pixels wide)
	//   ## 
	//  # # 
	// ## # 
	// #  # 
	// #####
	//    # 
	//    # 
	//  
	//  
	0x1C, 0x16, 0x11, 0x7F, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @50 '5' (4 pixels wide)
	// ####
	// #   
	// #   
	// ### 
	//    #
	//    #
	// ### 
	//  
	//  
	0x4F, 0x49, 0x49, 0x31,
	0x00, 0x00, 0x00, 0x00,

	// @58 '6' (4 pixels wide)
	//  ###
	// #   
	// #   
	// ####
	// #  #
	// #  #
	//  ## 
	//  
	//     
	0x3E, 0x49, 0x49, 0x39,
	0x00, 0x00, 0x00, 0x00,

	// @66 '7' (5 pixels wide)
	// #####
	//    ##
	//    # 
	//   ## 
	// #
	//  ##  
	// #
	//  
	//      
	0x01, 0x61, 0x39, 0x0F, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @76 '8' (5 pixels wide)
	//  ### 
	//   ##  
	//  # #  
	//  ### 
	// #   #
	//  # #  
	//  # ## 
	//      
	//      
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @86 '9' (4 pixels wide)
	//  ## 
	// #  #
	// #   # 
	// ####
	//    #
	//    #
	// #   ##
	//       
	//     
	0x4E, 0x49, 0x49, 0x3E,
	0x00, 0x00, 0x00, 0x00,

	// @94 'A' (6 pixels wide)
	//   ##  
	//   ##  
	//   # # 
	//  #  # 
	//  #### 
	//  #   #
	// #    #
	//       
	//       
	0x40, 0x38, 0x17, 0x13, 0x1C, 0x60,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @106 'B' (5 pixels wide)
	// ####
	// #  #
	// #  #
	// ####
	// #  #
	// #  #
	// #### 
	//      
	//     
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @116 'C' (5 pixels wide)
	//  ### 
	// ##  #
	// #    
	// #    
	// #    
	// ##  #
	//  ### 
	//      
	//      
	0x3E, 0x63, 0x41, 0x41, 0x22, 
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @126 'D' (5 pixels wide)
	// #### 
	// #   ##
	// #    #
	// #    #
	// #    #
	// #   ##
	// #### 
	//      
	//       
	0x7F, 0x41, 0x41, 0x63, 0x3E,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @136 'E' (4 pixels wide)
	// ####
	// #   
	// #   
	// ####
	// #   
	// #   
	// ####
	//     
	//     
	0x7F, 0x49, 0x49, 0x49, 
	0x00, 0x00, 0x00, 0x00,

	// @144 'F' (4 pixels wide)
	// ####
	// #   
	// #   
	// ####
	// #   
	// #   
	// #   
	//     
	//     
	0x7F, 0x09, 0x09, 0x09, 
	0x00, 0x00, 0x00, 0x00,

	// @152 'G' (5 pixels wide)
	//  ####
	//   ### 
	// #     
	// #  ##
	// #    #
	// ##   #
	//   ####
	//       
	//      
	0x3E, 0x63, 0x41, 0x49, 0x7B,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @162 'H' (5 pixels wide)
	// #   #
	// #   #
	// #   #
	// #####
	// #   #
	// #   #
	// #   #
	//      
	//      
	0x7F, 0x08, 0x08, 0x08, 0x7F, 
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @172 'I' (1 pixels wide)
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	//  
	//  
	0x7F, 
	0x00,

	// @174 'J' (3 pixels wide)
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	// ###
	//    
	//   
	0x40, 0x40, 0x7F,
	0x00, 0x00, 0x00,

	// @180 'K' (4 pixels wide)
	// #   #
	// #  # 
	// # #  
	// ##   
	// # #  
	// #  # 
	// #   #
	//      
	//     
	0x7F, 0x08, 0x16, 0x61,
	0x00, 0x00, 0x00, 0x00,

	// @188 'L' (4 pixels wide)
	// #   
	// #   
	// #   
	// #   
	// #   
	// #   
	// ####
	//     
	//     
	0x7F, 0x40, 0x40, 0x40, 
	0x00, 0x00, 0x00, 0x00,

	// @196 'M' (8 pixels wide)
	// ##    ##
	// ##    ##
	// ##    ##
	// # #  # #
	// # #  # #
	// #  ##  #
	// #  ##  #
	//         
	//         
	0x7F, 0x03, 0x0C, 0x70, 0x70, 0x0C, 0x03, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @212 'N' (5 pixels wide)
	// ##  #
	// ##   #
	// ##   #
	// # #  #
	// #  # #
	// #   ##
	// #   ##
	//       
	//      
	0x7F, 0x03, 0x0C, 0x70, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @222 'O' (5 pixels wide)
	//  ### 
	// #   #
	// #   #
	// #     #
	// #     #
	// #     #
	//  ### 
	//      
	//        
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @232 'P' (4 pixels wide)
	// ### 
	// #   #
	// #   #
	// ### 
	// #   
	// #    
	// #    
	//      
	//     
	0x7F, 0x09, 0x09, 0x06,
	0x00, 0x00, 0x00, 0x00,

	// @240 'Q' (6 pixels wide)
	//  ###  
	// #   # 
	// #   # 
	// #     # 
	// #     # 
	// #     # 
	// ##   ## 
	//      #
	//       
	0x3E, 0x41, 0x41, 0x41, 0x7E, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @252 'R' (4 pixels wide)
	// ### 
	//       ##
	// #   #
	// ### 
	// #   #
	// #  # 
	// #   #
	//      
	//     
	0x7F, 0x09, 0x09, 0x76,
	0x00, 0x00, 0x00, 0x00,

	// @260 'S' (4 pixels wide)
	//  ###
	// #  #
	// #    
	// ##   
	//     #
	// #   #
	//  ### 
	//      
	//     
	0x66, 0x49, 0x49, 0x33,
	0x00, 0x00, 0x00, 0x00,

	// @268 'T' (5 pixels wide)
	// #####
	//   #  
	//   #  
	//   #  
	//   #  
	//   #  
	//   #  
	//      
	//      
	0x01, 0x01, 0x7F, 0x01, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @278 'U' (5 pixels wide)
	// #    #
	// #    #
	// #    #
	// #    #
	// #    #
	// #    #
	//  ### 
	//      
	//       
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @288 'V' (6 pixels wide)
	// #    #
	// ##   ##
	//  #   # 
	//  #   # 
	//  ###  
	//   # #  
	//   ##   
	//        
	//       
	0x03, 0x1E, 0x70, 0x70, 0x0E, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @300 'W' (9 pixels wide)
	// ##   #   ##
	//  #  ##  #
	//  #  ###  # 
	//  #  # #  # 
	//   # # # #  
	//   # # # #  
	//   ##  ## 
	//          
	//            
	0x01, 0x0F, 0x70, 0x7C, 0x07, 0x0F, 0x70, 0x78, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @318 'X' (5 pixels wide)
	// ##  #
	//  #   # 
	//  ### 
	//   # #  
	//   ###  
	//   # #  
	// #   #
	//      
	//        
	0x41, 0x37, 0x1C, 0x1C, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @328 'Y' (5 pixels wide)
	// #   #
	// #   #
	//  # # 
	//   #  
	//   #  
	//   #  
	//   #  
	//      
	//      
	0x01, 0x06, 0x78, 0x06, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @338 'Z' (5 pixels wide)
	// #####
	//     #
	//    # 
	//   #  
	//  #   
	// #    
	// #####
	//      
	//      
	0x41, 0x71, 0x49, 0x47, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @348 '_' (6 pixels wide)
	//       
	//       
	//       
	//       
	//       
	//       
	//       
	//       
	// ######
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

// Character descriptors for Calibri 8pt
// { [Char width in bits], [Offset into calibri_8ptCharBitmaps in bytes] }
const GlypthInfo calibri_8ptDescriptors[] = 
{
	{ 3, 0 }, 		// - 
	{ 1, 6 }, 		// . 
	{0, 0}, 		// / 
	{ 4, 8 }, 		// 0 
	{ 4, 16 }, 		// 1 
	{ 4, 24 }, 		// 2 
	{ 4, 32 }, 		// 3 
	{ 5, 40 }, 		// 4 
	{ 4, 50 }, 		// 5 
	{ 4, 58 }, 		// 6 
	{ 5, 66 }, 		// 7 
	{ 5, 76 }, 		// 8 
	{ 4, 86 }, 		// 9 
	{0, 0}, 		// : 
	{0, 0}, 		// ; 
	{0, 0}, 		// < 
	{0, 0}, 		// = 
	{0, 0}, 		// > 
	{0, 0}, 		// ? 
	{0, 0}, 		// @ 
	{ 6, 94 }, 		// A 
	{ 5, 106 }, 		// B 
	{ 5, 116 }, 		// C 
	{ 5, 126 }, 		// D 
	{ 4, 136 }, 		// E 
	{ 4, 144 }, 		// F 
	{ 5, 152 }, 		// G 
	{ 5, 162 }, 		// H 
	{ 1, 172 }, 		// I 
	{ 3, 174 }, 		// J 
	{ 4, 180 }, 		// K 
	{ 4, 188 }, 		// L 
	{ 8, 196 }, 		// M 
	{ 5, 212 }, 		// N 
	{ 5, 222 }, 		// O 
	{ 4, 232 }, 		// P 
	{ 6, 240 }, 		// Q 
	{ 4, 252 }, 		// R 
	{ 4, 260 }, 		// S 
	{ 5, 268 }, 		// T 
	{ 5, 278 }, 		// U 
	{ 6, 288 }, 		// V 
	{ 9, 300 }, 		// W 
	{ 5, 318 }, 		// X 
	{ 5, 328 }, 		// Y 
	{ 5, 338 }, 		// Z 
	{ 0, 0 }, 		// [ 
	{ 0, 0 },
	{ 0, 0 }, 		// ] 
	{ 0, 0 }, 		// ^ 
	{ 6, 348 }, 		// _ 
};

const uint8_t* getGlypth_calibri8(int charID, int* advance, int* w, int* rows)
{
	charID = ToUpper(charID);
	static const char FIRST = '-';
	static const char LAST = '_';
	if (charID >= FIRST && charID <= LAST) {
		int index = charID - FIRST;
		int width  = calibri_8ptDescriptors[index].width;
		int offset = calibri_8ptDescriptors[index].offset;
		*advance = width + 2;
		*w = width;
		*rows = 1;
		return calibri_8ptBitmaps + offset;
	}
	*advance = 4;
	*w = 0;
	*rows = 0;
	return 0;
}


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


const uint8_t* getGlypth_tomThumb5(int charID, int* advance, int* w, int* rows)
{
	static uint8_t glyph[4];
	getGlypth_tomThumb5(charID, glyph, advance, w, rows);
	return glyph;
}

// This could be more efficient; core loop goes in the wrong direction.
// Could use bit shift / mask to populate glyph[]
//
void getGlypth_tomThumb5(int charID, uint8_t* glyph, int* advance, int* w, int* rows)
{
	// 4x6
	// 3x5 usable

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

		if (advance) *advance = 4;
		if (w) *w = 3;
		if (rows) *rows = 1; 
	}
}

