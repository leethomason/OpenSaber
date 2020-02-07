#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"
#include "Grinliz_Util.h"
#include "saberUtil.h"
#include "rgb.h"
#include "uirenderdata.h"


/* Renders the UI to RGB LEDs */
class DotStarUI
{
public:
    void Draw(osbr::RGB* uiLedStart,    // target LEDs
              int nLED,                 // must be 4 or 6
              uint32_t time, 
              UIMode mode, bool bladeIgnited, const UIRenderData& data) const;

	static bool Test();

private:
    void DrawVolume(osbr::RGB* led, int nLED, uint32_t time, int vol04) const;
};


/* Renders the UI to a 7x5 dot matrix (90 turn; usually they are 7 high, 
   this uses them as 7 wide.)
 */
class Pixel_7_5_UI
{
public:
    Pixel_7_5_UI();
    void Draw(uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData* data);

    // Returns columns.
    // So this will a pointer to 7 bytes.
	const uint8_t* Pixels() { return m_col; }

private:
	void DrawBar(int x, int y);
	void DrawDot(int x, int y);

	static const int NROWS = 5;
	static const int NCOLS = 7;
    static const int ALLOCATED = NCOLS + 1;
	uint8_t m_col[ALLOCATED];		// enough memory for writing the glyphs 4+4 bytes
};


/* Renders the UI to a 4 7-segment displays. */
class Digit4UI
{
public:
	Digit4UI() {}

	void Draw(UIMode mode, const UIRenderData* data);
	const CStr<9>& Output() { return m_output; }

private:
	CStr<9> m_output;
};

void calcCrystalColorHSV(uint32_t msec, const osbr::RGB& base, osbr::RGB* out);

#endif // SKETCHER_INCLUDED
