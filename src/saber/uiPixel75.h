#pragma once

#include <stdint.h>

#include "rgb.h"
#include "uirenderdata.h"
#include "modes.h"

#include "./src/util/Grinliz_Util.h"

class Pixel75UI
{
public:
    void Process(   UIMode mode, 
                    const UIRenderData& data);

	static const int NROWS = 5;
	static const int NCOLS = 7;

    // Returns columns.
	const uint8_t* Pixels() { return m_col; }

private:
	void DrawBar(int x, int y);
	void DrawDot(int x, int y);

    static const int ALLOCATED = NCOLS + 1;

    uint8_t m_col[ALLOCATED] = { 0 };		// enough memory for writing the glyphs 4+4 bytes
};
