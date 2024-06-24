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



void write_dijkstra_cache(float edge_value, int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	float datab = edge_value;
	long result = ALT_CI_DIJKSTRA32_FLOAT_0(0,dataa,datab);
}

float read_dijkstra_cache(int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	return ALT_CI_DIJKSTRA32_FLOAT_0(1,dataa,0);
}

float read_path_vector(int node){
	long dataa = node;
	return ALT_CI_DIJKSTRA32_FLOAT_0(3,dataa,0);
}

float run_dijkstra (int from_node, int to_node, int number_of_nodes){
	ALT_CI_DIJKSTRA32_FLOAT_0(2,0,0); //a manual software reset cuz hardware reset buggy
	long dataa = (to_node <<16) + from_node;
	long datab = number_of_nodes;
	return ALT_CI_DIJKSTRA32_FLOAT_0(2,dataa,datab);
}

int main()
{
  printf("Hello from Nios II!\n");
//  for (int row=0; row < 128; row++){
//	  for (int column=0; column < 128; column++){
//		  if (row == column){
//			  write_dijkstra_cache(0,row, column);
//		  }
//		  else{
//			  write_dijkstra_cache(0x7F800000,row, column);
//		  }
//	  }
//  }
//
//  write_dijkstra_cache(33.532, 0, 6);
//  write_dijkstra_cache(53.887, 0, 12);
//
//  write_dijkstra_cache(62.337, 1, 11);
//  write_dijkstra_cache(33.73, 1, 12);
//
//  write_dijkstra_cache(19.803, 2, 12);
//  write_dijkstra_cache(28.225, 2, 1);
//
//  write_dijkstra_cache(10.796, 3, 8);
//  write_dijkstra_cache(154.774, 3, 5);
//  write_dijkstra_cache(122.668, 3, 4);
//
//  write_dijkstra_cache(122.668, 4, 3);
//
//  write_dijkstra_cache(154.774, 5, 3);
//
//  write_dijkstra_cache(33.532, 6, 0);
//  write_dijkstra_cache(181.396, 6, 10);
//  write_dijkstra_cache(183.129, 6, 9);
//
//  write_dijkstra_cache(143.458, 7, 8);
//
//  write_dijkstra_cache(41.055, 8, 10);
//  write_dijkstra_cache(10.796, 8, 3);
//  write_dijkstra_cache(143.458, 8, 7);
//
//  write_dijkstra_cache(183.129, 9, 6);
//
//  write_dijkstra_cache(41.055, 10, 8);
//  write_dijkstra_cache(181.396, 10, 6);
//
//  write_dijkstra_cache(62.337, 11, 1);
//
//  write_dijkstra_cache(53.887, 12, 0);
//  write_dijkstra_cache(19.803, 12, 2);
//
//  printf("shortest distance: %f \n", run_dijkstra(1,5,13));
//
//  printf("%d ",read_path_vector(1));
//
//
//
//
//  printf("done");


  float testvector[8][8] = {
		  {0,205,201,248,188,181,125,301},
		  {205,0,160,132,176,123,215,189},
		  {201,160,0,130,140,87,88,122},
		  {248,132,130,0,259,67,237,175},
		  {188,176,140,259,0,226,170,193},
		  {181,123,87,67,226,0,79,225},
		  {125,215,88,237,170,79,0,177},
		  {301,189,122,175,193,225,177,0}};
  for (int row=0; row <8; row++){
	  for (int column=0; column <8; column++){
		  write_dijkstra_cache(testvector[row][column],row,column);
	  }
  }
  for (int row = 0; row <8; row++){
  	  for (int column=0; column <8; column++){
  		  printf("%f \t", read_dijkstra_cache(row,column));
  	  }
  	  printf("\n");
  }


  printf("%p \n", run_dijkstra(0,7,8)); //expecting 301

  int testvector2[8][8] = {
  		  {0,205,201,248,188,181,125,201},
  		  {205,0,160,132,176,123,215,189},
  		  {201,160,0,130,140,87,88,122},
  		  {248,132,130,0,259,67,237,175},
  		  {188,176,140,259,0,226,170,193},
  		  {181,123,87,67,226,0,79,225},
  		  {125,215,88,237,170,79,0,177},
  		  {201,189,122,175,193,225,177,0}};
    for (int row=0; row <8; row++){
  	  for (int column=0; column <8; column++){
  		  write_dijkstra_cache(testvector2[row][column],row,column);
  	  }
    }
  printf("%li \n", run_dijkstra(0,7,8)); //expecting 201


  printf("start profiling");
  int testvector3[30][30] = {
      {0,171,118,128,228,109,105,194,225,140,15,118,33,151,140,133,38,1,10,6,125,6,102,166,75,114,85,126,114,178},
      {51,0,76,157,9,115,200,133,116,181,235,193,43,79,62,76,47,76,149,120,18,89,13,222,92,236,105,213,6,100},
      {48,40,0,158,133,143,191,229,37,59,113,95,96,240,208,32,62,138,140,93,64,157,90,231,150,147,133,36,83,127},
      {228,5,92,0,154,241,83,140,230,135,180,80,157,0,215,207,74,252,131,61,67,144,112,201,84,116,229,255,56,62},
      {184,48,145,117,0,9,236,190,79,230,206,161,132,144,228,71,62,23,91,110,7,253,22,98,158,246,80,155,47,239},
      {114,11,215,78,231,0,90,34,54,81,163,71,109,59,112,165,68,181,13,101,19,107,8,96,209,197,124,100,129,253},
      {157,133,208,230,248,168,0,30,255,19,4,148,106,74,123,195,214,60,229,250,93,169,40,188,209,179,40,59,234,222},
      {29,225,94,238,165,126,216,0,99,167,157,252,65,23,200,128,87,37,111,191,154,217,89,134,216,247,207,101,41,145},
      {208,112,43,110,197,118,147,239,0,109,139,11,161,135,119,26,48,199,239,182,96,100,82,87,149,254,2,8,10,5},
      {38,166,100,193,117,59,164,133,5,0,163,88,177,207,84,114,9,247,132,177,24,94,130,83,131,77,11,141,228,81},
      {154,198,175,98,21,148,170,122,185,145,0,217,244,213,183,100,196,111,226,11,226,97,238,147,112,11,225,25,97,95},
      {45,6,89,88,237,38,51,16,151,218,3,0,174,122,157,2,133,121,199,15,78,163,180,103,118,7,179,102,249,179},
      {157,183,113,139,195,205,122,55,88,251,252,248,0,117,115,214,185,93,102,139,206,82,217,236,3,165,135,29,78,11},
      {201,11,16,60,123,103,191,187,129,146,181,28,192,0,216,73,136,210,139,220,117,179,81,183,15,131,106,216,213,28},
      {58,213,78,111,65,76,11,25,103,11,90,237,162,129,0,144,1,16,33,33,172,230,40,72,205,106,83,242,160,151},
      {68,159,150,64,229,31,79,83,15,51,249,140,173,10,244,0,80,70,21,222,195,80,64,129,50,96,107,251,210,82},
      {185,150,15,143,28,71,27,216,57,58,216,204,13,146,78,206,0,71,183,44,235,245,91,44,15,253,87,203,77,254},
      {157,95,110,210,132,28,193,49,177,87,57,208,41,218,240,194,175,0,20,166,64,134,236,150,79,74,162,168,166,246},
      {149,34,117,160,170,127,44,99,41,249,103,155,251,48,127,138,68,17,0,101,94,215,29,102,123,158,203,194,221,60},
      {135,179,244,73,215,192,240,145,168,243,21,94,154,143,240,17,10,145,131,0,73,29,195,199,208,226,132,189,90,100},
      {134,32,81,119,118,245,37,119,27,51,78,187,86,95,8,56,29,156,223,162,0,127,126,209,220,111,144,200,59,247},
      {236,7,140,32,212,235,75,221,40,0,212,109,92,205,165,175,61,103,178,68,238,0,185,119,229,240,132,105,36,80},
      {210,165,222,117,238,35,176,128,49,185,9,50,225,176,241,12,198,124,243,164,99,102,0,30,114,147,166,172,35,14},
      {29,179,60,248,81,67,29,155,131,33,245,179,118,155,228,56,21,167,234,74,29,241,157,0,58,24,114,16,192,100},
      {226,123,13,1,251,2,66,147,244,250,204,158,39,213,77,177,26,115,140,241,192,1,164,10,0,32,254,28,186,194},
      {220,65,205,83,47,81,164,199,53,198,137,15,18,157,181,188,42,210,130,29,145,35,120,19,144,0,140,99,109,184},
      {194,20,131,81,172,38,42,202,37,106,40,111,27,132,179,150,165,35,30,7,216,88,239,143,138,147,0,231,56,229},
      {231,146,199,60,24,160,108,39,116,104,126,111,116,228,234,16,180,218,232,208,46,224,94,84,23,143,172,0,119,55},
      {98,155,45,226,81,215,155,77,176,215,94,133,121,231,225,113,226,28,244,65,68,213,200,252,114,161,118,227,0,99},
      {217,69,193,99,71,1,129,199,153,100,77,108,178,155,96,43,19,98,45,100,80,112,234,121,171,195,8,211,174,0}
  };

  alt_64 proc_ticks = 0;
  alt_u64 time1 = 0;
  alt_u64 time3 = 0;

  int iterations = 1000;
  int size = 30;
  // The code that you want to time goes here
  alt_timestamp_start();

  for (int i=0; i<iterations; i++){

	  for (int row=0; row <size; row++){
		  for (int column=0; column <size; column++){
			  write_dijkstra_cache(testvector3[row][column],row,column);
		  }
	  }
	  time1 = alt_timestamp();
	  run_dijkstra(0,size-1,size);
	  time3 = alt_timestamp();
	  proc_ticks += (time3 - time1);
  }
  int k = alt_timestamp_freq() * 1e-6 * iterations; // ticks per ms
  double proc_us = (double)proc_ticks / (double)k;

  printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
		  iterations, proc_ticks, proc_us, proc_us);




//  }



  return 0;
}
