#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.h>
#include <Slave.h>

#define WIFI FALSE

const int SPI_CS = D2; // Slave select is active-low

std::string message;

// WPA2 Personal Authentication

const char *PER_SSID = "LAPTOP-GLC7SFN7-8537";
const char *PER_PASSWORD = "U00#10s0";

// Server declaration
WiFiServer server(80);
WiFiClient client;
std::string request;

// LED states
const int LED = D0;
int LEDState;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

Slave FPGA(SPI_CS);

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
  int adjMatx[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};

  JsonDocument doc;

  JsonArray parent = doc["adj"].to<JsonArray>();

  
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
#endif

  // Begin FPGA SPI Slave
  FPGA.begin();

  // Initiate LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  LEDState = LOW;
}

void loop()
{

#if WIFI
  // WIFI SERVER

  client = server.available(); // Listen for incoming clients

  if (client)
  { // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;

    Serial.print("New Client: IP "); // print a message out in the serial port
    Serial.print(client.localIP());
    Serial.print(" PORT ");
    Serial.println(client.localPort());

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
            Graph graph(request);
            FPGA.spi_tx_string(graph.to_json().c_str());
            std::string response = FPGA.spi_rx_string();
            client.println(response.c_str());

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

    // Clear the header variable
    request = "";
    // Close the connection
    client.stop();
    Serial.println("");
    Serial.println("Client disconnected.");
    Serial.println("");
  }
#else
  Serial.print("Response: ");
  Serial.println(FPGA.spi_tx_string(message.c_str()).c_str());
  // std::string response = FPGA.spi_rx_string();
  // Serial.println(response.c_str());
#endif

  sleep(2);
}