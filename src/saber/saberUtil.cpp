#include "saberUtil.h"
#include "Grinliz_Arduino_Util.h"
#include "blade.h"
#include "sfx.h"
#include "bladeflash.h"

using namespace osbr;

void BladeState::change(uint8_t state)
{
    ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
    m_currentState   = state;
    m_startTime = millis();

    switch(m_currentState) {
    case BLADE_OFF:     EventQ.event("[BLADE_OFF]");        break;
    case BLADE_IGNITE:  EventQ.event("[BLADE_IGNITE]");     break;
    case BLADE_ON:      EventQ.event("[BLADE_ON]");         break;
    case BLADE_RETRACT: EventQ.event("[BLADE_RETRACT]");    break;
    
    default:
        ASSERT(false);
        break;
    }
}

bool BladeState::bladeOn() const
{
    ASSERT(m_currentState >= BLADE_OFF && m_currentState <= BLADE_RETRACT);
    return m_currentState >= BLADE_ON && m_currentState < BLADE_RETRACT;
}

void BladeState::process(Blade* blade, const BladeFlash& saber, uint32_t time)
{
    static const uint32_t FLASH_TIME = 120;
    SFX* sfx = SFX::instance();

    switch (state()) {
    case BLADE_OFF:
        break;

    case BLADE_ON:
        blade->setRGB(saber.getColor());
        break;

    case BLADE_IGNITE:
    {
        uint32_t igniteTime = sfx->getIgniteTime();
        uint32_t t = millis() - startTime();

        if (t >= igniteTime) {
            blade->setRGB(saber.getColor());
            change(BLADE_ON);
        }
        else {
            RGB c = RGB::lerp1024(RGB(RGB::BLACK), saber.getColor(), t * 1024 / igniteTime);
            blade->setRGB(c);
        }
    }
    break;

    case BLADE_RETRACT:
    {
        uint32_t retractTime = retractTime = sfx->getRetractTime();
        uint32_t t = millis() - startTime();

        if (t >= retractTime) {
            blade->setRGB(RGB(RGB::BLACK));
            change(BLADE_OFF);
        }
        else {
            RGB c = RGB::lerp1024(saber.getColor(), RGB(RGB::BLACK), t * 1024 / retractTime);
            blade->setRGB(c);
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}


UIModeUtil::UIModeUtil()
{
    lastActive = millis();
}

void UIModeUtil::setActive()
{
    lastActive = millis();
}

bool UIModeUtil::isIdle()
{
    if (millis() - lastActive > IDLE_TIME)
        return true;
    return false;
}

void UIModeUtil::nextMode()
{
    SFX* sfx = 0;
    #ifdef SABER_AUDIO_UI
    sfx = SFX::instance();
    #endif

    switch(m_mode) {
        case UIMode::NORMAL:        
            m_mode = UIMode::PALETTE;       
            Log.p("mode: palette").eol();    
            //if (sfx) sfx->playUISound("palette");
            break;
        case UIMode::PALETTE:       
            m_mode = UIMode::VOLUME;        
            Log.p("mode: volume").eol();     
            //if (sfx) sfx->playUISound("audio");
            break;
        case UIMode::VOLUME:        
            m_mode = UIMode::NORMAL;        
            Log.p("mode: normal").eol();     
            //if (sfx) sfx->playUISound("ready");
            break;
        default:
            ASSERT(false);
            m_mode = UIMode::NORMAL;
            break;
    }
}

