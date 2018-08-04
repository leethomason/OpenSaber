#ifndef OLED_SIM_INCLUDED
#define OLED_SIM_INCLUDED

#include <stdint.h>

class OledSim
{
public:
	OledSim(int w, int h);
	~OledSim();

	const uint32_t* Pixels() const { return pixels; }
	uint8_t* Buffer() { return buffer;}
	void Commit();                          // copies the buffer TO the pixels
    void CommitFrom5x7(const uint8_t* src); // use this as a dot matrix renderer
	void Clear();

private:
    void DrawRect(int x, int y, int size, bool on);

	int width;
	int height;
	uint32_t* pixels;
	uint8_t* buffer;
};

#endif // OLED_SIM_INCLUDED
