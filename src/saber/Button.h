/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef BUTTON_LIBRARY_INCLUDED
#define BUTTON_LIBRARY_INCLUDED

#include <inttypes.h>

class Button;
typedef void (*buttonEventHandler)(const Button&);

struct ButtonCBHandlers {
    ButtonCBHandlers() : cb_onPress(0), cb_onRelease(0), cb_onClick(0), cb_onHold(0) {}
    buttonEventHandler  cb_onPress;
    buttonEventHandler  cb_onRelease;
    buttonEventHandler  cb_onClick;
    buttonEventHandler  cb_onHold;
};

/**
  * A simple button class that configures the Arduino, filters bounces, and
  * detects clicks and holds. Use `ButtoCB` if you would like to add callback handlers.
  */
class Button {
public:

    enum {
        PULL_DOWN       = 0,
        PULL_UP         = 1,
        INTERNAL_PULLUP = 2,

        DEFAULT_HOLD_TIME       = 500,
        DEFAULT_BOUNCE_DURATION = 20
    };

    /**
     *  Construct a button.
     *  'buttonPin' is the pin for this button to use.
     *  'resistor' is the wiring of the button.
     *      - A PULL_DOWN resistor means the button is tied to ground, and the button connects to HIGH on close.
     *      - A PULL_UP resistor is tied to Vcc, and the button connects to LOW on close.
     *      - An INTERNAL_PULLUP uses the internal resistor, and the button connects to LOW on close.
     *  'debounceDuration' is how long it takes the button to settle, mechanically, when pressed.
     */
    Button(uint8_t buttonPin=255, uint8_t resistor = PULL_DOWN, uint16_t debounceDuration = DEFAULT_BOUNCE_DURATION);

    void init(uint8_t buttonPin, uint8_t resistor = PULL_DOWN, uint16_t debounceDuration = DEFAULT_BOUNCE_DURATION);
    int pin() const {
        return m_myPin;
    }

    /** Process the button state change. Should be called from loop().
     *  Will call the callbacks, if needed.
     */
    void process(uint32_t currentTime);

    /// true if pressed on this loop
    bool press() const;
    /// true if the the button is down
    bool isDown() const;
    /// true if the button is held to the trigger time
    bool held() const;
    /// return the time the button has been held down
    uint32_t holdTime(uint32_t currentTime) const;
    /// return the last time the button was pressed down
    uint32_t pressedTime() const {
        return m_pressedStartTime;
    }

    void setHoldThreshold(uint32_t holdTime);
    uint16_t holdThreshold() const {
        return m_holdEventThreshold;
    }

    /** If false (the default) only one hold event is set.
      * If true the a hold event is sent every holdTime()
      */
    void setHoldRepeats(bool holdRepeats) { m_holdRepeats = holdRepeats; }

    /// Return true of the hold event repeats.
    bool holdRepeats() const {
        return m_holdRepeats;
    }

    /** If a repeating hold, returns the number of holds
      * times. The first is '1'.
      */
    int nHolds() const {
        return m_nHolds;
    }

    int cycle(bool* on = 0) const {
        if (on) 
            *on = false;            
        if (held()) {
            if (on) 
                *on = (m_nHolds & 1) ? true : false;
            return (m_nHolds + 1) / 2;
        }
        return 0;
    }

    // For testing - do not call in normal use.
    const ButtonCBHandlers* queryHandlers() const {
        return m_handlers;
    }
    void enableTestMode(bool testMode);
    void testPress();
    void testRelease();

private:
    bool stateChanged() const;
    bool queryButtonDown() const;

    bool                m_holdRepeats;
    uint8_t             m_myPin;
    uint8_t             m_mode;
    uint8_t             m_state;
    uint16_t            m_nHolds;
    uint16_t            m_holdEventThreshold;
    uint16_t            m_debounceDuration;
    uint32_t            m_pressedStartTime;
    uint32_t            m_debounceStartTime;

protected:
    const ButtonCBHandlers* m_handlers;
};


/**
 * Creates a button that supports Callbacks for Press, Release, Click, and Hold.
 * Uses a bit more memory (24 bytes) so the functionality is opt-in.
 */
class ButtonCB : public Button {
public:
    ButtonCB(uint8_t buttonPin=255,  uint8_t resistor = PULL_DOWN, uint16_t debounceDuration = DEFAULT_BOUNCE_DURATION) :
        Button(buttonPin, resistor, debounceDuration)
    {
        m_handlers = &m_handlerData;
    }

    /** Every button press generates 'press' and 'release'.
      * It may also generate a 'click' or a 'hold'.
      */
    void setPressHandler(buttonEventHandler handler);
    void setReleaseHandler(buttonEventHandler handler);
    void setClickHandler(buttonEventHandler handler);
    void setHoldHandler(buttonEventHandler handler);

private:
    ButtonCBHandlers m_handlerData;
};

#endif // BUTTON_LIBRARY_INCLUDED
