# Acceleration

## Direct Memory Access Controller

One of the limitations of the custom system is the bandwidth of the CPU's data path. In early iterations, custom hardware accelerators were connected to the CPU through the custom instruction interface. The custom instruction interface assigns an op-code to a user-defined multi-cycle instruction and provides buses to connect the CPU to a peripheral.

When the custom instruction is called, the ALU is bypassed and the 2 32-bit operands are sent to a custom peripheral hardware block (the accelerator). The 32-bit result is retrieved and stored in a CPU register after an arbitray number of cycles or after the flipping of a hardware flag.

There are 2 principal issues with this interface. Firstly, input data has to pass through the CPU on its journey between SDRAM and peripherals, increasing latency. Secondly, only 2 inputs can be fed to the accelerator per instruction call. These 2 factors act as bottlenecks for the throughput of accelerator block.

The Direct Memory Access Controller is a module that can carry out the transfer of data from one contiguous memory array to another, bypassing the CPU. The DMA controller does not only reduce the latency of read/write operations between peripherals and the SDRAM but also allows for more than 2 inputs and outputs. The CPU only needs to trigger the DMA transfer and is then free to begin other processes.

A [custom software driver](../software/pathfinder/src/DMA/DMA.h) was written for the DMA controller.

## Accelerating A* Algorithm

## Accelerating Dijkstra Algorithm

### Full Hardware Dijkstra
See documenation on this [branch](https://github.com/Diegovano/pathfinder/tree/h-dijkstra-mem_debug) for details on Full Hardware Dijkstra implementation. 

