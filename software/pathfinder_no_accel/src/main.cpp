/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
*/
#include "sys/alt_stdio.h" // for alt_printf
// #include "system.h" // useful MACRO_DEFS

#include <string> // for string
#include <sstream> // for stringstream
#include <queue> // for queue

#include "sys/alt_irq.h" // for alt_ic_isr_register
#include "sys/alt_timestamp.h" // for timing

#include "graph/graph.h"
#include "helpers/helpers.h"
#include "spi_isr/spi_isr.h"
#include "state.h"
#include "json/json.h"

#define DEBUG true
#define TIMING true

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
        if (stateChange) printf("\nIDLE:\n");
        #endif

        //nextState set by ISR
        break;
      }
      case GRAPH_RX:
      {
        #if DEBUG
        if (stateChange) printf("\nGRAPH_RX:\n");
        #endif

        // nextState set by ISR
        break;
      }
      case PATHFINDING:
      {
        #if DEBUG
        if (stateChange) printf("\nPATHFINDING:\n");
        #endif
        GraphFormat graphf(NUM_VERTICES);
        std::string err = deserialiseGraph(context.response, graphf);
        if (err != "") 
        {
          printf("\n%s\n", err.c_str());
          nextState = IDLE;
          break;
        }

        Graph myGraph = Graph((float**)graphf.adj, NUM_VERTICES);

        res.pathfindAvg = 0;
        #if TIMING
        if (graphf.averageOver != 0)
        {
          alt_u64 ticks;
          // alt_u64 freq = alt_timestamp_freq();

          // The code that you want to time goes here
          alt_timestamp_start();

          for (int i=0; i<graphf.averageOver; i++)
          {
            myGraph.reset();
            myGraph.dijkstra();
            // usleep(1e5);
          }

          ticks = alt_timestamp();

          int k = 50 * graphf.averageOver; // ticks per ms
          double proc_us = (double)ticks / (double)k;

          // WARNING: THIS PRINT CAN CAUSE THE SOFTWARE TO HANG RIGHT AFTER ENTERING THE 'PATHFINDING' STATE.
          // printf("Profiling Results: %i iteration(s), \nproc_ticks: %llu,\tproc_us: %f\tavg: %f\n",
          //   graphf.averageOver, ticks, proc_us, proc_us);

          res.pathfindAvg = proc_us;
        }
        else myGraph.dijkstra();
        #else
        myGraph.dijkstra();
        #endif

        const int *shortest = myGraph.shortest();

        res.shortest = new int[NUM_VERTICES];

        for (int i = 0; i < NUM_VERTICES; i++) res.shortest[i] = shortest[i];

        res.start = graphf.start;
        res.end = graphf.end;

        nextState = States::ENQUEUE_RESPONSE;
      }
      break;

      case ENQUEUE_RESPONSE:
      {
        #if DEBUG
        if (stateChange) printf("\nPREPARING RESPONSE:\n");
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
      break;

      case RESPONSE_TX:
        #if DEBUG
        if (stateChange) printf("\nRESPONDING:\n");
        #endif

        delete[] res.shortest;

      break;

      default:
        printf("\nPathfinder in unknown state.\n");
    }
  }
}
