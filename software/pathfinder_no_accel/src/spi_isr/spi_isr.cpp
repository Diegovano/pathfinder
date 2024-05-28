#include "spi_isr.h"

#define DEBUG false

// dijkstra code adapted from geeks for geeks

const int NUM_BYTES = 4;
int hitcount = 0;

std::string conStr;

// is the ISR that runs when the SPI Slave receives data
void spi_rx_isr(void* isr_context)
{
  IsrContext *context = (IsrContext*)isr_context;
  std::queue<char> *TX_QUEUE = context->queue;
  States *state = context->state;

  #ifdef __INTELLISENSE__
  #pragma diag_suppress 20 // ignore missing __builtin_stwio etc...
  #endif

  uint32_t data = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_BASE);

  if (*state == States::IDLE)
  {
    if (data == 0) *context->nextState = States::GRAPH_RX;
    else return;
  }

  //This resets the IRQ flag. Otherwise the IRQ will continuously run.
  IOWR_ALTERA_AVALON_SPI_STATUS(SPI_BASE, 0x0);

  #ifdef __INTELLISENSE__
  #pragma diag_default 20 // restore default behaviour
  #endif

  uint32_t TX_DATA = 0;

  if (*state == States::RESPONSE_TX)
  {
    for (int i = 0; i < 4; i++)
    {
      char next;
      if (TX_QUEUE->empty()) 
      {
        next = '\0';
        // *context->nextState = States::IDLE; 
        *context->nextState = States::GRAPH_RX; 
      }
      else 
      {
        next = TX_QUEUE->front();
        TX_QUEUE->pop();
      }

      TX_DATA = (TX_DATA << 8) | next;
    }
    #if DEBUG
      printf("Transmitting: %ld\n", TX_DATA);
    #endif
  }

  IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_BASE, TX_DATA);

  if (*state == States::GRAPH_RX)
  {
    #if DEBUG
      int status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_BASE);

      printf("\nISR iter %d, status %s, got: %lx \n" , hitcount++, decToBinary(status).c_str(), data);
    #endif
    for (int i = 32 - NUM_BYTES * 8; i < 32; i += 8)
    {
      #if DEBUG
        printf("%c", (char) ((data & (0xFF000000 >> i)) >> (24 - i)));
      #endif

      if ( (char) ((data & (0xFF000000 >> i)) >> (24 - i)) == '\0')
      {
        if (conStr != "") 
        {
          #if !DEBUG
            // printf("%d: %s\n\n\n", hitcount++, conStr.c_str());
          #else
            printf("\n%s\n", conStr.c_str());
          #endif
          context->response = conStr;
          *context->nextState = States::PATHFINDING;
        }

        conStr = "";
        break;
      }
      else conStr += (char) ((data & (0xFF000000 >> i)) >> (24 - i)) ; 
    }
  }
}