/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
*/
// #include "sys/alt_stdio.h" // for alt_printf
// #include "system.h" // useful MACRO_DEFS

#include <string> // for string
#include <sstream> // for stringstream
#include <queue> // for queue

#include "sys/alt_irq.h" // for alt_ic_isr_register

#include "graph/graph.h"
#include "helpers/helpers.h"
#include "spi_isr/spi_isr.h"
#include "state.h"
#include "json/json.h"

#define DEBUG true
#define TIMING false

int main () 
{
  printf("Starting Pathfinder!\n");

  const int NUM_VERTICES = 9;

  std::queue<char> TX_QUEUE;
  States state = States::IDLE, nextState = States::IDLE;
  IsrContext context(&TX_QUEUE, &state, &nextState);

  ResultFormat res;

  int ret = alt_ic_isr_register(SPI_IRQ_INTERRUPT_CONTROLLER_ID, SPI_IRQ, spi_rx_isr, (void *)&context, 0x0);
  printf("IRQ register return %d \n", ret);

  #ifdef __INTELLISENSE__
  #pragma diag_suppress 20 // ignore missing __builtin_stwio etc...
  #endif

  // //You need to enable the IRQ in the IP core control register as well.
  IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_BASE, ALTERA_AVALON_SPI_CONTROL_IRRDY_MSK); // trigger when is ready

  #ifdef __INTELLISENSE__
  #pragma diag_default 20 // restore default behaviour
  #endif

  while (true)
  {
    bool stateChange = state != nextState;
    state = nextState;

    switch (state)
    {
      case IDLE:
      {
        #if DEBUG
        if (stateChange) printf("IDLE:\n");
        #endif


      }
      case GRAPH_RX:
      {
        #if DEBUG
        if (stateChange) printf("GRAPH_RX:\n");
        #endif

        // nextState set by ISR
        break;
      }
      case PATHFINDING:
      {
        #if DEBUG
        if (stateChange) printf("PATHFINDING:\n");
        #endif

        GraphFormat graphf(NUM_VERTICES);

        std::string err = deserialiseGraph(context.response, graphf);

        if (err != "") 
        {
          printf(err.c_str());
          break;
        }

        Graph myGraph = Graph((int**)graphf.adj, NUM_VERTICES);

        #if TIMING
        alt_u64 ticks;
        alt_u64 freq = alt_timestamp_freq();

        // The code that you want to time goes here
        alt_timestamp_start();

        int T = 100;

        for (int i=0; i<T; i++)
        {
          myGraph.dijkstra();
          // usleep(1e5);
        }

        ticks = alt_timestamp();

        int k = 50 * T; // ticks per ms
        float proc_us = (float)ticks / (float)k;
        printf("proc_ticks: %llu, proc_us: %f\n", ticks, proc_us);
        #endif

        myGraph.dijkstra();

        res.shortest = myGraph.shortest();

        nextState = States::RESPONSE_TX;
      }
      break;

      case ENQUEUE_RESPONSE:
      {
        #if DEBUG
        if (stateChange) printf("PREPARING RESPONSE:\n");
        #endif

        std::string output;

        serialiseResult(res, output);

        #if DEBUG
          printf("Adding %s to queue\n", output.c_str());
        #endif
        
        for(char i : output)
        {
          TX_QUEUE.push(i);
        }

        nextState = RESPONSE_TX;
      }

      case RESPONSE_TX:
        #if DEBUG
        if (stateChange) printf("RESPONDING:\n");
        #endif

      break;

      default:
        printf("\nPathfinder in unknown state.\n");
    }
  }
}