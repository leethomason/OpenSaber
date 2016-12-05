#include <Arduino.h>
#include "Button.h"

#define BIT_CURRENT         0
#define BIT_PREVIOUS        1
#define BIT_CHANGED         2
#define BIT_HOLD_TRIGGERED  3
#define BIT_HOLD_NOW        4
#define BIT_TEST_MODE       5
#define BIT_TEST_PRESSED    6

//#define DEBUG_SERIAL

Button::Button(uint8_t buttonPin, uint8_t buttonMode, uint16_t _debounceDuration)
{
  handlers = 0;
  init(buttonPin, buttonMode, _debounceDuration);
}

void Button::init(uint8_t buttonPin, uint8_t buttonMode, uint16_t _debounceDuration)
{
	myPin = buttonPin;
  mode = buttonMode;
  state = 0;
  holdEventThreshold = DEFAULT_HOLD_TIME;
  debounceDuration = _debounceDuration;
  debounceStartTime = 0;
  pressedStartTime = 0;

  if (myPin != 255) {
    pinMode(myPin, INPUT);
    bitWrite(state, BIT_CURRENT, (digitalRead(myPin) == mode));

  #ifdef DEBUG_SERIAL
    Serial.print("Button init:");
    Serial.println(buttonPin);
  #endif
  }
}

void Button::process(void)
{
  uint32_t currentMillis = millis();
  uint32_t interval = currentMillis - debounceStartTime;

  if(interval < uint32_t(debounceDuration)) {
    bitWrite(state, BIT_CHANGED, false);
    // not enough time has passed; ignore
    return;
  }

  // save the previous value
  bitWrite(state, BIT_PREVIOUS, bitRead(state, BIT_CURRENT));
  
  if (bitRead(state, BIT_TEST_MODE))
    bitWrite(state, BIT_CURRENT, bitRead(state, BIT_TEST_PRESSED));
  else
    bitWrite(state, BIT_CURRENT, (digitalRead(myPin) == mode));

  // clear the hold, if it was set.
  bitWrite(state, BIT_HOLD_NOW, false);

  if (bitRead(state, BIT_CURRENT) != bitRead(state, BIT_PREVIOUS)) {
    debounceStartTime = currentMillis;

    if (bitRead(state, BIT_CURRENT)) {
      // Pressed.
      #ifdef DEBUG_SERIAL
      Serial.println("Button press.");
      #endif

      if (handlers && handlers->cb_onPress) { 
        handlers->cb_onPress(*this); 
      }
      pressedStartTime = currentMillis;        //start timing
      bitWrite(state, BIT_HOLD_TRIGGERED, false);
      bitWrite(state, BIT_HOLD_NOW, false);
    } 
    else {
      // Released.
      #ifdef DEBUG_SERIAL
      Serial.println("Button release.");
      #endif

      if (handlers && handlers->cb_onRelease) { 
        handlers->cb_onRelease(*this); 
      }
      // Don't fire both hold and click.
      if (!bitRead(state, BIT_HOLD_TRIGGERED)) {
        #ifdef DEBUG_SERIAL
        Serial.println("Button click.");
        #endif
        if (handlers && handlers->cb_onClick) { 
          handlers->cb_onClick(*this);    //fire the onClick event AFTER the onRelease
        }
      }
      //reset states (for timing and for event triggering)
      pressedStartTime = 0;
    }
    bitWrite(state, BIT_CHANGED, true);
  }
  else {
    // State did NOT change.
    bitWrite(state, BIT_CHANGED, false);

    // should we trigger an onHold event? If so - only trigger one!
    if (isDown() && !bitRead(state, BIT_HOLD_TRIGGERED)) 
    {
      if (pressedStartTime && (currentMillis - pressedStartTime > uint32_t(holdEventThreshold))) 
      { 
        #ifdef DEBUG_SERIAL
        Serial.print("Button hold. startTime=");
        Serial.print(pressedStartTime);
        Serial.print(" currentTime=");
        Serial.println(currentMillis);
        #endif
        bitWrite(state, BIT_HOLD_TRIGGERED, true);
        bitWrite(state, BIT_HOLD_NOW, true);
        if (handlers && handlers->cb_onHold) {
          handlers->cb_onHold(*this);
        } 
      }
    }
  }
}


bool Button::isDown() const
{
	return bitRead(state, BIT_CURRENT);
}


bool Button::stateChanged() const
{
  return bitRead(state, BIT_CHANGED);
}


bool Button::press() const
{
  return (isDown() && stateChanged());
}


bool Button::held() const
{
  return isDown() && bitRead(state, BIT_HOLD_NOW); 
}


uint32_t Button::holdTime() const
{
  if (!isDown())
    return 0;
  return millis() - pressedStartTime;
}


void Button::setHoldThreshold(uint32_t holdTime) 
{ 
  holdEventThreshold = holdTime; 
}

void Button::enableTestMode(bool testMode)
{
  state = 0;
  pressedStartTime = 0;
  debounceStartTime = 0;
  bitWrite(state, BIT_TEST_MODE, testMode);
  bitWrite(state, BIT_TEST_PRESSED, false);
}

void Button::testPress()
{
  bitWrite(state, BIT_TEST_PRESSED, true);
}


void Button::testRelease()
{
  bitWrite(state, BIT_TEST_PRESSED, false);
}


void ButtonCB::pressHandler(buttonEventHandler handler)
{
  handlerData.cb_onPress = handler;
}


void ButtonCB::releaseHandler(buttonEventHandler handler)
{
  handlerData.cb_onRelease = handler;
}


void ButtonCB::clickHandler(buttonEventHandler handler)
{
  handlerData.cb_onClick = handler;
}


void ButtonCB::holdHandler(buttonEventHandler handler)
{
  handlerData.cb_onHold = handler;
}
