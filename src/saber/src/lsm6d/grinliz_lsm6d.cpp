#include "grinliz_lsm6d.h"
#include "../util/Grinliz_Arduino_Util.h"
#include <limits.h>

static const uint8_t SPI_READ = 0x80;

static constexpr uint8_t REG_CTRL1_XL = 0x10;
static constexpr uint8_t REG_CTRL2_G = 0x11;
static constexpr uint8_t REG_CTRL3_G = 0x12;
static constexpr uint8_t REG_CTRL4_G = 0x13;
static constexpr uint8_t REG_CTRL5_G = 0x14;

static constexpr uint8_t REG_FIFO_CTRL1 = 0x07;
static constexpr uint8_t REG_FIFO_CTRL2 = 0x08;
static constexpr uint8_t REG_FIFO_CTRL3 = 0x09;
static constexpr uint8_t REG_FIFO_CTRL4 = 0x0a;

static constexpr uint8_t REG_FIFO_STATUS1 = 0x3a;
static constexpr uint8_t REG_FIFO_STATUS2 = 0x3b;
static constexpr uint8_t REG_FIFO_DATA_OUT_TAG = 0x78;
static constexpr uint8_t REG_FIFO_DATA_OUT_X_L = 0x79;

static constexpr uint8_t TAG_GYRO  = 0x01;
static constexpr uint8_t TAG_ACCEL = 0x02;
static constexpr uint8_t TAG_TEMP  = 0x03;
static constexpr uint8_t TAG_TIME  = 0x04;

GrinlizLSM6D::GrinlizLSM6D(int cs)
{
    _cs = cs;
}

void GrinlizLSM6D::begin(SPIClass *spi, GrinlizLSM6D::Freq f)
{
    //Log.p("begin cs=").p(_cs).eol();
    _freq = f;
    _spiDriver.begin(spi, _cs, 1000000);    // 8000000?
    delay(10);
    init();
}

uint8_t GrinlizLSM6D::whoAmI()
{
    constexpr uint8_t REG_WHO_AM_I = 0x0f;
    return _spiDriver.read8(SPI_READ | REG_WHO_AM_I);
}

void GrinlizLSM6D::init()
{
    uint8_t data_ctrl1_xl =
        (uint8_t(_freq) << 4) |
        (0b11 << 2);    // 16g

	_spiDriver.write8(REG_CTRL1_XL, data_ctrl1_xl);

    uint8_t data_ctrl2_g =
        (uint8_t(_freq) << 4) |
        (0b10 << 2);    // 1000 dps

	_spiDriver.write8(REG_CTRL2_G, data_ctrl2_g);

    // CTRL-3 is reset stuff. 
    // CTRL-4 is unused (I hope) features.
    // CTRL-5 testing
    // 

    // Reset FIFO
    _spiDriver.write8(REG_FIFO_CTRL4, 0);
    delay(10);

    _spiDriver.write8(REG_FIFO_CTRL1, 0);
    _spiDriver.write8(REG_FIFO_CTRL2, 0);

    uint8_t data_fifo_3 =
        (uint8_t(_freq) << 4) | // gyro
        (uint8_t(_freq));       // accel
    _spiDriver.write8(REG_FIFO_CTRL3, data_fifo_3);

    uint8_t data_fifo_4 = 0b00000110;   // continuous FIFO mode
  //  uint8_t data_fifo_4 = 0b00000001;   // FIFO mode
    _spiDriver.write8(REG_FIFO_CTRL4, data_fifo_4);

    //readStatus();
}

void GrinlizLSM6D::readStatus()
{
    uint8_t data = _spiDriver.read8(SPI_READ | REG_CTRL1_XL);
    uint8_t freq = data >> 4;
    uint8_t g = (data >> 2) & 3;
    Log.p("freq=").p(freq).p(" g=").p(g).p(" expect 4, 3").eol();
    data = _spiDriver.read8(SPI_READ | REG_CTRL2_G);
    uint8_t freqG = data >> 4;
    uint8_t dps = (data >> 2) & 3;
    Log.p("freqG=").p(freqG).p(" dps=").p(dps).p(" expect 4, 2").eol();
}

uint32_t GrinlizLSM6D::numWords()
{
    uint32_t nWords = _spiDriver.read8(SPI_READ | REG_FIFO_STATUS1);
    nWords |= (_spiDriver.read8(SPI_READ | REG_FIFO_STATUS2) & 0b11) << 8;
    return nWords;
}

uint8_t GrinlizLSM6D::readWord(Vec3<int16_t> *data)
{
    uint8_t tag = _spiDriver.read8(SPI_READ | REG_FIFO_DATA_OUT_TAG) >> 3;

    for (int i = 0; i < 3; ++i)
    {
        (*data)[i]  = _spiDriver.read8(SPI_READ | (REG_FIFO_DATA_OUT_X_L + i*2));
        (*data)[i] |= _spiDriver.read8(SPI_READ | (REG_FIFO_DATA_OUT_X_L + i*2 + 1)) << 8;
    }
    return tag;
}

bool GrinlizLSM6D::sampleAccel(Vec3<Fixed115> *accel, Vec3<int32_t> *gyro)
{
    static const int32_t DIV = 2048;
    
    // Pulled from the Adafruit source.
    // Having a very difficult time finding this in the data sheet.
    // static const int32_t G_SCALE = 35;
    // static const int32_t G_DIV = 1000;

    // BUT, assuming max value 32767 is 1000 dps -> 32.8
    static const int32_t G_SCALE = 328;
    static const int32_t G_DIV = 10000;

    uint32_t nWords = numWords();
    if (nWords < 2)
        return false;

    accel->set(Fixed115{0}, Fixed115{0}, Fixed115{0});
    gyro->set(INT32_MAX, INT32_MAX, INT32_MAX);

    Vec3<int16_t> data;
    for (int i = 0; i < 2; ++i) {
        uint8_t tag = readWord(&data);
        if (tag == TAG_ACCEL) {
            accel->x = divToFixed<Fixed115>(data.x, DIV);
            accel->y = divToFixed<Fixed115>(data.y, DIV);
            accel->z = divToFixed<Fixed115>(data.z, DIV);
        }
        else if (tag == TAG_GYRO) {
            gyro->x = data.x * G_SCALE / G_DIV;
            gyro->y = data.y * G_SCALE / G_DIV;
            gyro->z = data.z * G_SCALE / G_DIV;
        }
    }
    if (accel->x == kZero_Fixed115 && accel->y == kZero_Fixed115 && accel->z == kZero_Fixed115)
        Log.p("Error reading accel.").eol();
    if (gyro->x == INT32_MAX)
        Log.p("Error reading gyro.").eol();
    return true;
}

void GrinlizLSM6D::test()
{
    uint32_t start = millis();
    uint32_t end = 0;
    int n = 0;
    flush();
    while(true) {
        end = millis();
        if (end - start > 1000)
            break;
        Vec3<Fixed115> a;
        Vec3<int32_t> g;
        if(sampleAccel(&a, &g)) {
            ++n;
        }
    }
    Fixed115 sps = divToFixed<Fixed115>(n * 1000, end - start);
    Log.p(n).p(" samples read. ").p(sps).p(" Hz").eol();
}

void GrinlizLSM6D::log(int n)
{
    flush();
    while(n > 0) {
        Vec3<Fixed115> a;
        Vec3<int32_t> g;
        if (sampleAccel(&a, &g)) {
            n--;
            Log.p("Accel: ").v3(a).p(" Gyro: ").v3(g).eol();
        }
    }
}

void GrinlizLSM6D::flushAccel(int reserve)
{
    int32_t n = numWords() - reserve;
    while(n > 0) {
        Vec3<Fixed115> a;
        Vec3<int32_t> g;
        sampleAccel(&a, &g);
        n--;
    }
}

