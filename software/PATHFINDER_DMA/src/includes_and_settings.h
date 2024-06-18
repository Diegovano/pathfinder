#ifndef _INCLUDES_AND_SETTINGS_h
#define _INCLUDES_and_SETTINGS_h

#include <cstdio>
#include <cstddef>
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "sys/alt_timestamp.h"
#include "sys/alt_cache.h"
#include <unistd.h>
#include "DMA/DMA.h"
#include "Accelerator/Accelerator.h"

#define BUFFER_SIZE 8 /*data buffer size, at least 68 bytes of data and multiple of 4 bytes */

#define VECTOR_OP_BASE 0x1001040
#define VECTOR_OP_SPAN 32
#define ACCELERATOR_IRQ 5
#define ACCELERATOR_IRQ_INTERRUPT_CONTROLLER_ID 0

#if (BUFFER_SIZE == 0)
    #error You have set the buffer size to be zero, increase it to a positive value that is a multiple of 4 bytes
#elif ((BUFFER_SIZE & 0x3) != 0)
    #error Make sure BUFFER_SIZE is a multiple of 4 bytes to ensure word boundary accesses
#elif (BUFFER_SIZE > 2*1024*1024)
    #error Reduce BUFFER_SIZE to be 2MB or less so that the source buffer will fit in SSRAM
#endif

#endif