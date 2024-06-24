/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <alt_types.h>
#include <stdio.h>
#include <sys/alt_timestamp.h> // for timing
#include <system.h>
#include <math.h>
#include "includes_and_settings.h"


#define ALT_CI_DIJKSTRA32_FLOAT_4(n,A,B) __builtin_custom_pnif(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_4_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_4_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_1(n,A,B) __builtin_custom_fnii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_1_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_1_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_2(n,A,B) __builtin_custom_fnii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_2_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_2_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_3(n,A,B) __builtin_custom_inii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_3_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_3_N_MASK ((1<<8)-1)

// macro determines data type of inputs, doing problematic casting
void write_dijkstra_cache(float edge_value, int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
//	int dummy = 0;
//	memcpy((void*)&dummy, (void*)&edge_value, 4);
	float datab = edge_value;
	ALT_CI_DIJKSTRA32_FLOAT_4(0,dataa,datab);
}

//writing single data with DMA
void DMA_write_dijkstra_cache(float edge_value, int from_node, int to_node){
	unsigned int ctrl = DMA_IRQ_E_BIT | DMA_LEEN | DMA_WORD;
	  DMA dma(DMA_0_BASE, ctrl);
	  dma.irq_reg(DMA_0_IRQ_INTERRUPT_CONTROLLER_ID, DMA_0_IRQ);


	float* float_in_ptr = (float*)alt_uncached_malloc(4);
	 *float_in_ptr = edge_value;
	 unsigned int address = DIJKSTRA32_FLOAT_0_BASE + (to_node << 6) + (from_node << 2);
	 dma.copy((void*)float_in_ptr, (void*)(address), 4);

}

float read_dijkstra_cache(int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	return ALT_CI_DIJKSTRA32_FLOAT_1(1,dataa,0);
}

int read_path_vector(int node){
	long dataa = node;
	return ALT_CI_DIJKSTRA32_FLOAT_3(3,dataa,0);
}

float run_dijkstra (int from_node, int to_node, int number_of_nodes){
	ALT_CI_DIJKSTRA32_FLOAT_2(2,0,0); //a manual software reset cuz hardware reset buggy
	long dataa = (to_node <<16) + from_node;
	long datab = number_of_nodes;
	return ALT_CI_DIJKSTRA32_FLOAT_2(2,dataa,datab);
}

int main()
{
  printf("Hello from Nios II!\n");

  //DMA setup



  for (int row=0; row < 16; row++){
	  for (int column=0; column < 16; column++){
		  if (row == column){
			  DMA_write_dijkstra_cache(0,row, column);
		  }
		  else{
			  DMA_write_dijkstra_cache(0x7F800000,row, column);
		  }
	  }
  }

//  float* float_in_ptr = (float*)alt_uncached_malloc(4);
//  float* float_out_ptr = (float*) alt_uncached_malloc(4);
  write_dijkstra_cache(33.3, 13, 6);
  printf("Custom instruction read before DMA: %f \n", read_dijkstra_cache(13,6));

//  *float_in_ptr = 24.4;
//  unsigned int node_from = 13;
//  unsigned int node_to = 6;
//  unsigned int address = DIJKSTRA32_FLOAT_0_BASE + (node_to << 10) + (node_from << 2);
//  dma.copy((void*)float_in_ptr, (void*)(address), 4);
  DMA_write_dijkstra_cache(22.4,13,6);


  printf("Custom instruction read after DMA: %f \n", read_dijkstra_cache(13,6));


  printf("\n writing test matrix \n");

  DMA_write_dijkstra_cache(33,13,6);
  DMA_write_dijkstra_cache(53.887, 13, 12);

  DMA_write_dijkstra_cache(62.337, 1, 11);
  DMA_write_dijkstra_cache(33.73, 1, 12);

  DMA_write_dijkstra_cache(19.803, 2, 12);
  DMA_write_dijkstra_cache(28.225, 2, 1);

  DMA_write_dijkstra_cache(10.796, 3, 8);
  DMA_write_dijkstra_cache(154.774, 3, 5);
  DMA_write_dijkstra_cache(122.668, 3, 4);

  DMA_write_dijkstra_cache(122.668, 4, 3);

  DMA_write_dijkstra_cache(154.774, 5, 3);

  DMA_write_dijkstra_cache(33.532, 6, 13);
  DMA_write_dijkstra_cache(181.396, 6, 10);
  DMA_write_dijkstra_cache(183.129, 6, 9);

  DMA_write_dijkstra_cache(143.458, 7, 8);

  DMA_write_dijkstra_cache(41.055, 8, 10);
  DMA_write_dijkstra_cache(10.796, 8, 3);
  DMA_write_dijkstra_cache(143.458, 8, 7);

  DMA_write_dijkstra_cache(183.129, 9, 6);

  DMA_write_dijkstra_cache(41.055, 10, 8);
  DMA_write_dijkstra_cache(181.396, 10, 6);

  DMA_write_dijkstra_cache(62.337, 11, 1);

  DMA_write_dijkstra_cache(53.887, 12, 13);
  DMA_write_dijkstra_cache(19.803, 12, 2);



  for (int row = 0; row <14; row++){
  	  for (int column=0; column <14; column++){
  		  printf("%f \t", read_dijkstra_cache(row,column));
  	  }
  	  printf("\n");
  }

  printf("\nshortest distance: %f \n", run_dijkstra(1,5,14));
 for (int i=0; i <14; i++){
	 printf("%d: %d \n",i,read_path_vector(i));
 }



//
 float adjMatrix[16][16];
 for(int i = 0; i<16;i++){
	 for (int j = 0; j <16 ; j++){
		 	if (i==j){
		 		adjMatrix[i][j] = 0;
		 	}
		 	else{
		 		adjMatrix[i][j] = 0x7F800000;
		 	}

		 }
 }
 adjMatrix[13][6] = 33.532;
  adjMatrix[13][12] = 53.887;
  adjMatrix[1][11] = 62.337;
  adjMatrix[1][12] = 33.73;
  adjMatrix[2][12] = 19.803;
  adjMatrix[2][1] = 28.225;
  adjMatrix[3][8] = 10.796;
  adjMatrix[3][5] = 154.774;
  adjMatrix[3][4] = 122.668;
  adjMatrix[4][3] = 122.668;
  adjMatrix[5][3] = 154.774;
  adjMatrix[6][13] = 33.532;
  adjMatrix[6][10] = 181.396;
  adjMatrix[6][9] = 183.129;
  adjMatrix[7][6] = 143.458;
  adjMatrix[8][10] = 41.055;
  adjMatrix[8][3] = 10.796;
  adjMatrix[8][7] = 143.458;
  adjMatrix[9][6] = 183.129;
  adjMatrix[10][8] = 41.055;
  adjMatrix[10][6] = 181.396;
  adjMatrix[11][1] = 62.337;
  adjMatrix[12][13] = 53.887;
  adjMatrix[12][2] = 19.803;


//
 float* float_in_ptr = (float*)alt_uncached_malloc(1024);
 float* temp_pointer = float_in_ptr;
 for(int from = 0; from<16;from++){
	 for (int column = 0; column <16 ; column++){
		 	 *temp_pointer = adjMatrix[column][from];
		 	 temp_pointer++;

		 }
 }
 	 unsigned int ctrl = DMA_IRQ_E_BIT | DMA_LEEN | DMA_WORD;
 	  DMA dma(DMA_0_BASE, ctrl);
 	  dma.irq_reg(DMA_0_IRQ_INTERRUPT_CONTROLLER_ID, DMA_0_IRQ);

 	 unsigned int address = DIJKSTRA32_FLOAT_0_BASE;
 	  //write cache with data
 	 dma.copy((void*)float_in_ptr, (void*)(address), 1024);
 	 printf("done DMA copying cache\n");

 	for (int row = 0; row <14; row++){
 	  	  for (int column=0; column <14; column++){
 	  		  printf("%f \t", read_dijkstra_cache(row,column));
 	  	  }
 	  	  printf("\n");
 	  }


	 printf("done reseting cache\n");


	 alt_64 proc_ticks = 0;
	 alt_64 cache_ticks = 0;
	 alt_64 total_ticks = 0;
	 alt_u64 time1 = 0;
	 alt_u64 time2 = 0;
	 alt_u64 time3 = 0;

    int iterations = 1000;
    // The code that you want to time goes here
    alt_timestamp_start();
    time1 = alt_timestamp(); //
    for (int i=0; i<iterations; i++){
//
      time1 = alt_timestamp();
      dma.copy((void*)float_in_ptr, (void*)(address), 1024);
  	  time2 = alt_timestamp();
  	  run_dijkstra(1,5,14);
  	  time3 = alt_timestamp();

  	  proc_ticks += (time3 - time2);
	  cache_ticks += (time2 - time1);
	  total_ticks += (time3 - time1);
    }
    int k = alt_timestamp_freq() * 1e-6 * iterations; // ticks per ms
    double proc_us = (double)proc_ticks / (double)k;
    double total_us = (double)total_ticks / (double)k;
    double cache_us = (double)cache_ticks / (double)k;

    printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
  		  iterations, proc_ticks, proc_us, proc_us);
    printf("Profiling Results: %i iteration(s), \ncache_ticks: %lld,\tcache_us: %f\tavg: %f\n",
      		  iterations, cache_ticks, cache_us, cache_us);
    printf("Profiling Results: %i iteration(s), \ntotal_ticks: %lld,\ttotal_us: %f\tavg: %f\n",
      		  iterations, proc_ticks, total_us, total_us);


  printf("done");

  return 0;
}
