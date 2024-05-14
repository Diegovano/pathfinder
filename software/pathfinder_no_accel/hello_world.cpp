/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
 * 
 * algos from geeksforgeeks
*/

#include <stdio.h>
#include <io.h>
#include <sys/alt_stdio.h>
#include <sys/alt_alarm.h>
#include <alt_types.h>
#include <system.h>
#include <unistd.h>
#include <sys/alt_timestamp.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "altera_avalon_spi_regs.h"

// dijkstra bottom left to top right

const int NUM_VERTICES = 9;

// int myArr[9][9] = 

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
};

// /* begin copied code */
// static void spi_rx_isr(void* isr_context){ 
// IOWR_ALTERA_AVALON_SPI_STATUS(SPI_SLAVE_BASE, 0x0); 
// } 

// int ret; 
// int status; 
// alt_u16 rddata; 

// //this registers slave IRQ with NIOS 

// auto ret = alt_iic_isr_register(SPI_SLAVE_IRQ_INTERRUPT_CONTROLLER_ID,SPI_SLAVE_IRQ,spi_rx_isr,(void *)spi_command_string_tx,0x0); 

// //check if slave status is good 

// do{ 
// status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_SLAVE_BASE); 
// } 
// while ((status & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0); 

// //copy received byte into rddata variable 
// rddata = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_SLAVE_BASE); 

// //set slave IRQ to enable a new byte receiving 

// IOWR_ALTERA_AVALON_SPI_CONTROL(SPI_SLAVE_BASE, ALTERA_AVALON_SPI_CONTROL_SSO_MSK | ALTERA_AVALON_SPI_CONTROL_IRRDY_MSK); 

// /* end copied code */

int main () 
{
  printf("%d", IORD_ALTERA_AVALON_SPI_RXDATA(0));

  // 	// The following is used for timing
	// alt_u64 ticks;
	// alt_u64 freq = alt_timestamp_freq();

  // int myArr[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};

  // Graph myGraph = Graph(myArr);

	// int T = 100;

	// // The code that you want to time goes here
	// alt_timestamp_start();
	// for (int i=0; i<T; i++)
  // {
  //   // myGraph.dijkstra();
  //   usleep(1e5);
	// }
	// ticks = alt_timestamp();


	// int k = 50 * T; // ticks per ms
	// float proc_us = (float)ticks / (float)k;
	// printf("proc_ticks: %llu, proc_us: %f\n", ticks, proc_us);

  // myGraph.print();
}