#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.h>
#include <Slave.h>
#include <vector>

#define SKIP_RX false
#define WIFI false
#define ADJ_MATX false

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

struct Edge {
    int target;
    float length;
};

bool convertToJson(const Edge& t, JsonVariant variant) {
  char buffer[128];

  JsonObject edge;

  variant.add(t.target);
  variant.add(t.length);
  
  return variant;
}

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
  #if ADJ_MATX
  
  const int SIZE = 9;
  float adjMatx[SIZE][SIzE] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};
  float x[SIZE] = {9.5, 7.74, 4.51, 2.53, 2.30, 6.07, 2.73, 6.71, 10.88};
  float y[SIZE] = {1.72, 4.59, 4.31, 4.38, 10.46, 6.24, 6.40, 9.03, 6.46};

  #else


  const int SIZE = 13;
  std::vector<std::vector<Edge>> adjList(SIZE);;
  
  adjList[0]  = {{6, 33.532}, {12, 53.887}};
  adjList[1]  = {{11, 62.337}, {12, 33.73}};
  adjList[2]  = {{12, 19.803}, {1, 28.225}};
  adjList[3]  = {{8, 10.796}, {5, 154.774}, {4, 122.668}};
  adjList[4]  = {{3, 122.668}};
  adjList[5]  = {{3, 154.774}};
  adjList[6]  = {{0, 33.532}, {10, 181.396}, {9, 183.129}};
  adjList[7]  = {{8, 143.458}};
  adjList[8]  = {{10, 41.055}, {3, 10.796}, {7, 143.458}};
  adjList[9]  = {{6, 183.129}};
  adjList[10] = {{8, 41.055}, {6, 181.396}};
  adjList[11] = {{1, 62.337}};
  adjList[12] = {{0, 53.887}, {2, 19.803}};

  // nodes[0]  = {5709222.9834205909, 696108.62011866691}; // Node 276548
  // nodes[1]  = {5709191.8782056486, 696118.27943041269}; // Node 25291708
  // nodes[2]  = {5709211.2912476128, 696055.84373104712}; // Node 25291709
  // nodes[3]  = {5709236.2511749854, 696033.52349726949}; // Node 25473589
  // nodes[4]  = {5709284.5927414894, 695997.60383067257}; // Node 25473590
  // nodes[5]  = {5709208.1398356995, 696036.22818386171}; // Node 25473592
  // nodes[6]  = {5709009.2122519007, 696190.09544731071}; // Node 26389375
  // nodes[7]  = {5709019.8233349705, 696188.06609560957}; // Node 26389434
  // nodes[8]  = {5709006.0310580395, 696323.27681472222}; // Node 26389442
  // nodes[9]  = {5709029.0720731048, 696311.54391213995}; // Node 32618386
  // nodes[10] = {5709012.7329801535, 696147.49481777672}; // Node 34519894
  // nodes[11] = {5709225.9788216352, 696298.79496300314}; // Node 1691189781
  // nodes[12] = {5709058.8068370707, 696308.78188286372}; // Node 3764277424

  std::vector<float> x = {5709222.9834205909, 5709191.8782056486, 5709211.2912476128, 5709236.2511749854, 5709284.5927414894, 5709208.1398356995, 5709009.2122519007, 5709019.8233349705, 5709006.0310580395, 5709029.0720731048, 5709012.7329801535, 5709225.9788216352, 5709058.8068370707};
  std::vector<float> y = {696108.62011866691, 696118.27943041269, 696055.84373104712, 696033.52349726949, 695997.60383067257, 696036.22818386171, 696190.09544731071, 696188.06609560957, 696323.27681472222, 696311.54391213995, 696147.49481777672, 696298.79496300314, 696308.78188286372};
  
  #endif

  int start = 1, end = 5;
  int averageOver = 1000; // number of pathfinding tasks to perform to determine average time required
  std::string message;

  JsonDocument doc;

  #if ADJ_MATX
  JsonArray parent = doc["adjMatx"].to<JsonArray>();
  #else
  JsonArray parent = doc["adjList"].to<JsonArray>();
  #endif
  JsonArray xArr = doc["x"].to<JsonArray>();
  JsonArray yArr = doc["y"].to<JsonArray>();

  if (SIZE != y.size()) throw "Size mismatch";
  #if ADJ_MATX
  if (SIZE != adjMatx.size() && SIZE != adjMatx.at(0).size()) throw "Size Mismatch";
  #endif // no error case for adjList, empty list means no connections
  
  for (int i = 0; i < SIZE; i++)
  {
    xArr.add(x.at(i));
    yArr.add(y.at(i));
    JsonArray child = parent.add<JsonArray>();
    #if ADJ_MATX
    for (int j = 0; j < SIZE; j++)
    {
      child.add(adjMatx[i][j]);
    }
    #else
    for (int j = 0; j < adjList.at(i).size(); j++)
    {
      child.add(adjList.at(i).at(j));
    }
    #endif
  }

  doc["size"] = SIZE;
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
    Serial.println(request.c_str());
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