#include "SAMD21turboPWM.h"

void TurboPWM::setClockDivider(unsigned int GCLKDiv, bool turbo) {
  // Configure input clock
  if (turbo) {
    // Configure GCLK5 to use DFLL48M
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(5);
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    // Set GCLK5's prescaler to 48 for 1 MHz
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(48) | GCLK_GENDIV_ID(5);
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    // Enable GCLK5 and connect it to GCLK_DPLL
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID(1);
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    // Set DPLL ratio to 1 MHz * (95 + 1) = 96 MHz
    SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDRFRAC(0) | SYSCTRL_DPLLRATIO_LDR(95);
    
    // Configure DPLL to disregard phase lock and select GCLK as source
    SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_LBYPASS | SYSCTRL_DPLLCTRLB_WUF | SYSCTRL_DPLLCTRLB_REFCLK(SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val);
    
    // Enable DPLL
    SYSCTRL->DPLLCTRLA.reg |= SYSCTRL_DPLLCTRLA_ENABLE;
    
    // Configure GCLK4 to use DPLL96M
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    _turbo = true;
    
  } else {
    // Configure GCLK4 to use DFLL48M
    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    _turbo = false;
  }
  
  // Clamp GCLKDiv to 1 - 255
  if (GCLKDiv < 1) {
    GCLKDiv = 1;
  }
  if (GCLKDiv > 255) {
    GCLKDiv = 255;
  }
  _GCLKDiv = GCLKDiv;
  
  // Set GCLK4's prescaler
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(_GCLKDiv) | GCLK_GENDIV_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);
  
  // Connect GCLK4 to TCC0, TCC1, TCC2, and TC3
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TCC2_TC3;
  while (GCLK->STATUS.bit.SYNCBUSY);
}

int TurboPWM::timer(unsigned int timerNumber, unsigned int TCCDiv, unsigned long long int steps, bool fastPWM) {
  // Check timer number
  if (timerNumber >= timerTableSize) {
    return 0;
  }
  
  // Derive TCC prescaler from parameter TCCDiv; default to 1 if a wrong number was entered
  unsigned int my_TCC_CTRLA_PRESCALER_DIV;
  if (TCCDiv == 1) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1;
  } else if (TCCDiv == 2) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV2;
  } else if (TCCDiv == 4) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV4;
  } else if (TCCDiv == 8) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV8;
  } else if (TCCDiv == 16) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV16;
  } else if (TCCDiv == 64) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV64;
  } else if (TCCDiv == 256) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV256;
  } else if (TCCDiv == 1024) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1024;
  } else {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1;
    TCCDiv = 1;
  }

  // Set prescaler TCCDiv for TCCx
  timerTable[timerNumber].TCCDiv = TCCDiv;
  *(RwReg*)timerTable[timerNumber].REG_TCCx_CTRLA |= my_TCC_CTRLA_PRESCALER_DIV;
  
  // Select single- or dual-slope PWM and invert output for dual-slope PWM
  timerTable[timerNumber].fastPWM = fastPWM;
  if (timerTable[timerNumber].fastPWM) {
    *(RwReg*)timerTable[timerNumber].REG_TCCx_WAVE |= TCC_WAVE_WAVEGEN_NPWM;
  } else {
    *(RwReg*)timerTable[timerNumber].REG_TCCx_WAVE |= TCC_WAVE_POL(0xF) | TCC_WAVE_WAVEGEN_DSBOTTOM;
  }
  while (timerTable[timerNumber].TCCx->SYNCBUSY.bit.WAVE);
  
  // Clamp resolution to TCCx's counter size
  if (steps < 2) {
    steps = 2;
  }
  if (steps > timerTable[timerNumber].counterSize) {
    steps = timerTable[timerNumber].counterSize;
  }
  
  // Set the resolution
  timerTable[timerNumber].steps = steps;
  *(RwReg*)timerTable[timerNumber].REG_TCCx_PERB = timerTable[timerNumber].steps;
  while (timerTable[timerNumber].TCCx->SYNCBUSY.bit.PERB);
  
  // Enable timer
  enable(timerNumber, timerTable[timerNumber].enabled);
  
  return 1;
}

int TurboPWM::analogWrite(int pin, unsigned int dutyCycle) {
  // Check if an acceptable pin is used
  unsigned int i;
  for (i = 0; i < pinTableSize; i++) {
    if (pinTable[i].arduinoPin == pin) {
      break;
    }
  }
  if (i >= pinTableSize || pin < 0) {
    return 0;
  }
  
  // Enable a SAMD21 pin as multiplexed and connect it to a pin using the port multiplexer
  PORT->Group[pinTable[pin].port].PINCFG[pinTable[pin].samd21Pin].bit.PMUXEN = 1;
  PORT->Group[pinTable[pin].port].PMUX[pinTable[pin].samd21Pin >> 1].reg |= pinTable[pin].pMux;
  
  // Clamp dutycycle to the maximum duty cycle set in the header file; duty cycle will be (dutyCycle / _maxDutyCycle) * 100%
  if (dutyCycle > _maxDutyCycle) {
    dutyCycle = _maxDutyCycle;
  }

  // Set duty cycle
  *(RwReg*)pinTable[pin].REG_TCCx_CCBy = (timerTable[pinTable[pin].timer].steps * dutyCycle) / _maxDutyCycle;
  while (timerTable[pinTable[pin].timer].TCCx->SYNCBUSY.vec.CCB);
  
  return 1;
}

int TurboPWM::enable(unsigned int timerNumber, bool enabled) {
  // Check timer number
  if (timerNumber >= timerTableSize) {
    return 0;
  }
  
  timerTable[timerNumber].enabled = enabled;
  if (timerTable[timerNumber].enabled) {
    *(RwReg*)timerTable[timerNumber].REG_TCCx_CTRLA |= TCC_CTRLA_ENABLE;
  } else {
    *(RwReg*)timerTable[timerNumber].REG_TCCx_CTRLA &= ~(TCC_CTRLA_ENABLE);
  }
  while (timerTable[timerNumber].TCCx->SYNCBUSY.bit.ENABLE);
  
  return 1;
}

float TurboPWM::frequency(unsigned int timerNumber) {
  // Check timer number
  if (timerNumber >= timerTableSize) {
    return 0;
  }
  
  unsigned int fastDivider;
  unsigned int PLL96M;
  if (_turbo) {
    PLL96M = 2;
  } else {
    PLL96M = 1;
  }
  if (timerTable[timerNumber].fastPWM) {
    fastDivider = 1;
  } else {
    fastDivider = 2;
  }
  return (static_cast<float>(VARIANT_MCK) * PLL96M) / (fastDivider * _GCLKDiv * timerTable[timerNumber].TCCDiv * timerTable[timerNumber].steps);
}
