#include <Arduino.h>
#include <SPI.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  Serial.println("Hello all :)");

  SPI.begin();
  
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}