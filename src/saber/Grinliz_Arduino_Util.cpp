#include "Grinliz_Arduino_Util.h"
#include <Arduino.h>
#include <SPI.h>

#define TEST_IS_TRUE(x) {         \
    if((x)) {                     \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

#define TEST_IS_FALSE(x) {        \
    if(!(x)) {                    \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

#define TEST_IS_EQ(x, y) {        \
    if((x) == (y)) {              \
    }                             \
    else {                        \
        ASSERT(false);            \
        return false;             \
    }                             \
}

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

bool strStarts(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return false;

    while(*prefix) {
        if (*prefix++ != *str++)
            return false;
    }
    return true;
}

bool TestCStr()
{
    TEST_IS_TRUE(strStarts("FooBar", "Foo"));
    TEST_IS_FALSE(strStarts("FooBar", "Bar"));
    TEST_IS_FALSE(strStarts("Foo", "FooBar"));
    TEST_IS_FALSE(strStarts(0, "Foo"));
    TEST_IS_FALSE(strStarts("Foo", 0));

    static const char* STR4 = "Four";
    static const char* STR8 = "ThisIs-8";
    static const char* STR9 = "ThisIs-8+";

    {
        CStr<9> s;
        TEST_IS_TRUE(s.empty());
        TEST_IS_TRUE(s == "");

        s = STR4;
        TEST_IS_TRUE(s[4] == 0);
        TEST_IS_TRUE(s.size() == 4);
        TEST_IS_FALSE(s.empty());
        TEST_IS_EQ(s.capacity(), 8);
        s.append("F");
        TEST_IS_TRUE(s == "FourF");
        s.append("ourF");
        TEST_IS_TRUE(s.size() == 8);
        TEST_IS_TRUE(s == "FourFour");
        TEST_IS_FALSE(s != "FourFour");
        TEST_IS_TRUE(s[8] == 0);
        TEST_IS_TRUE(s[0] == 'F');

        CStr<9> s2 = s;
        TEST_IS_TRUE(s == s2.c_str());
        s.clear();
        TEST_IS_TRUE(s.empty());
        TEST_IS_TRUE(s != s2.c_str());
    }
    {
        CStr<9> s = STR9;
        TEST_IS_TRUE(s == STR8);
        s += "TooLooong";
        TEST_IS_TRUE(s == STR8);
    }
    {
        CStr<9> s;
        s += STR4;
        TEST_IS_TRUE(s == STR4);
    }
    {
        CStr<5> s;
        s.append('F');
        s.append("our");
        TEST_IS_TRUE(s == "Four");
    }
    return true;
}


int hexToDec(char h) 
{
    if (h >= '0' && h <= '9') return h - '0';
    if (h >= 'a' && h <= 'f') return 10 + h - 'a';
    if (h >= 'A' && h <= 'F') return 10 + h - 'A';
    return 0;
}

char decToHex(int v) 
{
    if (v >= 0 && v < 10) return '0' + v;
    if (v>= 10 && v < 16) return 'a' + v - 10;
    return '0';
}

bool TestHexDec()
{
    TEST_IS_TRUE(hexToDec('A') == 10);
    TEST_IS_TRUE(hexToDec('F') == 15);
    TEST_IS_TRUE(hexToDec('a') == 10);
    TEST_IS_TRUE(hexToDec('f') == 15);
    TEST_IS_TRUE(hexToDec('0') == 0);
    TEST_IS_TRUE(hexToDec('9') == 9);
    TEST_IS_TRUE(hexToDec('G') == 0);
    TEST_IS_TRUE(hexToDec('#') == 0);

    TEST_IS_TRUE(decToHex(0) == '0');
    TEST_IS_TRUE(decToHex(9) == '9');
    TEST_IS_TRUE(decToHex(10) == 'a');
    TEST_IS_TRUE(decToHex(15) == 'f');
    TEST_IS_TRUE(decToHex(-1) == '0');
    TEST_IS_TRUE(decToHex(16) == '0');

    return true;
}


void parseHex(const CStr<7>& str, uint8_t* c)
{
    for (int i = 0; i < 3; ++i) {
        c[i] = hexToDec(str[i*2+0]) * 16 + hexToDec(str[i*2+1]);
    }
}

void parseHex(const CStr<4>& str, uint8_t* c)
{
    for (int i = 0; i < 3; ++i) {
        int h = hexToDec(str[i]);
        c[i] = h * 16 + h;
    }
}

void writeHex(const uint8_t* c, CStr<7>* str) 
{
    str->clear();
    for(int i=0; i<3; ++i) {
        str->append(decToHex(c[i] / 16));
        str->append(decToHex(c[i] & 15));
    }
}

bool TestHex()
{
    CStr<7> inLong("a1b2c3");
    CStr<4> inShort("abc");
    CStr<7> out;

    uint8_t rgb[3];

    parseHex(inLong, rgb);
    TEST_IS_TRUE(rgb[0] == 0xa1);    
    TEST_IS_TRUE(rgb[1] == 0xb2);    
    TEST_IS_TRUE(rgb[2] == 0xc3);

    parseHex(inShort, rgb);
    TEST_IS_TRUE(rgb[0] == 0xaa);    
    TEST_IS_TRUE(rgb[1] == 0xbb);    
    TEST_IS_TRUE(rgb[2] == 0xcc);    

    writeHex(rgb, &out);
    TEST_IS_TRUE(out == "aabbcc");
    
    return true;
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
