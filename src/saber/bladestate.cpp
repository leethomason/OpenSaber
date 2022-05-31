#include "bladestate.h"
#include "./src/util/Grinliz_Util.h"
#include "./src/util/fixed.h"

void BladeStateManager::ignite(uint32_t eventTime)
{
    ASSERT(m_state == BladeState2::kOff);
    m_state = BladeState2::kIgnite;
    m_eventTime = eventTime;
    m_startTime = m_currentTime;
}

void BladeStateManager::retract(uint32_t eventTime)
{
    ASSERT(m_state == BladeState2::kOn);
    ASSERT(m_currentTime);
    m_state = BladeState2::kRetract;
    m_eventTime = eventTime;
    m_startTime = m_currentTime;
}

osbr::RGB BladeStateManager::process(osbr::RGB fullColor, uint32_t currentTime)
{
    osbr::RGB color(0), start(0), end(0);
    bool lerp = false;
    uint32_t deltaTime = 0;

    m_currentTime = currentTime;
    if (m_eventTime) {
        deltaTime = currentTime - m_startTime;
    }

    // Apply state changes BEFORE switch(), which
    // is only used to compute color.
    if (m_state == BladeState2::kIgnite && deltaTime >= m_eventTime) {
        m_state = BladeState2::kOn;
        m_startTime = m_eventTime = 0;
    }
    else if (m_state == BladeState2::kRetract && deltaTime >= m_eventTime) {
        m_state = BladeState2::kOff;
        m_startTime = m_eventTime = 0;
    }

    switch(m_state) {
        case BladeState2::kOff:
            break;
        
        case BladeState2::kOn:
            color = fullColor;
            break;

        case BladeState2::kIgnite:
            lerp = true;
            start.set(0);
            end = fullColor;
            break;
        
        case BladeState2::kRetract:
            lerp = true;
            start = fullColor;
            end.set(0);
            break;
    }

    if (lerp) {
        ASSERT(deltaTime < m_eventTime);
        ASSERT(deltaTime >= 0);
        FixedNorm fraction = divToFixed<FixedNorm>(deltaTime, m_eventTime);

        for (int i = 0; i < 3; ++i) {
            // A little defensive programming range checking. The blade flash
            // bug is still undiagnosed. Be very careful with int8 range.
            int32_t c = start[i] + scale(fraction, end[i] - start[i]);
            color[i] = (uint8_t)glClamp<int32_t>(c, 0, 255);
        }
    }
    return color;
}

bool BladeStateManager::Test()
{
    BladeStateManager bsm;
    const osbr::RGB testColor(64, 128, 255);

    // warm up
    bsm.process(testColor, 0);

    // t=0, off
    TEST(bsm.bladeState() == BladeState2::kOff);
    TEST(bsm.isOff());

    // ignite - at the next time
    // t=0 ignite
    bsm.ignite(1000);
    TEST(bsm.bladeState() == BladeState2::kIgnite);
    TEST(!bsm.isOff());

    osbr::RGB prevC(0), c(0);
    for (int i = 1; i < 10; i++) {
        c = bsm.process(testColor, i * 100);
        TEST(bsm.bladeState() == BladeState2::kIgnite);
        TEST(c.r >= prevC.r);
        TEST(c.g >= prevC.g);
        TEST(c.b >= prevC.b);
    }
    // full on
    c = bsm.process(testColor, 1000);
    TEST(c == testColor);
    TEST(bsm.bladeState() == BladeState2::kOn);

    return true;
}
