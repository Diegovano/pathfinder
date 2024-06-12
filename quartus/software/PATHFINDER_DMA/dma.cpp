#include "includes_and_settings.h"


volatile int dma_done = 0; /*indication for completion of the DMA trasnfer operation*/


/***********************************************
 * Interrupt handler for the DMA IRQ. When the *
 * IRQ is fired, dma_done is set to 1. The IRQ *
 * is cleared before exiting the function.     *
 * Change the function contents according to   *
 * the handler you wish to have.               *
 **********************************************/
void dma_isr(void * context)
{
	printf("DMA COPY COMPLETED\n");
	printf("Custom instruction result: %d\n\n", vector_op(0,0));
	dma_done++;
	DMA_WR_STATUS(DMA_BASE, 0);  /*clear the interrupt*/
	dma_reset();
}
/**********************************************/

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

void dma_reset()
{
	printf("DMA RESET\n");
	IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_BASE, 0x1 << 12);
	IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_BASE, 0x1 << 12);
	dma_check();
	printf("\n");
}


/***********************************************
 * Register the IRQ. Return 1 if the           *
 * registration failed.                        *
 **********************************************/
int dma_irq_reg()
{
    if(alt_ic_isr_register(DMA_IRQ_INTERRUPT_CONTROLLER_ID, DMA_IRQ, dma_isr, NULL, NULL) != 0)
    {
       return 1;
    }
    return 0;
}
/**********************************************/


/***********************************************
 * Function to write values to DMA registers.  *
 * Writing registers can't be done if the DMA  *
 * is busy and the function returns 1.         *
 **********************************************/
int mem_to_mem_copy(int * read_location, int * write_location, int length, int control)
{
	printf("mem_to_mem_copy initiated\n");

    if((DMA_RD_STATUS(DMA_BASE) & DMA_BUSY_BIT)==DMA_BUSY_BIT)
    {
        return 1;
    }

    /*Read buffer address*/
    DMA_WR_READADDR(DMA_BASE, (unsigned int)read_location);
    /*Write buffer address*/
    DMA_WR_WRITEADDR(DMA_BASE, (unsigned int)write_location);
    /*Data length size*/
    DMA_WR_LEN(DMA_BASE, length);
    /*Control register*/
    DMA_WR_CTRL(DMA_BASE, control);

    dma_check();
    printf("\n");
    return 0;
}
/**********************************************/
