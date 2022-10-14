#include "bladestate.h"
#include "./src/util/Grinliz_Util.h"
#include "./src/util/fixed.h"

void BladeStateManager::ignite(uint32_t eventTime)
{
    Log.p("BladeStateManager ignite time=").p(eventTime).eol();
    ASSERT(m_state == BladeState::kOff);
    m_state = BladeState::kIgnite;
    m_eventTime = eventTime;
    m_startTime = m_currentTime;
}

void BladeStateManager::retract(uint32_t eventTime)
{
    Log.p("BladeStateManager retract time=").p(eventTime).eol();
    ASSERT(m_state == BladeState::kOn);
    ASSERT(m_currentTime);
    m_state = BladeState::kRetract;
    m_eventTime = eventTime;
    m_startTime = m_currentTime;
}

osbr::RGB BladeStateManager::process(osbr::RGB fullColor, uint32_t currentTime)
{
    osbr::RGB color(0), start(0), end(0);
    bool needLerp = false;
    uint32_t deltaTime = 0;

    m_currentTime = currentTime;
    if (m_eventTime) {
        deltaTime = currentTime - m_startTime;
    }

    // Apply state changes BEFORE switch(), which
    // is only used to compute color.
    if (m_state == BladeState::kIgnite && deltaTime >= m_eventTime) {
        m_state = BladeState::kOn;
        m_startTime = m_eventTime = 0;
    }
    else if (m_state == BladeState::kRetract && deltaTime >= m_eventTime) {
        m_state = BladeState::kOff;
        m_startTime = m_eventTime = 0;
    }

    switch(m_state) {
        case BladeState::kOff:
            break;
        
        case BladeState::kOn:
            color = fullColor;
            break;

        case BladeState::kIgnite:
            needLerp = true;
            start.set(0);
            end = fullColor;
            break;
        
        case BladeState::kRetract:
            needLerp = true;
            start = fullColor;
            end.set(0);
            break;
    }

    if (needLerp) {
        FixedNorm fraction = divToFixed<FixedNorm>(deltaTime, m_eventTime);
        if (fraction < kZero_FixedNorm) fraction = kZero_FixedNorm;
        if (fraction > kOne_FixedNorm) fraction = kOne_FixedNorm;

        for (int i = 0; i < 3; ++i) {
            int32_t c = start[i] + scale(fraction, end[i] - start[i]);
            color[i] = (uint8_t)glClamp<int32_t>(c, 0, 255);
        }
    }
    //if (color != osbr::RGB(0))
    //   Log.p("BladeStateManager time=").p(currentTime).p(" color=").ptc(color).p(" full=").ptc(fullColor).eol();
    return color;
}

bool BladeStateManager::Test()
{
    BladeStateManager bsm;
    const osbr::RGB testColor(64, 128, 255);

    // warm up
    bsm.process(testColor, 0);

    // t=0, off
    TEST(bsm.state() == BladeState::kOff);
    TEST(bsm.isOff());

    // ignite - at the next time
    // t=0 ignite
    bsm.ignite(1000);
    TEST(bsm.state() == BladeState::kIgnite);
    TEST(!bsm.isOff());

    osbr::RGB prevC(0), c(0);
    for (int i = 1; i < 10; i++) {
        c = bsm.process(testColor, i * 100);
        TEST(bsm.state() == BladeState::kIgnite);
        TEST(c.r >= prevC.r);
        TEST(c.g >= prevC.g);
        TEST(c.b >= prevC.b);
    }
    // full on
    c = bsm.process(testColor, 1000);
    TEST(c == testColor);
    TEST(bsm.state() == BladeState::kOn);

    return true;
}
