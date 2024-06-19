#include "includes_and_settings.h"

int main (int argc, char* argv[], char* envp[])
{
	printf("Testing DMA!\n\n");

	//INITIALISE MEMORY
	printf("INITIALISING ARRAY\n");
	float* array = (float*)alt_uncached_malloc(40);
	array[0] = 2.;
	array[1] = 3.;
	array[2] = 7.;
	array[3] = 8.;
	array[4] = 2.;
	array[5] = 3.;
	array[6] = 7.;
	array[7] = 8.;
	array[8] = 0.;
	array[9] = 0.;
	printf("SDRAM array base address = %x\n", array);
	for (int i=0; i < 10; i++)
	{
		printf("array[%d]: %f\n",i, array[i]);
	}

	//INITIALISE DMA
	printf("INITIALISE DMA\n");
	DMA dma(DMA_BASE);
	if (dma.irq_reg(DMA_IRQ_INTERRUPT_CONTROLLER_ID,DMA_IRQ))
	{
		printf("ERROR failed to register dma isr\n");
	}

//	//INITIALISE ACCELERATOR
	printf("INITIALISE ACCELERATOR\n");
	Accelerator accel(EUCLID_DIST_BASE, EUCLID_DIST_SPAN);
	accel.check();
	if (accel.irq_reg(EUCLID_DIST_IRQ_INTERRUPT_CONTROLLER_ID,EUCLID_DIST_IRQ))
	{
		printf("ERROR failed to register custom isr\n");
	}

	//TRANSFER ARGUMENTS SDRAM -> ACCEL
	int ctrl = DMA_WORD | DMA_LEEN | DMA_IRQ_E_BIT;
	dma.copy((void*)(array), (void*)(EUCLID_DIST_BASE+4), 32, ctrl);

	//EXECUTE ACCELERATED FUNCTION
	accel.exec();


	//TRANSFER RESULT ACCEL -> SDRAM
	dma.copy((void*)(EUCLID_DIST_BASE+36), (void*)(array+8), 8, ctrl);
	printf("SDRAM array base address = %x\n", array);
	for (int i=0; i < 10; i++)
	{
		printf("array[%d]: %f\n",i,array[i]);
	}


	alt_uncached_free(array);

	return 0;
}

