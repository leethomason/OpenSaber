#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <stdint.h>

typedef const uint8_t* (*glyphMetrics)(int charID, int* advance, int* w, int* rows);


class Display
{
public:
	Display();

	void Attach(int w, int h, uint8_t* buffer);

	bool DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h,
					bool mask = true, int clip0 = 0, int clip1 = 256);
	bool DrawStr(const char* str, int x, int y, glyphMetrics metrics, 
				 int clip0 = 0, int clip1 = 256);
	void Fill(int c);

private:
	int width;
	int height;
	uint8_t* buffer;

	enum { MAX_ROWS = 4};
	uint8_t mask[MAX_ROWS];
};

#endif // DISPLAY_INCLUDED
