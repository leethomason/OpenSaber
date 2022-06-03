#pragma once

#include <stdint.h>

#include "rgb.h"
#include "uirenderdata.h"
#include "modes.h"

#include "./src/util/Grinliz_Util.h"


/* Platform indenpendent - tested on Win32 */

enum class UILEDConfig
{
    kOne = 1,
    kFour = 4,
    kSix = 6
};

class DotStarUI
{
public:
    DotStarUI(bool _lockSupported) : lockSupported(_lockSupported) {}

    // Return true if the dotsars need update (the uiLEDs were written to)
    bool Process(
              osbr::RGBA* uiLEDs,       // INPUT/OUTPUT: target LEDs
              UILEDConfig nLED,
              uint32_t brightness,
              uint32_t currentTime,
              UIMode mode, 
              bool bladeIgnited, 
              const UIRenderData& data);

	static bool Test();

private:
    static constexpr int MAX_LED = 6;   // max in the UI

    void DrawVolume(osbr::RGB* led, int nLEDs, uint32_t time, int vol04) const;
    bool DrawOneLED(osbr::RGBA* led, uint32_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data);
    bool DrawMultiLED(osbr::RGBA* led, int nLEDs, uint32_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data);
    bool Commit(const osbr::RGB* in, int nLEDs, osbr::RGBA* out, uint32_t brightness);
    void PowerLockIndicator(osbr::RGB* led, uint32_t time, UIMode mode, const UIRenderData& data, const osbr::RGB& color);

    const bool lockSupported;

    UIMode lastMode = UIMode::NORMAL;
    uint32_t lastTime = 0;
    AnimateProp animate;
};

