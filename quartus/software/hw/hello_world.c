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
#include <system.h>



int main()
{
  int adj_matrix[4] = {0,3,
		  	  	  	   5,0};
  int *base_address = &adj_matrix;

  printf("address %u, value %d \n", base_address ,base_address[0]);
  printf("address %u, value %d \n", base_address+1 ,base_address[1]);
  printf("address %u, value %d \n", base_address+2 ,base_address[2]);
  printf("address %u, value %d \n", base_address+3 ,base_address[3]);


  printf("hello world \n \n");

//  unsigned long c = ALT_CI_DUMMY_0(4,6);
//  printf ("c lu %lu \n", c);

//  int number_of_nodes = 2;
//  int source_node = 0;
//  int destination_node = number_of_nodes -1;
//  long datab = number_of_nodes <<20 + destination_node <<10 + source_node;
  unsigned long datab = 1;


  for(int i = 0; i < 5; i++){
	  long unsigned b = ALT_CI_DUMMY_0(base_address,datab);
	  printf ("b %lu \n", b);
  }
//
//  unsigned long c = ALT_CI_DUMMY_0(base_address+1,0);
//  printf ("c lu %lu \n", c);
//  c = ALT_CI_DUMMY_0(base_address+1,0);
//  printf ("c lu %lu \n", c);
//  c = ALT_CI_DUMMY_0(base_address+1,0);
//	printf ("c lu %lu \n", c);
//	c = ALT_CI_DUMMY_0(base_address+1,0);
//	printf ("c lu %lu \n", c);
//	c = ALT_CI_DUMMY_0(base_address+1,0);
//	printf ("c lu %lu \n", c);
//	c = ALT_CI_DUMMY_0(base_address+1,0);
//	printf ("c lu %lu \n", c);
//	c = ALT_CI_DUMMY_0(base_address+1,0);
//	printf ("c lu %lu \n", c);

  return 0;
}
