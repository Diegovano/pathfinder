#include <stdio.h>
#include "sys/alt_dma.h"
#include "../dma_test_bsp/system.h" // tell vscode to use correct bsp
#include "altera_avalon_dma_regs.h" // for DMA_STATUS

extern int __builtin_ldwio(const volatile void* src);
extern void __builtin_stwio(volatile void* dst, int data);
extern int __builtin_custom_inii(int a, int b, int c);

#define DMA_STATUS printf("Status: %u\n", IORD_ALTERA_AVALON_DMA_STATUS(DMA_BASE));


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

  int res = ALT_CI_MEM_ACCESS_0(10, 10);

  printf("res: %d\n", res);
  DMA_STATUS
}

int main()
{
  printf("Hello from Nios II!\n");

  int *a = new int[4]();

  printf("address: %p\n", a);

  for (int i = 0; i < 4; i++) a[i] = i + 1;

  alt_dma_txchan tx;

  tx = alt_dma_txchan_open ("/dev/dma");
  if (tx == NULL)
  {
    printf("shit\n");

  }
  else
  {
    // alt_dma_txchan_reg(tx);
    alt_dma_txchan_ioctl(tx, ALT_DMA_TX_ONLY_ON, 0);
    // alt_dma_txchan_ioctl(tx, ALT_DMA_SET_MODE_32, nullptr);
    dma_check();
    int ret = alt_dma_txchan_send(tx, a, 4 * 1, doneCallback, nullptr);
    printf("yay? %u\n", ret);
  }

  return 0;
}
