#include "oledsim.h"
#include <string.h>
#include "../saber/rgb.h"

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
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int row = j / 8;
			int index = i + row * width;
			int shift = j & 7;
			int mask = 1 << shift;
			int bit = buffer[index] & mask;
			pixels[j*width + i] = bit ? 0xffffffff : 0;
		}
	}
}


void OledSim::DrawRect(int x, int y, int size, uint32_t c)
{
    for (int j = y * size; j < (y + 1)*size - 1; j++) {
        for (int i = x * size; i < (x + 1)*size - 1; i++) {
            pixels[j * width + i] = c;
        }
    }
}


void OledSim::CommitFrom5x7(const uint8_t* col)
{
    memset(pixels, 0, width * height * sizeof(uint32_t));
    int w = width / 7;
    int h = height / 5;
    int size = (w < h) ? w : h;

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 7; ++x) {
            uint8_t mask = col[x] & (1 << y);
            DrawRect(x, y, size, mask ? 0x00ff00ff : 0x222222ff);
        }
    }
    //DrawRect(0, 0, size);
    //DrawRect(1, 0, size);
    //DrawRect(1, 1, size);
}


void OledSim::CommitFromDotstar(const RGB* dotstar, int n)
{
    //memset(pixels, 0, width * height * sizeof(uint32_t));
    const int s = width * height;
    for (int i = 0; i < s; ++i) {
        pixels[i] = 0x666666ff;
    }

    int size = height / 4;

    for (int i = 0; i < n; ++i) {
        DrawRect(i, 0, size, 0xff | (dotstar[i].get() << 8));
    }
}


void OledSim::Clear()
{
	memset(pixels, 0, width * height * sizeof(uint32_t));
	memset(buffer, 0, width * height * sizeof(uint8_t));
}
