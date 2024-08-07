#ifndef SLAVE_H
#define SLAVE_H

#include <Arduino.h>
#include <string>
#include <SPI.h>

class Slave
{
private:
    uint8_t SPI_CS;
    uint32_t BUF_LEN;
    const u_int32_t delay = 1e3;

public:
    Slave(uint8_t, uint32_t);

    void begin();

    void spi_tx_string(char *, unsigned int);

    bool spi_rx_string(std::string &);
};

#endif