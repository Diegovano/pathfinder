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
//  int adj_matrix[4];
//  adj_matrix[0] = 0;
//  adj_matrix[1] = 1;
//  adj_matrix[2] = 1;
//  adj_matrix[3] = 0;
//
//  printf ("hi \n");
//  printf ("number: %d \n",adj_matrix[2]);

  int adj_matrix[4];
  int *base_address;
  base_address = &adj_matrix;
  base_address[0] = 0;
  base_address[1] = 1;
  base_address[2] = 1;
  base_address[3] = 0;

  printf("num: %d \n", base_address[0]);
  printf("num: %d \n", base_address[1]);
  printf("pointer %p \n", base_address);

  int source = 0;
  int destination = 1;
  int number_nodes = 32;

  long datab = number_nodes << 20 + destination <<10 + source;

  printf("Hello from Nios II!\n");
  int a = ALT_CI_DIJKSTRA32_0(base_address, datab);
  printf("done %d",a);


  return 0;
}
