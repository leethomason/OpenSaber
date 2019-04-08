#ifndef OLED_SIM_INCLUDED
#define OLED_SIM_INCLUDED

#include <stdint.h>

namespace osbr {
    struct RGB;
}

class OledSim
{
public:
	OledSim(int w, int h, int bytesPerPixel);
	~OledSim();

	const uint32_t* Pixels() const { return pixels; }
	uint8_t* Buffer() { return buffer;}
	void Commit();                          // copies the buffer TO the pixels
    void CommitFrom5x7(const uint8_t* src); // use this as a dot matrix renderer
    void CommitFromDotstar(const osbr::RGB* dotstars, int n);
	void Clear();

private:
    void DrawRect(int x, int y, int size, uint32_t c);

	int width;
	int height;
    int bytesPerPixel;
	uint32_t* pixels;
	uint8_t* buffer;
};

#endif // OLED_SIM_INCLUDED
