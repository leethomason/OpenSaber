#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

#include <Arduino.h>
#include <SPI.h>


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

const SPLog& SPLog::v(int x, int y, int z, const char* bracket) const
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

const SPLog& SPLog::v(int x, int y, const char* bracket) const
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

const SPLog& SPLog::v(float x, float y, float z, const char* bracket) const
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

const SPLog& SPLog::v(float x, float y, const char* bracket) const
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
