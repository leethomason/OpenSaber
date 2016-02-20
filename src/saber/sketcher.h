#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"

class Sketcher
{
public:
	enum {
		WIDTH = 128,
		HEIGHT = 32,
		DATA_WIDTH = 90,
		DIAL_WIDTH = 28,
		INNERX = 38
	};

	void Draw(Renderer* d, uint32_t time, bool restMode);

	int power;
	int volume;

private:
	textureData GetDial(int value);

	int line = 0;
	uint32_t prevTime = 0;
	uint32_t animTime = 0;
};

#endif // SKETCHER_INCLUDED