# Acceleration

## Direct Memory Access Controller

One of the limitations of the custom system is the latency of the CPU's data path. In early iterations, custom hardware accelerators were connected to the CPU through the custom instruction interface. The custom instruction interface assigns an op-code to a user-defined multi-cycle instruction and provides buses to connect the CPU to a peripheral.

When the custom instruction is called, the ALU is bypassed and the 2 32-bit operands are sent to a custom peripheral hardware block (the accelerator). The 32-bit result is retrieved and stored in a CPU register after an arbitrary number of cycles or after the flipping of a hardware flag.

There are 2 principal issues with this interface. Firstly, input data has to pass through the CPU on its journey between SDRAM and peripherals, increasing latency. Secondly, only 2 inputs can be fed to the accelerator per instruction call. These 2 factors act as bottlenecks for the throughput of the accelerator block.

The Direct Memory Access Controller is a module that can carry out the transfer of data from one contiguous memory array to another, bypassing the CPU. The DMA controller does not only reduce the latency of read/write operations between peripherals and the SDRAM but also allows for more than 2 inputs and outputs. The CPU only needs to trigger the DMA transfer and is then free to begin other processes. Furthermore, a single DMA Controller module can be connected to the SDRAM and all the memory-mapped peripherals thanks to the avalon bus' addressing.

A [custom software driver](../software/pathfinder/src/DMA/DMA.h) was written for the DMA controller. After initialising a `DMA` object, an interrupt mechanism is available to perform procedures upon the completion of a DMA transfer, for example starting an accelerator block's state machine. A memory-to-peripheral copy can be performed with the simple function `DMA::copy`.

By instantiating a small register file within the custom hardware accelerator blocks, an array of inputs can be taken and processed to calculate complex functions with more than 2 inputs, for example the euclidian distance between 2 points in 2D space. Similarly, Same Instruction Multiple Data (SIMD) instructions can be accelerated by reading a full input vector with the DMA controller. This can reduce the time complexity of the algorithm by computing vector operations in parallel rather than through 1 or more nested loops in software.

## Accelerating A* Algorithm
The A* Algorithm is a pathfinding algorithm used to find the shortest path between nodes in a weighted graph. Unlike other algorithms, such as Dijkstra's, it utilises a heuristic function to 'guide' the algorithm towards the goal more efficiently. The heuristic function estimates the cost to reach the goal from the current node, allowing A* to prioritise paths that appear more promising.

The implementation of this algorithm, as detailed below, incorporates custom hardware to accelerate the heuristic calculations. This enhancement improves the performance of the algorithm by offloading the computationally intensive distance calculations to specialised hardware.

### Algorithm Explanation 
#### Initialisation 
First, we initialise the necessary data structures for the A* Algorithm:

```cpp
// g_value is a vector of floats which stores the cost of the cheapest path to a node
std::vector<float> g_value(NUM_VERTICES, INFINITY);

// f_value is a vector of floats which stores the estimated total cost from the start node to the goal node
std::vector<float> f_value(NUM_VERTICES, INFINITY);

// came_from is a vector of ints which stores the parent node of a node
std::vector<int> came_from(NUM_VERTICES, -1);

// closed_nodes is a set of ints which stores the nodes that have been visited
std::set<int> closed_nodes;

// initialise the g value of the source node to 0
g_value[start] = 0;
// initialise the f value of the source node to the heuristic value
f_value[start] = getDistance(dma, accel, nodes[start], nodes[end]);

// create a priority queue of pairs of floats and ints which represents the f value and the node
std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> queue;
// push the source node onto the queue
queue.push({0.0f, start});
```
#### Main Loop 
The main loop of the algorithm processes nodes from the priority queue until the goal node is reached or the queue is empty:
```cpp
while (!queue.empty())
{
  // get the node with the smallest f value
  int current_node = queue.top().second;
  queue.pop();

  // if the node is in the closed nodes set, skip the rest of the loop
  if (closed_nodes.count(current_node)) continue;
  // if the node is the target node, we have found the path
  if (current_node == end) break;

  // add the node to the closed nodes set
  closed_nodes.insert(current_node);

  // for each neighbour of the current node
  for (int i = 0; i < NUM_VERTICES; i++)
  {
    // if the neighbour node is in the closed nodes set, skip the rest of the loop
    if (closed_nodes.count(i)) continue;

    // calculate the tentative g value
    float temp_g_score = ALT_CI_ADDF(g_value[current_node], graph[current_node][i]);

    // if the tentative g value is less than the g value of the neighbour node
    if (ALT_CI_LTF(temp_g_score, g_value[i]))
    {
        // update the came from, g value and f value of the neighbour node
        came_from[i] = current_node;
        g_value[i] = temp_g_score;
        f_value[i] = ALT_CI_ADDF(temp_g_score, getDistance(dma, accel, nodes[i], nodes[end]));
        // push the f value and the neighbour node to the queue
        queue.push({f_value[i], i});
    }
  }
}
```
#### Path Reconstruction 
Once the goal node is reached, the path is reconstructed from the `came_from` vector:
```cpp
// reconstruct the path
std::vector<int> path;
// start from the target node
for (int current_node = end; current_node != -1; current_node = came_from[current_node]) {
  path.push_back(current_node);
}
// std::reverse(path.begin(), path.end());

predecessor = path.data();
```
The resulting path is backwards and the reversing is omitted on the FPGA side to save time.

### Acceleration of A* Algorithm  
The heuristic calculation is performed using custom hardware, which is interfaced through the `getDistance` function:
```cpp
float Graph::getDistance(DMA& dma, StarAccelerator& accel, const Node& n1, const Node& n2)
{
  accel.input[0] = n1.x;
  accel.input[1] = n2.x;
  accel.input[2] = n1.y;
  accel.input[3] = n2.y;

  dma.copy((void*)(accel.input), (void*)(accel.base() + 4), 16);

  accel.exec();

  dma.copy((void*)(accel.base() + 36), (void*)(accel.res), 4);

  return *(accel.res);
}
```
This function begins by loading the coordinates of the nodes in the accelerator's input array. The DMA is then used to copy the input array into the accelerators custom memory block, where it is then executed. After the computation, the result is copied from the accelerator's memory to the host where it is then returned. For more information about the DMA, see the [Direct Memory Access Controller](#direct-memory-access-controller) section.

Custom floating point hardware is also utilised to provide acceleration and is referenced by `ALT_CI_ADDF` and `ALT_CI_LTF` used for addition and comparison, respectively. These hardware components are implemented as Custom Instruction Slaves on the NIOSII processer. 

![Block Diagram of Custom Hardware](ADD LINK HERE "Block Diagram of Custom Hardware")

## Accelerating Dijkstra Algorithm

### Full Hardware Dijkstra
See documenation on this [branch](https://github.com/Diegovano/pathfinder/tree/h-dijkstra-mem_debug) for details on Full Hardware Dijkstra implementation. 

