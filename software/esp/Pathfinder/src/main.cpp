#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <array>

#include <ArduinoJson.hpp>

#define SPI_SS D2

using namespace ArduinoJson;

const int NUM_CHARS = 4;
const int BUF_LEN = 512;

int shift = 0;

char TX_BUF[BUF_LEN];
char RX_BUF[BUF_LEN];


// std::array<std::array<int, 9>, 9> adjMatx = {{{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}}};
int adjMatx[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:

  // uint8_t TX_BUF[NUM_CHARS] = {'p', '\0', 'a'};

  Serial.begin(115200);

  Serial.println("Hello all :)");

  SPI.begin();

  String myStr = "ICL";
  // myStr.toCharArray(TX_BUF, NUM_CHARS); // + 1 for \0 (null terminated)

  // SPI.endTransaction();

  // SPI.end();

  int result = myFunction(2, 3);

  pinMode(SPI_SS, OUTPUT);

  JsonDocument doc;

  JsonArray parent = doc.to<JsonArray>();

  
  for (int i = 0; i < 9; i++)
  {
    JsonArray child = parent.add<JsonArray>();
    for (int j = 0; j < 9; j++)
    {
      child.add(adjMatx[i][j]);
    }
  }

  // char output[256];

  // for (int i = 0; i < BUF_LEN; i++)
  // {
  //   // TX_BUF[i] = RX_BUF[i] = '\0';
  //   // TX_BUF[i] = RX_BUF[i] = i % 26 + 'a';
  //   TX_BUF[i] = i % 26 + 'a';
  // }

  // TX_BUF[511] = '\0';

  serializeJson(doc, TX_BUF);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.transfer32(0);
  SPI.endTransaction();
}

void loop() {
  if (shift % 128 == 0) SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(SPI_SS, LOW);
  SPI.transferBytes((const uint8_t *)TX_BUF + 4 * (shift % 128), (uint8_t*)RX_BUF, NUM_CHARS);
  digitalWrite(SPI_SS, HIGH);

  Serial.println(shift % 128);
  // digitalWrite(SPI_SS, HIGH);
  

  shift++;
  if (shift % 128 == 0) 
  {
    // SPI.endTransaction();

    // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(SPI_SS, LOW);
    SPI.transfer32((uint32_t)0x0);
    digitalWrite(SPI_SS, HIGH);

    SPI.endTransaction();

    sleep(2);

  }
  // SPI.endTransaction();

  usleep(1e3);
  char hi = ' ';
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}