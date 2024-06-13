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
#include <sys/alt_cache.h>


int main()
{
  long int val;
  long int *base_address = &val;
  *base_address = 23;
  //alt_dcache_flush(base_address,2);
  int dummy = 2;
  int dummy2 = 3;
  int dummy3 = 4;
  int dummy4 = 5;
  int dummy5 = 6;
  int dummy6 = 7;
  int dum;
  dummy = 6;

  *(base_address + 1) = 24;
//  *(base_address + 2) = 25;


  long int b = ALT_CI_DUMMY_0(base_address,0);
  printf("b: %lu",b);


  long int c = ALT_CI_DUMMY_0(base_address+1,0);
  printf("c: %lu",c);

//  long int d = ALT_CI_DUMMY_0(base_address+2,0);
//    printf("c: %lu",d);




  return 0;
}
