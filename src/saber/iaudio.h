#ifndef AUDIO_INTERFACE_INCLUDED
#define AUDIO_INTERFACE_INCLUDED

class IAudio
{
public:
    virtual void init() = 0;
    virtual bool play(const char* filename, bool loop, int channel) = 0;
    virtual bool canLoop() const = 0;
    virtual void stop(int channel) = 0;
    virtual bool isPlaying(int channel) const = 0;

    // Volume 256 is "full" - can boost or cut from there.
    virtual void setVolume(int v, int channel) = 0;
    virtual int volume(int channel) const = 0;

    virtual void process() = 0;
};

#endif // AUDIO_INTERFACE_INCLUDED
