#include <Slave.h>

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
    usleep(delay);
}

void Slave::spi_tx_string(char* tx_str, unsigned int size)
{
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    const uint8_t NULL_BUF[BUF_LEN] = {0};

    for (int i = 0; i < size; i++)
    {

        // Assert slave select -> begin transfer
        digitalWrite(SPI_CS, LOW);
        
        SPI.transferBytes((uint8_t*)(tx_str+i), NULL, BUF_LEN);

        digitalWrite(SPI_CS, HIGH);

        // De-assert slave select -> end of transfer
        digitalWrite(SPI_CS, HIGH);

        usleep(delay);
    }

    digitalWrite(SPI_CS, LOW);
    SPI.transferBytes(NULL_BUF,NULL,BUF_LEN);
    digitalWrite(SPI_CS, HIGH);
    usleep(delay);
    
    SPI.endTransaction();
}

bool Slave::spi_rx_string(std::string &rx_str)
{   

    const uint8_t NULL_BUF[BUF_LEN] = {0};

    unsigned long startTime;
    const long timeoutTime = 5000; // in ms

    rx_str = "";
    char RX_BUF[4];

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    
    // WAIT FOR START OF MESSAGE
    startTime = millis();
    bool TIMEOUT = false;
    bool WAIT = true;

    Serial.println("\tWAIT");
    while(WAIT & !TIMEOUT) 
    {
        digitalWrite(SPI_CS, LOW);
        SPI.transferBytes(NULL_BUF,(uint8_t *)RX_BUF,BUF_LEN);
        digitalWrite(SPI_CS, HIGH);
        usleep(delay);

        for (int i=0; i<BUF_LEN; i++) 
        {
            char rx_c = RX_BUF[i];
            // Serial.println(rx_c);
            //Serial.print("\tWAIT: ");
            //Serial.println(WAIT);
            if (!WAIT)
            {
                rx_str += rx_c;
            }
            else if (rx_c != '\0')
            {
                WAIT = false;
                //Serial.print("first valid char: ");
                //Serial.println(rx_c);
                rx_str += rx_c;
            }
        }
        
        if (millis() - startTime > timeoutTime)
        {
            TIMEOUT = WAIT;
            rx_str = "FPGA_RESPONSE_TIMEOUT";
        }

        usleep(10 * delay);
        // usleep(500 * delay);
    }

    bool APPEND = !TIMEOUT;
    Serial.println("\tAPPEND");
    while(APPEND)
    {   
        digitalWrite(SPI_CS, LOW);
        SPI.transferBytes(NULL_BUF,(uint8_t *)RX_BUF,BUF_LEN);
        digitalWrite(SPI_CS, HIGH);
        usleep(delay);

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

        usleep(20 * delay);
    }

    SPI.endTransaction();

    return TIMEOUT;
}