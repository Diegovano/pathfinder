/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
*/
#include "sys/alt_stdio.h" // for alt_printf
#include "sys/alt_cache.h" // for alt_dcache_flush, alt_icacheflush
#include "unistd.h" // for usleep
#include "system.h" // useful MACRO_DEFS

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
#define DIJKSTRA true
#define FULL_HW_DIJKSTRA false
#define DELTA false
#define ASTAR false

#define MULTI true

int main () 
{
  printf("Starting Pathfinder!\n");

  std::queue<char> TX_QUEUE;
  States state, nextState = States::IDLE;
  IsrContext context(&TX_QUEUE, &state, &nextState);

  ResultFormat res;

  int ret = alt_ic_isr_register(SPI_IRQ_INTERRUPT_CONTROLLER_ID, SPI_IRQ, spi_rx_isr, (void *)&context, 0x0);
  printf("IRQ register return %d \n", ret);

  // //You need to enable the IRQ in the IP core control register as well.
  IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_BASE, ALTERA_AVALON_SPI_CONTROL_IRRDY_MSK); // trigger when is ready

  float* HW_Dijkstra_float_pointer = (float*)alt_uncached_malloc(65536);
  unsigned int ctrl = DMA_IRQ_E_BIT | DMA_LEEN | DMA_WORD;
  DMA dma(DMA_BASE, ctrl);
  dma.irq_reg(DMA_IRQ_INTERRUPT_CONTROLLER_ID, DMA_IRQ);

  StarAccelerator myPrecious(ASTAR_ACCEL_BASE, ASTAR_ACCEL_SPAN);
  if (myPrecious.irq_reg(ASTAR_ACCEL_IRQ_INTERRUPT_CONTROLLER_ID, ASTAR_ACCEL_IRQ))
  {
      printf("ERROR failed to register custom isr\n");
  }
  
  while (true)
  {
    bool stateChange = state != nextState;
    state = nextState;
    Graph *myGraph;

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
        GraphFormat *graphf = new GraphFormat;
        std::string err = deserialiseGraph(context.response, graphf);
        if (err != "") 
        {
          printf("\n%s\n", err.c_str());
          nextState = IDLE;
          break;
        }

        // delete myGraph; // this line breaks SPI
        graphf->matrixReshapeHWDijkstra(HW_Dijkstra_float_pointer);
        myGraph = new Graph((float**)graphf->adj, graphf->x, graphf->y, graphf->size, graphf->start, graphf->end); // NEED TO DELETE BUT DELETE CAUSES PROBLEMS

        res.pathfindAvg = 0;
        #if TIMING
        int multi = 0;
        if (graphf->averageOver != 0)
        {
          #if MULTI
          for (int multi = 0; multi < 2; multi++) 
          {
          printf("Starting algorithm %d:\n\n", multi + 1);
          #endif

          alt_64 proc_ticks = 0;
          alt_u64 time1 = 0;
          // alt_u64 overhead = 0;
          alt_u64 time3 = 0;
          // alt_u64 freq = alt_timestamp_freq();

          // The code that you want to time goes here
          alt_timestamp_start();

          for (int i=0; i<graphf->averageOver; i++)
          {
            // printf("Starting iteration %d of %d\n", i, graphf.averageOver);
            // myGraph->reset();

            alt_icache_flush_all();
            alt_dcache_flush_all();

            time1 = alt_timestamp();
            // overhead = alt_timestamp() - time1;
            
            #if MULTI
              switch (multi)
              {
                case 0:
                  myGraph->swDijkstra();
                  break;
                case 1:
                  myGraph->dijkstra();
                  break;
                case 2:
                  myGraph->astar(dma, myPrecious);
                  break;
                case 3:
                  myGraph->delta();
                  break;

              }
            #else
              #if DIJKSTRA
              myGraph->dijkstra();
              #elif FULL_HW_DIJKSTRA
              myGraph->HW_dijkstra(HW_Dijkstra_float_pointer,dma);
              #elif DELTA
              myGraph->delta(150);
              #elif ASTAR
              myGraph->astar(dma, myPrecious);
              #else
              printf("make up your mind");
              #endif
            #endif

            time3 = alt_timestamp();

            // proc_ticks += (time3 - time1 - 1 * overhead);
            proc_ticks += (time3 - time1);
          }

          // ticks = alt_timestamp();

          int k = alt_timestamp_freq() * 1e-6 * graphf->averageOver; // ticks per ms
          double proc_us = (double)proc_ticks / (double)k;

          printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
            graphf->averageOver, proc_ticks, proc_us, proc_us);


          #if MULTI
          switch(multi)
          {
            case 0:
              res.swDijkstraAvg = proc_us;
              res.swDijkstraShortest = cpyArray(myGraph->shortest(), graphf->size);
              break;
            case 1:
              res.dijkstraAvg = proc_us;
              res.dijkstraShortest = cpyArray(myGraph->shortest(), graphf->size);
              break;
            case 2:
              res.astarAvg = proc_us;
              res.astarShortest = cpyArray(myGraph->shortest(), graphf->size);
              break;
            case 3:
              res.deltaAvg = proc_us;
              res.deltaShortest = cpyArray(myGraph->shortest(), graphf->size);
              break;
          }
          #else
          res.pathfindAvg = proc_us;
          #endif
          #if MULTI
          }
          #endif
        }
        else
          #if DIJKSTRA
          myGraph->dijkstra();
          #else
          if(FULL_HW_DIJKSTRA)
            myGraph->HW_dijkstra(HW_Dijkstra_float_pointer, dma);
          else
            myGraph->delta(150);
          #endif
        
        #else
        #if DIJKSTRA
        myGraph->dijkstra();
        #else
        myGraph->delta(150);
        #endif
        #endif

        #if !MULTI
        const int *shortest = myGraph->shortest();

        res.shortest = new int[graphf.size];

        for (int i = 0; i < graphf.size; i++) res.shortest[i] = shortest[i];
        #endif

        res.start = graphf->start;
        res.end = graphf->end;

        nextState = States::ENQUEUE_RESPONSE;
      }
      break;

      case ENQUEUE_RESPONSE:
      {
        #if DEBUG
        if (stateChange) printf("\nPREPARING RESPONSE:\n");
        #endif

        std::string output;

        std::string status = serialiseResult(res, output);

        delete[] res.shortest;

        if (status != "") 
        {
          printf("Unable to serialise JSON object.\nIs there a path with this adjacency matrix? Routing from nodes %d to %d\n\n", res.start, res.end);
          // printf("\n%s", context.response.c_str());
//          myGraph->printAdj();
          myGraph->print();
          nextState = IDLE;
          break;
        }

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

      // delete myGraph;
      
      break;

      default:
        printf("\nPathfinder in unknown state.\n");


    }
  }
}
