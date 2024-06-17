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

#include <stdio.h>
#include <stdlib.h>
#include <system.h>

void write_dijkstra_cache(long edge_value, int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	long datab = edge_value;
	long result = ALT_CI_DIJKSTRA32_NOMEM_0(0,dataa,datab);
}

long read_dijkstra_cache(int from_node, int to_node){
	long dataa = (to_node<<16) + from_node;
	return ALT_CI_DIJKSTRA32_NOMEM_0(1,dataa,0);
}

long run_dijkstra (int from_node, int to_node, int number_of_nodes){
	ALT_CI_DIJKSTRA32_NOMEM_0(2,0,0); //a manual software reset cuz hardware reset buggy
	long dataa = (to_node <<16) + from_node;
	long datab = number_of_nodes;
	return ALT_CI_DIJKSTRA32_NOMEM_0(2,dataa,datab);
}

int main()
{
  printf("Hello from Nios II!\n");


  int testvector[8][8] = {
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
  		  printf("%d \t", read_dijkstra_cache(row,column));
  	  }
  	  printf("\n");
  }


  printf("%li \n", run_dijkstra(0,7,8)); //expecting 301

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


  printf("goodbye");



//  }



  return 0;
}
