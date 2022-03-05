#include "grinliz_lsm6d.h"
#include "../util/Grinliz_Arduino_Util.h"


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

static constexpr uint8_t FREQ_12  = 0b0001;
static constexpr uint8_t FREQ_100 = 0b0100;

static constexpr uint8_t FREQ = FREQ_12;

GrinlizLSM6D::GrinlizLSM6D(int cs)
{
    _cs = cs;
}

void GrinlizLSM6D::begin(SPIClass *spi)
{
    //Log.p("begin cs=").p(_cs).eol();
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
        (FREQ << 4) |
        (0b11 << 2);    // 16g

	_spiDriver.write8(REG_CTRL1_XL, data_ctrl1_xl);

    uint8_t data_ctrl2_g =
        (FREQ << 4) |
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
        (FREQ << 4) | // gyro
        (FREQ);       // accel
    _spiDriver.write8(REG_FIFO_CTRL3, data_fifo_3);

    uint8_t data_fifo_4 = 0b00000110;   // continuous FIFO mode
  //  uint8_t data_fifo_4 = 0b00000001;   // FIFO mode
    _spiDriver.write8(REG_FIFO_CTRL4, data_fifo_4);

    readStatus();
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


int GrinlizLSM6D::flushFifo()
{
    static const int N_TAG = 5;
    const char *TAG[N_TAG] = {
        "none",
        "gyro",
        "accel",
        "temp",
        "time"
    };

    uint32_t nWords = _spiDriver.read8(SPI_READ | REG_FIFO_STATUS1);
    nWords |= (_spiDriver.read8(SPI_READ | REG_FIFO_STATUS2) & 0b11) << 8;
    if (nWords == 0)
        return 0;

    Log.p("nFifo=").p(nWords).eol();
    for (uint32_t w = 0; w < nWords; ++w) {
        uint8_t tag = _spiDriver.read8(SPI_READ | REG_FIFO_DATA_OUT_TAG) >> 3;
        for (int i = 0; i < 6; ++i) {
            _spiDriver.read8(SPI_READ | (REG_FIFO_DATA_OUT_X_L + i));
        }
        const char *name = "--none--";
        if (tag < N_TAG)
            name = TAG[tag];
        Log.p("  tag=").p(tag).p(" ").p(name).eol();
    }
    return nWords;
}
