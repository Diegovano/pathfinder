# Full Hardware Dijkstra implementation

The documentation is discussed mainly subject to the [Dijkstra_float](/hardware/Dijkstra_float/) version of the hardware, which is the latest. Yet module names are generally consistent across versions and can also apply to the others.

## Modules

* [DijkstraInterface](/hardware/Dijkstra_float/hdl/dijkstra_interface.sv)
    The top level module. The hardware can be driven in two ways, as a NIOS custom instruction by the CPU or as an Avalon MM slave by the DMA. The module determines which is currently driving the hardware
    * [EdgeCache](/hardware/Dijkstra_float/hdl/edge_cache.v)
        The cache used to store values of the adjacency matrix. The submodule [EdgeCacheMem](/hardware/Dijkstra_float/hdl/edge_cache_mem.v) is written in the specific way such that it can be recognized correctly as block memory when synthesizing.
    * [DijkstraTop](/hardware/Dijkstra_float/hdl/dijkstra_top.sv)
    The main computation logic for finding the shortest path with the data in the EdgeCache.
        * [PriorityQueue](/hardware/Dijkstra_float/hdl/priority_queue.sv)
            * [MinHeap](/hardware/Dijkstra_float/hdl/minheap.sv)
            series of registers which are connected through logic in a way that performs a heap function. This is done with a clock divider to avoid timing analysis error due to long and unbalanced paths. 

## Parameters
* MAX_NODES, INDEX_WIDTH
    The maximum number of nodes and number of bits needed to index them. For the hardware to work properly as a custom instruction, 2^INDEX_WIDTH needs to be greater than or equal to MAX_NODES. For DMA driver to work properly, 2^INDEX_WIDTH needs to be strictly equal to MAX_NODES.
* VALUE_WIDTH 
    the width of value stored in EdgeCache, it should be 32 by default for the system to work correctly on single precision floating point. 