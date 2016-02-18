#include "oledsim.h"
#include <string.h>

OledSim::OledSim(int w, int h)
{
	width = w;
	height = h;

	pixels = new uint32_t[w*h];
	buffer = new uint8_t[w*h];

	memset(pixels, 0, w * h * sizeof(uint32_t));
	memset(buffer, 0, w * h * sizeof(uint8_t));
}


OledSim::~OledSim()
{
	delete[] pixels;
	delete[] buffer;
}


void OledSim::Commit()
{
	int nRows = height / 8;
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int index = nRows * i + j / 8;
			int shift = j & 7;
			int mask = 1 << shift;
			int bit = buffer[index] & mask;
			pixels[j*width + i] = bit ? 0xffffffff : 0;
		}
	}
}
