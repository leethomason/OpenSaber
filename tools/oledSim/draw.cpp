#include "draw.h"
#include <stdint.h>

const uint8_t hwBitmaps[] =
{
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0xFC, 0x3F, //   ############  
	0x00, 0x03, //         ##      
	0x00, 0x01, //         #       
	0x00, 0x07, //         ###     
	0x00, 0x3C, //           ####  
	0x00, 0x00, //                 
	0x80, 0x03, //        ###      
	0x00, 0x3F, //         ######  
	0x00, 0x38, //            ###  
	0x00, 0x08, //            #    
	0x00, 0x3B, //         ## ###  
	0x80, 0x3F, //        #######  
	0x00, 0x00, //                 
	0x00, 0x00, //                 
};

const uint8_t* get_hw(int* width, int* height)
{
	if (width) *width = 16;
	if (height) *height = 16;
	return hwBitmaps;
}

const uint8_t ttBitmaps[] =
{
	0x04, 0x00, //   #         
	0xFC, 0x0F, //   ##########
	0x3C, 0x00, //   ####      
	0x34, 0x04, //   # ##    # 
	0x24, 0x04, //   #  #    # 
	0x3C, 0x04, //   ####    # 
	0x00, 0x04, //           # 
	0x00, 0x04, //           # 
	0xFF, 0x07, // ########### 
	0x00, 0x04, //           #
};

const uint8_t* get_tt(int* width, int* height)
{
	if (width) *width = 10;
	if (height) *height = 12;
	return ttBitmaps;
}


// 
//  Font data for Consolas 10pt
// 

// Character bitmaps for Consolas 10pt
const uint8_t consolas_10ptBitmaps[] = 
{
	// @0 'A' (6 pixels wide)
	0xF0, 0x00, //     ####  
	0x3E, 0x00, //  #####    
	0x21, 0x00, // #    #    
	0x27, 0x00, // ###  #    
	0x38, 0x00, //    ###    
	0xE0, 0x00, //      ###  

	// @12 'B' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0x76, 0x00, //  ## ###   

	// @22 'C' (5 pixels wide)
	0x7C, 0x00, //   #####   
	0xC2, 0x00, //  #    ##  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  

	// @32 'D' (6 pixels wide)
	0xFF, 0x00, // ########  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x42, 0x00, //  #    #   
	0x3C, 0x00, //   ####    

	// @44 'E' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  

	// @54 'F' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x09, 0x00, // #  #      
	0x09, 0x00, // #  #      
	0x09, 0x00, // #  #      
	0x09, 0x00, // #  #      

	// @64 'G' (6 pixels wide)
	0x3C, 0x00, //   ####    
	0x42, 0x00, //  #    #   
	0x81, 0x00, // #      #  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0xF9, 0x00, // #  #####  

	// @76 'H' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x08, 0x00, //    #      
	0x08, 0x00, //    #      
	0x08, 0x00, //    #      
	0xFF, 0x00, // ########  

	// @86 'I' (5 pixels wide)
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0xFF, 0x00, // ########  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  

	// @96 'J' (4 pixels wide)
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x7F, 0x00, // #######   

	// @104 'K' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x10, 0x00, //     #     
	0x2C, 0x00, //   ## #    
	0x42, 0x00, //  #    #   
	0x81, 0x00, // #      #  

	// @114 'L' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  

	// @124 'M' (7 pixels wide)
	0xC0, 0x00, //       ##  
	0xFF, 0x00, // ########  
	0x0E, 0x00, //  ###      
	0x10, 0x00, //     #     
	0x0C, 0x00, //   ##      
	0xFF, 0x00, // ########  
	0xF8, 0x00, //    #####  

	// @138 'N' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x07, 0x00, // ###       
	0x18, 0x00, //    ##     
	0xE0, 0x00, //      ###  
	0xFF, 0x00, // ########  

	// @148 'O' (7 pixels wide)
	0x3C, 0x00, //   ####    
	0xC2, 0x00, //  #    ##  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x81, 0x00, // #      #  
	0x43, 0x00, // ##    #   
	0x3C, 0x00, //   ####    

	// @162 'P' (5 pixels wide)
	0xFF, 0x00, // ########  
	0x11, 0x00, // #   #     
	0x11, 0x00, // #   #     
	0x11, 0x00, // #   #     
	0x0E, 0x00, //  ###      

	// @172 'Q' (6 pixels wide)
	0x7C, 0x00, //   #####   
	0xC3, 0x00, // ##    ##  
	0x81, 0x01, // #      ## 
	0x81, 0x02, // #      # #
	0xC3, 0x02, // ##    ## #
	0x3E, 0x02, //  #####   #

	// @184 'R' (6 pixels wide)
	0xFF, 0x00, // ########  
	0x11, 0x00, // #   #     
	0x11, 0x00, // #   #     
	0x31, 0x00, // #   ##    
	0xEE, 0x00, //  ### ###  
	0x80, 0x00, //        #  

	// @196 'S' (5 pixels wide)
	0x86, 0x00, //  ##    #  
	0x89, 0x00, // #  #   #  
	0x89, 0x00, // #  #   #  
	0x91, 0x00, // #   #  #  
	0x60, 0x00, //      ##   

	// @206 'T' (5 pixels wide)
	0x01, 0x00, // #         
	0x01, 0x00, // #         
	0xFF, 0x00, // ########  
	0x01, 0x00, // #         
	0x01, 0x00, // #         

	// @216 'U' (6 pixels wide)
	0x7F, 0x00, // #######   
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  
	0x80, 0x00, //        #  
	0x7F, 0x00, // #######   

	// @228 'V' (7 pixels wide)
	0x07, 0x00, // ###       
	0x1C, 0x00, //   ###     
	0xF0, 0x00, //     ####  
	0x80, 0x00, //        #  
	0x70, 0x00, //     ###   
	0x1C, 0x00, //   ###     
	0x03, 0x00, // ##        

	// @242 'W' (5 pixels wide)
	0xFF, 0x00, // ########  
	0xC0, 0x00, //       ##  
	0x38, 0x00, //    ###    
	0xC0, 0x00, //       ##  
	0xFF, 0x00, // ########  

	// @252 'X' (7 pixels wide)
	0x81, 0x00, // #      #  
	0xC3, 0x00, // ##    ##  
	0x3C, 0x00, //   ####    
	0x18, 0x00, //    ##     
	0x3C, 0x00, //   ####    
	0xC3, 0x00, // ##    ##  
	0x81, 0x00, // #      #  

	// @266 'Y' (7 pixels wide)
	0x01, 0x00, // #         
	0x06, 0x00, //  ##       
	0x0C, 0x00, //   ##      
	0xF0, 0x00, //     ####  
	0x0C, 0x00, //   ##      
	0x06, 0x00, //  ##       
	0x01, 0x00, // #         

	// @280 'Z' (6 pixels wide)
	0x81, 0x00, // #      #  
	0xE1, 0x00, // #    ###  
	0x91, 0x00, // #   #  #  
	0x89, 0x00, // #  #   #  
	0x87, 0x00, // ###    #  
	0x01, 0x00, // #         
};

struct GlypthInfo {
	uint8_t width;
	uint16_t offset;
};

// Character descriptors for Consolas 10pt
// { [Char width in bits], [Offset into consolas_10ptCharBitmaps in bytes] }
const GlypthInfo consolas_10ptDescriptors[] = 
{
	{6, 0}, 		// A 
	{5, 12}, 		// B 
	{5, 22}, 		// C 
	{6, 32}, 		// D 
	{5, 44}, 		// E 
	{5, 54}, 		// F 
	{6, 64}, 		// G 
	{5, 76}, 		// H 
	{5, 86}, 		// I 
	{4, 96}, 		// J 
	{5, 104}, 		// K 
	{5, 114}, 		// L 
	{7, 124}, 		// M 
	{5, 138}, 		// N 
	{7, 148}, 		// O 
	{5, 162}, 		// P 
	{6, 172}, 		// Q 
	{6, 184}, 		// R 
	{5, 196}, 		// S 
	{5, 206}, 		// T 
	{6, 216}, 		// U 
	{7, 228}, 		// V 
	{5, 242}, 		// W 
	{7, 252}, 		// X 
	{7, 266}, 		// Y 
	{6, 280}, 		// Z 
};

const uint8_t* getGlypth_consolas(int charID, int* advance, int* w, int* rows)
{
	if (charID >= 'A' && charID <= 'Z') {
		int index = charID - 'A';
		int width = consolas_10ptDescriptors[index].width;
		int offset = consolas_10ptDescriptors[index].offset;
		*advance = width + 2;
		*w = width;
		*rows = 2;
		return consolas_10ptBitmaps + offset;
	}
	*advance = 2;
	*w = 0;
	*rows = 0;
	return 0;
}
