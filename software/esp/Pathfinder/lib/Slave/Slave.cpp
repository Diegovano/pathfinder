#include <Slave.h>

Slave::Slave(uint8_t pin)
: SPI_CS{pin}
{}

void Slave::begin()
{
    SPI.begin();
    pinMode(SPI_CS, OUTPUT);
    digitalWrite(SPI_CS, HIGH);
}

std::string Slave::spi_tx_string(std::string tx_string) 
{
    std::string response; // constatnt response
    
    // Serial.print("Length: ");
    // Serial.println(tx_string.length());

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    const uint8_t* NULLBUF = (const uint8_t*)"\0\0\0\0";

    char TX_BUF[5];
    TX_BUF[4]='\0';

    for (std::string::iterator it = tx_string.begin(); it < tx_string.end(); std::advance(it, 4))
    { 
        char RX_BUF[5];

        RX_BUF[4] = '\0';

        strncpy(TX_BUF, (&*it), 4);
        // strncpy(TX_BUF, "{\"ad", 4);
        //Serial.println(TX_BUF);

        // Assert slave select -> begin transfer

        // Serial.print("TXing ");
        // Serial.println(*it);
        digitalWrite(SPI_CS, LOW);
        //SPI.transfer((uint8_t)(*it));
        // SPI.transferBytes((const uint8_t*)(&*it), (uint8_t*)NULL, 4);
        // SPI.transferBytes((/*const*/ uint8_t*)(&*it), (uint8_t*)NULL, 4);
        SPI.transferBytes((const uint8_t*)TX_BUF, (uint8_t*)RX_BUF, 4);
        digitalWrite(SPI_CS, HIGH);

        response += RX_BUF;

        // De-assert slave select -> end of transfer

        usleep(1e4);
    }

    digitalWrite(SPI_CS, LOW);
    SPI.transferBytes(NULLBUF,NULL,4);
    digitalWrite(SPI_CS, HIGH);
    
    SPI.endTransaction();
    
    return response;
}

std::string Slave::spi_rx_string()
{

    unsigned long currentTime;
    unsigned long previousTime;
    const long timeoutTime = 2000; // in ms

    std::string rx_str = ""; 
    char rx_c = '\0';

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    
    // WAIT FOR START OF MESSAGE
    while(rx_c == 0) 
    {
        rx_c = SPI.transfer(0);
    }

    rx_str += rx_c;
    currentTime = millis();
    previousTime = currentTime;

    std::string currentLine = "";
    while((currentTime - previousTime) <= timeoutTime)
    {
        rx_c = SPI.transfer(0);

        if (rx_c != 0)
        {
            previousTime == currentTime;
            rx_str += rx_c;
        }
        else
        {
            break;
        }

        currentTime = millis();
    }

    SPI.endTransaction();
    
    return rx_str;
}