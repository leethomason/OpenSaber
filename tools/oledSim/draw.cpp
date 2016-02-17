#include "draw.h"
#include <stdint.h>

const uint8_t hwBitmaps[] =
{
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x3F, 0xFC, //   ############  
	0x03, 0x00, //       ##        
	0x01, 0x00, //        #        
	0x07, 0x00, //      ###        
	0x3C, 0x00, //   ####          
	0x00, 0x00, //                 
	0x03, 0x80, //       ###       
	0x3F, 0x00, //   ######        
	0x38, 0x00, //   ###           
	0x08, 0x00, //     #           
	0x3B, 0x00, //   ### ##        
	0x3F, 0x80, //   #######       
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
