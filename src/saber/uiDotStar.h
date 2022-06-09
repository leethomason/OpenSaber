#pragma once

#include <stdint.h>

#include "rgb.h"
#include "uirenderdata.h"
#include "modes.h"

#include "./src/util/Grinliz_Util.h"


/* Platform indenpendent - tested on Win32 */

class DotStarUI
{
public:
    DotStarUI(bool _lockSupported) : lockSupported(_lockSupported) {}

    // Return true if the dotsars need update (the uiLEDs were written to)
    void Process(
              osbr::RGBA* uiLEDs,       // INPUT/OUTPUT: target LEDs
              int nLED,
              uint8_t brightness,
              uint32_t currentTime,
              UIMode mode, 
              bool bladeIgnited, 
              const UIRenderData& data);

	static bool Test();

private:
    static constexpr int MAX_LED = 6;   // max in the UI

    void DrawVolume(osbr::RGB* led, int nLEDs, uint32_t time, int vol04) const;
    void DrawOneLED(osbr::RGBA* led, uint8_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data);
    void DrawMultiLED(osbr::RGBA* led, int nLEDs, uint8_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data);
    void Commit(const osbr::RGB* in, int nLEDs, osbr::RGBA* out, uint8_t brightness);
    void PowerLockIndicator(osbr::RGB* led, uint32_t time, UIMode mode, const UIRenderData& data, const osbr::RGB& color);

    const bool lockSupported;

    UIMode lastMode = UIMode::NORMAL;
    uint32_t lastTime = 0;
    AnimateProp animate;
};

