#include "DMA.h"

DMA::DMA(unsigned int base):_base{base}
{
	reset();
}

int DMA::irq_reg(unsigned int interrupt_controller_id, unsigned int irq)
{
    if(alt_ic_isr_register(interrupt_controller_id, irq, isr, this, NULL) != 0)
    {
       return 1;
    }
    return 0;
}

void DMA::isr(void * context)
{
	DMA* dma = (DMA*)context;
	printf("DMA COPY COMPLETED\n");
	dma->check();
	DMA_WR_STATUS(dma->_base, 0);  /*clear the interrupt*/
	dma->reset();
	dma->done++;
}

int DMA::copy(int * read_location, int * write_location, int length, int control)
{
	printf("mem_to_mem_copy initiated\n");

    if((DMA_RD_STATUS(_base) & DMA_BUSY_BIT)==DMA_BUSY_BIT)
    {
        return 1;
    }

    /*Read buffer address*/
    DMA_WR_READADDR(_base, (unsigned int)read_location);
    /*Write buffer address*/
    DMA_WR_WRITEADDR(_base, (unsigned int)write_location);
    /*Data length size*/
    DMA_WR_LEN(_base, length);
    /*Control register*/
    DMA_WR_CTRL(_base, control | DMA_GO_BIT);

    while(!done);
    done = 0;

    return 0;
}

void DMA::check()
{
	int STATUS_MASK = 0x11111;

	printf("readaddress: %x, writeaddress: %x, length: %x, status: %x, control: %x\n",
			IORD_ALTERA_AVALON_DMA_RADDRESS(_base),
			IORD_ALTERA_AVALON_DMA_WADDRESS(_base),
			IORD_ALTERA_AVALON_DMA_LENGTH(_base),
			IORD_ALTERA_AVALON_DMA_STATUS(_base) & STATUS_MASK,
			IORD_ALTERA_AVALON_DMA_CONTROL(_base)
			);
}

void DMA::reset()
{
	printf("DMA RESET\n");
	IOWR_ALTERA_AVALON_DMA_CONTROL(_base, 0x1 << 12);
	IOWR_ALTERA_AVALON_DMA_CONTROL(_base, 0x1 << 12);
	printf("\n");
}



