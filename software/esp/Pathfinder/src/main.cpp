#include <Arduino.h>
#include <SPI.h>
#include <string>

#define SPI_SS D2


const int NUM_CHARS = 4;

char TX_BUF[NUM_CHARS];
char RX_BUF[NUM_CHARS];


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:

  // uint8_t TX_BUF[NUM_CHARS] = {'p', '\0', 'a'};

  Serial.begin(115200);

  Serial.println("Hello all :)");

  SPI.begin();

  String myStr = "ICL";
  myStr.toCharArray(TX_BUF, NUM_CHARS); // + 1 for \0 (null terminated)

  // SPI.endTransaction();

  // SPI.end();

  int result = myFunction(2, 3);

  pinMode(SPI_SS, OUTPUT);
}

void loop() {
  char myChar = '?';
  // put your main code here, to run repeatedly:
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(SPI_SS, LOW);
  // SPI.transferBytes(NULL, (uint8_t *)TX_BUF, NUM_CHARS + 1); // DOES NOT WORK
  // SPI.transferBytes((const uint8_t *)TX_BUF, NULL, NUM_CHARS + 1);
  SPI.transferBytes((const uint8_t *)TX_BUF, (uint8_t*)RX_BUF, NUM_CHARS);
  // SPI.transferBytes((const uint8_t *)RX_BUF, (uint8_t*)TX_BUF, NUM_CHARS + 1);
  // SPI.transfer(TX_BUF, NUM_CHARS + 1);
  // SPI.transfer(myChar);

  Serial.println((const char*)RX_BUF);
  digitalWrite(SPI_SS, HIGH);
  
  SPI.endTransaction();

  sleep(1);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}