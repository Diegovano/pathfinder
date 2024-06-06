#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.h>
#include <Slave.h>

#define SKIP_RX false
#define WIFI false

const int SPI_CS = D2; // Slave select is active-low
const int BUF_LEN = 4;

enum State
{
  HOST_LISTEN,
  HOST_RX,
  FPGA_TX,
  FPGA_RX,
  HOST_TX,
  RESET
};

State state;

// WPA2 Personal Authentication

const char *PER_SSID = "LAPTOP-GLC7SFN7-8537";
const char *PER_PASSWORD = "U00#10s0";

// Server declaration
WiFiServer server(80);
WiFiClient client;
std::string request;
std::string response;

// LED states
const int LED = D0;
int LEDState;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

Slave FPGA(SPI_CS, BUF_LEN);

void setup()
{

  // Serial setup
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Serial started");

#if WIFI
  // Connect to Wi-Fi network with SSID and password
  // WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");

  Serial.print(PER_SSID);
  WiFi.begin(PER_SSID, PER_PASSWORD);

  // Serial.println(EAP_SSID);
  // WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD, ROOT_CA);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
#else
  float adjMatx[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};
  float x[9] = {9.5, 7.74, 4.51, 2.53, 2.30, 6.07, 2.73, 6.71, 10.88};
  float y[9] = {1.72, 4.59, 4.31, 4.38, 10.46, 6.24, 6.40, 9.03, 6.46};
  const int size = 9;
  int start = 0, end = 8;
  int averageOver = 1000; // number of pathfinding tasks to perform to determine average time required
  std::string message;

  JsonDocument doc;

  JsonArray parent = doc["adj"].to<JsonArray>();
  JsonArray xArr = doc["x"].to<JsonArray>();
  JsonArray yArr = doc["y"].to<JsonArray>();
  
  for (int i = 0; i < 9; i++)
  {
    xArr.add(x[i]);
    yArr.add(y[i]);
    JsonArray child = parent.add<JsonArray>();
    for (int j = 0; j < 9; j++)
    {
      child.add(adjMatx[i][j]);
    }
  }

  doc["size"] = size;
  doc["start"] = start;
  doc["end"] = end;
  doc["avgCount"] = averageOver;

  // for (int i = 0; i < BUF_LEN; i++)
  // {
  //   // TX_BUF[i] = RX_BUF[i] = '\0';
  //   // TX_BUF[i] = RX_BUF[i] = i % 26 + 'a';
  //   TX_BUF[i] = i % 26 + 'a';
  // }

  // TX_BUF[511] = '\0';

  serializeJson(doc, message);
#endif

  // Begin FPGA SPI Slave
  FPGA.begin();

  // Initiate LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  LEDState = LOW;

  // Initiate state
  #if WIFI
  state = HOST_LISTEN;
  #else
  state = FPGA_TX;
  request = message;
  // Serial.println(message.c_str());
  #endif
}

void loop()
{

  switch (state)
  {
  case HOST_LISTEN:
  {
    #if !WIFI
    state = FPGA_TX;
    break;
    #endif
    Serial.println("HOST_LISTEN");
    client = server.available(); // Listen for incoming clients

    if (client)
    {                                  // If a new client connects,
      Serial.print("New Client: IP "); // print a message out in the serial port
      Serial.print(client.localIP());
      Serial.print(" PORT ");
      Serial.println(client.localPort());
      state = HOST_RX;
    }
    break;
  }

  case HOST_RX:
  {
    Serial.println("HOST_RX");
    currentTime = millis();
    previousTime = currentTime;

    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected

      currentTime = millis();
      if (client.available())
      { // if there's bytes to read from the client,

        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        request += c;
        if (c == '\n')
        {
          // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            state = FPGA_TX;
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    break;
  }

  case FPGA_TX:
  {
    Serial.println("FPGA_TX");
    FPGA.spi_tx_string(request);
    #if SKIP_RX
    state = RESET;
    #else
    state = FPGA_RX;
    // state = FPGA_TX;
    #endif
    break;
  }

  case FPGA_RX:
  {
    Serial.println("FPGA_RX");
    FPGA.spi_rx_string(response);
    #if WIFI
    state = HOST_TX;
    #else
    Serial.println(response.c_str());
    state = FPGA_TX;
    #endif
    break;
  }

  case HOST_TX:
  {
    Serial.println("HOST_TX");
    client.println(response.c_str());
    Serial.println("SUCCESSFUL EXCHANGE! moving to RESET\n");
    state = RESET;
    break;
  }

  case RESET:
  {
    Serial.println("RESET");
    // clear request and response
    request = "";
    response = "";

    // Close the connection
    client.stop();
    Serial.println("Client disconnected, moving to HOST_LISTEN");
    state = HOST_LISTEN;
    break;
  }

  default:
    Serial.println("default");
    Serial.println("ERROR! moving to RESET");
    state = RESET;
  }

  sleep(2);
}