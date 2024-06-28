# Acceleration

## Accelerating Dijkstra's Algorithm

Intially, when thinking of how to accelerate Dijkstra's Algorithm, we started by inspecting the dissasembly of the software implementation of the algorithm.

The dissassembly of the program shows how the compiler converts each line of our program into lines of assembly. Each assembly instruction is executed in one cycle, so seeing which lines need the most assembly instruction was a good way to get an idea of which parts of the algorithm were most ripe for optimisation. In this, case the following line was the most computationally complex:

```
      if (!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != __INT_MAX__ && dist[min_index] + graph[min_index][i] < dist[i]) 
  8003bc:	e0bff917 	ldw	r2,-28(fp)
  8003c0:	10c00217 	ldw	r3,8(r2)
  8003c4:	e0bffa17 	ldw	r2,-24(fp)
  8003c8:	1885883a 	add	r2,r3,r2
  8003cc:	10800003 	ldbu	r2,0(r2)
  8003d0:	1080005c 	xori	r2,r2,1
  8003d4:	10803fcc 	andi	r2,r2,255
  8003d8:	10005526 	beq	r2,zero,800530 <_ZN5Graph8dijkstraEv+0x288>
  8003dc:	e0bff917 	ldw	r2,-28(fp)
  8003e0:	10c00017 	ldw	r3,0(r2)
  8003e4:	e0bffc17 	ldw	r2,-16(fp)
  8003e8:	100490ba 	slli	r2,r2,2
  8003ec:	1885883a 	add	r2,r3,r2
  8003f0:	10c00017 	ldw	r3,0(r2)
  8003f4:	e0bffa17 	ldw	r2,-24(fp)
  8003f8:	100490ba 	slli	r2,r2,2
  8003fc:	1885883a 	add	r2,r3,r2
  800400:	10c00017 	ldw	r3,0(r2)
  800404:	000b883a 	mov	r5,zero
  800408:	1809883a 	mov	r4,r3
  80040c:	08c12a40 	call	8c12a4 <__gesf2>
  800410:	0080470e 	bge	zero,r2,800530 <_ZN5Graph8dijkstraEv+0x288>
  800414:	e0bff917 	ldw	r2,-28(fp)
  800418:	10c00117 	ldw	r3,4(r2)
  80041c:	e0bffc17 	ldw	r2,-16(fp)
  800420:	100490ba 	slli	r2,r2,2
  800424:	1885883a 	add	r2,r3,r2
  800428:	10c00017 	ldw	r3,0(r2)
  80042c:	0153c034 	movhi	r5,20224
  800430:	1809883a 	mov	r4,r3
  800434:	08c12340 	call	8c1234 <__eqsf2>
  800438:	10003d26 	beq	r2,zero,800530 <_ZN5Graph8dijkstraEv+0x288>
  80043c:	e0bff917 	ldw	r2,-28(fp)
  800440:	10c00117 	ldw	r3,4(r2)
  800444:	e0bffc17 	ldw	r2,-16(fp)
  800448:	100490ba 	slli	r2,r2,2
  80044c:	1885883a 	add	r2,r3,r2
  800450:	11000017 	ldw	r4,0(r2)
  800454:	e0bff917 	ldw	r2,-28(fp)
  800458:	10c00017 	ldw	r3,0(r2)
  80045c:	e0bffc17 	ldw	r2,-16(fp)
  800460:	100490ba 	slli	r2,r2,2
  800464:	1885883a 	add	r2,r3,r2
  800468:	10c00017 	ldw	r3,0(r2)
  80046c:	e0bffa17 	ldw	r2,-24(fp)
  800470:	100490ba 	slli	r2,r2,2
  800474:	1885883a 	add	r2,r3,r2
  800478:	10c00017 	ldw	r3,0(r2)
  80047c:	180b883a 	mov	r5,r3
  800480:	08c0dc80 	call	8c0dc8 <__addsf3>
  800484:	1007883a 	mov	r3,r2
  800488:	1809883a 	mov	r4,r3
  80048c:	e0bff917 	ldw	r2,-28(fp)
  800490:	10c00117 	ldw	r3,4(r2)
  800494:	e0bffa17 	ldw	r2,-24(fp)
  800498:	100490ba 	slli	r2,r2,2
  80049c:	1885883a 	add	r2,r3,r2
  8004a0:	10c00017 	ldw	r3,0(r2)
  8004a4:	180b883a 	mov	r5,r3
  8004a8:	08c135c0 	call	8c135c <__lesf2>
  8004ac:	1000200e 	bge	r2,zero,800530 <_ZN5Graph8dijkstraEv+0x288>
```

This is the step in Dijkstra's algorithm where we check if a candidate node is connected to the current node, and whether the path through this node to the candidate is shorter than the current shortest distance recorded.

The assembly highlights the numerous operations required, including several calls to software emulation for floating point operations, these are formatted as `<__*f*>`. For instance, `<__addsf3>` is a function which itself contains 282 assembly instructions, `<__lesf2>` 44 instructions, `<__eqsf2>` 27 instructions and finally, `<__gesf2>` 45 instructions.

Therefore, one single execution of this function could, depending on the values at runtime, take up to around 450 cycles. Reducing this number by exploiting hardware was a key strategy for the development of the Hybrid Dijkstra optimisation.

As a point of comparaison, the Intel Floating Point IP blocks can perform floating point addition (replacing `<__addsf3>`) in 2 cycles at 50 MHz. Furthermore, the comparaisons are done combinationally.

Therefore, the `dijkstra_check_step` block was developed.

This block is implmented as a Nios II Custom Instruction. The framework is such that the custom hardware block has two 32-bit inputs `dataa` and `datab`, and allows for multiple latency options, including a fixed cycle latency for known delays, or a flexible latency, where the block signals to the CPU completion of computation by setting a `done` output flag to high. There is only one 32-bit output, `result`.

The hard limit of two 32-bit inputs was challenging, because the line to accelerate has three distinct data values. In the end, `dist[min_index]` and `graph[min_index][i]` were selected, in part because of their role in the addition.

Using Intel Floating Point IP, the comparaison and addition operations were accelerated using dedicated hardware. The next step in the algorithm is to write the distance to the candiate node as the new computed distance (the floating point addition) in the case the new route is shorter. 

Here, the original `if` statement first checks wether the candidate node is even worth considering, by checking if the node is connected to the current node (`graph[min_index][i] > 0` as negative indices, or infinity both represent disconnected nodes). When it is, then the addition is performed.

One of the advantages of hardware is that it is possible to perform multiple operations in parallel, therefore, time could be saved if the addition could be started before the connectedness of the candidate node can even be checked.

Therefore, designing the custom block to compute the new potential distance could allow us to start the addition immediately, then if the candiadate is invalid, return infinity as the new distance, and if not, then one cycle out of two required for the addition will have elapsed, with the new distance via the candidate reported during the next cycle. The next step in software would use Floating Point Hardware to determine whether this new distance is better than the existing distance.

In the end, even though Floating Point Addition emulation is far more costly, the sheer number of times comparaisons are made, it seems that most performance uplift was linked to the use of custom hardware to accelerate the comparaisons.

## Full Hardware Dijkstra
See documenation on this [branch](https://github.com/Diegovano/pathfinder/tree/h-dijkstra-mem_debug) for details on Full Hardware Dijkstra implementation. 

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

## Delta Algorithm

All of the above described algorithms are sequential algorithms, and it is difficult to paralellise them due to data interdependency between executions of the loop body. To further leverage the advantages of hardware, we also wanted to consider a parallel pathfinding algorithm. During our research, we learned about the Delta Stepping algorithm, detailed [here](https://www.sciencedirect.com/science/article/pii/S0196677403000762).

The fundamental advantage of this algorithm is that it is constructed in such a fashion that multiple edges are split into buckets where they can be "relaxed" in parallel, this means that the tentative distances can be updated in parallel.

Challenges with this algorithm include selecting the value of Delta. Delta determines the size of each bucket as well as whether nodes will be considered "light" or "heavy", essentially separating the work to be done into simpler operations (light) and more involved operations (heavy).

Although we were able to implement the Delta-Stepping algorithm in software, we did not have enough time to integrate it, experiment with different delta values, and developping hardware to perform parallel computation, although all of the infrastructure to facilitate this is now in place, and any desired pathfinding algorithms can be added to `graph.cpp`.