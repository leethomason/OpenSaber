#include "Grinliz_SPI.h"

#include "Arduino.h"
#include "SPI.h"

void Grinliz_SPI::begin(SPIClass *spiinterface, uint8_t cs, uint32_t clock)
{
    _spi = spiinterface;
    _cs = cs;
    _settings = SPISettings(clock, MSBFIRST, SPI_MODE0);

    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _spi->begin();
}


void Grinliz_SPI::write(uint8_t command, const uint8_t *data, uint32_t len)
{
    digitalWrite(_cs, LOW);
    _spi->beginTransaction(_settings);

    _spi->transfer(command);
    while (len--)
    {
        _spi->transfer(*data++);
    }

    _spi->endTransaction();
    digitalWrite(_cs, HIGH);
}


void Grinliz_SPI::read(uint8_t command, uint8_t *data, uint32_t len)
{
    digitalWrite(_cs, LOW);
    _spi->beginTransaction(_settings);

    _spi->transfer(command);
    while (len--)
    {
        *data++ = _spi->transfer(0xFF);
    }

    _spi->endTransaction();
    digitalWrite(_cs, HIGH);
}
