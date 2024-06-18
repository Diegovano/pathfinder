#include <stdio.h>
#include <stdlib.h>
#include "sys/alt_dma.h"
#include "../dma_access_bsp/system.h"
#include "altera_avalon_dma_regs.h"

extern int __builtin_ldwio(const volatile void* src);
extern void __builtin_stwio(volatile void* dst, int data);
//extern int __builtin_custom_fnff(int a, int b, int c);

#define DMA_BASE 0x1001020
#define DMA_WRITE_MASTER_DMA_ACCESS_0_BASE 0x0
#define DMA_STATUS() printf("Status: %u\n", IORD_ALTERA_AVALON_DMA_STATUS(DMA_BASE))

#define ALT_CI_DMA_ACCESS_0(A,B) __builtin_custom_fnff(ALT_CI_DMA_ACCESS_0_N,(A),(B))

void dma_check()
{
  int STATUS_MASK = 0x11111;

  printf("readaddress: %x, writeaddress: %x, length: %x, status: %x, control: %x\n",
      IORD_ALTERA_AVALON_DMA_RADDRESS(DMA_BASE),
      IORD_ALTERA_AVALON_DMA_WADDRESS(DMA_BASE),
      IORD_ALTERA_AVALON_DMA_LENGTH(DMA_BASE),
      IORD_ALTERA_AVALON_DMA_STATUS(DMA_BASE) & STATUS_MASK,
      IORD_ALTERA_AVALON_DMA_CONTROL(DMA_BASE)
      );
}

void doneCallback(void *handle)
{
  dma_check();
  printf("done\n");

  int res = ALT_CI_DMA_ACCESS_0(10.0, 10.0);

  printf("res: %d\n", res);
  DMA_STATUS();
}

int main()
{
  printf("Hello from Nios II!\n");

  // allocate memory for data to be sent over DMA
  int *a = (int*) malloc(sizeof(int));
  if (a == NULL)
  {
    printf("Memory allocation failed for int\n");
    return 1;
  }
  *a = 94;

  // array
  float *b = (float*) malloc(10 * sizeof(float));
  if (b == NULL)
  {
    printf("Memory allocation failed for array \n");
    return 1;
  }

  // fill the array with data
  for (int i = 0; i < 10; i++) {
    b[i] = i + 1.0;
  }


  // open TX channel of DMA
  alt_dma_txchan tx;
  tx = alt_dma_txchan_open ("/dev/dma");
  if (tx == NULL)
  {
    printf("Failed to open DMA channel\n");
  }
  else
  {

    // ALT_DMA_TX_ONLY_ON is an integer specifying the control command to be execuded 
    // parameters = channel, control command and pointer for additional control commands
    alt_dma_txchan_ioctl(tx, ALT_DMA_TX_ONLY_ON, 0);
    //alt_dma_txchan_ioctl(tx, ALT_DMA_SET_MODE_32, NULL);
    dma_check();

    // initiate data transfer over DMA
    // parameters = channel, data (pointer to data), size of data, callback function, pointer for callback function
    //int ret = alt_dma_txchan_send(tx, a, sizeof(int), doneCallback, NULL);
    int ret = alt_dma_txchan_send(tx, b, 10 * sizeof(float), doneCallback, NULL);
    if (ret < 0) {
      printf("Failed to send data over DMA channel\n");
    } else {
      printf("Data sent successfully\n");
    }
  }

  // free allocated memory
  free(a);
  return 0;
}
