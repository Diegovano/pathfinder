#pragma once

#include <stdio.h>
#include <system.h>
#include "io.h"
#include "sys/alt_cache.h"
#include "sys/alt_irq.h"

extern int __builtin_ldwio(const volatile void* src);
extern void __builtin_stwio(volatile void* dst, int data);

class StarAccelerator
{
private:
	volatile int done = 0;
	unsigned int _base;
	unsigned int _span;
public:
	float* input;
	float* res;

	StarAccelerator(unsigned int, unsigned int);
	~StarAccelerator();
	int irq_reg(unsigned int, unsigned int);
	static void isr(void*);
	const unsigned int base() const;
	void check();
	void reset();
	void write(unsigned int, int);
	int read(unsigned int);
	void exec();
};