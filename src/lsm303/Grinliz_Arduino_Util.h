#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include <Arduino.h>

class SPISettings;
// --- Interupts & Time --- //
class SPITransaction
{
public:
    SPITransaction(uint8_t cs, const SPISettings& settings);
    ~SPITransaction();

private:
    uint8_t cs;
};

struct ProfileData
{
    ProfileData(const char* _name) { 
        name = _name;
        next = root;
        root = this; 
    }

    const char* name = 0;
    ProfileData* next = 0;

    uint32_t nCalls;
    uint32_t totalTime;
    uint32_t maxTime;
    uint32_t startTime;

    static ProfileData* root;
};

void ProfileStart(ProfileData* data);
void ProfileEnd(ProfileData* data);
void DumpProfile();

class ProfileBlock
{
public:
    ProfileBlock(ProfileData* data) {
        m_data = data;
        ProfileStart(m_data);
    }

    ~ProfileBlock() {
        ProfileEnd(m_data);
    }

private:
    ProfileData* m_data;
};

#endif // CSTR_INCLUDED
