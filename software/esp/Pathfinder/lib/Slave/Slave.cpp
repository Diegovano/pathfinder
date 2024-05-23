#include <Slave.h>

#define DEBUG false

Slave::Slave(uint8_t pin, uint32_t length)
: SPI_CS{pin}
, BUF_LEN{length}
{}

void Slave::begin()
{
    SPI.begin();
    pinMode(SPI_CS, OUTPUT);
    const uint8_t NULL_BUF[4]{0};
    digitalWrite(SPI_CS, LOW);
    SPI.transferBytes(NULL_BUF, NULL, 4);
    digitalWrite(SPI_CS, HIGH);
}

void Slave::spi_tx_string(std::string &tx_str) 
{
    
    Serial.print("Length: ");
    Serial.println(tx_str.length());

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    const uint8_t NULL_BUF[BUF_LEN] = {0};
    char TX_BUF[BUF_LEN + 1];
    TX_BUF[BUF_LEN]='\0';

    for (std::string::iterator it = tx_str.begin(); it < tx_str.end(); std::advance(it, BUF_LEN))
    {

        //strncpy(TX_BUF, (&*it), 4);
        //Serial.println(TX_BUF);

        // Assert slave select -> begin transfer
        digitalWrite(SPI_CS, LOW);

        //SPI.transfer((uint8_t)(*it));
        SPI.transferBytes((uint8_t*)(&*it), NULL, BUF_LEN);

        // De-assert slave select -> end of transfer
        digitalWrite(SPI_CS, HIGH);

        usleep(delay);

    }

    digitalWrite(SPI_CS, LOW);
    SPI.transferBytes(NULL_BUF,NULL,BUF_LEN);
    digitalWrite(SPI_CS, HIGH);
    
    SPI.endTransaction();
    
    
}

bool Slave::spi_rx_string(std::string &rx_str)
{   
    #if DEBUG
    char debug[] = "\0\0GOOD MORN\0ING";
    char *rx_sim = debug;
    #endif

    unsigned long startTime;
    const long timeoutTime = 20000; // in ms

    rx_str = "";
    char RX_BUF[4];

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    
    // WAIT FOR START OF MESSAGE
    startTime = millis();
    bool TIMEOUT = false;
    bool WAIT = true;
    while(WAIT & !TIMEOUT) 
    {
        digitalWrite(SPI_CS, LOW);
        #if DEBUG
        memcpy(RX_BUF,rx_sim,BUF_LEN);
        rx_sim +=4;
        #else
        SPI.transferBytes(NULL,(uint8_t *)RX_BUF,BUF_LEN);
        #endif
        digitalWrite(SPI_CS, HIGH);

        for (int i=0; i<BUF_LEN; i++) 
        {
            char rx_c = RX_BUF[i];
            Serial.println(rx_c);
            Serial.print("!WAIT: ");
            Serial.println(!WAIT);
            if (!WAIT)
            {
                rx_str += rx_c;
            }
            else if (rx_c != '\0')
            {
                WAIT = false;
                Serial.print("first valid char: ");
                Serial.println(rx_c);
                rx_str += rx_c;
            }
        }
        
        if (millis() - startTime > timeoutTime)
        {
            TIMEOUT = WAIT;
            rx_str = "FPGA_RESPONSE_TIMEOUT";
        }

        usleep(delay);
    }

    bool APPEND = !TIMEOUT;
    while(APPEND)
    {   
        digitalWrite(SPI_CS, LOW);

        #if DEBUG
        strncpy(RX_BUF,rx_sim,BUF_LEN);
        rx_sim += 4;
        #else
        SPI.transferBytes(NULL,(uint8_t *)RX_BUF,BUF_LEN);
        #endif

        digitalWrite(SPI_CS, HIGH);

        for (int i= 0; i<BUF_LEN; i++) 
        {
            char rx_c = RX_BUF[i];
            if (rx_c != 0)
            {
                rx_str += rx_c;
            }
            else
            {
                APPEND = false;
                break;
            }
        }

        usleep(delay);
    }

    SPI.endTransaction();

    return TIMEOUT;
}