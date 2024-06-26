#ifndef _DMA_H_
#define _DMA_H_

#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"
#include "altera_avalon_dma_regs.h"

/* bit masks */
#define DMA_DONE_BIT        0x1
#define DMA_BUSY_BIT        0x2
#define DMA_CLR_BIT         0x1
#define DMA_WORD			0x4
#define DMA_GO_BIT          0x8
#define DMA_IRQ_E_BIT       0x10
#define DMA_LEEN			0x80


/* register byte offsets */
#define DMA_STATUS_REG    	0
#define DMA_READ_ADDR_REG   4
#define DMA_WRITE_ADDR_REG  8
#define DMA_LENGTH_REG   	12
#define DMA_CTRL_REG   		24

/* register access macros */
#define DMA_WR_STATUS(base, data)   	IOWR_32DIRECT(base, DMA_STATUS_REG, data)
#define DMA_WR_READADDR(base, data)   	IOWR_32DIRECT(base, DMA_READ_ADDR_REG, data)
#define DMA_WR_WRITEADDR(base, data)   	IOWR_32DIRECT(base, DMA_WRITE_ADDR_REG, data)
#define DMA_WR_LEN(base, data)   		IOWR_32DIRECT(base, DMA_LENGTH_REG, data)
#define DMA_WR_CTRL(base, data)   		IOWR_32DIRECT(base, DMA_CTRL_REG, data)
#define DMA_RD_STATUS(base)   			IORD_32DIRECT(base, DMA_STATUS_REG)

class DMA
{
private:
	volatile int done = 0;
	unsigned int _base;
	unsigned int _control;
public:
	DMA(unsigned int, unsigned int);
	int irq_reg(unsigned int, unsigned int);
	static void isr(void*);
	int copy(void*, void*, int);
	void check();
	void reset();
};

#endif
