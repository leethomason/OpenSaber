#ifndef SABER_ACCELEROMETER_INCLUDED
#define SABER_ACCELEROMETER_INCLUDED

class Accelerometer
{
public:
    Accelerometer();
    static Accelerometer& instance() { return *_instance; }

    void begin();
    void read(float* ax, float* ay, float* az, float* g2, float* g2normal);
    const float range() const { return m_range; }

private:
    static Accelerometer* _instance;
    float m_range;  // not really set correctly; always 4g. But removes global constants.
};


#endif // SABER_ACCELEROMETER_INCLUDED
