#include "includes_and_settings.h"

int main (int argc, char* argv[], char* envp[])
{
	printf("Testing DMA!\n");

	//INITIALISE MEMORY
	int source_ptr[8]{0};
	int* destination_ptr = 0;

	//INITIALISE DMA
	dma_reset();


	int _control = (/*DMA_GO_BIT |*/ DMA_WORD | DMA_IRQ_E_BIT | DMA_LEEN);

	if (dma_irq_reg()){
		printf("ERROR failed to register isr\n");
	}

	source_ptr[0] = 32;
	source_ptr[1] = 64;

	alt_icache_flush_all();
	alt_dcache_flush_all();

	// FIRST TRANSFER
	if (mem_to_mem_copy(source_ptr, destination_ptr, BUFFER_SIZE, _control))
	{
		printf("ERROR DMA busy\n");
	}
	DMA_WR_CTRL(DMA_BASE, _control | DMA_GO_BIT);

	alt_icache_flush_all();
	alt_dcache_flush_all();

	while(true);

	free(source_ptr);

	return 0;
}

