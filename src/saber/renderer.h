#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <stdint.h>

#if defined(_MSC_VER)
#	define OSASSERT( x )		if ( !(x)) { _asm { int 3 } }
#else
#   include "Grinliz_Arduino_Util.h"
#	define OSASSERT( x ) if (!(x)) { Log.p("ASSERT: ").p(#x).p(" ").p(__FILE__).p(" ").p(__LINE__).eol(); }
#endif

typedef const uint8_t* (*glyphMetrics)(int charID, int* advance, int* w, int* rows);
typedef const uint8_t* (*textureData)(int* w, int* h);

class Renderer
{
public:
	Renderer() {}
	void Attach(int w, int h, uint8_t* buffer);

	enum {
		NO_MASK = 0x01,
		FLIP_X = 0x02
	};
	bool DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h,
					int flags = 0, int clip0 = 0, int clip1 = 256);
	bool DrawBitmap(int x, int y, textureData data, 
					int flags = 0, int clip0 = 0, int clip1 = 256);
	bool DrawStr(const char* str, int x, int y, glyphMetrics metrics, 
				 int clip0 = 0, int clip1 = 256);
	int StrWidth(const char* str, glyphMetrics metrics);
	void DrawRectangle(int x, int y, int w, int h);

	void Fill(int c);

	uint8_t* Buffer() { return buffer;  }

private:
	void CalcMask(int y, int h, int* r0, int* r1);

	int width	= 0;
	int height	= 0;
	int nRows	= 0;
	uint8_t* buffer = 0;

	enum { MAX_ROWS = 4};
	uint8_t mask[MAX_ROWS];
};

#endif // DISPLAY_INCLUDED
