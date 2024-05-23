#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.h>
#include <Slave.h>

#define SKIP_RX false

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

  // Begin FPGA SPI Slave
  FPGA.begin();

  // Initiate LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  LEDState = LOW;

  // Initiate state
  state = HOST_LISTEN;
}

void loop()
{

  switch (state)
  {
  case HOST_LISTEN:
  {
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
    #endif
    break;
  }

  case FPGA_RX:
  {
    Serial.println("FPGA_RX");
    FPGA.spi_rx_string(response);
    state = HOST_TX;
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

  usleep(1e6);
}