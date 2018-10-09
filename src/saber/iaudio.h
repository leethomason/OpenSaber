#ifndef AUDIO_INTERFACE_INCLUDED
#define AUDIO_INTERFACE_INCLUDED

class IAudio
{
public:
    virtual void init() = 0;
    virtual bool play(const char* filename) = 0;
    virtual void stop() = 0;
    virtual bool isPlaying() const = 0;

    virtual void setVolume(float v) = 0;
    virtual float volume() const = 0;

    virtual void process() = 0;
};

#endif // AUDIO_INTERFACE_INCLUDED