#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <stdint.h>

typedef const uint8_t* (*glyphMetrics)(int charID, int* advance, int* w, int* rows);


class Display
{
public:
	Display(int w, int h);
	~Display();

	void DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h, bool mask=true);
	void DrawStr(const char* str, int x, int y, glyphMetrics metrics);
	void Fill(int c);

	const uint32_t* Pixels() const { return pixels; }
	uint8_t* Buffer() { return buffer;}
	void Commit();

private:
	int width;
	int height;
	enum { MAX_ROWS = 4};
	uint8_t mask[MAX_ROWS];
	uint32_t* pixels;
	uint8_t* buffer;
};

#endif // DISPLAY_INCLUDED
