#ifndef SKETCHER_INCLUDED
#define SKETCHER_INCLUDED

#include <stdint.h>
#include "renderer.h"
#include "Grinliz_Util.h"
#include "saberUtil.h"
#include "rgb.h"

struct UIRenderData
{
    uint8_t volume  = 0;    // 0-4
    uint8_t palette = 0;    // 0-7
	uint32_t mVolts = 0;    // actual voltage in milli-volts
    const char* fontName = 0;

	osbr::RGB color;	// NOT the RGB of the LED. An GGB LED would be
				        // green if set to (1, 0, 0), so the bladeColor
				        // should be (0, 1, 0)

    int powerLevel(int maxLevel) const;
    UIRenderData() {}
};


/* Renders the UI to 4 RGB LEDs */
class DotStarUI
{
public:
	// Returns true if changed.
    bool Draw(osbr::RGB* uiLedStart,    // target LEDs
              int nLED,                 // must be 4 or 6
              uint32_t time, 
              UIMode mode, bool bladeIgnited, const UIRenderData& data) const;

	void SetBrightness(uint16_t v) { m_brightness = v; }
	uint16_t Brightness() const { return m_brightness; }

	static bool Test();

private:
	uint16_t m_brightness = 256;
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


class Sketcher
{
public:
	enum {
		WIDTH = 128,
		HEIGHT = 32,

		TWEAK_X0    = 0, //2,
		TWEAK_X1    = 0,
		X0			= 0 + TWEAK_X0,
		X1			= WIDTH - TWEAK_X1,
		CENTER		= (X0 + X1) / 2,

        DIAL_WIDTH  = 28,
		DATA_WIDTH	= WIDTH - DIAL_WIDTH * 2 - 20,
		BAR_WIDTH   = 38 - TWEAK_X0 - TWEAK_X1,
    };

    Sketcher();
    void Draw(Renderer* d, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData* data);
    void Push(uint8_t value);

private:
    textureData GetDial(int value);

	void DrawBladeMode(Renderer* d, uint32_t time, bool ignited, const UIRenderData* data);
	void DrawPaletteMode(Renderer* d, uint32_t time, const UIRenderData* data);
	void DrawVolumeMode(Renderer* d, uint32_t time, const UIRenderData* data);
	void DrawMeditationMode(Renderer* d, uint32_t time, const UIRenderData* data);

	void DrawDials(Renderer* d, const UIRenderData* data, bool labels=true);
	void DrawStateDisplay(Renderer* d, const UIRenderData* data);

    uint8_t  line = 0;
    uint8_t  pos = 0;
    uint32_t animTime = 0;
	uint8_t  accelData[DATA_WIDTH];
};

void calcCrystalColor(uint32_t msec, 
    int32_t lowVariation,       // how much variation around dim color channels
    int32_t highVariation,      // how much variation around bright color channels
    const osbr::RGB& base, osbr::RGB* out);

uint8_t calcSingleCrystalColor(uint32_t msec);

bool TestCrystalColor();

#endif // SKETCHER_INCLUDED
