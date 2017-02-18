#include "Grinliz_Arduino_Util.h"
#include <Arduino.h>
#include <SPI.h>

SPClass SPrint;

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
    digitalWrite(m_pin, m_on ? HIGH : LOW );
    m_nBlink = 0;
}

void LEDManager::blink(uint8_t n, uint32_t cycle, BlinkHandler h, uint8_t bias)
{
    //SPrint.p("blink n=").p(n).p(" cycle=").p(cycle).eol();

    m_handler = 0;
    m_nCallbacks = 0;
    m_bias = bias;
    if (n == 0 || cycle == 0) {
        m_nBlink = 0;
        m_cycle = 1;
        m_startTime = 0;
        digitalWrite(m_pin, m_on ? HIGH : LOW );
    }
    else {
        m_handler = h;
        m_nBlink = n;
        m_cycle = cycle > 0 ? cycle : 1;
        m_startTime = millis();
    }
}

void LEDManager::process()
{
    if (m_nBlink) {
        uint32_t n = (millis() - m_startTime) / m_cycle;
        const uint32_t half = m_cycle / 2;
        uint32_t p = (millis() - m_startTime) / half;

        if (n >= m_nBlink) {
            m_nBlink = 0;
            digitalWrite(m_pin, m_on ? HIGH : LOW );
        }
        else {
            digitalWrite(m_pin, ((p & 1) == m_bias) ? LOW : HIGH );

            if (n >= m_nCallbacks) {
                if (m_handler) {
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
    if (m_startTime == 0) return 0;

    uint32_t n = (millis() - m_startTime) / m_cycle;
    return n + 1;
}


bool Timer::tick()
{
    if (time == 0) {
        time = millis();  // start it up.
    }
    else {
        uint32_t delta = millis() - time;
        if (delta >= trigger) {
            time = millis();
            return true;
        }
    }
    return false;
}


void SPLog::attachSerial(Stream* stream) 
{
    serialStream = stream;
}

void SPLog::attachLog(Stream* stream)
{
    logStream = stream;
}

const SPLog& SPLog::p(const char v[]) const
{
  if (serialStream)
      serialStream->print(v);
  if (logStream)
      logStream->print(v);
  return *this;  
}
 
const SPLog& SPLog::p(char v) const
{
  if (serialStream)
      serialStream->print(v);
  if (logStream)
      logStream->print(v);
  return *this;  
}

const SPLog& SPLog::p(unsigned char v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

const SPLog& SPLog::p(int v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

const SPLog& SPLog::p(unsigned int v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

const SPLog& SPLog::p(long v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

const SPLog& SPLog::p(unsigned long v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

const SPLog& SPLog::p(double v, int p) const
{
  if (serialStream)
      serialStream->print(v, p);
  if (logStream)
      logStream->print(v, p);
  return *this;  
}

void SPLog::eol() const
{
  if (serialStream)
      serialStream->println("");
  if (logStream) {
      logStream->println("");
      logStream->flush();
    }
}

void SPLog::event(const char* e)
{
  ASSERT(e);
  _hasEvent = true;
  eventName = e;
  eventStrData.clear();
  eventIData = 0;

  p(eventName.c_str()).eol();
}

void SPLog::event(const char* e, const char* d)
{
  ASSERT(e);
  ASSERT(d);
  _hasEvent = true;

  eventName = e;
  eventStrData = d;
  eventIData = 0;

  p(eventName.c_str()).p(" ").p(eventStrData.c_str()).eol();
}

void SPLog::event(const char* e, int d)
{
  ASSERT(e);
  _hasEvent = true;

  eventName = e;
  eventStrData.clear();
  eventIData = d;

  p(eventName.c_str()).p(" ").p(eventIData).eol();
}

const char* SPLog::popEvent(const char** n, const char** d, int* di)
{
    if (_hasEvent) {
      _hasEvent = false;
      if (n) 
          *n = eventName.c_str();
      if (d)
          *d = eventStrData.c_str();
      if (di)
          *di = eventIData;
      return eventName.c_str();
    }
    if (n) *n = 0;
    if (d) *d = 0;
    if (di) *di = 0;
    return 0;
}

SPLog Log;

void AssertOut(const char* message, const char* file, int line)
{
  Log.p("ASSERT: ").p(message).p(" ").p(file).p(" ").p(line).eol();
}


bool TestEvent()
{
    const char* name = 0;
    const char* data = 0;
    int iData = 0;

    const char* savedName = 0;
    const char* savedData = 0;

    if (Log.hasEvent()) {
        Log.popEvent(&savedName, &savedData, 0);
    }

    TEST_IS_FALSE(Log.hasEvent());

    Log.event("foo");
    Log.popEvent(&name, &data, &iData);
    TEST_IS_TRUE(strEqual(name, "foo"));
    TEST_IS_TRUE(*data == 0);
    TEST_IS_TRUE(iData == 0);
    TEST_IS_FALSE(Log.hasEvent());

    //if (savedName) {
    //    Log.event(savedName, savedData);
    //}

    return true;
}


SPITransaction::SPITransaction(uint8_t cs, const SPISettings& settings) {
    SPI.beginTransaction(settings);
    digitalWrite(cs, LOW);
    this->cs = cs;
}

SPITransaction::~SPITransaction() {
    digitalWrite(cs, HIGH);
    SPI.endTransaction();
}
