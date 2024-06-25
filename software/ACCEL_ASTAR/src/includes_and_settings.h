#ifndef _INCLUDES_AND_SETTINGS_h
#define _INCLUDES_and_SETTINGS_h

#include <cstdio>
#include <cstddef>
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdio>
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

#define EUCLID_DIST_BASE 0x1001000
#define EUCLID_DIST_SPAN 64
#define EUCLID_DIST_IRQ 5
#define EUCLID_DIST_IRQ_INTERRUPT_CONTROLLER_ID 0

#define addf(A,B) __builtin_custom_fnff(ALT_CI_ADDF_N,(A),(B))
#define ALT_CI_ADDF_N 0x1
#define ltf(A,B) __builtin_custom_inff(ALT_CI_LTF_N,(A),(B))
#define ALT_CI_LTF_N 0x0

#if (BUFFER_SIZE == 0)
    #error You have set the buffer size to be zero, increase it to a positive value that is a multiple of 4 bytes
#elif ((BUFFER_SIZE & 0x3) != 0)
    #error Make sure BUFFER_SIZE is a multiple of 4 bytes to ensure word boundary accesses
#elif (BUFFER_SIZE > 2*1024*1024)
    #error Reduce BUFFER_SIZE to be 2MB or less so that the source buffer will fit in SSRAM
#endif

#endif
