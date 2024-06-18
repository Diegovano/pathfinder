#include "system.h"
#include <stdio.h>
#include <string.h>
#include "sys/alt_dma.h"

/*

	In this code we have an array of test data and then we assign the targetdata to be stored on the onchipdata memory.
	We then open the DMA channels. We then loop twice.

	In the loop, we first post a transmit request to the DMA transmit channel. The input arguements are the channel,
	the data to send, the length of the data to send in bytes, a callback after the data has been sent.

	We then post a receive request to the DMA receive channel. The input arguements are the channel, a pointer to 
	the location that data will be received to, the maximum length of the data to receive in bytes, a callback that is
	called afdter the data is receieved.

	In this case, the data to be sent is the test array and the received data will be stored in the targetdata array.


*/

unsigned testdata[4] = {7,2,1,3};

unsigned int __attribute__((section(".onchipdata"))) targetdata[4];

volatile int doneFlag = 0;
static void dmadone() {
	doneFlag = 1;
}

int main() {

	unsigned i;

	printf("Clear target\n");
	memset(targetdata, 0, sizeof(unsigned int) * 4);

	printf("Setup DMA\n");
	alt_dma_txchan txchan;
	alt_dma_rxchan rxchan;
	txchan = alt_dma_txchan_open ("/dev/dma");
	if (txchan == NULL)
		printf("Error opening tx channel\n");
		rxchan = alt_dma_rxchan_open ("/dev/dma");
	if (rxchan == NULL)
		printf("Error opening rx channel\n");

	for (i=0; i<2; i++) {


		// performs the read operation from the source memory
		alt_dma_txchan_send( txchan,
							 testdata + i*2,
							 2 * sizeof(unsigned int),
							 NULL,
							 NULL);
		// performs the write operation to the destination memory
		alt_dma_rxchan_prepare( rxchan,
								targetdata + i*2,
								2 * sizeof(unsigned int),
								dmadone,
								NULL);

		while (!doneFlag)
		  printf("Wait for DMA\n");

		doneFlag = 0;
	}

	for (i=0; i<4; i++)
		printf("Data[%d] = %d\n", i, targetdata[i]);

	while (1);

	return 0;
}
