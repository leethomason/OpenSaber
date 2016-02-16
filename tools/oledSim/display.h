#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <stdint.h>

class Display
{
public:
	Display(int w, int h);
	~Display();

	void DrawBitmap(int x, int y, const uint8_t* bitmap, int w, int h);

	const uint32_t* Pixels() const { return pixels; }
	uint8_t* Buffer() { return buffer;}
	void Commit();

private:
	int width;
	int height;
	uint32_t* pixels;
	uint8_t* buffer;
};

#endif // DISPLAY_INCLUDED
