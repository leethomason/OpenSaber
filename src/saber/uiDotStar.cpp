#include "uiDotStar.h"
#include "sketcher.h"

static constexpr uint32_t LOCK_COLOR    = 0xff0000;
static constexpr uint32_t UNLOCK_COLOR  = 0x00ff00;
static const uint32_t COLOR_AUDIO_ON    = 0x0000ff;
static const uint32_t COLOR_AUDIO_OFF   = 0xffd800;


bool DotStarUI::Process(
    osbr::RGBA* uiLEDs,       // INPUT/OUTPUT: target LEDs
    UILEDConfig ledConfig,
    uint32_t brightness,
    uint32_t currentTime,
    UIMode mode, 
    bool bladeIgnited, 
    const UIRenderData& data)
{
    bool change = false;
    if (ledConfig == UILEDConfig::kOne) {
        change = DrawOneLED(uiLEDs, brightness, currentTime, mode, bladeIgnited, data);
    }
    else {
        change = DrawMultiLED(uiLEDs, (int)ledConfig, brightness, currentTime, mode, bladeIgnited, data);
    }
    lastTime = currentTime;
    lastMode = mode;
    return change;
}


bool DotStarUI::Commit(const osbr::RGB* in, int nLEDs, osbr::RGBA* out, uint32_t brightness)
{
    uint8_t b = glClamp<int32_t>(brightness, 0, 255);
    bool change = false;
    for(int i=0; i<nLEDs; ++i) {
        osbr::RGBA rgba(in[i], b);
        if (rgba != out[i]) {
            change = true;
            out[i] = rgba;
        }
    }
    return change;
}


void DotStarUI::DrawVolume(osbr::RGB *led, int n, uint32_t /*time*/, int vol04) const
{
    if (n >= 4)
    {
        int i = 0;
        for (; i < vol04 && i < 4; ++i) {
            led[i].set(COLOR_AUDIO_ON);
        }
        for (; i < 4; ++i) {
            led[i].set(COLOR_AUDIO_OFF);
        }
        for (; i < n; ++i) {
            led[i].set(0);
        }
    }
    else
    {
        for (int i = 0; i < n; ++i) {
            if (i < vol04 * n / 4)
                led[i].set(COLOR_AUDIO_ON);
            else
                led[i].set(COLOR_AUDIO_OFF);
        }
        if (vol04 > 0)
            led[0].set(COLOR_AUDIO_ON);
    }
}


void DotStarUI::PowerLockIndicator(osbr::RGB* led, uint32_t time, UIMode mode, const UIRenderData& data, const osbr::RGB& color)
{
    static const int WHITE = 150;
    static const uint32_t PERIOD = 700;

    if (mode == UIMode::NORMAL && mode != lastMode) {
        int powerLevel = UIRenderData::powerLevel(data.mVolts, 4);
        //Log.p("start blink n=").p(powerLevel).eol();
        animate.startBlink(PERIOD * powerLevel, PERIOD, WHITE);
    }
    else {
        animate.tick(time - lastTime);
    }
    if (animate.done())
        *led = color;
    else
        led->setWhite(animate.value());

    if (data.lockFlash) {
        led->set(0);
    }
}



bool DotStarUI::DrawOneLED(osbr::RGBA* out, uint32_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data)
{
    static const uint32_t ONE_VOLUME[5] = {
        0xffff00,   // yellow
        0xff8800,   // orange
        0xff0088, 
        0x8800ff,
        0x0000ff
    };

    osbr::RGB led;

    switch(mode) {
        case UIMode::PALETTE:
        {
            led = data.color;
        }
        break;

        case UIMode::VOLUME:
        {
            led.set(ONE_VOLUME[data.volume]);
        }
        break;

        case UIMode::LOCK:
        {
            led = data.locked ? LOCK_COLOR : UNLOCK_COLOR;
        }
        break;

        default:
        {   
            PowerLockIndicator(&led, time, mode, data, osbr::RGB(0xffffff));
        }
        break;
    }
    return Commit(&led, 1, out, brightness);
}


bool DotStarUI::DrawMultiLED(osbr::RGBA* out, int nLEDs, uint32_t brightness, uint32_t time, UIMode mode, bool bladeIgnited, const UIRenderData& data)
{
    osbr::RGB lockLED = data.locked ? LOCK_COLOR : UNLOCK_COLOR;
    osbr::RGB led[MAX_LED] = {0};

    if (bladeIgnited)
    {
        // Set the power level.
        int i = 0;
        int powerLevel = UIRenderData::powerLevel(data.mVolts, nLEDs);
        for (; i < powerLevel && i < nLEDs; ++i) {
            led[i] = data.color;
        }
    }
    else
    {
        switch (mode)
        {
        case UIMode::NORMAL:
        {
            DrawVolume(led, nLEDs - 2, time, data.volume);
            PowerLockIndicator(&led[nLEDs - 2], time, mode, data, osbr::RGB(0));
            led[nLEDs - 1] = data.color;
        }
        break;

        case UIMode::PALETTE:
        {
            for (int i = 1; i < nLEDs - 1; ++i) {
                led[i] = data.color;
            }
        }
        break;

        case UIMode::VOLUME:
        {
            DrawVolume(led, nLEDs, time, data.volume);
        }
        break;

        case UIMode::LOCK: 
        { 
            for (int i = 0; i < nLEDs; ++i) {
                led[i] = lockLED;
            }
        }
        break;

        case UIMode::MEDITATION:
        {
            static const uint32_t TIME_STEP = 800;
            for (int i = 0; i < nLEDs; ++i) {
                calcCrystalColorHSV(time + TIME_STEP * i, data.color, &led[i]);
            }
        }
        break;

        default:
            break;
        }
    }
    return Commit(led, nLEDs, out, brightness);
}

bool DotStarUI::Test()
{
    osbr::RGBA leds[6];

    UIRenderData data;
    data.mVolts = 3700;
    data.volume = 1;
    data.palette = 7;
    data.fontName = "FontName";
    data.soundBank = 0;
    data.color.set(0xff, 0, 0);

    // Test in range
    {
        DotStarUI dotstar(true);
        bool change = dotstar.Process(&leds[1], UILEDConfig::kFour,  255, 0, UIMode::NORMAL, false, data);
        TEST(change);
        TEST(leds[0].get() == 0);
        TEST(leds[1].get() != 0);
        TEST(leds[4].get() != 0);
        TEST(leds[5].get() == 0);
    }

    // Test 4 doesn't change
    // Test 4 does change
    // Test 4 blinks
    // Test 1 doesn't change
    // Test 1 does change
    // Test 1 blinks

    return true;
}

