/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
 * 
 * algos from geeksforgeeks
*/
// #include "sys/alt_stdio.h" // for alt_printf
// #include "system.h" // useful MACRO_DEFS

#include "alt_types.h" // for alt_* types
#include "altera_avalon_spi_regs.h" // for IO**_SPI stuff
#include "sys/alt_irq.h" // for alt_ic_isr_register

#include "altera_avalon_pio_regs.h" // for IO**_PIO stuff
#include "ArduinoJson-v6.21.5.h"

#include <iostream> // for cout
#include <string> // for string
#include <sstream> // for stringstream
#include <queue> // for queue

#define DEBUG false
#define TIMING false

using namespace ArduinoJson;

// dijkstra bottom left to top right

const int NUM_BYTES = 4;

const int NUM_VERTICES = 9;

int hitcount = 0;

bool ready = false;

std::string res;

std::queue<char> TX_QUEUE;

// std::string *conStr;
std::string conStr = "";

std::string decToBinary(int n)
{
    // Array to store binary number
    int binaryNum[10];
 
    // Counter for binary array
    int i = 0;
    while (n > 0 && i < 10) {
        // Storing remainder in binary
        // array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
 
    std::stringstream myStr;

    // Printing binary array in reverse
    // order
    for (int j = i - 1; j >= 0; j--)
        myStr << binaryNum[j];

    return myStr.str();
}

class Graph
{
  int **graph;

  int *dist;
  bool *inShortestPath;
  int *predecessor;

  public:
  Graph(int inArr[NUM_VERTICES][NUM_VERTICES])
  {
    graph = new int*[NUM_VERTICES];
    for (int i = 0; i < NUM_VERTICES; i++) graph[i] = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      for (int j = 0; j < NUM_VERTICES; j++) graph[i][j] = inArr[i][j];
    }

    dist = new int[NUM_VERTICES]; 
    inShortestPath = new bool[NUM_VERTICES];
    predecessor = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      dist[i] = __INT_MAX__;
      inShortestPath[i] = false;
    }

    dist[0] = 0; // dist to source is 0 (0 is bottom left)
    predecessor[0] = 0;
  }
  Graph(int **inArr) : graph(inArr)
  {
    dist = new int[NUM_VERTICES]; 
    inShortestPath = new bool[NUM_VERTICES];
    predecessor = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      dist[i] = __INT_MAX__;
      inShortestPath[i] = false;
    }

    dist[0] = 0; // dist to source is 0 (0 is bottom left)
    predecessor[0] = 0;
  }

  void dijkstra()
  {
    for (int step = 0; step < NUM_VERTICES - 1; step++)
    {
      // get min distance node

      int min = __INT_MAX__;
      int min_index = -1;

      for (int i = 0; i < NUM_VERTICES; i++)
      {
        if (!inShortestPath[i] && dist[i] <= min) 
        {
          min = dist[i], min_index = i;
        }
      }

      inShortestPath[min_index] = true;

      for (int i = 0; i < NUM_VERTICES; i++)
      {
        if (!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != __INT_MAX__ && dist[min_index] + graph[min_index][i] < dist[i]) 
        {
          dist[i] = dist[min_index] + graph[min_index][i];
          predecessor[i] = min_index;
        }
      }
    }
  }

  void print()
  {
    std::cout << "distances from bottom left: " << std::endl;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      std::cout << i << ": " << dist[i] << std::endl;
    }

    std::cout << "path from top right: ";

    int vert_id = NUM_VERTICES - 1;

    std::cout << NUM_VERTICES - 1 << ' ';

    do
    {
      std::cout << predecessor[vert_id] << ' ';

      vert_id = predecessor[vert_id];

    } while (predecessor[vert_id] != 0);

    std::cout << 0 << std::endl;
  }

  const int* shortest() const
  {
    return predecessor;
  }
};

//This is the ISR that runs when the SPI Slave receives data

static void spi_rx_isr(void* isr_context)
{
  uint32_t data = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_BASE);
  #if DEBUG
    int status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_BASE);

    printf("\nISR iter %d, status %s, got: %lx \n" , hitcount++, decToBinary(status).c_str(), data);
  #endif

  //This resets the IRQ flag. Otherwise the IRQ will continuously run.
  IOWR_ALTERA_AVALON_SPI_STATUS(SPI_BASE, 0x0);

  uint32_t TX_DATA = 0;

  for (int i = 0; i < 4; i++)
  {
    char next;
    if (TX_QUEUE.empty()) next = '\0';
    else 
    {
      next = TX_QUEUE.front();
      TX_QUEUE.pop();
    }

    TX_DATA = (TX_DATA << 8) | next;
  }
  #if DEBUG
    std::cout << "Transmitting: " << TX_DATA << std::endl;
  #endif

  IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_BASE, TX_DATA);

  for (int i = 32 - NUM_BYTES * 8; i < 32; i += 8)
  {
    #if DEBUG
      std::cout << (char) ((data & (0xFF000000 >> i)) >> (24 - i));
    #endif

    if ( (char) ((data & (0xFF000000 >> i)) >> (24 - i)) == '\0')
    {
      if (conStr != "") 
      {
        #if !DEBUG
          std::cout << hitcount++ << ": " << conStr << std::endl << std::endl << std::endl;
        #endif
        res = conStr;
        ready = true;
      }

      conStr = "";
      break;
    }
    else conStr += (char) ((data & (0xFF000000 >> i)) >> (24 - i)) ; 
  }
}

int main () 
{
  printf("Hello from Nios II!\n");

  char spi_command_string_tx[10] = "CIAO";
  int ret = alt_ic_isr_register(SPI_IRQ_INTERRUPT_CONTROLLER_ID, SPI_IRQ, spi_rx_isr, (void *)spi_command_string_tx, 0x0);
  printf("IRQ register return %d \n", ret);

  // //You need to enable the IRQ in the IP core control register as well.
  IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_BASE, ALTERA_AVALON_SPI_CONTROL_IRRDY_MSK); // trigger when is ready

  const int NUM_NODES = 100;
  const int CAPACITY = 2 * JSON_OBJECT_SIZE(NUM_NODES) + 1 * JSON_OBJECT_SIZE(NUM_NODES ^ 2) + 3 * JSON_OBJECT_SIZE(1);

  StaticJsonDocument<CAPACITY> inDoc;

  while (true)
  {
   if (ready)
   {
      ready = false;

      // std::cout << "Deserialising data: " << res << std::endl;

      DeserializationError err = deserializeJson(inDoc, res.c_str());

      if (err)
      {
        std::cout << "Error Deserialising Received Data: " << err.c_str() << std::endl;
      }

      int tempArr[NUM_VERTICES][NUM_VERTICES];

      for (int i = 0; i < NUM_VERTICES; i++)
      {
        for (int j = 0; j < NUM_VERTICES; j++)
        {
          tempArr[i][j] = inDoc["adj"][i][j];
        }
      }

      int T = 100;
      // Graph myGraph = Graph(inDoc.as<int[NUM_VERTICES][NUM_VERTICES]>());
      Graph myGraph = Graph(tempArr);

      #if TIMING
      alt_u64 ticks;
      alt_u64 freq = alt_timestamp_freq();

      // The code that you want to time goes here
      alt_timestamp_start();
      #endif

      for (int i=0; i<T; i++)
      {
        myGraph.dijkstra();
        // usleep(1e5);
      }

      #if TIMING
      ticks = alt_timestamp();

      int k = 50 * T; // ticks per ms
      float proc_us = (float)ticks / (float)k;
      printf("proc_ticks: %llu, proc_us: %f\n", ticks, proc_us);
      #endif

      // myGraph.print();


      const int *shortest = myGraph.shortest();

      StaticJsonDocument<CAPACITY> outDoc;


      JsonArray sht = outDoc["sht"].to<JsonArray>();

      int vert_id = NUM_VERTICES - 1; // starting vertex

      do
      {
        sht.add(shortest[vert_id]);

        vert_id = shortest[vert_id];

      } while (shortest[vert_id] != 0);

      sht.add(0);

      std::string response;
      
      serializeJson(outDoc, response);

      for(char i : response)
      {
        #if DEBUG
          std::cout << "Adding " << i << " to queue" << std::endl;
        #endif
        TX_QUEUE.push(i);
      }
    }
  }
}