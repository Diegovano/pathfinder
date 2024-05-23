#pragma once
#include <queue>
#include <iostream>

#include "altera_avalon_spi_regs.h" // for IO**_SPI stuff
#include "system.h"
#include "stdint-gcc.h" // exact width types
#include "../state.h"
#include "../helpers/helpers.h"
// #include "alt_types.h" // for alt_* types
// #include "altera_avalon_pio_regs.h" // for IO**_PIO stuff

struct IsrContext
{
  std::queue<char> *queue;
  States *state, *nextState;
  std::string response;

  IsrContext(std::queue<char> *p_queue, States *p_state, States *p_nextState) : queue(p_queue), state(p_state), nextState(p_nextState)
  {

  }
};

void spi_rx_isr(void* isr_context);