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

#include "Grinliz_Arduino_Util.h"
#include <Arduino.h>
#include <SPI.h>

LEDManager::LEDManager(uint8_t pin, bool on)
{
    m_pin = pin;
    m_on = on;
    digitalWrite(m_pin, m_on ? HIGH : LOW);
    pinMode(m_pin, OUTPUT);
}

void LEDManager::set(bool on)
{
    m_on = on;
    if (m_analogMode)
        analogWrite(m_pin, m_on ? 255 : 0);
    else
        digitalWrite(m_pin, m_on ? HIGH : LOW);
    m_nBlink = 0;
}

bool LEDManager::isOn() const
{
    return m_on;
}

void LEDManager::blink(uint8_t n, uint32_t cycle, BlinkHandler h, uint8_t style)
{
    //SPrint.p("blink n=").p(n).p(" cycle=").p(cycle).eol();

    m_handler = 0;
    m_nCallbacks = 0;
    m_style = style;
    if (n == 0 || cycle == 0)
    {
        m_nBlink = 0;
        m_cycle = 1;
        m_startTime = 0;
        set(m_on);
    }
    else
    {
        m_handler = h;
        m_nBlink = n;
        m_cycle = cycle > 0 ? cycle : 1;
        m_startTime = millis();
    }
}

void LEDManager::process()
{
    if (m_nBlink)
    {
        uint32_t mill = millis();
        uint32_t dMillis = mill - m_startTime;

        uint32_t n = dMillis / m_cycle;
        const uint32_t half = m_cycle / 2;
        uint32_t p = dMillis / half;

        if (n >= m_nBlink)
        {
            m_nBlink = 0;
            set(m_on);
        }
        else
        {
            if (m_style == BLINK_BREATH)
            {
                FixedNorm dt(dMillis % m_cycle, m_cycle);
                int32_t val = 128 + iSin(dt).scale(128);
                if (val < 0)
                    val = 0;
                if (val > 255)
                    val = 255;
                m_analogMode = true;
                analogWrite(m_pin, (uint8_t)val);
            }
            else
            {
                if (m_analogMode)
                    analogWrite(m_pin, ((p & 1) == m_style) ? 0 : 255);
                else
                    digitalWrite(m_pin, ((p & 1) == m_style) ? LOW : HIGH);
            }

            if (n >= m_nCallbacks)
            {
                if (m_handler)
                {
                    m_handler(*this);
                }
                m_nCallbacks = n + 1;
                //SPrint.p("LED callback. m_nCallbacks=").p(m_nCallbacks).p(" n=").p(n).eol();
            }
        }
    }
}

int LEDManager::numBlinks() const
{
    if (m_startTime == 0)
        return 0;

    uint32_t n = (millis() - m_startTime) / m_cycle;
    return n + 1;
}

void SPLog::attachSerial(Stream *stream)
{
    serialStream = stream;
}

void SPLog::attachLog(Stream *stream)
{
    logStream = stream;
}

const SPLog &SPLog::p(const char v[], int width) const
{
    if (serialStream)
        serialStream->print(v);
    if (logStream)
        logStream->print(v);

    if (width) {
        int len = strlen(v);
        for(; len < width; ++len) {
            if (serialStream)
                serialStream->print(" ");
            if (logStream)
                logStream->print(" ");
        }
    }

    return *this;
}

const SPLog &SPLog::p(char v) const
{
    if (serialStream)
        serialStream->print(v);
    if (logStream)
        logStream->print(v);
    return *this;
}

const SPLog &SPLog::p(unsigned char v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog &SPLog::p(int v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog &SPLog::p(unsigned int v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog &SPLog::p(long v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog &SPLog::p(unsigned long v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog &SPLog::p(double v, int p) const
{
    if (serialStream)
        serialStream->print(v, p);
    if (logStream)
        logStream->print(v, p);
    return *this;
}

const SPLog& SPLog::v3(int32_t x, int32_t y, int32_t z, const char* bracket) const
{
    static const char* BRACKET = "()";
    if (!bracket) bracket = BRACKET;

    if (serialStream) {
        serialStream->print(bracket[0]);
        serialStream->print(x);
        serialStream->print(",");
        serialStream->print(y);
        serialStream->print(",");
        serialStream->print(z);
        serialStream->print(bracket[1]);
    }
    return *this;
}

const SPLog& SPLog::v2(int32_t x, int32_t y, const char* bracket) const
{
    static const char* BRACKET = "()";
    if (!bracket) bracket = BRACKET;

    if (serialStream) {
        serialStream->print(bracket[0]);
        serialStream->print(x);
        serialStream->print(",");
        serialStream->print(y);
        serialStream->print(bracket[1]);
    }
    return *this;
}

const SPLog& SPLog::v3(float x, float y, float z, const char* bracket) const
{
    static const char* BRACKET = "()";
    if (!bracket) bracket = BRACKET;

    if (serialStream) {
        serialStream->print(bracket[0]);
        serialStream->print(x);
        serialStream->print(",");
        serialStream->print(y);
        serialStream->print(",");
        serialStream->print(z);
        serialStream->print(bracket[1]);
    }
    return *this;
}

const SPLog& SPLog::v2(float x, float y, const char* bracket) const
{
    static const char* BRACKET = "()";
    if (!bracket) bracket = BRACKET;

    if (serialStream) {
        serialStream->print(bracket[0]);
        serialStream->print(x);
        serialStream->print(",");
        serialStream->print(y);
        serialStream->print(bracket[1]);
    }
    return *this;
}

void SPLog::eol() const
{
    if (serialStream)
        serialStream->println("");
    if (logStream)
    {
        logStream->println("");
        logStream->flush();
    }
}

void AssertOut(const char *message, const char *file, int line)
{
    Log.p("ASSERT: ").p(message).p(" ").p(file).p(" ").p(line).eol();
}

void AssertOut2(const char *message, int x, int y, const char *file, int line)
{
    Log.p("ASSERT: ").p(message).p(" ").p(file).p(" ").p(line).eol();
    Log.p("   value0=").p(x).p(" value1=").p(y).eol();
}

SPITransaction::SPITransaction(uint8_t cs, const SPISettings &settings)
{
    SPI.beginTransaction(settings);
    digitalWrite(cs, LOW);
    this->cs = cs;
}

SPITransaction::~SPITransaction()
{
    digitalWrite(cs, HIGH);
    SPI.endTransaction();
}

ProfileData *ProfileData::root = 0;

void ProfileStart(ProfileData *data)
{
    data->startTime = micros();
}

void ProfileEnd(ProfileData *data)
{
    uint32_t t = micros();
    uint32_t delta = t - data->startTime;
    data->nCalls++;
    data->totalTime += delta;
    if (delta > data->maxTime)
        data->maxTime = delta;
}

void DumpProfile()
{
    Log.p("Profile:").eol();
    for (ProfileData *data = ProfileData::root; data; data = data->next)
    {
        uint32_t aveTime = data->totalTime / data->nCalls;
        Log.p("  ").p(data->name, 20).p(" aveTime=").p(aveTime / 1000.0f).p("ms maxTime=").p(data->maxTime / 1000.0f).p("ms nCalls=").p(data->nCalls).eol();

        data->nCalls = 0;
        data->totalTime = 0;
        data->maxTime = 0;
    }
}
