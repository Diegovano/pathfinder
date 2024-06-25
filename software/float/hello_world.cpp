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
#include <vector>

using namespace std;


// macro determines data type of inputs, doing problematic casting
void write_dijkstra_cache(float edge_value, int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	float datab = edge_value;
	ALT_CI_DIJKSTRA32_FLOAT_4(0,dataa,datab);
}

//writing single data with DMA
void DMA_write_dijkstra_cache(float edge_value, int from_node, int to_node, DMA& dma){
	float* float_in_ptr = (float*)alt_uncached_malloc(4);
	 *float_in_ptr = edge_value;
	 unsigned int address = DIJKSTRA32_FLOAT_0_BASE + (to_node << 9) + (from_node << 2);
	 dma.copy((void*)float_in_ptr, (void*)(address), 4);
	 alt_uncached_free(float_in_ptr);
}
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
	ALT_CI_DIJKSTRA32_FLOAT_2(2,0,1); //a manual software reset cuz hardware reset buggy
	long dataa = (to_node <<16) + from_node;
	long datab = number_of_nodes;
	return ALT_CI_DIJKSTRA32_FLOAT_2(2,dataa,datab);
}

float run_dijkstra_withCache (int from_node, int to_node, int number_of_nodes, float *mem_addr, DMA& dma){
	 const unsigned int address = DIJKSTRA32_FLOAT_0_BASE;
	 dma.copy((void*)mem_addr, (void*)(address), 65536);

	 //printf("done copying cache");
	ALT_CI_DIJKSTRA32_FLOAT_2(2,0,0); //a manual software reset cuz hardware reset buggy
	long dataa = (to_node <<16) + from_node;
	long datab = number_of_nodes;
	return ALT_CI_DIJKSTRA32_FLOAT_2(2,dataa,datab);
}

void full_HW_dijkstra_reshape(float *mem_pointer, float **adj_matrix){
	float *temp = mem_pointer;
	//temp++;
	for(int from = 0; from<128;from++){
		//temp++;
		 for (int column = 0; column <128 ; column++){
			 	*temp = adj_matrix[column][from];
			 	temp++;
			 }
	 }

}

int main()
{
  printf("Hello from Nios II!\n");
  float* float_in_ptr = (float*)alt_uncached_malloc(65536);
  float* temp_pointer = float_in_ptr;
  unsigned int ctrl = DMA_IRQ_E_BIT | DMA_LEEN | DMA_WORD;
  DMA dma(DMA_0_BASE, ctrl);
  dma.irq_reg(DMA_0_IRQ_INTERRUPT_CONTROLLER_ID, DMA_0_IRQ);

//reset cache
 for (int row=0; row < 128; row++){
	  for (int column=0; column < 128; column++){
		  if (row == column){
			  //DMA_write_dijkstra_cache(0,row, column,dma);
		  }
		  else{
			  //DMA_write_dijkstra_cache(0x7F800000,row, column,dma);
		  }
	  }
  }




 float *x, *y, **adjMatrix;
 x = new float[128];
 y = new float[128];
 adjMatrix = new float*[128];
 for (int i = 0; i < 128; i++) adjMatrix[i] = new float[128];

 for(int i = 0; i<128;i++){
	 for (int j = 0; j <128 ; j++){
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

  full_HW_dijkstra_reshape(float_in_ptr, adjMatrix);



  printf("\nshortest distance: %f \n", run_dijkstra_withCache(1,5,14,float_in_ptr,dma));

  for (int i=0; i <14; i++){
	 printf("%d: %d \n",i,read_path_vector(i));
  }


	 alt_64 proc_ticks = 0;
	 alt_u64 time1 = 0;
	 alt_u64 time3 = 0;

    int iterations = 1000;
    // The code that you want to time goes here
    alt_timestamp_start();
    time1 = alt_timestamp(); //
    for (int i=0; i<iterations; i++){
//
      time1 = alt_timestamp();
  	  run_dijkstra_withCache(1,5,14,float_in_ptr,dma);
  	  time3 = alt_timestamp();

  	  proc_ticks += (time3 - time1);

    }
    int k = alt_timestamp_freq() * 1e-6 * iterations; // ticks per ms
    double proc_us = (double)proc_ticks / (double)k;
    printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
  		  iterations, proc_ticks, proc_us, proc_us);

  printf("done with 13 node matrix\n");


  vector<vector<pair<int, float>>> adjList(128);
  // Initialise the adjacency list with the edges
  adjList[109] = {{1, 303.519}, {2, 156.256}, {3, 41.309}, {4, 184.269}};
  adjList[1] = {{21, 6.469}, {22, 158.274}, {109, 303.519}};
  adjList[2] = {{33, 8.134}, {30, 18.088}, {109, 156.256}};
  adjList[3] = {{102, 12.853}, {108, 41.046}};
  adjList[4] = {{5, 33.532}, {109, 184.269}, {6, 53.887}};
  adjList[5] = {{4, 33.532}, {50, 181.396}, {51, 183.129}};
  adjList[6] = {{10, 19.803}, {4, 53.887}};
  adjList[7] = {{8, 11.932}, {9, 87.616}, {10, 299.832}};
  adjList[8] = {{7, 11.932}, {72, 45.248}};
  adjList[9] = {{12, 11.471}, {13, 99.879}};
  adjList[10] = {{6, 19.803}, {40, 28.225}, {7, 299.832}};
  adjList[11] = {{12, 99.828}, {13, 12.184}, {14, 50.876}};
  adjList[12] = {{9, 11.471}, {60, 52.567}, {8, 87.146}};
  adjList[13] = {{11, 12.184}, {56, 21.38}};
  adjList[14] = {{80, 61.58}, {81, 45.668}, {11, 50.876}, {71, 31.192}};
  adjList[15] = {{16, 161.072}, {17, 57.548}};
  adjList[16] = {{34, 55.096}, {72, 12.546}, {7, 44.789}};
  adjList[17] = {{34, 160.516}, {15, 57.548}, {32, 92.211}};
  adjList[18] = {{19, 9.452}, {20, 292.743}};
  adjList[19] = {{18, 9.452}, {21, 19.619}};
  adjList[20] = {{101, 13.449}, {18, 292.743}};
  adjList[21] = {{1, 6.469}, {19, 19.028}};
  adjList[22] = {{76, 31.392}, {87, 36.661}, {1, 158.274}};
  adjList[23] = {{24, 63.576}, {25, 30.539}};
  adjList[24] = {{26, 21.383}, {27, 62.293}, {23, 63.576}};
  adjList[25] = {{95, 46.425}, {23, 30.539}};
  adjList[26] = {{49, 61.768}, {68, 85.51}};
  adjList[27] = {{24, 62.293}};
  adjList[28] = {{29, 41.125}, {30, 35.057}, {31, 72.929}};
  adjList[29] = {{28, 41.125}, {39, 28.433}, {31, 53.581}};
  adjList[30] = {{28, 35.057}, {33, 18.83}};
  adjList[31] = {{29, 53.581}, {28, 72.929}, {40, 62.337}};
  adjList[32] = {{33, 83.249}, {17, 92.211}, {34, 197.262}};
  adjList[33] = {{2, 8.134}, {32, 83.249}};
  adjList[34] = {{16, 55.096}, {32, 197.262}, {17, 160.516}};
  adjList[35] = {{36, 52.921}, {37, 65.778}, {38, 49.173}};
  adjList[36] = {{78, 9.761}, {107, 106.961}, {35, 52.921}};
  adjList[37] = {{92, 73.342}, {35, 65.778}};
  adjList[38] = {{91, 68.171}, {35, 49.173}};
  adjList[39] = {{29, 28.433}};
  adjList[40] = {{31, 62.337}, {6, 33.73}};
  adjList[41] = {{42, 10.796}, {43, 154.774}, {44, 122.668}};
  adjList[42] = {{50, 41.055}, {41, 10.796}, {55, 143.458}};
  adjList[43] = {{41, 154.774}};
  adjList[44] = {{41, 122.668}};
  adjList[45] = {{46, 64.446}, {47, 45.849}};
  adjList[46] = {{90, 66.782}, {74, 187.738}};
  adjList[47] = {{45, 45.849}, {79, 49.829}};
  adjList[48] = {{49, 48.283}};
  adjList[49] = {{26, 61.768}, {23, 24.546}, {48, 48.283}};
  adjList[50] = {{42, 41.055}, {5, 181.396}, {78, 266.825}};
  adjList[51] = {{5, 183.129}, {52, 282.168}, {75, 42.826}};
  adjList[52] = {{53, 43.977}, {54, 50.728}, {51, 282.168}};
  adjList[53] = {{52, 43.977}, {86, 103.918}, {65, 84.102}};
  adjList[54] = {{52, 50.728}, {85, 26.736}, {88, 145.457}};
  adjList[55] = {{42, 143.458}};
  adjList[56] = {{57, 82.659}, {58, 55.22}, {59, 12.015}};
  adjList[57] = {{66, 11.912}, {67, 187.945}};
  adjList[58] = {{56, 55.22}};
  adjList[59] = {{56, 12.015}, {11, 21.481}};
  adjList[60] = {{12, 52.567}, {61, 25.604}, {62, 45.22}};
  adjList[61] = {{60, 25.604}, {71, 100.741}};
  adjList[62] = {{60, 45.22}, {63, 162.404}};
  adjList[63] = {{62, 162.404}};
  adjList[64] = {{65, 25.312}};
  adjList[65] = {{64, 25.312}, {75, 109.154}, {53, 84.102}};
  adjList[66] = {{74, 67.296}, {59, 82.785}, {57, 11.912}};
  adjList[67] = {{96, 9.517}, {97, 87.655}};
  adjList[68] = {{69, 40.412}, {70, 39.009}};
  adjList[69] = {};
  adjList[70] = {{68, 39.009}};
  adjList[71] = {{14, 31.192}, {61, 100.741}};
  adjList[72] = {{16, 12.546}, {15, 160.778}};
  adjList[73] = {{74, 66.093}};
  adjList[74] = {{66, 67.296}, {73, 66.093}, {46, 187.738}};
  adjList[75] = {{65, 109.154}, {76, 84.72}, {77, 65.657}, {51, 42.826}};
  adjList[76] = {{86, 3.726}, {22, 31.392}, {75, 84.72}};
  adjList[77] = {{75, 65.657}};
  adjList[78] = {{36, 9.761}, {50, 266.825}, {94, 140.769}};
  adjList[79] = {{47, 49.829}, {73, 92.489}};
  adjList[80] = {{14, 61.58}};
  adjList[81] = {{82, 9.493}, {14, 45.668}};
  adjList[82] = {{81, 9.493}};
  adjList[83] = {{84, 37.495}};
  adjList[84] = {};
  adjList[85] = {{54, 26.736}};
  adjList[86] = {{53, 103.918}, {76, 3.726}, {88, 47.556}};
  adjList[87] = {{22, 36.661}};
  adjList[88] = {{89, 7.153}, {86, 47.556}, {54, 145.457}};
  adjList[89] = {{88, 7.153}};
  adjList[90] = {{67, 11.572}, {66, 188.014}};
  adjList[91] = {{38, 68.171}};
  adjList[92] = {{93, 5.414}, {69, 112.385}, {37, 73.342}};
  adjList[93] = {{92, 5.414}, {94, 22.298}, {36, 125.363}};
  adjList[94] = {{103, 174.384}};
  adjList[95] = {{103, 12.354}, {93, 185.367}};
  adjList[96] = {{100, 11.367}};
  adjList[97] = {{98, 9.088}, {99, 180.81}};
  adjList[98] = {{96, 87.726}};
  adjList[99] = {{78, 107.001}};
  adjList[100] = {{90, 9.331}, {83, 46.897}};
  adjList[101] = {{109, 40.113}};
  adjList[102] = {{20, 11.386}, {101, 9.717}};
  adjList[103] = {};
  adjList[104] = {{25, 34.092}, {105, 29.532}};
  adjList[105] = {};
  adjList[106] = {{95, 33.122}, {104, 38.258}};
  adjList[107] = {{99, 8.879}, {98, 180.91}};
  adjList[108] = {{102, 31.976}};

  for (int row=0; row < 128; row++){
 	  for (int column=0; column < 128; column++){
 		  if (row == column){
 			  //DMA_write_dijkstra_cache(0,row, column,dma);
 		  }
 		  else{
 			  //DMA_write_dijkstra_cache(0x7F800000,row, column,dma);
 		  }
 	  }
   }
  printf("done clearing cache\n");

  for(int i = 0; i<128;i++){
  	 for (int j = 0; j <128 ; j++){
  		 	if (i==j){
  		 		adjMatrix[i][j] = 0;
  		 	}
  		 	else{
  		 		adjMatrix[i][j] = 0x7F800000;
  		 	}
  	 }
   }
   for (int i = 0; i < 128; ++i) {
           for (auto edge : adjList[i]) {
               int neighbor = edge.first;
               float weight = edge.second;
               adjMatrix[i][neighbor] = weight;
           }
       }
   full_HW_dijkstra_reshape(float_in_ptr, adjMatrix);
   printf("%f \n", run_dijkstra_withCache(15,94,110,float_in_ptr,dma));

  for (int i=0; i <128; i++){
	 printf("%d: %d \n",i,read_path_vector(i));
  }


    	  alt_64 proc_ticks2 = 0;
    	  	 alt_u64 time12 = 0;
    	  	 alt_u64 time32 = 0;

    	      int iterations2 = 1000;
    	      // The code that you want to time goes here
    	      alt_timestamp_start();
    	      for (int i=0; i<iterations2; i++){
    	  //
    	        time12 = alt_timestamp();
    	    	run_dijkstra_withCache(15,94,110,float_in_ptr,dma);
    	    	time32 = alt_timestamp();
    	    	proc_ticks2 += (time32 - time12);
    	      }

    	      double proc_us2 = (double)proc_ticks2 / (double)k;



    	      printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
    	    		  iterations, proc_ticks2, proc_us2, proc_us2);

  printf("trying to break HW with ridiculous node numbers\n");
  run_dijkstra_withCache(0xffffe,0xfffff,3,float_in_ptr,dma);
  printf("exiting properly\n");

  return 0;
}
