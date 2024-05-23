#ifndef SLAVE_H
#define SLAVE_H

#include <Arduino.h>
#include <string>
#include <SPI.h>

class Slave {
private:
    uint8_t SPI_CS;
    
public:
    Slave(uint8_t);

    void begin();

    // void spi_tx_string(std::string);
    std::string spi_tx_string(std::string);

    std::string spi_rx_string();

};

#endif