#include "Accelerator.h"

Accelerator::Accelerator(unsigned int base, unsigned int span)
: _base{base},
  _span{span}
{
	reset();
}

int Accelerator::irq_reg(unsigned int interrupt_controller_id, unsigned int irq)
{
    if(alt_ic_isr_register(interrupt_controller_id, irq, isr, this, NULL) != 0)
    {
       return 1;
    }
    return 0;
}

void Accelerator::isr(void * context)
{
	Accelerator* accel = (Accelerator*)context;
	printf("CUSTOM HARDWARE COMPLETED\n\n");
	accel->res = accel->read(1);
	accel->write(0,0);  /*clear the interrupt*/
	accel->done++;
}

void Accelerator::check()
{
	printf("ACCELERATOR INTERNAL MEMORY\n");
	for (int i=0; i < 32; i+=4)
	{
		int val = IORD_32DIRECT(_base, i);
		printf("OFFSET %d = decimal %d, hex %x\n", i, val, val);
	}
	printf("\n");
}

void Accelerator::reset()
{
	printf("CUSTOM RESET\n");
	//CUSTOM_WR_CTRL(_base, 0);
	for (int i=0; i < 32; i+=4)
	{
		IOWR_32DIRECT(_base, i, 0);
	}
	printf("\n");

}

void Accelerator::write(unsigned int word_offset, int data)
{
	unsigned int byte_offset = word_offset << 2;
	if (byte_offset > _span)
	{
		printf("ERROR: out of range register");
		return;
	}
	else if (byte_offset == 0)
	{
		printf("WARNING: writing to CTRL register\n");
	}
	else if (byte_offset == 4)
	{
		printf("WARNING: writing to RES register\n");
	}
	IOWR_32DIRECT(_base, byte_offset, data);
}

int Accelerator::read(unsigned int word_offset)
{
	unsigned int byte_offset = word_offset << 2;
	if (byte_offset > _span) {
		printf("WARNING: out of range register");
	}
	else if (byte_offset == 0)
	{
		printf("INFO: reading from CTRL register\n");
	}
	else if (byte_offset == 1)
	{
		printf("INFO: reading from RES register\n");
	}
	return IORD_32DIRECT(_base, byte_offset);
}

int Accelerator::exec() {
	write(0,0x00000001);

	while (!done);
	done = 0;

	return res;
}
