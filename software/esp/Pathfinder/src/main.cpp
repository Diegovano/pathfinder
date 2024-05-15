#include <Arduino.h>
#include <SPI.h>
#include <string>

#define SPI_SS D2


const int NUM_CHARS = 3;

char TX_BUF[NUM_CHARS];
uint8_t RX_BUF[NUM_CHARS];


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:

  // uint8_t TX_BUF[NUM_CHARS] = {'p', '\0', 'a'};

  Serial.begin(115200);

  Serial.println("Hello all :)");

  SPI.begin();

  String myStr = "AM";
  myStr.toCharArray(TX_BUF, NUM_CHARS + 1); // + 1 for \0 (null terminated)

  // SPI.endTransaction();

  // SPI.end();

  int result = myFunction(2, 3);

  pinMode(SPI_SS, OUTPUT);
}

void loop() {
  char myChar = 'M';
  // put your main code here, to run repeatedly:
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(SPI_SS, LOW);
  // SPI.transferBytes(NULL, (uint8_t *)TX_BUF, NUM_CHARS);
  // SPI.transferBytes((const uint8_t *)TX_BUF, NULL, NUM_CHARS + 1);
    // SPI.transfer(TX_BUF, NUM_CHARS);

  SPI.transfer(myChar);
  // SPI.transfer('p');
  // SPI.transfer('\0');
  // SPI.transfer('a');

  // Serial.println((const char*)RX_BUF);
  digitalWrite(SPI_SS, HIGH);
  
  SPI.endTransaction();

  sleep(1);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}