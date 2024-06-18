#ifndef _ACCELERATOR_H_
#define _ACCELERATOR_H_

#include <stdio.h>
#include <system.h>
#include "io.h"
#include "sys/alt_irq.h"

class Accelerator
{
private:
	volatile int done = 0;
	volatile int res;
	unsigned int _base;
	unsigned int _span;
public:
	Accelerator(unsigned int, unsigned int);
	int irq_reg(unsigned int, unsigned int);
	static void isr(void*);
	void check();
	void reset();
	void write(unsigned int, int);
	int read(unsigned int);
	int exec();
};

#endif
