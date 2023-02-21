#include "pwmwrite.h"
#include "linear.h"
#include "SAMD21turboPWM.h"

static TurboPWM turboPWM;
static bool turboInit = false;

bool TurboPin(int pin)
{
    return (pin == 3) || (pin == 4) || (pin >= 8 && pin <= 13);
}

void InitPWM()
{
    turboPWM.setClockDivider(1, false);    
    turboPWM.timer(0, 1, 1000, true);
    turboPWM.timer(1, 1, 1000, true);
    turboPWM.timer(2, 1, 1000, true);
    turboInit = true;
}

void PWMWrite(int pin, uint8_t pwm)
{
    if (turboInit && TurboPin(pin)) {
        turboPWM.analogWrite(pin, uint32_t(pwm) * 1000 / 255);
    }
    else {
        analogWrite(pin, pwm);
    }
}


void PWMWriteColor(int pin, uint8_t color)
{
    if (turboInit && TurboPin(pin)) {
        uint16_t pwm = LinearToPWM1000(color);
        turboPWM.analogWrite(pin, pwm);
    }
    else {
        uint16_t pwm = LinearToPWM1024(color);
        analogWrite(pin, pwm >> 2);
    }
}

void PWMWrite1000(int pin, uint32_t pwm)
{
    if (TurboPin(pin))
        turboPWM.analogWrite(pin, pwm);
    else
        analogWrite(pin, pwm * 255 / 1000);
}

void PWMWrite1024(int pin, uint32_t pwm)
{
    if (TurboPin(pin))
        turboPWM.analogWrite(pin, pwm * 1000 / 1023);
    else
        analogWrite(pin, pwm >> 2);
}
