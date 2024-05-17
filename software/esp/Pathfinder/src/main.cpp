#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.hpp>

const int SPI_SS = D2; // Slave select is active-low

// WPA2 Enterprise Authentication
/*
const char *EAP_SSID = "eduroam";
const char *EAP_ANONYMOUS_IDENTITY = "aem21@ic.ac.uk";
const char *EAP_IDENTITY = "aem21@ic.ac.uk";
const char *EAP_PASSWORD = "";
const char *EAP_USERNAME = "aem21@ic.ac.uk";
const char *ROOT_CA = "-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----";
*/

// WPA2 Personal Authentication

const char *PER_SSID = "AEML_iphone";
const char *PER_PASSWORD = "password";

// Server declaration
WiFiServer server(80);
WiFiClient client;
String request;

// LED states
const int LED = D0;
int LEDState;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const int NUM_CHARS = 3;

char TX_BUF[NUM_CHARS];
uint8_t RX_BUF[NUM_CHARS];

void control(String request)
{
  // turns the GPIOs on and off
  if (request.indexOf("GET /LED26_ON") >= 0) 
  {
    Serial.println("GPIO 26 on");
    LEDState = HIGH;
    digitalWrite(LED, HIGH);
  } 
  else if (request.indexOf("GET /LED26_OFF") >= 0) 
  {
    Serial.println("GPIO 26 off");
    LEDState = LOW;
    digitalWrite(LED, LOW);
  }
}

void setup()
{

  // Serial setup
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Serial started");

  // Connect to Wi-Fi network with SSID and password
  //WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");

  Serial.print(PER_SSID);
  WiFi.begin(PER_SSID, PER_PASSWORD);

  //Serial.println(EAP_SSID);
  //WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD, ROOT_CA);

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

  // Instantiate SPI
  SPI.begin();
  pinMode(SPI_SS, OUTPUT);
  digitalWrite(SPI_SS, HIGH);

  //Initiate LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  LEDState = LOW;
}

void loop()
{

  // WIFI SERVER

  client = server.available(); // Listen for incoming clients

  if (client) 
  { // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;

    //Serial.print("New Client: IP "); // print a message out in the serial port
    //Serial.print(client.localIP());
    //Serial.print(" PORT ");
    //Serial.println(client.localPort());


    String currentLine = "";       // make a String to hold incoming data from the client
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

            control(request);
            client.print("Message received");
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
    request= "";
    // Close the connection
    client.stop();
    Serial.println("");
    Serial.println("Client disconnected.");
    Serial.println("");

  }

  // SPI COMMUNICATION

  char myChar = 'M';
  // put your main code here, to run repeatedly:
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  // Assert slave select -> begin transfer
  digitalWrite(SPI_SS, LOW);
  // SPI.transferBytes(NULL, (uint8_t *)TX_BUF, NUM_CHARS);
  // SPI.transferBytes((const uint8_t *)TX_BUF, NULL, NUM_CHARS + 1);
  // SPI.transfer(TX_BUF, NUM_CHARS);

  SPI.transfer(myChar);
  // SPI.transfer('p');
  // SPI.transfer('\0');
  // SPI.transfer('a');

  // Serial.println((const char*)RX_BUF);

  // De-assert slave select -> end of transfer
  digitalWrite(SPI_SS, HIGH);

  SPI.endTransaction();

  sleep(1);
}