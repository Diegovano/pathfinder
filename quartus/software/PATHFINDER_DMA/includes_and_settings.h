#ifndef _includes_and_settings_h
#define _includes_and_settings_h

#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"
#include "sys/alt_cache.h"
#include "alt_types.h"
#include "sys/alt_timestamp.h"
#include "altera_avalon_dma_regs.h"
#include "sys/alt_cache.h"
#include "dma_avalon_access.h"
#include "unistd.h"

#define IRQ 1  /*waiting for the DMA masters interrupt, otherwise Done bit polling is used*/
#define BUFFER_SIZE 8 /*data buffer size, at least 68 bytes of data and multiple of 4 bytes */

#define vector_op(A,B) __builtin_custom_inii(ALT_CI_VECTOR_OP_N,(A),(B))
#define ALT_CI_VECTOR_OP_N 0x0

#if (BUFFER_SIZE == 0)
    #error You have set the buffer size to be zero, increase it to a positive value that is a multiple of 4 bytes
#elif ((BUFFER_SIZE & 0x3) != 0)
    #error Make sure BUFFER_SIZE is a multiple of 4 bytes to ensure word boundary accesses
#elif (BUFFER_SIZE > 2*1024*1024)
    #error Reduce BUFFER_SIZE to be 2MB or less so that the source buffer will fit in SSRAM
#endif



int dma_irq_reg();
int mem_to_mem_copy(int * read_location, int * write_location, int length, int control);
void dma_irq(void * context);
void dma_check();
void dma_reset();

#endif
