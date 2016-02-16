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
