#ifndef OLED_SIM_INCLUDED
#define OLED_SIM_INCLUDED

#include <stdint.h>

namespace osbr {
    struct RGB;
}

class SimDisplay
{
public:
    SimDisplay(int w, int h, int bytesPerPixel);
	~SimDisplay();

	// After commit, this is the RGBA pixels that are displayed in the simulator.
    const uint32_t* Pixels() const { return pixels; }
   
	void CommitFromBuffer(const uint8_t* src, int w, int h);
    void CommitFrom5x7(const uint8_t* src); // use this as a dot matrix renderer
    void CommitFromDotstar(const osbr::RGB* dotstars, int n);
	void Clear();

private:
    void DrawRect(int x, int y, int size, uint32_t c);

	int width;
	int height;
    int bytesPerPixel;
	uint32_t* pixels;
};

#endif // OLED_SIM_INCLUDED
