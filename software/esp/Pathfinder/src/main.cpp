#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <array>

#include <ArduinoJson.hpp>

#define SPI_SS D2

using namespace ArduinoJson;

const int BUF_LEN = 4;

int shift = 0;

char TX_BUF[BUF_LEN + 1];
char RX_BUF[BUF_LEN];

std::string message;

bool terminated = true;

// std::array<std::array<int, 9>, 9> adjMatx = {{{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}}};
int adjMatx[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:

  // uint8_t TX_BUF[BUF_LEN] = {'p', '\0', 'a'};

  TX_BUF[BUF_LEN] = '\0'; // hacky to ensure null char at end to print easily

  Serial.begin(115200);

  Serial.println("Hello all :)");

  SPI.begin();

  String myStr = "ICL";
  // myStr.toCharArray(TX_BUF, BUF_LEN); // + 1 for \0 (null terminated)

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

  // for (int i = 0; i < BUF_LEN; i++)
  // {
  //   // TX_BUF[i] = RX_BUF[i] = '\0';
  //   // TX_BUF[i] = RX_BUF[i] = i % 26 + 'a';
  //   TX_BUF[i] = i % 26 + 'a';
  // }

  // TX_BUF[511] = '\0';

  serializeJson(doc, message);

  // message = "#include <string> #include <iostream> int main(){std::string str = \"hello\";char buf[3];buf[2] = \'\\0\';strncpy(buf, str.c_str() + 6, 2);std::cout << buf << std::endl;}";
  message = "Est quasi autem blanditiis nesciunt magnam. Suscipit et ex qui enim velit dolores. Modi accusamus aliquam odit ex consequatur minus et sint. Et magnam vel distinctio et placeat perspiciatis. Error dolor quos aut. Et ullam ab accusantium asperiores nesciunt. Et voluptatibus voluptatem delectus. Aut quis ad fugiat pariatur eveniet rerum sed. In soluta sint dolore id dignissimos et. Consequatur rerum qui impedit blanditiis nihil rerum. Sint sapiente et consectetur eum labore officia est non. Quidem quia molestias ut provident et. Magnam corrupti ut cumque voluptatem nam rem deserunt. Ut velit aut ea voluptatem quisquam. Repellendus placeat repudiandae quidem rerum. Accusamus dignissimos non ea tempore consequatur tenetur libero omnis. Amet omnis quae repellat suscipit vero. Autem enim maiores omnis. Beatae reiciendis quod vero est natus veniam. A id reiciendis harum vero. Totam dolor voluptatum sunt soluta nihil dolorem repudiandae officia. Impedit assumenda sed illo provident illum.";
  // message = "Est quasi autem blanditiis nesciunt magnam.";

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(SPI_SS, LOW);
  SPI.transfer32(0);
  digitalWrite(SPI_SS, HIGH);
  SPI.endTransaction();
}

void loop() {
  if (terminated)
  {
    terminated = false;
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    shift = 0;
  }

  strncpy(TX_BUF, message.c_str() + shift, BUF_LEN);
  Serial.println(TX_BUF);

  digitalWrite(SPI_SS, LOW);
  SPI.transferBytes((const uint8_t *)TX_BUF, (uint8_t*)RX_BUF, BUF_LEN);
  digitalWrite(SPI_SS, HIGH);

  Serial.println(shift);

  shift += BUF_LEN;
  if (shift > message.size()) 
  {
    terminated = true;

    digitalWrite(SPI_SS, LOW);
    SPI.transfer32(0);
    digitalWrite(SPI_SS, HIGH);

    SPI.endTransaction();

    // sleep(2);
  }

  usleep(1e3);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}