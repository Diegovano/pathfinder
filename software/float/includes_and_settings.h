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
#include "DMA.h"
//#include "Accelerator/Accelerator.h"

#define BUFFER_SIZE 8 /*data buffer size, at least 68 bytes of data and multiple of 4 bytes */

#define VECTOR_OP_BASE 0x1001040
#define VECTOR_OP_SPAN 32
#define ACCELERATOR_IRQ 5
#define ACCELERATOR_IRQ_INTERRUPT_CONTROLLER_ID 0

#define ALT_CI_DIJKSTRA32_FLOAT_4(n,A,B) __builtin_custom_pnif(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_4_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_4_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_1(n,A,B) __builtin_custom_fnii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_1_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_1_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_2(n,A,B) __builtin_custom_fnii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_2_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_2_N_MASK ((1<<8)-1)

#define ALT_CI_DIJKSTRA32_FLOAT_3(n,A,B) __builtin_custom_inii(ALT_CI_DIJKSTRA32_FLOAT_0_N+(n&ALT_CI_DIJKSTRA32_FLOAT_0_N_MASK),(A),(B))
#define ALT_CI_DIJKSTRA32_FLOAT_3_N 0x0
#define ALT_CI_DIJKSTRA32_FLOAT_3_N_MASK ((1<<8)-1)

#if (BUFFER_SIZE == 0)
    #error You have set the buffer size to be zero, increase it to a positive value that is a multiple of 4 bytes
#elif ((BUFFER_SIZE & 0x3) != 0)
    #error Make sure BUFFER_SIZE is a multiple of 4 bytes to ensure word boundary accesses
#elif (BUFFER_SIZE > 2*1024*1024)
    #error Reduce BUFFER_SIZE to be 2MB or less so that the source buffer will fit in SSRAM
#endif

#endif
