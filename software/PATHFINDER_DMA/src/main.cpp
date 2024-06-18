#include "includes_and_settings.h"

int main (int argc, char* argv[], char* envp[])
{
	printf("Testing DMA!\n\n");

	//INITIALISE MEMORY
	printf("INITIALISING ARRAY\n");
	int* array = (int*)alt_uncached_malloc(BUFFER_SIZE);
	printf("SDRAM array base address = %x\n", array);
	for (int i=0; i < 8; i++)
	{
		array[i] = 1 << i;
		printf("%x\n", array[i]);
	}
	printf("\n");

	//INITIALISE DMA
	printf("INITIALISE DMA\n");
	DMA dma(DMA_BASE);
	if (dma.irq_reg(DMA_IRQ_INTERRUPT_CONTROLLER_ID,DMA_IRQ))
	{
		printf("ERROR failed to register dma isr\n");
	}

	//INITIALISE ACCELERATOR
	printf("INITIALISE ACCELERATOR\n");
	Accelerator accel(VECTOR_OP_BASE, VECTOR_OP_SPAN);
	accel.check();
	if (accel.irq_reg(ACCELERATOR_IRQ_INTERRUPT_CONTROLLER_ID,ACCELERATOR_IRQ))
	{
		printf("ERROR failed to register custom isr\n");
	}

//	IOWR_32DIRECT(VECTOR_OP_BASE, 4, 420);
//	accel.check();

	//POPULATE ACCELERATOR INTERNAL MEMORY
	printf("DMA TRANSFER: SRAM -> INTERNAL MEMORY\n");
	int ctrl = DMA_WORD | DMA_LEEN | DMA_IRQ_E_BIT;
	dma.copy(array, (int *) (VECTOR_OP_BASE+8), 24, ctrl);
	accel.check();

//	accel.write(1, 420);
//	dma.copy((int *) VECTOR_OP_BASE, (int*)(array+1), 8, ctrl);
//	printf("SDRAM array base address = %x\n", array);
//	for (int i=0; i < 8; i++)
//	{
//		printf("%x\n", array[i]);
//	}
//	printf("\n");

	//EXECUTE ACCELERATED FUNCTION
	printf("EXECUTE ACCELERATED FUNCTION\n");
	int res = accel.exec();

	printf("output = %x\n", res);
	alt_uncached_free(array);

	return 0;
}

