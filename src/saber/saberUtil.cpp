#include "saberUtil.h"
#include "Grinliz_Arduino_Util.h"
#include "blade.h"
#include "saberDB.h"
#include "sfx.h"

//#define USE_PS_NXP

void BladeState::change(uint8_t state)
{
    ASSERT(state >= BLADE_OFF && state <= BLADE_RETRACT);
    m_currentState   = state;
    m_startTime = millis();

    switch(m_currentState) {
    case BLADE_OFF:
        EventQ.event("[BLADE_OFF]");
        break;
    case BLADE_IGNITE:
        EventQ.event("[BLADE_IGNITE]");
        break;
    case BLADE_ON:
        EventQ.event("[BLADE_ON]");
        break;
    case BLADE_FLASH:
        EventQ.event("[BLADE_FLASH]");
        break;
    case BLADE_RETRACT:
        EventQ.event("[BLADE_RETRACT]");
        break;
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

void BladeState::process(Blade* blade, const SaberDB& saberDB, uint32_t time)
{
    static const uint32_t FLASH_TIME = 120;
    SFX* sfx = SFX::instance();

    switch (state()) {
    case BLADE_OFF:
        break;

    case BLADE_ON:
        blade->setRGB(saberDB.bladeColor());
        break;

    case BLADE_IGNITE:
    {
        uint32_t igniteTime = 1000;
        #ifdef SABER_SOUND_ON
        igniteTime = sfx->getIgniteTime();
        #endif

        bool done = blade->setInterp(millis() - startTime(), igniteTime, RGB(RGB::BLACK), saberDB.bladeColor());
        if (done) {
            change(BLADE_ON);
        }
    }
    break;

    case BLADE_RETRACT:
    {
        uint32_t retractTime = 1000;
        #ifdef SABER_SOUND_ON
            retractTime = sfx->getRetractTime();
        #endif
        bool done = blade->setInterp(millis() - startTime(), retractTime, saberDB.bladeColor(), RGB(RGB::BLACK));
        if (done) {
            change(BLADE_OFF);
        }
    }
    break;

    case BLADE_FLASH:
        // interpolate?
    {
        uint32_t delta = time - startTime();
        if (delta > FLASH_TIME) {
            blade->setRGB(saberDB.bladeColor());
            change(BLADE_ON);
        }
        else {
            blade->setRGB(saberDB.impactColor());
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
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
            if (sfx) sfx->playUISound("palette");
            break;
        case UIMode::PALETTE:       
            m_mode = UIMode::VOLUME;        
            Log.p("mode: volume").eol();     
            if (sfx) sfx->playUISound("audio");
            break;
#if SABER_MODEL == SABER_MODEL_SHOCK2
        case UIMode::VOLUME:        
            m_mode = UIMode::MEDITATION;        
            Log.p("mode: meditation").eol();     
            break;
        case UIMode::MEDITATION:        
            m_mode = UIMode::NORMAL;        
            Log.p("mode: normal").eol();     
            break;
#else            
        case UIMode::VOLUME:        
            m_mode = UIMode::NORMAL;        
            Log.p("mode: normal").eol();     
            #if defined(OVERRIDE_READY_SOUND)
                if (sfx) sfx->playUISound(OVERRIDE_READY_SOUND, false);
            #else
                if (sfx) sfx->playUISound("ready");
            #endif
            break;
#endif

        default:
            ASSERT(false);
            m_mode = UIMode::NORMAL;
            break;
    }
}

