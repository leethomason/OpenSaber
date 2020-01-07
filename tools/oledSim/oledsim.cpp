#include "oledsim.h"
#include <string.h>
#include "../saber/rgb.h"

SimDisplay::SimDisplay(int w, int h, int bytes)
{
	width = w;
	height = h;
    bytesPerPixel = bytes;

	pixels = new uint32_t[w*h];
    Clear();
}


SimDisplay::~SimDisplay()
{
	delete[] pixels;
}


void SimDisplay::CommitFromBuffer(const void* _buffer, int width, int height)
{
    if (bytesPerPixel == 1) {
        // Convert from OLED -> RGBA
        const uint8_t* buffer = (const uint8_t*)_buffer;
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
}


void SimDisplay::DrawRect(int x, int y, int size, uint32_t c)
{
    for (int j = y * size; j < (y + 1)*size - 1; j++) {
        for (int i = x * size; i < (x + 1)*size - 1; i++) {
            pixels[j * width + i] = c;
        }
    }
}


void SimDisplay::CommitFrom5x7(const void* _col)
{
    const uint8_t* col = (const uint8_t*)_col;

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


void SimDisplay::CommitFromDotstar(const osbr::RGB* dotstar, int n, const osbr::RGB* rgb)
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
    if (rgb) {
        DrawRect(0, 2, size, 0xff | (rgb->get() << 8));
    }
}


void SimDisplay::Clear()
{
	memset(pixels, 0, width * height * sizeof(uint32_t));
}
