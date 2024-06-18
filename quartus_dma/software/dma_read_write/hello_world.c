#include "system.h"
#include <stdio.h>
#include <string.h>
#include "sys/alt_dma.h"
#include "altera_avalon_dma_regs.h"
#include <io.h>

#define DMA_ACCESS_0_BASE 0x0
#define REG1   (DMA_ACCESS_0_BASE + 0)
#define REG2   (DMA_ACCESS_0_BASE + 4)
#define REG3   (DMA_ACCESS_0_BASE + 8)
#define REG4   (DMA_ACCESS_0_BASE + 12)
#define RESULT (DMA_ACCESS_0_BASE + 16)


#define DMA_STATUS printf("Status: %u\n", IORD_ALTERA_AVALON_DMA_STATUS(DMA_BASE));


float testdata[4] = {17.0, 12.0, 11.0, 13.0};

union {
	unsigned int ui;
	float f;
} float_ieee754;

void dma_check()
{
  int STATUS_MASK = 0x11111;

  printf("readaddress: %x, writeaddress: %x, length: %x, status: %x, control: %x\n",
      IORD_ALTERA_AVALON_DMA_RADDRESS(DMA_0_BASE),
      IORD_ALTERA_AVALON_DMA_WADDRESS(DMA_0_BASE),
      IORD_ALTERA_AVALON_DMA_LENGTH(DMA_0_BASE),
      IORD_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE) & STATUS_MASK,
      IORD_ALTERA_AVALON_DMA_CONTROL(DMA_0_BASE)
      );
}

volatile int doneFlag = 0;
static void dmadone(void *handle, void *data) {
	dma_check();
	unsigned int value = IORD(REG1, 0);
	float_ieee754.ui = value;
	printf("Read back value: %f\n", float_ieee754.f);

	value = IORD(REG2, 0);
	float_ieee754.ui = value;
	printf("Read back value: %f\n", float_ieee754.f);

	value = IORD(REG3, 0);
	float_ieee754.ui = value;
	printf("Read back value: %f\n", float_ieee754.f);

	value = IORD(REG4, 0);
	float_ieee754.ui = value;
	printf("Read back value: %f\n", float_ieee754.f);

	value = IORD(RESULT,0);
	float_ieee754.ui = value;
	printf("Read back value: %f\n", float_ieee754.f);


	doneFlag = 1;
}


int writeToModule(alt_dma_txchan tx, alt_dma_rxchan rx, const void* data, alt_u32 size, alt_txchan_done* callback, void* location ) {

	// write to the dma from location
	if (alt_dma_txchan_send(tx, data, size, NULL, NULL) < 0) {
		printf("Error during DMA send (write)\n");
		return -1;
	}

	// read from dma to custom module
	if (alt_dma_rxchan_prepare(rx, location, size, callback, NULL) < 0) {
		printf("Error during DMA prepare (write)\n");
		return -1;
	}

	while (!doneFlag) {
		usleep(100);
	}
	doneFlag = 0;
	return 0;
}

int readFromModule(alt_dma_txchan tx, alt_dma_rxchan rx, void* data, alt_u32 size, alt_txchan_done* callback, const void* location ) {

	// write to the dma from the custom module
	if (alt_dma_txchan_send(tx, location, size, NULL, NULL) < 0) {
		printf("Error during DMA send (write)\n");
		return -1;
	}

	// read from dma to software
	if (alt_dma_rxchan_prepare(rx, data, size, callback, NULL) < 0) {
		printf("Error during DMA prepare (write)\n");
		return -1;
	}

	while (!doneFlag) {
		usleep(100);
	}
	doneFlag = 0;
	return 0;
}

int main() {
	unsigned i;

	printf("Setup DMA\n");
	alt_dma_txchan txchan;
	alt_dma_rxchan rxchan;

	txchan = alt_dma_txchan_open("/dev/dma_0");
	if (txchan == NULL) {
		printf("Error opening tx channel\n");
		return 1;
	}

	rxchan = alt_dma_rxchan_open("/dev/dma_0");
	if (rxchan == NULL) {
		printf("Error opening rx channel\n");
		return 1;
	}

	// write testdata to custom module
	if (writeToModule(txchan, rxchan, testdata, 4 * sizeof(float), dmadone, (void *)DMA_ACCESS_0_BASE) < 0) {
		printf("Error during write\n");
		return 1;
	}

	//read data from REG2 into REG4
	if(readFromModule(txchan, rxchan, (void *)REG4, sizeof(float), dmadone, (void *)REG2) < 0) {
		printf("Error during read\n");
		return 1;
	}

	printf("\n\n");

//	// another way to access the registers
//	volatile float *DMAA = (float *) DMA_ACCESS_0_BASE;
//
//	DMAA[0] = 1.0;
//	DMAA[1] = 10.0;
//	DMAA[2] = 34.0;
//
//	printf("read ip1 %f\n", DMAA[0]);
//	printf("read ip2 %f\n", DMAA[1]);
//	printf("read ip3 %f\n", DMAA[2]);
//	printf("read ip4 %f\n", DMAA[3]); // should be already set to 12
//	printf("read result %f\n", DMAA[4]);

	return 0;
}

